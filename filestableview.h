#ifndef FILESTABLEWIDGET_H
#define FILESTABLEWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QPainter>

class FilesTableView : public QTableView
{
    Q_OBJECT
public:
    FilesTableView(QWidget * parent = nullptr);

private:
    void paintEvent(QPaintEvent *e);

signals:

public slots:
};

#endif // FILESTABLEWIDGET_H
