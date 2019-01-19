#include "controller.h"
#include "indexedfile.h"
#include "directorywrapper.h"

#include <string>

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QThread>
#include <QtConcurrent/QtConcurrentRun>

namespace qqipa {

Controller::Controller()
    : processingDirectories(0)
{
    //connect(&searchingWatcher, SIGNAL(finished()), this, SLOT(onSearchingFinished()));
}

Controller::~Controller()
{
    for (auto& x : directoryWrappers)
    {
        x->interruptIndexing();
    }
    for (auto ptr : threads) {
        ptr->quit();
        ptr->wait();
        delete ptr; // TODO: is this right way?
    }
}

void Controller::startIndexing(std::vector<QString> &startDirs)
{
    bool oneDirIndexed = false;
    for (auto& availableFolder : directoryWrappers)
    {
        if (!availableFolder->wasSuccessfullyIndexed)
        {
            auto handle = connect(this, &Controller::temporaryStartIndexing, availableFolder, &DirectoryWrapper::startIndexing);
            emit temporaryStartIndexing();
            ++processingDirectories;
            disconnect(this, &Controller::temporaryStartIndexing, nullptr, nullptr);
            oneDirIndexed = true;
        }
    }
    for (auto& currentPath : startDirs)
    {
        // TODO: WHAT IS SHIT WITH THIS CODE
        QThread *newDirThread = new QThread();
        threads.insert(newDirThread);

        directoryWrappers.emplace_back(new DirectoryWrapper(directoryWrappers.size(), currentPath));
        directoryWrappers.back()->moveToThread(newDirThread);

        connect(newDirThread, &QThread::started, directoryWrappers.back(), &DirectoryWrapper::startIndexing);
        connect(this, &Controller::startDirSearch, directoryWrappers.back(), &DirectoryWrapper::startSearching);
        connect(directoryWrappers.back(), &DirectoryWrapper::dirSearchingStarted, this, &Controller::on_dirBeforeSearching);
        connect(directoryWrappers.back(), &DirectoryWrapper::preprocessStarted, this, &Controller::on_dirBeforePreprocessing);
        connect(directoryWrappers.back(), &DirectoryWrapper::preprocessFinished, this, &Controller::on_dirIsReadyToScan);
        connect(directoryWrappers.back(), &DirectoryWrapper::newIndexedFiles, this, &Controller::passingIndexedFiles);
        connect(directoryWrappers.back(), &DirectoryWrapper::newFoundFile, this, &Controller::passingFoundFile);
        connect(directoryWrappers.back(), &DirectoryWrapper::indexingFinished, this, &Controller::on_directoryIndexingFinished);
        connect(directoryWrappers.back(), &DirectoryWrapper::searchingFinished, this, &Controller::on_directorySearchingFinished);

        newDirThread->start();
        ++processingDirectories;
        oneDirIndexed = true;
    }
    if (!oneDirIndexed)
    {
        emit indexingFinished(0);
    }
}

void Controller::startSearching(const QString &pattern)
{
    qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();

    foundFilesAll = 0;
    PatternUtil patternUtil(pattern);
    emit startDirSearch(patternUtil);
}

void Controller::interruptIndexing()
{
    for (auto& dir : directoryWrappers)
    {
        dir->interruptIndexing();
    }
}

void Controller::interruptSearching()
{
    for (auto& dir : directoryWrappers)
    {
        dir->interruptSearching();
    }
}

void Controller::on_deleteDirectory(size_t iDir)
{
    if (directoryWrappers.size() < iDir + 1)
        return;
    directoryWrappers[iDir]->interruptIndexing();
    // delete thread from 'threads' ???
    // TODO: memory leak or not?
    directoryWrappers.erase(directoryWrappers.begin() + int(iDir));
    for (size_t i = iDir; i < directoryWrappers.size(); ++i)
    {
        directoryWrappers[i]->id = i; // TODO: workaround?
    }
}

void Controller::on_dirBeforePreprocessing()
{
    emit dirPreprocessStarted(qobject_cast<DirectoryWrapper *>(QObject::sender())->id);
}

void Controller::on_dirBeforeSearching()
{
    ++processingSDirectories;
}

void Controller::on_dirIsReadyToScan(fsize_t upcomingFiles)
{
    emit dirIsReadyToIndexing(qobject_cast<DirectoryWrapper *>(QObject::sender())->id, upcomingFiles);
}

void Controller::passingIndexedFiles(fsize_t filesDirCompleted, std::vector<std::pair<fsize_t, QString> > indexedFiles)
{    
    emit newIndexedFiles(qobject_cast<DirectoryWrapper *>(QObject::sender())->id, filesDirCompleted, indexedFiles);
}

void Controller::passingFoundFile(fsize_t filesCompleted, std::pair<fsize_t, QString> foundFile)
{
    qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();
    emit newFoundFile(filesCompleted, foundFile);
}

void Controller::on_directoryIndexingFinished(fsize_t indexedDirFiles)
{
    indexedFilesAll += indexedDirFiles;
    if (!--processingDirectories)
    {
        qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();
        emit indexingFinished(indexedFilesAll);
    }
}

void Controller::on_directorySearchingFinished(fsize_t foundFiles)
{
    foundFilesAll += foundFiles;
    if (!--processingSDirectories)
    {
        qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();
        emit searchingFinished(foundFilesAll);
    }
}

}
