#ifndef FILESLISTVIEW_H
#define FILESLISTVIEW_H

#include <QListView>
#include <QObject>
#include <QPainter>
#include <QWidget>

struct FilesListView : public QListView
{
    FilesListView(QWidget * parent = nullptr);
    ~FilesListView();

private:
    void paintEvent(QPaintEvent *e);

signals:

public slots:

};

#endif // FILESLISTVIEW_H
