#include "widget.h"

#include <QCommonStyle>
#include <QLabel>
#include <QStandardPaths>
#include <QFileDialog>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
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
    fsModel->setFilter(QDir::Dirs|QDir::Drives|QDir::NoDotAndDotDot);
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
    fileTable1 = new QTableWidget(0, 2, this);
    fileTable1->setHorizontalHeaderLabels(QStringList() << tr("File Name") << tr("Size"));
    fileTable1->horizontalHeader()->setStretchLastSection(true);
    fileTable1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    fileTable1->setSelectionMode(QAbstractItemView::SingleSelection);
    fileTable1->setSelectionBehavior(QAbstractItemView::SelectRows);

    // FILE TABLE 2
    fileTable2 = new QTableWidget(0, 2, this);
    fileTable2->setHorizontalHeaderLabels(QStringList() << tr("File Name") << tr("Size"));
    fileTable2->horizontalHeader()->setStretchLastSection(true);
    fileTable2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    fileTable2->setSelectionMode(QAbstractItemView::SingleSelection);
    fileTable2->setSelectionBehavior(QAbstractItemView::SelectRows);

    // DUPLICATES TABLE
    resultTable = new QTableWidget(0, 4, this);
    resultTable->setHorizontalHeaderLabels(QStringList() << tr("Group Number") << tr("File(s) 1 Path") << tr("File(s) 2 Path") << tr("Size"));
    resultTable->horizontalHeader()->setStretchLastSection(true);
    resultTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    resultTable->setSelectionMode(QAbstractItemView::SingleSelection);
    resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    // STATUS BAR
    statusBar = new QStatusBar(this);
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
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
    });
    connect(browseButton2, &QPushButton::clicked, this, [this]() {
        browseDirectory(dirPath2, dirPath2->text());
    });
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
}

void Widget::browseDirectory(QLineEdit* lineEdit, const QString& startDir)
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"), startDir,
                              QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty())
    {
        lineEdit->setText(QDir::toNativeSeparators(dir));
    }
}
