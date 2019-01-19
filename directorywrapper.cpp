#include "directorywrapper.h"
#include "magic_number.h"

#include <QDebug>
#include <QThread>
#include <QDirIterator>

namespace qqipa {

DirectoryWrapper::DirectoryWrapper(const size_t &id, const QString &directoryName)
    : wasIndCancelled(false), wasSearchCancelled(false), wasSuccessfullyIndexed(false), id(id), name(directoryName)
{

}

void DirectoryWrapper::startIndexing()
{
    qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();

    clearData();
    emit preprocessStarted();
    const auto res = impoverishDirectory();
    if (wasIndCancelled)
    {
        emit newIndexedFiles(0, {});
        emit indexingFinished(0);
        return;
    }
    emit preprocessFinished(fsize_t(res.size()));
    indexFoundFiles(res);
}

void DirectoryWrapper::startSearching(PatternUtil patternUtil)
{
    if (!wasSuccessfullyIndexed)
    {
        emit searchingFinished(0);
        return;
    }
    qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();

    emit dirSearchingStarted();
    fsize_t foundFilesCount = 0;
    for (auto& curFile : indexedFileVector)
    {
        auto result = curFile.searchPattern(patternUtil.rawSize_, patternUtil.trigrams_, patternUtil.regex_);
        if (result > 0)
        {
            ++foundFilesCount;
            emit newFoundFile(foundFilesCount, std::pair{result, curFile.getFullPath()});
        }
    }
    emit searchingFinished(foundFilesCount);
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
            if (wasIndCancelled)
                return result;

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
        if (wasIndCancelled)
        {
            emit indexingFinished(textIndexedFiles);
            return;
        }
        auto file = IndexedFile(QFileInfo(filesToIndex[i]));
        auto notBinary = file.calculateIndex();
        if (notBinary)
        {
            indexedFileVector.push_back(file);
            indexedFilesVector.emplace_back(file.container_.size(), file.getFullPath());
            ++textIndexedFiles;
            if (indexedFilesVector.size() > queueSize)
            {
                emit newIndexedFiles(fsize_t(i), indexedFilesVector);
                indexedFilesVector.clear();
            }
        }
    }
    wasSuccessfullyIndexed = !wasIndCancelled;
    emit newIndexedFiles(fsize_t(filesToIndex.size()), indexedFilesVector);
    emit indexingFinished(textIndexedFiles);
}

void DirectoryWrapper::interruptIndexing()
{
    wasIndCancelled = true;
}

void DirectoryWrapper::interruptSearching()
{
    wasSearchCancelled = true;
}

void DirectoryWrapper::clearData()
{
    wasIndCancelled = false;
    wasSuccessfullyIndexed = false;
    indexedFileVector = {};
}

}
