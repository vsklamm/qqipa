#ifndef PATTERNCONTAINER_H
#define PATTERNCONTAINER_H

#include <QObject>

namespace qqipa {

class PatternContainer : public QObject
{
    Q_OBJECT
public:
    explicit PatternContainer(QObject *parent = nullptr);

signals:

public slots:

};

}

#endif // PATTERNCONTAINER_H
