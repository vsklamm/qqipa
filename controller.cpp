#include "controller.h"
#include "indexedfile.h"
#include "patternsearcher.h"
#include "directorywrapper.h"

#include <bitset>
#include <string>

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QThread>

namespace qqipa {

Controller::Controller()
    : wasCanceled(false), processingDirectories(0)
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
            QThread * newDirThread = new QThread();
            indexedDirectories.push_back(new DirectoryWrapper(currentPath));
            indexedDirectories.back()->moveToThread(newDirThread);

            connect(newDirThread, &QThread::started, indexedDirectories.back(), &DirectoryWrapper::startIndexing);
            connect(indexedDirectories.back(), &DirectoryWrapper::newIndexedFiles, this, &Controller::passingIndexedFiles);
            connect(indexedDirectories.back(), &DirectoryWrapper::indexingFinished, this, &Controller::onDirectoryIndexingFinished);

            newDirThread->start();
            ++processingDirectories;
        }
        return;
    }
    catch (...)
    {
        return;
    }
}

void Controller::startSearching(const QString &pattern)
{
    qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();

    PatternSearcher patternSeacher;
    patternSeacher.search(pattern, indexedDirectories);
    connect(&patternSeacher, &PatternSearcher::searchingFinished, this, &Controller::onSearchingFinished);
}

void Controller::interruptIndexing()
{
    wasCanceled = true;
}

void Controller::interruptSearching()
{
    wasCanceled = true;
}

void Controller::on_deleteDirectory(QString dirName)
{
    // TODO: WHAT IS SHIT WITH THIS CODE
    auto ptr = std::find_if(indexedDirectories.begin(), indexedDirectories.end(),
                            [&](DirectoryWrapper * indexedDir) { return indexedDir->name == dirName; });
    if (ptr != indexedDirectories.end()) {
        delete *ptr;
        indexedDirectories.erase(ptr);
    }
}

void Controller::passingIndexedFiles(int filesCompleted, std::vector<std::pair<fsize_t, QString>> foundFiles)
{
    emit newIndexedFiles(std::forward<int>(filesCompleted), std::forward<std::vector<std::pair<fsize_t, QString>>>(foundFiles));
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
    wasCanceled = false;
    processingDirectories = 0;
    // indexedDirectories.clear(); // TODO: delete pointers???
}

}
