#include "indexedfile.h"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QThread>
#include <QtConcurrent/QtConcurrentMap>
#include <QList>

#include <algorithm>
#include <bitset>

namespace qqipa {

IndexedFile::IndexedFile(QString file_name, QString file_path, fsize_t size)
    : name_(file_name), path_(file_path), size_(size)
{

}

IndexedFile::IndexedFile(QFileInfo file_info)
    : name_(file_info.fileName()), path_(file_info.absolutePath()), size_(file_info.size())
{

}

IndexedFile::~IndexedFile()
{

}

bool IndexedFile::calculateIndex()
{
    return (size_ < small_large_size) ? calculateSmallFile() : calculateLargeFile();
}

void IndexedFile::interruptIndexing()
{

}

bool IndexedFile::calculateSmallFile()
{
    QFile read_file(getFullPath());
    if (!read_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    auto file_ptr = read_file.map(0, size_);
    read_file.close();

    std::unordered_set<trigram_t> temp;
    if (file_ptr != nullptr)
    {
        trigram_t mask = 0x00FFFFFF;
        trigram_t x = (trigram_t(0[file_ptr]) << 8) | (trigram_t(1[file_ptr]));
        for (fsize_t i = 2; i < size_; ++i)
        {
            x = (((x << 8) & mask) | trigram_t(file_ptr[i]));
            temp.insert(x);
        }
    }
    container_.resize(temp.size());
    std::copy(temp.begin(), temp.end(), container_.begin());
    return true;
}

bool IndexedFile::calculateLargeFile()
{
    char buffer[buffer_size];
    std::unordered_set<trigram_t> temp;

    QFile read_file(getFullPath());
    if (read_file.open(QIODevice::ReadOnly | QIODevice::Text)) {

        trigram_t mask = 0x00FFFFFF;
        temp.reserve(size_t(0.35 * double(read_file.size())));

        read_file.read(buffer, 2);
        trigram_t x = (trigram_t(uchar(buffer[0])) << 8) | (trigram_t(uchar(buffer[1])));
        temp.insert(x);
        while (!read_file.atEnd())
        {
            auto len = read_file.read(buffer, buffer_size);
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

void IndexedFile::clearData()
{

}

QString IndexedFile::getFullPath() const
{
    return QDir::cleanPath(path_ + QDir::separator() + name_);
}

}
