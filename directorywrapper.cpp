#include "directorywrapper.h"
#include "magic_number.h"

#include <QtConcurrent/QtConcurrentRun>
#include <QtConcurrent/QtConcurrentMap>
#include <QDirIterator>

namespace qqipa {

DirectoryWrapper::DirectoryWrapper(const size_t &id, const QString &directoryName)
    : wasCancelled(false), id(id), name(directoryName)
{

}

void DirectoryWrapper::startIndexing()
{
    clearData();
    const auto res = impoverishDirectory();
    emit preprocessFinished(fsize_t(res.size()));
    indexFoundFiles(res);
}

std::vector<QString> DirectoryWrapper::impoverishDirectory()
{
    qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();

    using namespace magic_numbers;
    std::vector<QString> result{};
    char buffer[maxMagicBytes];
    try
    {
        QDirIterator it(name, QDir::NoDotAndDotDot | QDir::Files | QDir::Readable | QDir::Hidden,
                        QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            if (wasCancelled)
            {
                emit indexingFinished(0);
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
                                      [&buffer](magic_number& magic_n) {
                                      return magic_n.find(buffer); }) != mnumbers.end())) {
                    result.emplace_back(file);
                }
            }
        }
        return result;
    }
    catch (...)
    {
        emit indexingFinished(0);
        return {};
    }
}

void DirectoryWrapper::indexFoundFiles(const std::vector<QString> &filesToIndex)
{
    const size_t queueSize = 4;
    fsize_t textIndexedFiles = 0;
    std::vector<std::pair<fsize_t, QString>> indexedFilesVector;
    indexedFilesVector.reserve(queueSize);
    for (size_t i = 0; i < filesToIndex.size(); ++i)
    {
        if (wasCancelled)
        {
            emit indexingFinished(textIndexedFiles);
            return;
        }
        auto file = IndexedFile(QFileInfo(filesToIndex[i]));
        auto notBinary = file.calculateIndex();
        if (notBinary)
        {
            indexedFileList.append(file);
            indexedFilesVector.emplace_back(file.container_.size(), file.getFullPath());
            ++textIndexedFiles;
            if (indexedFilesVector.size() > queueSize)
            {
                emit newIndexedFiles(fsize_t(i), indexedFilesVector);
                indexedFilesVector.clear();
            }
        }
    }
    emit newIndexedFiles(fsize_t(filesToIndex.size()), indexedFilesVector);
    emit indexingFinished(textIndexedFiles);
}

void DirectoryWrapper::interruptIndexing()
{
    wasCancelled = true;
}

void DirectoryWrapper::clearData()
{
    wasCancelled = false;
    indexedFileList = {};
}

}
