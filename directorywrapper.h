#ifndef DIRECTORYWRAPPER_H
#define DIRECTORYWRAPPER_H

#include "indexedfile.h"
#include "patternutil.h"

#include <QObject>

#include <atomic>

namespace qqipa {

struct DirectoryWrapper : public QObject
{
    Q_OBJECT
public:
    DirectoryWrapper(const size_t &id, const QString &directoryName);

signals:
    void preprocessStarted();
    void dirSearchingStarted();
    void preprocessFinished(fsize_t upcomingFiles);
    void newIndexedFiles(fsize_t filesDirCompleted, std::vector<std::pair<fsize_t, QString> > indexedFiles);
    void newFoundFile(fsize_t filesCompleted, std::pair<fsize_t, QString> foundFile);
    void indexingFinished(fsize_t indexedFiles);
    void searchingFinished(fsize_t foundFiles);

public slots:
    void startIndexing();
    void startSearching(PatternUtil patternUtil);

    void interruptIndexing();
    void interruptSearching();

private:
    inline void indexFoundFiles(const std::vector<QString> &filesToIdndex);
    inline std::vector<QString> impoverishDirectory();
    inline void clearData();

public:
    std::atomic<bool> wasIndCancelled;
    std::atomic<bool> wasSearchCancelled;
    std::atomic<bool> wasSuccessfullyIndexed;
    size_t id;
    QString name;
    std::vector<IndexedFile> indexedFileVector;
};

}

#endif // DIRECTORYWRAPPER_H
