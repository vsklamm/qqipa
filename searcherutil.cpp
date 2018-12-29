#include "searcherutil.h"
#include "indexedfile.h"
#include "patternsearcher.h"
#include "magic_number.h"

#include <bitset>
#include <string>

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QThread>
#include <QtConcurrent/QtConcurrentRun>
#include <QtConcurrent/QtConcurrentMap>

namespace qqipa {

SearcherUtil::SearcherUtil()
    : was_canceled(false)
{
    connect(&files_to_search_watcher, SIGNAL(finished()), this, SLOT(indexFoundFiles()));
    connect(&indexer_watcher, SIGNAL(finished()), this, SLOT(onIndexingFinished()));
}

SearcherUtil::~SearcherUtil()
{

}

void SearcherUtil::startIndexing(std::vector<QString> &start_dirs, bool recursively)
{
    qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();

    files_to_search_future = QtConcurrent::run(this, &SearcherUtil::processDirectories, start_dirs, recursively);
    files_to_search_watcher.setFuture(files_to_search_future);
}

void SearcherUtil::startSearching(QString pattern)
{
    qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();

    PatternSearcher patternSeacher;
    patternSeacher.search(pattern, indexedFileList);
    connect(&patternSeacher, &PatternSearcher::searchingFinished, this, &SearcherUtil::onSearchingFinished);
}

std::vector<QString> SearcherUtil::processDirectories(std::vector<QString> &start_dirs, bool recursively)
{
    qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();
    clearData();

    using namespace magic_numbers;
    std::vector<QString> result{};
    char buffer[12];
    try
    {
        for (auto& current_path : start_dirs)
        {
            if (was_canceled)
            {
                emit indexingFinished(int(result.size()));
                return {};
            }

            QDirIterator it(QDir(current_path).path(), QDir::Readable | QDir::Hidden | QDir::Files | QDir::NoDotAndDotDot, // TODO: tests for rights
                            recursively ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags);

            while (it.hasNext())
            {
                if (was_canceled)
                {
                    emit indexingFinished(int(result.size()));
                    return result;
                }

                auto file = it.next();
                const auto file_info = it.fileInfo();
                if (!file_info.isSymLink()) // TODO: what about them?
                {
                    QFile read_file(file);
                    if (read_file.open(QIODevice::ReadOnly)) { // TODO: too sloooow work with strings and files
                        auto size = file_info.size();
                        if (size < minsize)
                            continue;
                        if (!(read_file.read(buffer, max_magic_bytes) == max_magic_bytes
                                && std::find_if(mnumbers.begin(), mnumbers.end(),
                                                [=](magic_number& magic_n) {
                                                return magic_n.find(buffer); }) != mnumbers.end())) {
                            result.push_back(file);
                        }
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

void SearcherUtil::indexFoundFiles()
{
    indexer_future = QtConcurrent::run(this, &SearcherUtil::indexPortion);
    indexer_watcher.setFuture(indexer_future);
}

void SearcherUtil::indexPortion()
{
    for (auto& path : files_to_search_future.result())
    {
        auto file = IndexedFile(QFileInfo(path));
        auto not_binary = file.calculateIndex();
        if (not_binary)
        {
            indexedFileList.append(file);
            emit newIndexedFiles(indexedFileList.size(),
            { file.getFullPath() + " === " + QString::number(file.container_.size())});
        }
    }
}

bool SearcherUtil::containsMagicNumbers()
{

}

void SearcherUtil::interruptIndexing()
{
    was_canceled = true;
}

void SearcherUtil::interruptSearching()
{

}

void SearcherUtil::onIndexingFinished()
{
    qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();
    emit indexingFinished(indexedFileList.size());
}

void SearcherUtil::onSearchingFinished(fsize_t found_files)
{
    emit searchingFinished(found_files);
}

void SearcherUtil::clearData()
{
    was_canceled = false;
    indexedFileList.clear();
}

}
