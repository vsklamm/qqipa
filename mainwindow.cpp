#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCommonStyle>
#include <QFileDialog>
#include <QFuture>
#include <QThreadPool>
#include <QtConcurrent/QtConcurrentRun>
#include <QtConcurrent/QtConcurrentMap>

#include <QShortcut>
#include <set>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    fileSelection(new FileSelection),
    indexer(new IndexedFile),
    taskTimer(new QElapsedTimer)
{
    ui->setupUi(this);

    QCommonStyle style;
    ui->actionSelectDirectory->setIcon(style.standardIcon(QCommonStyle::SP_DialogOpenButton));
    ui->actionStartSearch->setIcon(style.standardIcon(QCommonStyle::SP_MediaPlay));

    connect(ui->actionSelectDirectory, &QAction::triggered, this, &MainWindow::selectDirectory);
    connect(ui->actionStartSearch, &QAction::triggered, this, &MainWindow::startSearch);

    connect(fileSelection.get(), &FileSelection::indexingFinished, this, &MainWindow::on_indexingFinished);

    labelDupes = std::make_unique<QLabel>(ui->statusBar);
    labelDupes->setAlignment(Qt::AlignRight);
    labelDupes->setMinimumSize(labelDupes->sizeHint());

    ui->statusBar->addPermanentWidget(labelDupes.get());

    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Left), this, SLOT(moveSplitterLeft()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Right), this, SLOT(moveSplitterRight()));
}

MainWindow::~MainWindow()
{
}

void MainWindow::selectDirectory()
{
    start_dir = get_selected_directory();
}

void MainWindow::on_indexingFinished(int found_files)
{
    ui->statusBar->clearMessage();
    ui->statusBar->showMessage(QString("Search complete. Elapsed time: %1 ms").arg(taskTimer->elapsed()));
    labelDupes->setText(QString("Files found: %1").arg(found_files));
}

void MainWindow::on_updateFileList(int completed_files, QVector<QString> indexed_files)
{
    // labelDupes->setText(QString("Files found: %1").arg(completed_files));

    for (auto& name : indexed_files) {
        ui->listFoundFiles->addItem(name);
    }
}

void MainWindow::startSearch()
{
    qDebug() << QString(__func__) << " from Main thread: " << QThread::currentThreadId();

    if (start_dir.size() != 0) {
        std::set<QString> t = { start_dir };
        fileSelection->startSearch(t, true);
    }
}

QString MainWindow::get_selected_directory()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Directory for Scanning",
                                                    QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    return dir;
}

void MainWindow::moveSplitterLeft()
{
    moveSplitter(-1);
}

void MainWindow::moveSplitterRight()
{
    moveSplitter(1);
}

void MainWindow::moveSplitter(int direction)
{
    QList<int> currentSizes = ui->splitter->sizes();
    currentSizes[0] += 6 * direction;
    currentSizes[1] -= 6 * direction;
    ui->splitter->setSizes(currentSizes);
}
