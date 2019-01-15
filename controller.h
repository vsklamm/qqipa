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
    void dirIsReadyToIndexing(size_t iDir, fsize_t filesToIndex);
    void newIndexedFiles(size_t iDir, fsize_t filesDirCompleted, std::vector<std::pair<fsize_t, QString>> indexedFiles);

    void indexingFinished(fsize_t indexedFiles);
    void searchingFinished(fsize_t foundFiles);

public slots:
    void startIndexing(std::vector<QString> startDirs);
    void startSearching(const QString &pattern);

    void interruptIndexing();
    void interruptSearching();

    void on_deleteDirectory(QString dirName);

    void onDirIsReadyToScan(fsize_t upcomingFiles);
    void passingIndexedFiles(fsize_t filesDirCompleted, std::vector<std::pair<fsize_t, QString>> indexedFiles);
    void onDirectoryIndexingFinished(fsize_t indexedDirFiles);

private slots:
    void onSearchingFinished(fsize_t foundFiles);

public:
    // methods

private:
    inline void clearData();

public:
    std::atomic<fsize_t> processingDirectories;
    fsize_t indexedFilesAll = 0;
    std::vector<DirectoryWrapper *> directoryWrappers; // TODO: is it normal?

};

}

#endif // FILESELECTION_H
