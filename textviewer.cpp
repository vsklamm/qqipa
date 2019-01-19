#include "textviewer.h"

#include <QTextStream>

TextViewer::TextViewer(QWidget *parent) : QPlainTextEdit(parent)
{

}

void TextViewer::show(const QString &file)
{
    QFile f(file);
    f.open(QIODevice::ReadOnly | QFile::Text);
    QTextStream st(&f);
    setPlainText(st.readAll());
    f.close();
}
