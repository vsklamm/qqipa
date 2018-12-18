#ifndef THREEGRAMINDEXER_H
#define THREEGRAMINDEXER_H

#include <QObject>

#include <set>

struct ThreeGramIndexer : public QObject
{
    Q_OBJECT
public:
    explicit ThreeGramIndexer(QObject *parent = nullptr);
    ~ThreeGramIndexer();

signals:
    void preprocessFinished();
    void indexingFinished();

public slots:
    void indexDirectories(std::set<QString> &start_dirs, bool recursively = true);
    void interruptIndexing();

public:
    // methods

private:
    // methods

public:
    // fields

private:
    // fields

};

#endif // THREEGRAMINDEXER_H
