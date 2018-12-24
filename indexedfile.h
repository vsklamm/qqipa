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

    bool checkForMagicNumbers();

    void indexingFinished();

private:
    void clearData();

public:
    // std::atomic<bool> was_canceled;

    QString name_;
    QString path_;
    fsize_t size_;

    TrigramContainer container_;

private:
    static const int minsize = 1;
};

}

#endif // INDEXEDFILE_H
