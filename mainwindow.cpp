#include "fileslistview.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCommonStyle>
#include <QFileDialog>
#include <QFuture>
#include <QThreadPool>
#include <QtConcurrent/QtConcurrentRun>
#include <QtConcurrent/QtConcurrentMap>
#include <QShortcut>
#include <QListWidget>
#include <QProgressBar>
#include <QPushButton>

#include <set>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    fileSelection(new SearcherUtil),
    taskTimer(new QElapsedTimer)
{
    ui->setupUi(this);

    QCommonStyle style;
    ui->actionSelectDirectory->setIcon(style.standardIcon(QCommonStyle::SP_DialogOpenButton));
    ui->actionStartSearch->setIcon(style.standardIcon(QCommonStyle::SP_MediaPlay));

    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Up), this, SLOT(moveSplitterUp()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Down), this, SLOT(moveSplitterDown()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Left), this, SLOT(moveSplitterLeft()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Right), this, SLOT(moveSplitterRight()));

    connect(ui->actionSelectDirectory, &QAction::triggered, this, &MainWindow::selectDirectory);
    connect(ui->actionStartSearch, &QAction::triggered, this, &MainWindow::startSearch);

    connect(fileSelection.get(), &SearcherUtil::indexingFinished, this, &MainWindow::on_indexingFinished);
    connect(fileSelection.get(), &SearcherUtil::newIndexedFiles, this, &MainWindow::on_updateFileList);

    labelSearching = std::make_unique<QLabel>(ui->statusBar);
    labelSearching->setAlignment(Qt::AlignRight);
    labelSearching->setMinimumSize(labelSearching->sizeHint());

    ui->statusBar->addPermanentWidget(labelSearching.get());

    listFoundFiles = new FilesListView(ui->splitter_2);
    filesListModel = new FilesListViewModel;
    listFoundFiles->setModel(filesListModel);
    ui->splitter_2->addWidget(listFoundFiles);

    ui->directoriesTable->horizontalHeader()->hide();
    ui->directoriesTable->verticalHeader()->hide();

    QThreadPool::globalInstance()->setMaxThreadCount(std::max(2, int(std::thread::hardware_concurrency())));
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_addDirButton_clicked()
{
    selectDirectory();
}

void MainWindow::selectDirectory()
{
    QString directory = QFileDialog::getExistingDirectory(this, "Select Directory for Scanning",
                                                          QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (directory.size() != 0) {
        addDirectory(directory);
    }
}

void MainWindow::addDirectory(const QString &directory)
{
    ui->directoriesTable->insertRow(ui->directoriesTable->rowCount());

    QTableWidgetItem * item = new QTableWidgetItem();
    ui->directoriesTable->setItem(ui->directoriesTable->rowCount() - 1, 0, item);

    QProgressBar * new_prog_bar = new QProgressBar();
    new_prog_bar->setValue(0);
    new_prog_bar->setFormat(directory);
    ui->directoriesTable->setCellWidget(ui->directoriesTable->rowCount() - 1, 0, new_prog_bar);

    start_directories.insert(directory);
}

void MainWindow::removeDirectory(int row)
{
    QString name = getDirectoryName(row);
    start_directories.erase(name);
    ui->directoriesTable->removeRow(row);
    // TODO: what if indexing
}

QString MainWindow::getDirectoryName(int row) {
    return static_cast<QProgressBar*>(ui->directoriesTable->cellWidget(row, 0))->format();
}

bool MainWindow::isSubdirectory(int first_row, int second_row)
{

}

void MainWindow::on_indexingFinished(int found_files)
{
    ui->statusBar->clearMessage();
    ui->statusBar->showMessage(QString("Search complete. Elapsed time: %1 ms").arg(taskTimer->elapsed()));
    labelSearching->setText(QString("Files found: %1").arg(found_files));

    for (int i = 0; i < ui->directoriesTable->rowCount(); ++i) {
        // static_cast<QProgressBar*>(ui->directoriesTable->cellWidget(i, 0))->setValue(0);
    }
}

void MainWindow::on_updateFileList(int completed_files, std::vector<QString> indexed_files)
{
    // labelDupes->setText(QString("Files found: %1").arg(completed_files));

    for (auto& name : indexed_files) {
        *filesListModel << name;
     }
    labelSearching->setText(QString("Files found: %1").arg(completed_files));
}

void MainWindow::startSearch()
{
    qDebug() << QString(__func__) << " from Main thread: " << QThread::currentThreadId();

    if (start_directories.size() != 0) {
        taskTimer->restart();
        fileSelection->startIndexing(start_directories, true);
    }
}

void MainWindow::moveSplitterUp()
{
    moveSplitter(ui->splitter_2, -1);
}

void MainWindow::moveSplitterDown()
{
    moveSplitter(ui->splitter_2, 1);
}

void MainWindow::moveSplitterLeft()
{
    moveSplitter(ui->splitter, -1);
}

void MainWindow::moveSplitterRight()
{
    moveSplitter(ui->splitter, 1);
}

void MainWindow::moveSplitter(QSplitter * splitter, int direction)
{
    QList<int> currentSizes = splitter->sizes();
    currentSizes[0] += 8 * direction;
    currentSizes[1] -= 8 * direction;
    splitter->setSizes(currentSizes);
}

