#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    indexer(new ThreeGramIndexer),
    taskTimer(new QElapsedTimer)
{
    ui->setupUi(this);



}

MainWindow::~MainWindow()
{
}
