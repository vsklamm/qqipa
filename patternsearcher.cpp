#include "patternsearcher.h"

#include <QDebug>
#include <QThread>

#include <regex>

namespace qqipa {

PatternSearcher::PatternSearcher()
    : was_canceled(false)
{
}

void PatternSearcher::search(const QString &pattern, QList<IndexedFile> &indexed_files)
{
    pattern_std = pattern.toStdString();
    patternTrigrams = getPatternTrigrams(pattern_std);

    const std::regex special_characters { R"([-[\]{}()*+?.,\^$|#\s\\])" };
    pattern_regex.assign(std::regex_replace(pattern_std, special_characters, R"(\$&)"));

    std::vector<std::pair<QString, fsize_t>> matches;
    for (auto& cur_file : indexed_files)
    {
        auto result = searchInFile(cur_file);
        if (result > 0)
        {
            matches.emplace_back(cur_file.getFullPath(), result);
        }
    }
    emit searchingFinished(matches.size());
}

fsize_t PatternSearcher::searchInFile(IndexedFile &indexedFile)
{
    bool good = true;
    for (auto x: patternTrigrams) {
        if (std::lower_bound(indexedFile.container_.begin(), indexedFile.container_.end(), x) == indexedFile.container_.end()) {
            good = false;
            break;
        }
    }
    if (!good)
        return 0;

    fsize_t count = 0;

    QFile read_file(indexedFile.getFullPath());
    if (read_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        std::regex r(pattern_std);

        const int buffer_size = 1024 * 32;
        const fsize_t pattern_size = fsize_t(pattern_std.size());
        char buffer[buffer_size + pattern_size - 1];

        read_file.read(buffer, pattern_size - 1);

        while (!read_file.atEnd())
        {
            long long len = read_file.read(buffer + pattern_size - 1, buffer_size);

            auto match_count = uint64_t(std::distance(std::regex_iterator<char *>(buffer, buffer + pattern_size - 1 + len, r), std::regex_iterator<char *>()));
            count += match_count;
            std::copy(buffer + len, buffer + pattern_size - 1 + len, buffer);
        }
    }
    return count;
}

void PatternSearcher::findMathingTrigrams()
{

}

void PatternSearcher::interruptSearching()
{
    was_canceled = true;
}

TrigramContainer PatternSearcher::getPatternTrigrams(const std::string &pattern_std)
{
    TrigramContainer result;
    if (pattern_std.size() > 2)
    {
        result.reserve(pattern_std.size() - 2);

        trigram_t x = (trigram_t(pattern_std[0]) << 8) | trigram_t(pattern_std[1]);
        for (size_t i = 2; i < pattern_std.size(); ++i)
        {
            x = (((x << 8) & tbytes_mask) | trigram_t(pattern_std[i]));
            result.push_back(x);
        }
        result.resize(size_t(std::distance(result.begin(), std::unique(result.begin(), result.end()))));
    }
    return result;
}

void PatternSearcher::clearData()
{
    was_canceled = false;
}

}
