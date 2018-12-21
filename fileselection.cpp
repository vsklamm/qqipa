#include "fileselection.h"
#include "indexedfile.h"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QThread>
#include <QtConcurrent/QtConcurrentRun>
#include <QtConcurrent/QtConcurrentMap>

FileSelection::FileSelection()
    : was_canceled(false)
{
    connect(&files_to_search_watcher, SIGNAL(finished()), this, SLOT(indexFoundFiles()));
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

    int found_files = 0;
    try
    {
        std::vector<QString> result{};
        for (auto& current_path : start_dirs)
        {
            if (was_canceled)
            {
                emit indexingFinished(found_files);
                return {};
            }

            QDir current_dir(current_path);

            QDirIterator it(current_dir.path(), QDir::Readable | QDir::Hidden | QDir::Files | QDir::NoDotAndDotDot, // TODO: tests for rights
                            recursively ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags);


            while (it.hasNext())
            {
                if (was_canceled)
                {
                    emit indexingFinished(found_files);
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
        emit indexingFinished(found_files);
        return {}; // TODO: or not empty
    }
}

void FileSelection::indexFoundFiles()
{
    for (auto file : files_to_search_future.result()) {
        IndexedFile indexedFile((QFileInfo(file)));
        indexedFile.calculateIndex();

        qDebug() << indexedFile.name_;
        for (auto tr : indexedFile.trigrams_) {
            qDebug() << tr;
        }
    }
}

void FileSelection::interruptProcessing()
{

}

void FileSelection::clearData()
{
    was_canceled = false;
    // TODO: what about Future?
}
