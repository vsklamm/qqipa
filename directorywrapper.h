#ifndef DIRECTORYWRAPPER_H
#define DIRECTORYWRAPPER_H

#include "indexedfile.h"

#include <QFuture>
#include <QFutureWatcher>
#include <QObject>

#include <atomic>

namespace qqipa {

struct DirectoryWrapper : public QObject
{
    Q_OBJECT
public:
    DirectoryWrapper(const size_t &id, const QString &directoryName);

signals:
    void preprocessFinished(fsize_t upcomingFiles);
    void newIndexedFiles(fsize_t filesDirCompleted, std::vector<std::pair<fsize_t, QString>> indexedFiles);
    void indexingFinished(fsize_t indexedFiles);

public slots:
    void startIndexing();

    void interruptIndexing();

private slots:
    void indexFoundFiles(const std::vector<QString> &filesToIdndex);

private:
    inline std::vector<QString> impoverishDirectory();
    inline void clearData();

public:
    std::atomic<bool> wasCancelled;
    size_t id;
    QString name;
    QList<IndexedFile> indexedFileList;
};

}

#endif // DIRECTORYWRAPPER_H
