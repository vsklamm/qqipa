#ifndef PATTERNSEARCHER_H
#define PATTERNSEARCHER_H

#include "trigramcontainer.h"

#include <QString>
#include <regex>

namespace qqipa {

struct PatternUtil
{
public:
    explicit PatternUtil();
    explicit PatternUtil(const QString &pattern);
    ~PatternUtil() = default;

private:
    inline TrigramContainer getPatternTrigrams(const std::string &patternStd);

public:
    std::int64_t rawSize_;
    std::string stringStd_;
    std::regex regex_;
    TrigramContainer trigrams_;
};

}

#endif // PATTERNSEARCHER_H
