#ifndef PATTERNSEARCHER_H
#define PATTERNSEARCHER_H

#include <QFuture>
#include <QFutureWatcher>
#include <QObject>

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
    void search(const QString &pattern, QList<IndexedFile> &indexed_files);
    fsize_t searchInFile(IndexedFile &indexedFile);
    void findMathingTrigrams();

    void interruptSearching();

private:
    TrigramContainer getPatternTrigrams(const std::string &pattern_std);
    void clearData();

private:
    std::atomic<bool> was_canceled;

    std::string pattern_std;
    TrigramContainer patternTrigrams;
    std::regex pattern_regex;

    QFuture<std::vector<QString>> files_to_search_future;
    QFutureWatcher<std::vector<QString>> files_to_search_watcher;
    
};

}

#endif // PATTERNSEARCHER_H
