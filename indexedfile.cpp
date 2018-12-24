#include "indexedfile.h"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QThread>
#include <QtConcurrent/QtConcurrentMap>
#include <QList>

#include <algorithm>

namespace qqipa {

using namespace magic_numbers; // TODO: Is it bad?

IndexedFile::IndexedFile(QString file_name, QString file_path, fsize_t size)
    : /* was_canceled(false), */name_(file_name), path_(file_path), size_(size)
{

}

IndexedFile::IndexedFile(QFileInfo file_info)
    :  /*was_canceled(false),*/ name_(file_info.fileName()), path_(file_info.absolutePath()), size_(file_info.size())
{

}

IndexedFile::~IndexedFile()
{

}

void IndexedFile::calculateIndex()
{
    qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();

    char buffer[buffer_size];
    std::unordered_set<trigram_t> temp;

    QFile read_file(appendPath(path_, name_));
    QTextStream fileStream(&read_file);
    if (read_file.open(QIODevice::ReadOnly | QIODevice::Text)) {

        if (read_file.read(buffer, max_magic_bytes) == max_magic_bytes)
        {
            magic_t file_magic;
            std::copy(file_magic.bytes, file_magic.bytes + max_magic_bytes, buffer);
            if (!checkForMagicNumbers(file_magic)) {
                this->is_binary_ = true;
                return;
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
                this->is_binary_ = true;
                break;
            }
        }
        read_file.close();
    }

    if (!this->is_binary_) {
        container_.trigrams_.reserve(temp.size());
        for (auto& tr : temp)
        {
            container_.trigrams_.push_back(tr);
        }
    }
    qDebug() << QString("%1 %2 ").arg(name_).arg(container_.trigrams_.size()) << QThread::currentThreadId();
}

void IndexedFile::interruptIndexing()
{

}

bool IndexedFile::checkForMagicNumbers(magic_t &file_magic)
{
    return (!(file_magic.number == magic_rpm.number
            || file_magic.number == magic_class.number
            || file_magic.number == magic_mp3_idtv1.number
            || file_magic.number == magic_mp3_idtv2.number
            || file_magic.number == magic_flac.number
            || file_magic.number == magic_elf.number
            || file_magic.number == magic_mkv.number));
}

void IndexedFile::clearData()
{

}

QString IndexedFile::appendPath(QString path1, QString path2)
{
    return QDir::cleanPath(path1 + QDir::separator() + path2);
}

}
