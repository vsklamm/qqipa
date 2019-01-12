#ifndef DIRECTORYWRAPPER_H
#define DIRECTORYWRAPPER_H

#include "indexedfile.h"

#include <QFuture>
#include <QFutureWatcher>
#include <QObject>

namespace qqipa {

class DirectoryWrapper : public QObject
{
    Q_OBJECT
public:
    DirectoryWrapper(const QString &directoryName);

signals:
    void newIndexedFiles(int filesCompleted, std::vector<std::pair<fsize_t, QString>> foundFiles);
    void indexingFinished(int indexedFiles);

public slots:
    void startIndexing();

    void interruptIndexing();

private slots:
    void indexFoundFiles(const std::vector<QString> &filesToIdndex);

private:
    std::vector<QString> impoverishDirectory();

public:
    QString name;
    QList<IndexedFile> indexedFileList;

    // TODO: reduce qfutures
    // QFuture<void> indexerFuture;
    // QFutureWatcher<void> indexerWatcher;

};

}

#endif // DIRECTORYWRAPPER_H
