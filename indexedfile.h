#ifndef INDEXEDFILE_H
#define INDEXEDFILE_H

#include "trigramcontainer.h"

#include <QFileInfo>

#include <set>
#include <unordered_set>
#include <atomic>

namespace qqipa {

using fsize_t = int64_t;

constexpr size_t buffer_size = 8192;
constexpr size_t evil_number = 200000;

namespace magic_numbers {

union magic_t {
    uint8_t bytes[4];
    uint32_t number;
};

constexpr magic_t magic_rpm = {{ 0xED, 0xAB, 0xEE, 0xDB }};
constexpr magic_t magic_class = {{ 0xCA, 0xFE, 0xBA, 0xBE }};
constexpr magic_t magic_mp3_idtv1 = {{ 0xFF, 0xFB }};
constexpr magic_t magic_mp3_idtv2 = {{ 0x49, 0x44, 0x33 }};
constexpr magic_t magic_flac = {{ 0x66, 0x4C, 0x61, 0x43 }};
constexpr magic_t magic_mkv = {{ 0x1A, 0x45, 0xDF, 0xA3 }};
constexpr magic_t magic_elf = {{ 0x7F, 0x45, 0x4C, 0x46 }};

constexpr size_t max_magic_bytes = 4;

}

struct IndexedFile
{
public:
    IndexedFile() = default;
    IndexedFile(QString file_name, QString file_path, fsize_t size);
    IndexedFile(QFileInfo qFile);
    ~IndexedFile();

public:
    void calculateIndex();
    QString appendPath(QString path1, QString path2);
    void interruptIndexing();

    bool checkForMagicNumbers(magic_numbers::magic_t &file_magic);

    void indexingFinished();

private:
    void clearData();

public:
    // std::atomic<bool> was_canceled;

    QString name_;
    QString path_;
    fsize_t size_;
    bool is_binary_ = false;

    TrigramContainer container_;

private:
    static const int minsize = 1;
};

}

#endif // INDEXEDFILE_H
