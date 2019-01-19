#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    qRegisterMetaType<std::vector<std::pair<fsize_t, QString> > >("std::vector<std::pair<fsize_t, QString> >");
    qRegisterMetaType<size_t>("size_t");
    qRegisterMetaType<fsize_t>("fsize_t");
    qRegisterMetaType<PatternUtil>("PatternUtil");
    qRegisterMetaType<std::pair<fsize_t,QString>>("std::pair<fsize_t,QString>");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
