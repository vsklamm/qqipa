#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "fileselection.h"
#include "indexedfile.h"

#include <QMainWindow>

#include <QElapsedTimer>
#include <QFileSystemWatcher>
#include <QLabel>

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
    void startSearch();
    void selectDirectory();

    void on_indexingFinished(int found_files);
    void on_updateFileList(int completed_files, QVector<QString> indexed_files);

    void moveSplitterLeft();
    void moveSplitterRight();

public:
    // methods

private:
    QString get_selected_directory();
    void moveSplitter(int direction);

public:
     // std::set<QString> start_directories;
    QString start_dir;

private:
    std::unique_ptr<Ui::MainWindow> ui;
    std::unique_ptr<QLabel> labelDupes;

    std::unique_ptr<FileSelection> fileSelection;
    std::unique_ptr<IndexedFile> indexer;
    std::unique_ptr<QElapsedTimer> taskTimer;
};

#endif // MAINWINDOW_H
