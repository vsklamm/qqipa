#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "searcherutil.h"
#include "fileslistview.h"
#include "fileslistviewmodel.h"
#include "indexedfile.h"

#include <QMainWindow>

#include <QElapsedTimer>
#include <QFileSystemWatcher>
#include <QLabel>
#include <QSplitter>

#include <memory>

using namespace qqipa;

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
    void addDirectory(const QString &directory);
    void removeDirectory(int row);

    QString getDirectoryName(int row);
    bool isSubdirectory(int first_row, int second_row);

    void on_indexingFinished(int found_files);
    void on_updateFileList(int completed_files, std::vector<QString> indexed_files);

    void moveSplitterUp();
    void moveSplitterDown();
    void moveSplitterLeft();
    void moveSplitterRight();

public:
    // methods

private:
    QString get_selected_directory();
    void moveSplitter(QSplitter * splitter, int direction);

public:
    std::set<QString> start_directories;

private slots:
    void on_addDirButton_clicked();

private:
    // std::unique_ptr<Ui::MainWindow> ui;
    Ui::MainWindow * ui;
    std::unique_ptr<QLabel> labelSearching;
    FilesListView * listFoundFiles;
    FilesListViewModel * filesListModel;

    std::unique_ptr<SearcherUtil> fileSelection;
    std::unique_ptr<QElapsedTimer> taskTimer;
};

#endif // MAINWINDOW_H
