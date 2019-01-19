#include "patternutil.h"

#include <QDebug>
#include <QThread>

#include <functional>

namespace qqipa {

PatternUtil::PatternUtil() = default;

PatternUtil::PatternUtil(const QString &pattern)
{
    rawSize_ = std::int64_t(pattern.size());
    stringStd_ = pattern.toStdString();
    trigrams_ = getPatternTrigrams(stringStd_);

    const std::regex specialCharacters { R"([-[\]{}()*+?.,\^$|#\s\\])" };
    regex_.assign(std::regex_replace(stringStd_, specialCharacters, R"(\$&)"));
}

TrigramContainer PatternUtil::getPatternTrigrams(const std::string &patternStd)
{
    qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();
    TrigramContainer result;
    if (patternStd.size() > 2)
    {
        result.reserve(patternStd.size() - 2);

        trigram_t x = (trigram_t(uchar(patternStd[0])) << 8) | trigram_t(uchar(patternStd[1]));
        for (size_t i = 2; i < patternStd.size(); ++i)
        {
            x = (((x << 8) & tbytesMask) | trigram_t(uchar(patternStd[i])));
            result.push_back(x);
        }
        result.resize(size_t(std::distance(result.begin(), std::unique(result.begin(), result.end()))));
    }
    return result;
}

}
