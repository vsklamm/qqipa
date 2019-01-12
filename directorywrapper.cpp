#include "directorywrapper.h"
#include "magic_number.h"

#include <QtConcurrent/QtConcurrentRun>
#include <QtConcurrent/QtConcurrentMap>

#include <QDirIterator>

namespace qqipa {

DirectoryWrapper::DirectoryWrapper(const QString &directoryName)
    : name(directoryName)
{
    // connect(&indexerWatcher, SIGNAL(finished()), this, SLOT(onIndexingFinished()));
}

void DirectoryWrapper::startIndexing()
{
    const auto res = impoverishDirectory();
    // set progress
    indexFoundFiles(res);
//    indexerFuture = QtConcurrent::run(this, &DirectoryWrapper::indexFoundFiles, res);
//    indexerWatcher.setFuture(indexerFuture);
    emit indexingFinished(indexedFileList.size());
}

std::vector<QString> DirectoryWrapper::impoverishDirectory()
{
    qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();

    using namespace magic_numbers;
    std::vector<QString> result{};
    char buffer[maxMagicBytes];
    try
    {
        QDirIterator it(name, QDir::NoDotAndDotDot | QDir::Files | QDir::Readable | QDir::Hidden, QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            if (QThread::currentThread()->isInterruptionRequested())
            {
                emit indexingFinished(int(result.size()));
                return result;
            }
            auto file = it.next();
            if (it.fileInfo().isSymLink() || !it.fileInfo().size())
                continue;
            QFile readFile(file);
            if (readFile.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                if (!(readFile.read(buffer, maxMagicBytes) == maxMagicBytes
                      && std::find_if(mnumbers.begin(), mnumbers.end(),
                                      [=](magic_number& magic_n) {
                                      return magic_n.find(buffer); }) != mnumbers.end())) {
                    result.emplace_back(file);
                }
            }
        }
        return result;
    }
    catch (...)
    {
        emit indexingFinished(result.size());
        return {};
    }
}

void DirectoryWrapper::indexFoundFiles(const std::vector<QString> &filesToIdndex)
{
    for (auto& path : filesToIdndex)
    {
        auto file = IndexedFile(QFileInfo(path));
        auto notBinary = file.calculateIndex();
        if (notBinary)
        {
            indexedFileList.append(file);
            emit newIndexedFiles(indexedFileList.size(),
            { {file.container_.size(), file.getFullPath()} });
        }
    }
}

void DirectoryWrapper::interruptIndexing()
{

}

}
