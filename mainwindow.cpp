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

    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Up), this, SLOT(moveSplitterUp()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Down), this, SLOT(moveSplitterDown()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Left), this, SLOT(moveSplitterLeft()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Right), this, SLOT(moveSplitterRight()));

    connect(fileSelection.get(), &SearcherUtil::indexingFinished, this, &MainWindow::on_indexingFinished);
    connect(fileSelection.get(), &SearcherUtil::newIndexedFiles, this, &MainWindow::on_updateFileList);
    connect(fileSelection.get(), &SearcherUtil::searchingFinished, this, &MainWindow::on_searchingFinished);

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

    ui->searchButton->setEnabled(false);
    ui->indexDirectoriesButton->setEnabled(false);

    QThreadPool::globalInstance()->setMaxThreadCount(std::max(2, int(std::thread::hardware_concurrency())));
}

MainWindow::~MainWindow()
{

}

void MainWindow::on_addDirButton_clicked()
{
    QString directory = QFileDialog::getExistingDirectory(this, "Select Directory for Scanning",
                                                          QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (directory.size() != 0) {
        auto added = addDirectory(directory);
        if (added)
        {
            ui->indexDirectoriesButton->setEnabled(true);
        }
    }
}

void MainWindow::on_deleteDirButton_clicked()
{

}

void MainWindow::on_searchButton_clicked()
{
    auto pattern = ui->patternTextEdit->toPlainText();

    if (pattern.size() > 0) {
        taskTimer->restart();
        fileSelection->startSearching(pattern);
    }
}

void MainWindow::on_indexDirectoriesButton_clicked()
{
    qDebug() << QString(__func__) << QThread::currentThreadId();
    ui->searchButton->setEnabled(false);


    if (start_directories.size() != 0) {
        filesListModel->removeRows(0, filesListModel->rowCount());

        taskTimer->restart();
        fileSelection->startIndexing(start_directories, true);
    }
}

bool MainWindow::addDirectory(const QString &directory)
{
    ui->directoriesTable->insertRow(ui->directoriesTable->rowCount());

    auto * item = new QTableWidgetItem();
    ui->directoriesTable->setItem(ui->directoriesTable->rowCount() - 1, 0, item);

    auto * new_prog_bar = new QProgressBar();
    new_prog_bar->setValue(0);
    new_prog_bar->setFormat(directory);
    ui->directoriesTable->setCellWidget(ui->directoriesTable->rowCount() - 1, 0, new_prog_bar);

    start_directories.insert(directory);

    return true;
}

void MainWindow::removeDirectory(int row)
{
    auto name = getDirectoryName(row);
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
    ui->statusBar->showMessage(QString("Indexing complete. Elapsed time: %1 ms").arg(taskTimer->elapsed()));
    labelSearching->setText(QString("Files found: %1").arg(found_files));

    for (int i = 0; i < ui->directoriesTable->rowCount(); ++i) {
        // static_cast<QProgressBar*>(ui->directoriesTable->cellWidget(i, 0))->setValue(0);
    }

    ui->searchButton->setEnabled(true);
}

void MainWindow::on_searchingFinished(int found_files)
{
    ui->statusBar->clearMessage();
    ui->statusBar->showMessage(QString("Searching complete. Elapsed time: %1 ms").arg(taskTimer->elapsed()));
    labelSearching->setText(QString("Files found: %1").arg(found_files));

    ui->searchButton->setEnabled(true);
}

void MainWindow::on_updateFileList(int completed_files, std::vector<QString> indexed_files)
{
    for (auto& name : indexed_files) {
        *filesListModel << name;
    }
    labelSearching->setText(QString("Files found: %1").arg(completed_files));
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


