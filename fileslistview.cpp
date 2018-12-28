#include "fileslistview.h"

FilesListView::FilesListView(QWidget *parent)
    : QListView(parent)
{

}

FilesListView::~FilesListView()
{

}

void FilesListView::paintEvent(QPaintEvent *e)
{
    QListView::paintEvent(e);
    if (model() && model()->rowCount(rootIndex()) > 0)
    {
        return;
    }
    QPainter p(this->viewport());
    p.drawText(rect(), Qt::AlignCenter, "No file to open");
}
