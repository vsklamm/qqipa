#include "controller.h"
#include "indexedfile.h"
#include "patternsearcher.h"
#include "directorywrapper.h"

#include <string>

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QThread>

namespace qqipa {

Controller::Controller()
    : processingDirectories(0)
{

}

void Controller::startIndexing(std::vector<QString> startDirs)
{
    qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();
    try
    {
        for (auto& currentPath : startDirs)
        {
            // TODO: WHAT IS SHIT WITH THIS CODE
            QThread *newDirThread = new QThread();
            directoryWrappers.push_back(new DirectoryWrapper(directoryWrappers.size(), currentPath));
            directoryWrappers.back()->moveToThread(newDirThread);

            connect(newDirThread, &QThread::started, directoryWrappers.back(), &DirectoryWrapper::startIndexing);
            connect(directoryWrappers.back(), &DirectoryWrapper::preprocessFinished, this, &Controller::onDirIsReadyToScan);
            connect(directoryWrappers.back(), &DirectoryWrapper::newIndexedFiles, this, &Controller::passingIndexedFiles);
            connect(directoryWrappers.back(), &DirectoryWrapper::indexingFinished, this, &Controller::onDirectoryIndexingFinished);

            newDirThread->start();
            ++processingDirectories;
        }
    }
    catch (...)
    {
        // TODO: ignore ?
    }
}

void Controller::startSearching(const QString &pattern)
{
    qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();

    PatternSearcher patternSeacher;
    patternSeacher.search(pattern, directoryWrappers);
    connect(&patternSeacher, &PatternSearcher::searchingFinished, this, &Controller::onSearchingFinished);
}

void Controller::interruptIndexing()
{
    for (auto& dir : directoryWrappers) {
        dir->interruptIndexing();
    }
}

void Controller::interruptSearching()
{
}

void Controller::on_deleteDirectory(QString dirName)
{
    // TODO: WHAT IS SHIT WITH THIS CODE
    auto ptr = std::find_if(directoryWrappers.begin(), directoryWrappers.end(),
                            [&dirName](DirectoryWrapper * indexedDir) { return indexedDir->name == dirName; });
    if (ptr != directoryWrappers.end()) {
        delete *ptr;
        directoryWrappers.erase(ptr);
    }
}

void Controller::onDirIsReadyToScan(fsize_t upcomingFiles)
{
    emit dirIsReadyToIndexing(qobject_cast<DirectoryWrapper *>(QObject::sender())->id, upcomingFiles);
}

void Controller::passingIndexedFiles(fsize_t filesDirCompleted, std::vector<std::pair<fsize_t, QString>> indexedFiles)
{    
    emit newIndexedFiles(qobject_cast<DirectoryWrapper *>(QObject::sender())->id, filesDirCompleted, indexedFiles);
}

void Controller::onDirectoryIndexingFinished(fsize_t indexedDirFiles)
{
    indexedFilesAll += indexedDirFiles;
    if (!--processingDirectories)
    {
        qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();
        emit indexingFinished(indexedFilesAll);
    }
}

void Controller::onSearchingFinished(fsize_t found_files)
{
    emit searchingFinished(found_files);
}

void Controller::clearData()
{
    processingDirectories = 0;
    // indexedDirectories.clear(); // TODO: delete pointers???
}

}
