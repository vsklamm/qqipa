#include "indexedfile.h"

#include <QDebug>
#include <QDir>
#include <QThread>

#include <functional>
#include <algorithm>

#include "absl/container/flat_hash_set.h"

namespace qqipa {

using trigrams_t = absl::flat_hash_set<trigram_t>;

IndexedFile::IndexedFile(const QFileInfo &qFileInfo)
    : name_(qFileInfo.fileName()), path_(qFileInfo.absolutePath()), size_(qFileInfo.size())
{

}

bool IndexedFile::calculateIndex()
{
    return (size_ < smallLargeSize) ? calculateSmallFile() : calculateLargeFile();
}

bool IndexedFile::calculateSmallFile()
{
    QFile readFile(getFullPath());
    if (!readFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }
    auto filePtr = readFile.map(0, size_);
    readFile.close();

    trigrams_t trigrams;
    if (filePtr != nullptr)
    {
        trigram_t x = (trigram_t(0[filePtr]) << 8) | (trigram_t(1[filePtr]));
        for (fsize_t i = 2; i < size_; ++i)
        {
            x = (((x << 8) & tbytesMask) | trigram_t(filePtr[i]));
            trigrams.insert(x);
        }
    }
    container_.resize(trigrams.size());
    std::copy(trigrams.begin(), trigrams.end(), container_.begin());
    return true;
}

bool IndexedFile::calculateLargeFile()
{
    char buffer[bufferSize];
    trigrams_t trigramsSet;

    QFile readFile(getFullPath());
    if (readFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        readFile.read(buffer, 2);
        trigram_t x = (trigram_t(uchar(buffer[0])) << 8) | (trigram_t(uchar(buffer[1])));
        trigramsSet.insert(x);
        while (!readFile.atEnd())
        {
            const auto len = readFile.read(buffer, bufferSize);
            for (int i = 2; i < len; ++i)
            {
                x = (((x << 8) & tbytesMask) | trigram_t(uchar(buffer[i])));
                trigramsSet.insert(x);
            }
            if (trigramsSet.size() > evilNumber)
            {
                readFile.close();
                return false;
            }
        }
        readFile.close();
    }

    container_.reserve(trigramsSet.size());
    for (auto& tr : trigramsSet)
    {
        container_.push_back(tr);
    }
    return true;
}

fsize_t IndexedFile::searchPattern(fsize_t rawPatternSize, TrigramContainer &patternTrigrams, const std::regex &patternRegex)
{
    qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();
    container_.makeAccessible();
    for (auto& x : patternTrigrams)
    {
        if (std::lower_bound(container_.begin(), container_.end(), x) == container_.end())
        {
            return 0;
        }
    }

    fsize_t count = 0;

    QFile readFile(getFullPath());
    if (readFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        const int bufferSize = 1024 * 32;
        const fsize_t patternSize = fsize_t(rawPatternSize);
        char buffer[bufferSize];

        readFile.read(buffer, patternSize - 1);

        while (!readFile.atEnd())
        {
            long long len = readFile.read(buffer + patternSize - 1, bufferSize - patternSize + 1);

            auto matchCount = uint64_t(std::distance(std::regex_iterator<char *>(buffer, buffer + patternSize - 1 + len, patternRegex),
                                                     std::regex_iterator<char *>()));
            count += matchCount;
            std::copy(buffer + len, buffer + patternSize - 1 + len, buffer);
        }
    }
    return count;
}

QString IndexedFile::getFullPath() const
{
    return QDir::cleanPath(path_ + QDir::separator() + name_);
}

}
