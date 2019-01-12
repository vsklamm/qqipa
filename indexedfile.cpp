#include "indexedfile.h"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QThread>
#include <QtConcurrent/QtConcurrentMap>
#include <QList>

#include <algorithm>
#include <bitset>

#include "absl/container/flat_hash_set.h"
#include <unordered_set>

namespace qqipa {

using trigrams_t = absl::flat_hash_set<trigram_t>; // std::unordered_set<trigram_t>; //

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
    return (size_ < small_large_size) ? calculateSmallFile() : calculateLargeFile();
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
    char buffer[buffer_size];
    trigrams_t temp;

    QFile read_file(getFullPath());
    if (read_file.open(QIODevice::ReadOnly | QIODevice::Text)) {

        const trigram_t mask = 0x00FFFFFF;

        read_file.read(buffer, 2);
        trigram_t x = (trigram_t(uchar(buffer[0])) << 8) | (trigram_t(uchar(buffer[1])));
        temp.insert(x);
        while (!read_file.atEnd())
        {
            const auto len = read_file.read(buffer, buffer_size);
            for (int i = 2; i < len; ++i)
            {
                x = (((x << 8) & mask) | trigram_t(uchar(buffer[i])));
                temp.insert(x);
            }
            if (temp.size() > evil_number)
            {
                read_file.close();
                return false;
            }
        }
        read_file.close();
    }

    container_.reserve(temp.size());
    for (auto& tr : temp)
    {
        container_.push_back(tr);
    }
    return true;
}

void IndexedFile::interruptIndexing()
{

}

void IndexedFile::clearData()
{

}

QString IndexedFile::getFullPath() const
{
    return QDir::cleanPath(path_ + QDir::separator() + name_);
}

}
