#pragma once

#include <QFileInfo>

#include <set>
#include <unordered_set>
#include <atomic>
#include <indexedfile.h>

using fsize_t = int64_t;
using trigram_t = uint_fast32_t;

constexpr size_t buffer_size = 8192;

struct IndexedFile
{
public:
    explicit IndexedFile();
    explicit IndexedFile(const QString &file_name, const QString &file_path, fsize_t size);
    explicit IndexedFile(const QFileInfo &qFile);
    ~IndexedFile();

public:
    std::unordered_set<trigram_t> calculateIndex();
    QString appendPath(const QString &path1, const QString &path2);
    void interruptIndexing();

    void indexingFinished();

private:
    void clearData();

public:
    std::atomic<bool> was_canceled;

    QString name_;
    QString path_;
    fsize_t size_;
    std::unordered_set<trigram_t> trigrams_;

private:
    const int minsize = 1;
};
