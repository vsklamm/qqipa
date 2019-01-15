#include "indexedfile.h"

#include <QDebug>
#include <QDir>
#include <QDirIterator>

#include <algorithm>
#include <bitset>

#include "absl/container/flat_hash_set.h"

namespace qqipa {

using trigrams_t = absl::flat_hash_set<trigram_t>; // std::unordered_set<trigram_t>;

IndexedFile::IndexedFile(const QString &fileName, const QString &filePath, const fsize_t &size)
    : name_(fileName), path_(filePath), size_(size)
{

}

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
    QFile read_file(getFullPath());
    if (!read_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    auto file_ptr = read_file.map(0, size_);
    read_file.close();

    trigrams_t trigrams;
    if (file_ptr != nullptr)
    {
        const trigram_t mask = 0x00FFFFFF;
        trigram_t x = (trigram_t(0[file_ptr]) << 8) | (trigram_t(1[file_ptr]));
        for (fsize_t i = 2; i < size_; ++i)
        {
            x = (((x << 8) & mask) | trigram_t(file_ptr[i]));
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
    trigrams_t trigrams_set;

    QFile read_file(getFullPath());
    if (read_file.open(QIODevice::ReadOnly | QIODevice::Text)) {

        const trigram_t mask = 0x00FFFFFF;

        read_file.read(buffer, 2);
        trigram_t x = (trigram_t(uchar(buffer[0])) << 8) | (trigram_t(uchar(buffer[1])));
        trigrams_set.insert(x);
        while (!read_file.atEnd())
        {
            const auto len = read_file.read(buffer, bufferSize);
            for (int i = 2; i < len; ++i)
            {
                x = (((x << 8) & mask) | trigram_t(uchar(buffer[i])));
                trigrams_set.insert(x);
            }
            if (trigrams_set.size() > evilNumber)
            {
                read_file.close();
                return false;
            }
        }
        read_file.close();
    }

    container_.reserve(trigrams_set.size());
    for (auto& tr : trigrams_set)
    {
        container_.push_back(tr);
    }
    return true;
}

void IndexedFile::interruptIndexing()
{

}

QString IndexedFile::getFullPath() const
{
    return QDir::cleanPath(path_ + QDir::separator() + name_);
}

}
