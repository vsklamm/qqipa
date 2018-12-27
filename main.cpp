#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    qRegisterMetaType<std::vector<QString>>("std::vector<QString>");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
