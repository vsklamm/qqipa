#ifndef INDEXEDFILE_H
#define INDEXEDFILE_H

#include "trigramcontainer.h"

#include <QFileInfo>

#include <unordered_set>

namespace qqipa {

using fsize_t = int64_t;

constexpr size_t buffer_size = 1 << 14;
constexpr size_t evil_number = 200000;

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
