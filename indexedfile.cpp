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

using namespace magic_numbers; // TODO: Is it bad?

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

bool IndexedFile::containsMagicNumber(magic_t &file_magic)
{
    return std::find_if(mnumbers.begin(), mnumbers.end(), [=](auto& magic_n) {
        return file_magic.number == magic_n.second.number;
    }) != mnumbers.end();
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
        magic_t file_magic;
        std::copy(file_ptr, file_ptr + std::min(size_t(size_), max_magic_bytes), file_magic.bytes);
        if (containsMagicNumber(file_magic)) {
            return false;
        }

        trigram_t mask = 0x00FFFFFF;
        trigram_t x = (trigram_t(0[file_ptr]) << 8) | (trigram_t(1[file_ptr]));
        for (fsize_t i = 2; i < size_; ++i)
        {
            x = (((x << 8) & mask) | trigram_t(file_ptr[i]));
            temp.insert(x);
        }
    }
    container_.reserve(temp.size());
    for (auto& tr : temp)
    {
        container_.push_back(tr);
    }
    return true;
}

bool IndexedFile::calculateLargeFile()
{
    char buffer[buffer_size];
    std::unordered_set<trigram_t> temp;

    QFile read_file(getFullPath());
    if (read_file.open(QIODevice::ReadOnly | QIODevice::Text)) {

        if (read_file.read(buffer, max_magic_bytes) == max_magic_bytes)
        {
            magic_t file_magic;
            std::copy(buffer, buffer + max_magic_bytes, file_magic.bytes);
            if (containsMagicNumber(file_magic)) {
                return false;
            }
        }

        temp.reserve(size_t(0.35 * double(read_file.size()))); // TODO: ???? ???? ????

        trigram_t mask = 0x00FFFFFF;
        trigram_t x = (trigram_t(buffer[0]) << 16) | (trigram_t(buffer[1]) << 8) | (trigram_t(buffer[2]));
        temp.insert(x);
        x = (((x << 8) & mask) | trigram_t(buffer[3])); // TODO: what if only 3 chars
        temp.insert(x);

        while (!read_file.atEnd())
        {
            auto len = read_file.read(buffer, buffer_size);
            for (int i = 0; i < len; ++i)
            {
                x = (((x << 8) & mask) | trigram_t(buffer[i]));
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
