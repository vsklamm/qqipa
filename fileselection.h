#ifndef FILESELECTION_H
#define FILESELECTION_H

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFuture>
#include <QThread>
#include <QFutureWatcher>
#include <QList>

#include <set>

namespace qqipa {

struct FileSelection : QObject
{
    Q_OBJECT
public:
    explicit FileSelection();
    ~FileSelection();

signals:
    void indexingFinished(int found_files);
    void newIndexedFiles(int files_completed, std::vector<QString> found_files);

public slots:
    void startSearch(std::set<QString> &start_dirs, bool recursively = true);
    std::vector<QString> processDirectories(std::set<QString> &start_dirs, bool recursively = true);
    void indexFoundFiles();
    void indexPortion();
    void interruptProcessing();

    void onIndexingFinished();

public:
    // methods

private:
    void clearData();

public:
    std::atomic<bool> was_canceled;
    QFuture<std::vector<QString>> files_to_search_future;
    QFutureWatcher<std::vector<QString>> files_to_search_watcher;

    QFuture<void> indexer_future;
    QFutureWatcher<void> indexer_watcher;

private:
    int minsize= 1;

};

}

#endif // FILESELECTION_H
