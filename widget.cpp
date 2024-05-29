#include "widget.h"

#include <QCommonStyle>
#include <QLabel>
#include <QStandardPaths>
#include <QFileDialog>
#include <QStandardItemModel>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    this->setMinimumSize(520, 400);

    // MENU BAR
    QMenuBar *menuBar = new QMenuBar(this);
    QMenu *fileMenu = new QMenu(tr("&Program"), this);
    QMenu *settingsMenu = new QMenu(tr("&Settings"), this);
    menuBar->addMenu(fileMenu);
    menuBar->addMenu(settingsMenu);

    QAction *exitAction = new QAction(tr("&Exit"), this);
    fileMenu->addAction(exitAction);

    // DIR PATHS SELECTION
    QLabel* dirPathLabel = new QLabel(tr("Enter paths or browse for directories"), this);

    // Setup path completer
    pathCompleter = new QCompleter(this);
    QFileSystemModel* fsModel = new QFileSystemModel(pathCompleter);
    QString documentsPath = QDir::toNativeSeparators
        (QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    QString downloadPath = QDir::toNativeSeparators
        (QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
    fsModel->setRootPath(documentsPath);
    fsModel->setFilter(QDir::Dirs | QDir::Drives | QDir::NoDotAndDotDot);
    pathCompleter->setMaxVisibleItems(10);
    pathCompleter->setModel(fsModel);

    // Dir path 1
    dirPath1 = new QLineEdit(this);
    dirPath1->setText(documentsPath);
    //dirPath1->setReadOnly(true);
    dirPath1->setCompleter(pathCompleter);
    browseButton1 = new QPushButton(tr("Browse"), this);
    browseButton1->setMaximumWidth(60);

    // Dir path 2
    dirPath2 = new QLineEdit(this);
    dirPath2->setText(downloadPath);
    dirPath2->setCompleter(pathCompleter);
    browseButton2 = new QPushButton(tr("Browse"), this);
    browseButton2->setMaximumWidth(60);

    // FILE TABLE 1
    fileTable1 = new QTreeView(this);
    fileTable1->setRootIsDecorated(false);
    fileTable1->setAlternatingRowColors(true);

    // FILE TABLE 2
    fileTable2 = new QTreeView;
    fileTable2->setRootIsDecorated(false);
    fileTable2->setAlternatingRowColors(true);

    // DUPLICATES TABLE
    resultTable = new QTreeView;
    resultTable->setRootIsDecorated(false);
    resultTable->setAlternatingRowColors(true);

    // STATUS BAR
    statusBar = new QStatusBar(this);
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setMaximumSize(200, 18);
    progressBar->setFormat("");
    statusBar->addPermanentWidget(progressBar);

    // LAYOUTS
    QHBoxLayout* dirBrowseLayoutH = new QHBoxLayout();
    dirBrowseLayoutH->addWidget(dirPath1);
    dirBrowseLayoutH->addWidget(browseButton1);
    dirBrowseLayoutH->addWidget(dirPath2);
    dirBrowseLayoutH->addWidget(browseButton2);

    QVBoxLayout* dirBrowseLayoutV = new QVBoxLayout();
    dirBrowseLayoutV->addWidget(dirPathLabel);
    dirBrowseLayoutV->addLayout(dirBrowseLayoutH);

    QHBoxLayout* fileTablesLayout = new QHBoxLayout();
    fileTablesLayout->addWidget(fileTable1);
    fileTablesLayout->addWidget(fileTable2);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->setMenuBar(menuBar);
    mainLayout->addLayout(dirBrowseLayoutV);
    mainLayout->addLayout(fileTablesLayout);
    mainLayout->addWidget(resultTable);
    mainLayout->addWidget(statusBar);

    setLayout(mainLayout);

    // CONNECTS
    connect(browseButton1, &QPushButton::clicked, this, [this]() {
        browseDirectory(dirPath1, dirPath1->text());
        fileTable1->setModel(updateFileTable(dirPath1->text()));
    });
    connect(browseButton2, &QPushButton::clicked, this, [this]() {
        browseDirectory(dirPath2, dirPath2->text());
        fileTable2->setModel(updateFileTable(dirPath2->text()));
    });
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    // INITIALIZE TABLES
    fileTable1->setModel(updateFileTable(dirPath1->text()));
    fileTable1->header()->resizeSection(0, 190);
    fileTable1->header()->resizeSection(1, 45);

    fileTable2->setModel(updateFileTable(dirPath2->text()));
    fileTable2->header()->resizeSection(0, 190);
    fileTable2->header()->resizeSection(1, 45);
}

void Widget::browseDirectory(QLineEdit* lineEdit, const QString& startDir)
{
    updateStatusBar(tr("Browsing for directory..."));
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"), startDir,
                              QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty())
    {
        lineEdit->setText(QDir::toNativeSeparators(dir));
    }
    updateStatusBar(tr("Ready"));
}

QAbstractItemModel* Widget::updateFileTable(const QString& dirPath)
{
    updateStatusBar(tr("Searching for files in %1...").arg(dirPath));

    QStandardItemModel *model = new QStandardItemModel(0, 2, this);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Size"));

    QDirIterator it(dirPath, QDir::Files, QDirIterator::NoIteratorFlags);
    while (it.hasNext())
    {
        it.next();
        QFileInfo fileInfo = it.fileInfo();
        int row = model->rowCount();

        model->insertRow(row);
        model->setData(model->index(row, 0), fileInfo.fileName());
        model->setData(model->index(row, 1), fileInfo.size());
    }

    updateStatusBar(tr("Found %1 files in %2").arg(model->rowCount()).arg(dirPath));

    return model;
}

void Widget::updateStatusBar(const QString &message)
{
    statusBar->showMessage(message);
}
