#include "indexedfile.h"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QThread>
#include <QtConcurrent/QtConcurrentMap>
#include <QList>

namespace qqipa {

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
    temp.reserve(1000);

    QFile read_file(appendPath(path_, name_));
    if (read_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        trigram_t last_bytes = 0;

        // TODO: check for binary files in the first bytes
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
                temp.emplace(shift8);
                if (len > 1) {
                    temp.insert(trigram_t(0x00FFFFFF) | (shift8 << 8) | trigram_t(buffer[1]));
                }
            }
            for (auto i = 0; i < len - 2; ++i) {
                temp.insert((trigram_t(buffer[i]) << 16) | (trigram_t(buffer[i + 1]) << 8) | trigram_t(buffer[i + 2]));
            }
            if (len > 2) {
                last_bytes |= (trigram_t(buffer[len - 2]) << 16) | (trigram_t(buffer[len - 1]) << 8);
            }
        }
        read_file.close();
    }

    container_.trigrams_.reserve(temp.size());
    for (auto& tr : temp) {
        container_.trigrams_.push_back(tr);
    }
    qDebug() << QString("%1 %2 ").arg(name_).arg(container_.trigrams_.size()) << QThread::currentThreadId();
}

void IndexedFile::interruptIndexing()
{

}

bool IndexedFile::checkForMagicNumbers()
{
    union magic_t {
        uint8_t bytes[4];
        uint32_t number;
    };

    magic_t magic_rpm = {{ 0xED, 0xAB, 0xEE, 0xDB }};
    magic_t magic_class = {{ 0xCA, 0xFE, 0xBA, 0xBE }};
    magic_t magic_mp3_idtv1 = {{ 0xFF, 0xFB }};
    magic_t magic_mp3_idtv2 = {{ 0x49, 0x44, 0x33 }};
    magic_t magic_flac = {{ 0x66, 0x4C, 0x61, 0x43 }};
    magic_t magic_mkv = {{ 0x1A, 0x45, 0xDF, 0xA3 }};
    magic_t magic_elf = {{ 0x7F, 0x45, 0x4C, 0x46 }};

    magic_t file_magic;
    //    input.read((char *) file_magic.bytes, sizeof(file_magic));
    if (file_magic.number == magic_rpm.number
            || file_magic.number == magic_class.number
            || file_magic.number == magic_mp3_idtv1.number
            || file_magic.number == magic_mp3_idtv2.number
            || file_magic.number == magic_flac.number
            || file_magic.number == magic_elf.number
            || file_magic.number == magic_mkv.number) {

    }

    return true;
}

void IndexedFile::clearData()
{

}

QString IndexedFile::appendPath(QString path1, QString path2)
{
    return QDir::cleanPath(path1 + QDir::separator() + path2);
}

}
