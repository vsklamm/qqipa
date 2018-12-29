#include "filestableview.h"

FilesTableView::FilesTableView(QWidget *parent)
    : QTableView(parent)
{

}

void FilesTableView::paintEvent(QPaintEvent *e)
{
    QTableView::paintEvent(e);
    if (model() && model()->rowCount(rootIndex()) > 0)
    {
        return;
    }
    QPainter p(this->viewport());
    p.drawText(rect(), Qt::AlignCenter, "No file to open");
}
