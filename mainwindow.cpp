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
    searcherController(new Controller),
    taskTimer(new QElapsedTimer)
{
    ui->setupUi(this);

    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Up), this, SLOT(moveSplitterUp()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Down), this, SLOT(moveSplitterDown()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Left), this, SLOT(moveSplitterLeft()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Right), this, SLOT(moveSplitterRight()));

    connect(searcherController.get(), &Controller::dirIsReadyToIndexing, this, &MainWindow::on_dirIsReadyToIndexing);
    connect(searcherController.get(), &Controller::newIndexedFiles, this, &MainWindow::on_updateFileTable);
    connect(searcherController.get(), &Controller::indexingFinished, this, &MainWindow::on_indexingFinished);
    connect(this, &MainWindow::deleteDirectory, searcherController.get(), &Controller::on_deleteDirectory);
    connect(searcherController.get(), &Controller::searchingFinished, this, &MainWindow::on_searchingFinished);
    // connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this, SLOT(view_file(int, int)));

    labelSearching = std::make_unique<QLabel>(ui->statusBar);
    labelSearching->setAlignment(Qt::AlignRight);
    labelSearching->setMinimumSize(labelSearching->sizeHint());

    ui->statusBar->addPermanentWidget(labelSearching.get());

    model = new QStandardItemModel(0, 2, ui->filesTableView);
    model->setHorizontalHeaderLabels({ "Entries", "File" });
    ui->filesTableView->setModel(model);

    ui->filesTableView->horizontalHeader()->setStretchLastSection(true);
    ui->filesTableView->verticalHeader()->hide();
    ui->filesTableView->setShowGrid(false);
    ui->filesTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->filesTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->directoriesTable->horizontalHeader()->hide();
    ui->directoriesTable->verticalHeader()->hide();        
    ui->directoriesTable->setShowGrid(false);
    ui->directoriesTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->searchButton->setEnabled(false);
    ui->indexDirectoriesButton->setEnabled(false);

    QThreadPool::globalInstance()->setMaxThreadCount(std::max(2, int(std::thread::hardware_concurrency())));
}

MainWindow::~MainWindow()
{

}

void MainWindow::on_addDirButton_clicked()
{
    auto directory = QFileDialog::getExistingDirectory(this, "Select Directory for Scanning",
                                                          QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (directory.size() != 0)
    {
        const auto added = addDirectory(directory);
        if (added)
        {
            ui->indexDirectoriesButton->setEnabled(true);
        }
    }
}

void MainWindow::on_deleteDirButton_clicked()
{
    deleteDirectory("gorimhorosho");
}

void MainWindow::on_searchButton_clicked()
{
    const auto pattern = ui->patternTextEdit->toPlainText();

    if (pattern.size() > 0) {
        taskTimer->restart();
        searcherController->startSearching(pattern);
    }
}

void MainWindow::on_indexDirectoriesButton_clicked()
{
    ui->searchButton->setEnabled(false);

    if (startDirectories.size() != 0) {
        // filesListModel->removeRows(0, filesListModel->rowCount());
        taskTimer->restart();
        searcherController->startIndexing(startDirectories);
        startDirectories.clear();
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

    startDirectories.push_back(directory);

    return true;
}

void MainWindow::addTableRow(fsize_t entries, QString file_name)
{
    model->insertRow(model->rowCount());
    model->setItem(model->rowCount() - 1, 0, new QStandardItem(QString::number(entries)));
    model->setItem(model->rowCount() - 1, 1, new QStandardItem(file_name));
}

void MainWindow::removeDirectory(int row)
{
    // auto name = getDirectoryName(row);
    startDirectories.erase(startDirectories.begin() + row);
    ui->directoriesTable->removeRow(row);
    // TODO: what if indexing
}

QString MainWindow::getDirectoryName(int row)
{
    return static_cast<QProgressBar*>(ui->directoriesTable->cellWidget(row, 0))->format();
}

bool MainWindow::isSubdirectory(int first_row, int second_row)
{
    return false;
}

void MainWindow::on_dirIsReadyToIndexing(QString dirName, fsize_t filesToIndex)
{
    auto ptr = std::find(startDirectories.begin(), startDirectories.end(), dirName);
    if (ptr != startDirectories.end())
    {

    }
}

void MainWindow::on_updateFileTable(int completedFiles, std::vector<std::pair<fsize_t, QString>> indexedFiles)
{
    for (auto& file : indexedFiles)
    {
        addTableRow(file.first, file.second);
    }
    labelSearching->setText(QString("Files found: %1").arg(completedFiles));
}

void MainWindow::on_indexingFinished(int found_files)
{
    qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();

    ui->statusBar->clearMessage();
    ui->statusBar->showMessage(QString("Indexing complete. Elapsed time: %1 ms").arg(taskTimer->elapsed()));
    labelSearching->setText(QString("Files found: %1").arg(found_files));

    for (int i = 0; i < ui->directoriesTable->rowCount(); ++i)
    {
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

void MainWindow::moveSplitter(QSplitter *splitter, int direction)
{
    auto currentSizes = splitter->sizes();
    currentSizes[0] += 8 * direction;
    currentSizes[1] -= 8 * direction;
    splitter->setSizes(currentSizes);
}
