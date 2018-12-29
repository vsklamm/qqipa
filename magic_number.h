#ifndef MAGIC_NUMBER_H
#define MAGIC_NUMBER_H

#include <array>
#include <vector>
#include <string>

namespace magic_numbers {

struct magic_number
{
public:
    magic_number(std::string name_, std::vector<char> number_);

    std::string name;
    std::vector<char> number;

    bool find(const char * buffer);
};

constexpr size_t max_magic_bytes = 12;

static std::vector<magic_number> mnumbers = {
    { "elf",        { '\x7F', '\x45', '\x4C', '\x46' }},
    { "rpm",        { '\xED', '\xAB', '\xEE', '\xDB' }},
    { "mp3_idtv2",  { '\x49', '\x44', '\x33' }},
    { "doc_xls_",   { '\xD0', '\xCF', '\x11', '\xE0', '\xA1', '\xB1', '\x1A', '\xE1' }},
    { "pdf",        { '\x25', '\x50', '\x44', '\x46', '\x2d' }},
    { "jpg_1",      { '\xFF', '\xD8', '\xFF', '\xDB' }},
    { "jpg_2",      { '\xFF', '\xD8', '\xFF', '\xEE' }},
    { "jpg_3",      { '\xFF', '\xD8', '\xFF', '\xE0', '\x00', '\x10', '\x4A', '\x46', '\x49', '\x46', '\x00', '\x01' }},
    { "png",        { '\x89', '\x50', '\x4E', '\x47', '\x0D', '\x0A', '\x1A', '\x0A' }},
    { "bmp",        { '\x42', '\x4D' }},
    { "mkv",        { '\x1A', '\x45', '\xDF', '\xA3' }},
    { "gif_1",      { '\x47', '\x49', '\x46', '\x38', '\x37', '\x61' }},
    { "gif_2",      { '\x47', '\x49', '\x46', '\x38', '\x39', '\x61' }},
    { "gzip",       { '\x1F', '\x8B' }},
    { "bz2",        { '\x42', '\x5A', '\x68' }},
    { "zip_jar_1",  { '\x50', '\x4B', '\x03', '\x04' }},
    { "zip_jar_2",  { '\x50', '\x4B', '\x05', '\x06' }},
    { "zip_jar_3",  { '\x50', '\x4B', '\x07', '\x08' }},
    { "tar_lzw",    { '\x1F', '\x9D' }},
    { "tar_lzh",    { '\x1F', '\xA0' }},
    { "tar_lzip",   { '\x4C', '\x5A', '\x49', '\x50' }},
    { "7z",         { '\x37', '\x7A', '\xBC', '\xAF', '\x27', '\x1C' }},
    { "rar_1",      { '\x52', '\x61', '\x72', '\x21', '\x1A', '\x07', '\x00' }},
    { "rar_2",      { '\x52', '\x61', '\x72', '\x21', '\x1A', '\x07', '\x01', '\x00' }},
    { "class",      { '\xCA', '\xFE', '\xBA', '\xBE' }},
    { "flac",       { '\x66', '\x4C', '\x61', '\x43' }},
    { "psd",        { '\x38', '\x42', '\x50', '\x53' }},
    { "lz",         { '\x4C', '\x5A', '\x49', '\x50' }},
    { "exe",        { '\x4D', '\x5A' }},
    { "maco_32",    { '\xFE', '\xED', '\xFA', '\xCE' }},
    { "maco_64",    { '\xFE', '\xED', '\xFA', '\xCF' }}
};

}

#endif // MAGIC_NUMBER_H
