#ifndef TEXTVIEWER_H
#define TEXTVIEWER_H

#include <QObject>
#include <QPlainTextEdit>
#include <QWidget>

struct TextViewer : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit TextViewer(QWidget *parent = nullptr);

    void show(const QString &file);
};

#endif // TEXTVIEWER_H
