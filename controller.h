#ifndef FILESELECTION_H
#define FILESELECTION_H

#include "directorywrapper.h"
#include "indexedfile.h"
#include "patternutil.h"

#include <QFileSystemWatcher>
#include <QThread>

#include <set>

namespace qqipa {

struct Controller : QObject
{
    Q_OBJECT
public:
    explicit Controller();
    ~Controller();

signals:
    void startDirSearch(PatternUtil patternUtil);
    void temporaryStartIndexing();
    void dirPreprocessStarted(size_t iDir);
    void dirIsReadyToIndexing(size_t iDir, fsize_t filesToIndex);

    void newIndexedFiles(size_t iDir, fsize_t filesDirCompleted, std::vector<std::pair<fsize_t, QString> > indexedFiles);
    void newFoundFile(fsize_t filesCompleted, std::pair<fsize_t, QString> foundFile);

    void indexingFinished(fsize_t indexedFiles);
    void searchingFinished(fsize_t foundFiles);

public slots:
    void startIndexing(std::vector<QString> &startDirs);
    void startSearching(const QString &pattern);

    void interruptIndexing();
    void interruptSearching();

    void on_deleteDirectory(size_t iDir);

    void passingIndexedFiles(fsize_t filesDirCompleted, std::vector<std::pair<fsize_t, QString> > indexedFiles);
    void passingFoundFile(fsize_t filesCompleted, std::pair<fsize_t, QString> foundFile);

    void on_dirBeforePreprocessing();
    void on_dirBeforeSearching();
    void on_dirIsReadyToScan(fsize_t upcomingFiles);
    void on_directoryIndexingFinished(fsize_t indexedDirFiles);
    void on_directorySearchingFinished(fsize_t foundFiles);

    void on_dirModified(const QString &path);

private:
    void temporarySignalIndexing(DirectoryWrapper * availableFolder);

public:
    std::atomic<int> processingDirectories;
    std::atomic<int> processingSDirectories;
    fsize_t indexedFilesAll = 0;
    fsize_t foundFilesAll = 0;
    std::vector<DirectoryWrapper *> directoryWrappers; // TODO: is it normal?
    std::set<std::unique_ptr<QThread>> threads;
    std::unique_ptr<QFileSystemWatcher> fileSystemWatcher;
    std::set<QString> quWatcher;

};

}

#endif // FILESELECTION_H
