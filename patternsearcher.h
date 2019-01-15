#ifndef PATTERNSEARCHER_H
#define PATTERNSEARCHER_H

#include <QFuture>
#include <QFutureWatcher>
#include <QObject>

#include "directorywrapper.h"
#include "indexedfile.h"
#include <regex>

namespace qqipa {

class PatternSearcher : public QObject
{
    Q_OBJECT
public:
    PatternSearcher();

signals:
    void searchingFinished(fsize_t found_files);
    void newFoundFiles(int files_completed, std::vector<QString> found_files);

public slots:
    void search(const QString &pattern, const std::vector<DirectoryWrapper *> &indexedDirectories);

    void interruptSearching();

private slots:
    fsize_t searchInFile(IndexedFile &indexedFile);

private:
    inline TrigramContainer getPatternTrigrams(const std::string &patternStd);
    inline void clearData();

private:
    std::atomic<bool> wasCanceled;

    std::string patternStd;
    TrigramContainer patternTrigrams;
    std::regex patternRegex;

    QFuture<std::vector<QString>> filesToSearchFuture;
    QFutureWatcher<std::vector<QString>> filesToSearchWatcher;
    
};

}

#endif // PATTERNSEARCHER_H
