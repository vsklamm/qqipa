#ifndef FILESELECTION_H
#define FILESELECTION_H

#include "indexedfile.h"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFuture>
#include <QThread>
#include <QFutureWatcher>
#include <QList>

#include <set>

namespace qqipa {

struct SearcherUtil : QObject
{
    Q_OBJECT
public:
    explicit SearcherUtil();
    ~SearcherUtil();

signals:
    void indexingFinished(fsize_t found_files);
    void searchingFinished(fsize_t found_files);
    void newIndexedFiles(int files_completed, std::vector<QString> found_files);

public slots:
    void startIndexing(std::set<QString> &start_dirs, bool recursively = true);
    void startSearching(QString pattern);

    void interruptIndexing();
    void interruptSearching();

    void onIndexingFinished();
    void onSearchingFinished(fsize_t found_files);

private slots:
    std::vector<QString> processDirectories(std::set<QString> &start_dirs, bool recursively = true);
    void indexFoundFiles();
    void indexPortion();

public:
    // methods

private:
    void clearData();

public:
    std::atomic<bool> was_canceled;

    QList<IndexedFile> indexedFileList;

    QFuture<std::vector<QString>> files_to_search_future;
    QFutureWatcher<std::vector<QString>> files_to_search_watcher;

    QFuture<void> indexer_future;
    QFutureWatcher<void> indexer_watcher;

private:
    int minsize= 1;

};

}

#endif // FILESELECTION_H
