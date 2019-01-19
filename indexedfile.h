#ifndef INDEXEDFILE_H
#define INDEXEDFILE_H

#include "trigramcontainer.h"

#include <QFileInfo>

#include <regex>

namespace qqipa {

using fsize_t = std::int64_t;

constexpr size_t bufferSize = 1 << 14;
constexpr size_t evilNumber = 200000;

struct IndexedFile
{
public:
    IndexedFile() = default;
    IndexedFile(const QFileInfo &qFileInfo);
    ~IndexedFile() = default;

public:
    bool calculateIndex();
    fsize_t searchPattern(fsize_t rawPatternSize, TrigramContainer &patternTrigrams, const std::regex &patternRegex);

    QString getFullPath() const;

private:
    inline bool calculateSmallFile();
    inline bool calculateLargeFile();

public:
    QString name_;
    QString path_;
    fsize_t size_;

    TrigramContainer container_;

private:
    static const int minSize = 1;
    static const int smallLargeSize = 64 * 1024;
};

}

#endif // INDEXEDFILE_H
