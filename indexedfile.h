#ifndef INDEXEDFILE_H
#define INDEXEDFILE_H

#include "trigramcontainer.h"

#include <QFileInfo>

#include <unordered_set>

namespace qqipa {

using fsize_t = int64_t;

constexpr size_t buffer_size = 8192;
constexpr size_t evil_number = 200000;

namespace magic_numbers {

union magic_t {
    uint8_t bytes[4];
    uint32_t number;
};

constexpr size_t max_magic_bytes = 4;

const std::vector<std::pair<std::string, magic_t>> mnumbers =
{{
     { "magic_rpm", {{ 0xED, 0xAB, 0xEE, 0xDB }}},
     { "magic_class",{{ 0xCA, 0xFE, 0xBA, 0xBE }}},
     { "magic_mp3_idtv2", {{ 0x49, 0x44, 0x33 }}},
     { "magic_flac", {{ 0x66, 0x4C, 0x61, 0x43 }}},
     { "magic_mkv", {{ 0x1A, 0x45, 0xDF, 0xA3 }}},
     { "magic_elf", {{ 0x7F, 0x45, 0x4C, 0x46 }}},
     { "magic_tar_lzw", {{ 0x1F, 0x9D }}},
     { "magic_tar_lzh", {{ 0x1F, 0xA0 }}},
     { "magic_tar_lzip", {{ 0x4C, 0x5A, 0x49, 0x50 }}},
     { "magic_gzip", {{ 0x1F, 0x8B }}},
     { "magic_maco_32", {{ 0xFE, 0xED, 0xFA, 0xCE }}},
     { "magic_maco_64", {{ 0xFE, 0xED, 0xFA, 0xCF }}},
     { "magic_bmp", {{ 0x42, 0x4D }}},
     { "magic_bz2", {{ 0x42, 0x5A, 0x68 }}},
     { "magic_exe", {{ 0x4D, 0x5A }}},
     { "magic_jpg_1", {{ 0xFF, 0xD8, 0xFF, 0xDB }}},
     { "magic_jpg_2", {{ 0xFF, 0xD8, 0xFF, 0xEE }}}
 }};

}

struct IndexedFile
{
public:
    IndexedFile() = default;
    IndexedFile(QString file_name, QString file_path, fsize_t size);
    IndexedFile(QFileInfo qFile);
    ~IndexedFile();

public:
    bool calculateIndex();
    QString getFullPath() const;
    void interruptIndexing();

    bool containsMagicNumber(magic_numbers::magic_t &file_magic);

    void indexingFinished();

private:
    bool calculateSmallFile();
    bool calculateLargeFile();
    void clearData();

public:
    QString name_;
    QString path_;
    fsize_t size_;

    TrigramContainer container_;

private:
    static const int minsize = 1;
    static const int small_large_size = 64 * 1024;
};

}

#endif // INDEXEDFILE_H
