#ifndef INDEXEDFILE_H
#define INDEXEDFILE_H

#include "trigramcontainer.h"

#include <QFileInfo>

namespace qqipa {

using fsize_t = std::int64_t;

constexpr size_t buffer_size = 1 << 14;
constexpr size_t evil_number = 200000;

struct IndexedFile
{
public:
    IndexedFile() = default;
    IndexedFile(const QString &fileName, const QString &filePath, const fsize_t &size);
    IndexedFile(const QFileInfo &qFileInfo);
    ~IndexedFile() = default;

public:
    bool calculateIndex();
    QString getFullPath() const;
    void interruptIndexing();

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
