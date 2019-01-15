#include "patternsearcher.h"

#include <QDebug>
#include <QThread>

#include <regex>

namespace qqipa {

PatternSearcher::PatternSearcher()
    : wasCanceled(false)
{
}

void PatternSearcher::search(const QString &pattern, const std::vector<DirectoryWrapper *> &indexedDirectories)
{
    patternStd = pattern.toStdString();
    patternTrigrams = getPatternTrigrams(patternStd);

    const std::regex specialCharacters { R"([-[\]{}()*+?.,\^$|#\s\\])" };
    patternRegex.assign(std::regex_replace(patternStd, specialCharacters, R"(\$&)"));

    std::vector<std::pair<QString, fsize_t>> matches;
    for (auto& curDir : indexedDirectories) {
        for (auto& curFile : curDir->indexedFileList)
        {
            auto result = searchInFile(curFile);
            if (result > 0)
            {
                matches.emplace_back(curFile.getFullPath(), result);
            }
        }
    }
    emit searchingFinished(matches.size());
}

fsize_t PatternSearcher::searchInFile(IndexedFile &indexedFile)
{
    bool trigrContains = true;
    for (auto x: patternTrigrams) {
        if (std::lower_bound(indexedFile.container_.begin(), indexedFile.container_.end(), x) == indexedFile.container_.end()) {
            trigrContains = false;
            break;
        }
    }
    if (!trigrContains) return 0;

    fsize_t count = 0;

    QFile readFile(indexedFile.getFullPath());
    if (readFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        std::regex r(patternStd);

        const int bufferSize = 1024 * 32;
        const fsize_t patternSize = fsize_t(patternStd.size());
        char buffer[bufferSize + patternSize - 1];

        readFile.read(buffer, patternSize - 1);

        while (!readFile.atEnd())
        {
            long long len = readFile.read(buffer + patternSize - 1, bufferSize);

            auto matchCount = uint64_t(std::distance(std::regex_iterator<char *>(buffer, buffer + patternSize - 1 + len, r), std::regex_iterator<char *>()));
            count += matchCount;
            std::copy(buffer + len, buffer + patternSize - 1 + len, buffer);
        }
    }
    return count;
}

void PatternSearcher::interruptSearching()
{
    wasCanceled = true;
}

TrigramContainer PatternSearcher::getPatternTrigrams(const std::string &patternStd)
{
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

void PatternSearcher::clearData()
{
    wasCanceled = false;
}

}
