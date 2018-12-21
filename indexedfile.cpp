#include "indexedfile.h"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QThread>

IndexedFile::IndexedFile()
    : name_{}, path_{}, size_(0)
{

}

IndexedFile::IndexedFile(const QString &file_name, const QString &file_path, fsize_t size)
    : name_(file_name), path_(file_path), size_(size)
{

}

IndexedFile::IndexedFile(const QFileInfo &file_info)
    : name_(file_info.fileName()), path_(file_info.absolutePath()), size_(file_info.size())
{

}

IndexedFile::~IndexedFile()
{

}

std::unordered_set<trigram_t> IndexedFile::calculateIndex()
{
    qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();

    char buffer[buffer_size];

    QFile read_file(appendPath(path_, name_));
    if (read_file.open(QIODevice::ReadOnly)) {
        trigram_t last_bytes = 0;

        fsize_t read = 0;
        while (read < this->size_)
        {
            fsize_t len = read_file.read(buffer, sizeof(buffer));
            read += len;
            if (len == 0)
                break;

            if (read != len) // TODO: KLUDGE !!!
            {
                auto shift8 = last_bytes | trigram_t(buffer[0]);
                trigrams_.emplace(shift8);
                if (len > 1) {
                    trigrams_.insert(trigram_t(0x00FFFFFF) | (shift8 << 8) | trigram_t(buffer[1]));
                }
            }
            for (auto i = 0; i < len - 2; ++i) {
                trigrams_.insert((trigram_t(buffer[i]) << 16) | (trigram_t(buffer[i + 1]) << 8) | trigram_t(buffer[i + 2]));
            }
            if (len > 2) {
                last_bytes |= (trigram_t(buffer[len - 2]) << 16) | (trigram_t(buffer[len - 1]) << 8);
            }
        }
        read_file.close();
    }
    return trigrams_;
}

void IndexedFile::interruptIndexing()
{

}

void IndexedFile::clearData()
{

}

QString IndexedFile::appendPath(const QString &path1, const QString &path2)
{
    return QDir::cleanPath(path1 + QDir::separator() + path2);
}
