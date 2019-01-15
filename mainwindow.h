#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "controller.h"
#include "indexedfile.h"

#include <QMainWindow>

#include <QElapsedTimer>
#include <QFileSystemWatcher>
#include <QLabel>
#include <QProgressBar>
#include <QSplitter>
#include <QStandardItemModel>

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
    void startIndexing(std::vector<QString> &startDirs);
    void startSearching(QString pattern);

    void interruptIndexing();
    void interruptSearching();

    void deleteDirectory(QString dirName);

public slots:
    void on_dirIsReadyToIndexing(size_t iDir, fsize_t filesToIndex);
    void on_updateFileTable(size_t iDir, fsize_t filesDirCompleted, std::vector<std::pair<fsize_t, QString>> indexedFiles);

    void on_indexingFinished(int found_files);
    void on_searchingFinished(int found_files);

private slots:
    void moveSplitterUp();
    void moveSplitterDown();
    void moveSplitterLeft();
    void moveSplitterRight();

public:
    // methods

private:
    bool addDirectory(const QString &directory);
    void addTableRow(fsize_t entries, QString file_name);
    void removeDirectory(int row);

    inline QProgressBar *getDirProgressBar(int row);
    inline QString getDirectoryName(int row);
    inline bool isSubdirectory(int first_row, int second_row);
    inline QString getSelectedDirectory();
    inline void moveSplitter(QSplitter *splitter, int direction);

private slots:
    void on_addDirButton_clicked();

    void on_searchButton_clicked();

    void on_indexDirectoriesButton_clicked();

    void on_deleteDirButton_clicked();

    void on_deleteAllButton_clicked();

    void on_stopIndexingButton_clicked();

public:
    std::vector<QString> startDirectories;

private:
    std::unique_ptr<Ui::MainWindow> ui;
    // Ui::MainWindow * ui;
    std::unique_ptr<QLabel> labelSearching;
    QStandardItemModel *filesTableModel;

    std::unique_ptr<Controller> searcherController;
    std::unique_ptr<QElapsedTimer> taskTimer;
};

#endif // MAINWINDOW_H
