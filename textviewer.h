#ifndef TEXTVIEWER_H
#define TEXTVIEWER_H

#include <QObject>
#include <QPlainTextEdit>
#include <QWidget>

struct TextViewer : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit TextViewer(QWidget *parent = nullptr) : QPlainTextEdit(parent)
    {

    }

signals:

public slots:
};

#endif // TEXTVIEWER_H
