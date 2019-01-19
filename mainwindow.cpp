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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    taskTimer(new QElapsedTimer),
    searcherController(new Controller)
{
    ui->setupUi(this);

    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Up), this, SLOT(moveSplitterUp()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Down), this, SLOT(moveSplitterDown()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Left), this, SLOT(moveSplitterLeft()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Right), this, SLOT(moveSplitterRight()));

    connect(searcherController.get(), &Controller::dirIsReadyToIndexing, this, &MainWindow::on_dirIsReadyToIndexing);
    connect(searcherController.get(), &Controller::dirPreprocessStarted, this, &MainWindow::on_dirBeforePreprocessing);
    connect(searcherController.get(), &Controller::newIndexedFiles, this, &MainWindow::on_updateFileITable);
    connect(searcherController.get(), &Controller::newFoundFile, this, &MainWindow::on_updateFileSTable);
    connect(searcherController.get(), &Controller::indexingFinished, this, &MainWindow::on_indexingFinished);
    connect(searcherController.get(), &Controller::searchingFinished, this, &MainWindow::on_searchingFinished);
    connect(this, &MainWindow::deleteDirectory, searcherController.get(), &Controller::on_deleteDirectory);

    labelSearching = std::make_unique<QLabel>(ui->statusBar);
    labelSearching->setAlignment(Qt::AlignRight);
    labelSearching->setMinimumSize(labelSearching->sizeHint());

    ui->statusBar->addPermanentWidget(labelSearching.get());

    filesTableModel= std::make_unique<QStandardItemModel>(0, 2, ui->filesTableView);
    filesTableModel->setHorizontalHeaderLabels({ "Entries", "File" });
    ui->filesTableView->setModel(filesTableModel.get());

    ui->filesTableView->horizontalHeader()->setStretchLastSection(true);
    ui->filesTableView->setShowGrid(false);
    ui->filesTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

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

    auto selected = ui->directoriesTable->selectionModel()->selectedRows();
    if (selected.empty()) {
        return;
    }
    int iDir = selected.at(0).row();
    startDirectories.erase(std::remove(startDirectories.begin(), startDirectories.end(), getDirectoryName(iDir)), startDirectories.end());
    ui->directoriesTable->model()->removeRow(iDir);
    emit deleteDirectory(size_t(iDir));
}

void MainWindow::on_deleteAllButton_clicked()
{
    const size_t sz = startDirectories.size();
    startDirectories.clear();
    ui->directoriesTable->model()->removeRows(0, ui->directoriesTable->rowCount());
    for (size_t i = 0; i < sz; ++i)
    {
        emit deleteDirectory(i);
    }
}

void MainWindow::on_indexDirectoriesButton_clicked()
{
    qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();

    ui->searchButton->setEnabled(false);
    ui->deleteDirButton->setEnabled(false);
    ui->deleteAllButton->setEnabled(false);
    filesTableModel->removeRows(0, filesTableModel->rowCount());

    ui->statusBar->clearMessage();
    labelSearching->clear();

    taskTimer->restart();
    searcherController->startIndexing(startDirectories);
}

void MainWindow::on_stopIndexingButton_clicked()
{
    searcherController->interruptIndexing();
}

void MainWindow::on_searchButton_clicked()
{
    const auto pattern = ui->patternTextEdit->toPlainText();
    if (pattern.size() == 0)
        return;
    if (pattern.size() > 255)
    {
        ui->patternTextEdit->clear();
        return;
    }

    ui->statusBar->clearMessage();
    labelSearching->clear();

    ui->searchButton->setEnabled(false);
    ui->deleteDirButton->setEnabled(false);
    ui->deleteAllButton->setEnabled(false);
    filesTableModel->removeRows(0, filesTableModel->rowCount());

    taskTimer->restart();
    searcherController->startSearching(pattern);
}

bool MainWindow::addDirectory(const QString &directory)
{
    ui->directoriesTable->insertRow(ui->directoriesTable->rowCount());

    auto * item = new QTableWidgetItem();
    ui->directoriesTable->setItem(ui->directoriesTable->rowCount() - 1, 0, item);

    auto * newProgBar = new QProgressBar();
    newProgBar->setFormat(directory);
    newProgBar->setValue(0);

    ui->directoriesTable->setCellWidget(ui->directoriesTable->rowCount() - 1, 0, newProgBar);

    startDirectories.push_back(directory);

    return true;
}

void MainWindow::addTableRow(fsize_t entries, QString file_name)
{
    filesTableModel->insertRow(filesTableModel->rowCount());
    filesTableModel->setItem(filesTableModel->rowCount() - 1, 0, new QStandardItem(QString::number(entries)));
    filesTableModel->setItem(filesTableModel->rowCount() - 1, 1, new QStandardItem(file_name));
}

QProgressBar *MainWindow::getDirProgressBar(int row)
{
    return static_cast<QProgressBar *>(ui->directoriesTable->cellWidget(row, 0));
}

QString MainWindow::getDirectoryName(int row)
{
    return getDirProgressBar(row)->format();
}

bool MainWindow::isSubdirectory(int first_row, int second_row)
{
    return false;
}

void MainWindow::on_dirBeforePreprocessing(size_t iDir)
{
    auto * dirProgress = getDirProgressBar(int(iDir));
    dirProgress->setMaximum(0);
    dirProgress->setValue(0);
    //    dirProgress->setStyleSheet("QProgressBar::chunk {"
    //                               "background-color: #ffffff;"
    //                               "}");
    QPalette p = dirProgress->palette();
    p.setColor(QPalette::Highlight, Qt::darkCyan);
    dirProgress->setPalette(p);
}

void MainWindow::on_dirIsReadyToIndexing(size_t iDir, fsize_t filesToIndex)
{
    auto dirProgress = getDirProgressBar(int(iDir)); // TODO: casts are bad
    dirProgress->setMaximum(int(filesToIndex));
}

void MainWindow::on_updateFileITable(size_t iDir, fsize_t filesDirCompleted, std::vector<std::pair<fsize_t, QString>> indexedFiles)
{
    auto dirProgress = getDirProgressBar(int(iDir)); // TODO: casts are bad

    if (filesDirCompleted == dirProgress->maximum())
    {
        if (dirProgress->maximum() == 0)
            dirProgress->setMaximum(1); // TODO: workaround
        QPalette p = dirProgress->palette();
        p.setColor(QPalette::Highlight, Qt::darkGreen);
        dirProgress->setPalette(p);
        // TODO: write right erasing from startDirectories
        const auto dirName = getDirectoryName(int(iDir));
        startDirectories.erase(std::remove(startDirectories.begin(), startDirectories.end(), dirName), startDirectories.end());
    }

    dirProgress->setValue(int(filesDirCompleted));

    for (auto& file : indexedFiles)
    {
        addTableRow(file.first, file.second);
    }
    labelSearching->setText(QString("Files indexed: %1").arg(filesDirCompleted));
}

void MainWindow::on_updateFileSTable(int filesCompleted, std::pair<fsize_t, QString> foundFile)
{
    addTableRow(foundFile.first, foundFile.second);
    labelSearching->setText(QString("Files found: %1").arg(filesCompleted));
}

void MainWindow::on_indexingFinished(int found_files)
{
    qDebug() << QString(__func__) << " from work thread: " << QThread::currentThreadId();

    ui->statusBar->clearMessage();
    ui->statusBar->showMessage(QString("Indexing complete. Elapsed time: %1 ms").arg(taskTimer->elapsed()));
    labelSearching->setText(QString("Files indexed: %1").arg(found_files));

    ui->searchButton->setEnabled(true);
    ui->deleteDirButton->setEnabled(true);
    ui->deleteAllButton->setEnabled(true);

    startDirectories.clear();
}

void MainWindow::on_searchingFinished(int found_files)
{
    ui->statusBar->clearMessage();
    ui->statusBar->showMessage(QString("Searching complete. Elapsed time: %1 ms").arg(taskTimer->elapsed()));
    labelSearching->setText(QString("Files found: %1").arg(found_files));

    ui->searchButton->setEnabled(true);
    ui->deleteDirButton->setEnabled(true);
    ui->deleteAllButton->setEnabled(true);
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

void MainWindow::on_filesTableView_doubleClicked(const QModelIndex &index)
{
    int row = index.row();
    ui->plainTextEdit->show(index.sibling(row, 1).data().toString());
    // ui->widget_5->show();
}
