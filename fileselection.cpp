#include "fileselection.h"
#include "indexedfile.h"

#include <bitset>
#include <string>

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QThread>
#include <QtConcurrent/QtConcurrentRun>
#include <QtConcurrent/QtConcurrentMap>

namespace qqipa {

FileSelection::FileSelection()
    : was_canceled(false)
{
    connect(&files_to_search_watcher, SIGNAL(finished()), this, SLOT(indexFoundFiles()));
    connect(&indexer_watcher, SIGNAL(finished()), this, SLOT(onIndexingFinished()));
}

FileSelection::~FileSelection()
{

}

void FileSelection::startSearch(std::set<QString> &start_dirs, bool recursively)
{
    qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();

    files_to_search_future = QtConcurrent::run(this, &FileSelection::processDirectories, start_dirs, recursively);
    files_to_search_watcher.setFuture(files_to_search_future);
}

std::vector<QString> FileSelection::processDirectories(std::set<QString> &start_dirs, bool recursively)
{
    qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();
    clearData();

    std::vector<QString> result{};
    try
    {
        for (auto& current_path : start_dirs)
        {
            if (was_canceled)
            {
                emit indexingFinished(result.size());
                return {};
            }

            QDirIterator it(QDir(current_path).path(), QDir::Readable | QDir::Hidden | QDir::Files | QDir::NoDotAndDotDot, // TODO: tests for rights
                            recursively ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags);

            while (it.hasNext())
            {
                if (was_canceled)
                {
                    emit indexingFinished(result.size());
                    return result;
                }

                auto file = it.next();
                const auto file_info = it.fileInfo();
                if (!file_info.isSymLink()) // TODO: what about them?
                {
                    if (QFile(file).open(QIODevice::ReadOnly)) { // TODO: too sloooow work with strings and files
                        auto size = file_info.size();
                        if (size < minsize)
                            continue;
                        result.push_back(file);
                    }
                }
            }
        }
        return result;
    }
    catch (...)
    {
        emit indexingFinished(result.size());
        return {}; // TODO: or not empty
    }
}

void FileSelection::indexFoundFiles()
{
    indexer_future = QtConcurrent::run(this, &FileSelection::indexPortion);
    indexer_watcher.setFuture(indexer_future);
}

void FileSelection::indexPortion()
{
    for (auto& path : files_to_search_future.result()) {
        auto file = IndexedFile(QFileInfo(path));
        file.calculateIndex();
    }
}

void FileSelection::interruptProcessing()
{

}

void FileSelection::onIndexingFinished()
{
    qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();
    emit indexingFinished(0);
}

void FileSelection::clearData()
{
    was_canceled = false;
    // TODO: what about Future?
}

}
