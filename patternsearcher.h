#ifndef PATTERNSEARCHER_H
#define PATTERNSEARCHER_H

#include <QObject>

namespace qqipa {

class PatternSearcher : public QObject
{
    Q_OBJECT
public:
    explicit PatternSearcher(QObject *parent = nullptr);

signals:

public slots:
};

}

#endif // PATTERNSEARCHER_H
