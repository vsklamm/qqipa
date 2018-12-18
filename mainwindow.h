#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "threegramindexer.h"

#include <QMainWindow>

#include <QElapsedTimer>
#include <QFileSystemWatcher>

#include <memory>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:

public slots:

public:
    // methods

private:
    // methods

public:
    // fields

private:
    std::unique_ptr<Ui::MainWindow> ui;

    // std::unique_ptr<QThread> workingThread;
    std::unique_ptr<ThreeGramIndexer> indexer;
    std::unique_ptr<QElapsedTimer> taskTimer;
};

#endif // MAINWINDOW_H
