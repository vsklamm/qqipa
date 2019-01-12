#ifndef FILESELECTION_H
#define FILESELECTION_H

#include "directorywrapper.h"
#include "indexedfile.h"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFuture>
#include <QThread>
#include <QFutureWatcher>
#include <QList>

#include <set>
#include <stack>

namespace qqipa {

struct Controller : QObject
{
    Q_OBJECT
public:
    explicit Controller();
    ~Controller() = default;

signals:
    void newIndexedFiles(int indexedFiles, std::vector<std::pair<fsize_t, QString>> foundFiles);
    void dirIsReadyToIndexing(QString dirName, fsize_t filesToIndex);
    void indexingFinished(fsize_t indexedFiles);
    void searchingFinished(fsize_t foundFiles);

public slots:
    void startIndexing(std::vector<QString> startDirs);
    void startSearching(const QString &pattern);

    void interruptIndexing();
    void interruptSearching();

    void on_deleteDirectory(QString dirName);

    void passingIndexedFiles(int filesCompleted, std::vector<std::pair<fsize_t, QString>> indexedFiles);
    void onDirectoryIndexingFinished(fsize_t indexedDirFiles);

private slots:
    void onSearchingFinished(fsize_t foundFiles);

public:
    // methods

private:
    void clearData();

public:
    std::atomic<bool> wasCanceled;

    std::atomic<int> processingDirectories;
    fsize_t indexedFilesAll = 0;
    QList<DirectoryWrapper *> indexedDirectories; // TODO: is it normal?

};

}

#endif // FILESELECTION_H
