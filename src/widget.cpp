#include "../inc/widget.h"

#include <QCommonStyle>
#include <QLabel>
#include <QStandardPaths>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <chrono>

#include "../inc/dircompare.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    this->setMinimumSize(540, 400);

    lastDirPath1 = QDir::toNativeSeparators
        (QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    lastDirPath2 = QDir::toNativeSeparators
        (QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));

    // MENU BAR
    QMenuBar *menuBar = new QMenuBar(this);
    QMenu *fileMenu = new QMenu(tr("&Program"), this);
    QMenu *settingsMenu = new QMenu(tr("&Settings"), this); // todo auto update
    menuBar->addMenu(fileMenu);
    menuBar->addMenu(settingsMenu);

    QAction *exitAction = new QAction(tr("&Exit"), this);
    fileMenu->addAction(exitAction);


    // DIR PATHS SELECTION
    QLabel* dirPathLabel = new QLabel(tr("Enter paths or browse for directories"), this);

    // Setup path completer
    pathCompleter = new QCompleter(this);
    QFileSystemModel* fsModel = new QFileSystemModel(pathCompleter);
    fsModel->setRootPath(lastDirPath1);
    fsModel->setFilter(QDir::Dirs | QDir::Drives | QDir::NoDotAndDotDot);
    pathCompleter->setMaxVisibleItems(10);
    pathCompleter->setModel(fsModel);

    // Dir path 1
    lastDirPath1 = QDir::toNativeSeparators
        (QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    dirPathEdit1 = new QLineEdit(this);
    dirPathEdit1->setText(lastDirPath1);
    //dirPath1->setReadOnly(true);
    dirPathEdit1->setCompleter(pathCompleter);
    browseButton1 = new QPushButton(tr("Browse"), this);
    browseButton1->setMaximumWidth(60);

    // Dir path 2
    dirPathEdit2 = new QLineEdit(this);
    dirPathEdit2->setText(lastDirPath2);
    dirPathEdit2->setCompleter(pathCompleter);
    browseButton2 = new QPushButton(tr("Browse"), this);
    browseButton2->setMaximumWidth(60);

    // FILE TABLE 1
    fileTable1 = new QTreeView(this);
    fileTable1->setRootIsDecorated(false);
    fileTable1->setAlternatingRowColors(true);
    fileTable1->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // FILE TABLE 2
    fileTable2 = new QTreeView;
    fileTable2->setRootIsDecorated(false);
    fileTable2->setAlternatingRowColors(true);
    fileTable2->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // DUPLICATE TABLE CONTROLS
    startSearchButton = new QPushButton(tr("Scan for duplicates"), this);
    QLabel* sizeFilterLabel = new QLabel(tr(" Filter by size (MB)"), this);
    sizeFilterEdit = new QLineEdit(this);
    startSearchButton->setMinimumWidth(150);
    sizeFilterEdit->setMaximumWidth(50);
    sizeFilterEdit->setValidator(new QIntValidator(1, 1000000, this));
    sizeFilterEdit->setText("1000");

    // DUPLICATES TABLE
    resultTable = new QTreeView;
    resultTable->setRootIsDecorated(false);
    resultTable->setAlternatingRowColors(true);

    // STATUS BAR
    statusBar = new QStatusBar(this);
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setMaximumSize(150, 16);
    progressBar->setFormat("");
    statusBar->addPermanentWidget(progressBar);

    // LAYOUTS
    QHBoxLayout* dirBrowseLayoutH = new QHBoxLayout();
    dirBrowseLayoutH->addWidget(dirPathEdit1);
    dirBrowseLayoutH->addWidget(browseButton1);
    dirBrowseLayoutH->addWidget(dirPathEdit2);
    dirBrowseLayoutH->addWidget(browseButton2);

    QVBoxLayout* dirBrowseLayoutV = new QVBoxLayout();
    dirBrowseLayoutV->addWidget(dirPathLabel);
    dirBrowseLayoutV->addLayout(dirBrowseLayoutH);

    QHBoxLayout* fileTablesLayout = new QHBoxLayout();
    fileTablesLayout->addWidget(fileTable1);
    fileTablesLayout->addWidget(fileTable2);

    QHBoxLayout* duplicateControlsLayout = new QHBoxLayout();
    QSpacerItem* spacerItem = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed);
    duplicateControlsLayout->addWidget(startSearchButton);
    duplicateControlsLayout->addStretch();
    duplicateControlsLayout->addWidget(sizeFilterLabel);
    duplicateControlsLayout->addWidget(sizeFilterEdit);
    duplicateControlsLayout->addItem(spacerItem);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->setMenuBar(menuBar);
    mainLayout->addLayout(dirBrowseLayoutV);
    mainLayout->addLayout(fileTablesLayout, 2);
    mainLayout->addLayout(duplicateControlsLayout);
    mainLayout->addWidget(resultTable, 3);
    mainLayout->addWidget(statusBar);

    setLayout(mainLayout);

    // CONNECTS
    // Обновить список файлов при выборе директории
    connect(browseButton1, &QPushButton::clicked, this, [this]() {
        browseDirectory(dirPathEdit1, dirPathEdit1->text());
        fileTable1->setModel(updateFileTable(dirPathEdit1->text()));
        lastDirPath1 = dirPathEdit1->text();
    });
    connect(browseButton2, &QPushButton::clicked, this, [this]() {
        browseDirectory(dirPathEdit2, dirPathEdit2->text());
        fileTable2->setModel(updateFileTable(dirPathEdit2->text()));
        lastDirPath2 = dirPathEdit2->text();
    });

    // Обновить список файлов при изменении пути
    connect(dirPathEdit1, &QLineEdit::textEdited, this, [this]() {
        fileTable1->setModel(updateFileTable(dirPathEdit1->text()));
        lastDirPath1 = dirPathEdit1->text();
    });
    connect(dirPathEdit2, &QLineEdit::textEdited, this, [this]() {
        fileTable2->setModel(updateFileTable(dirPathEdit2->text()));
        lastDirPath2 = dirPathEdit2->text();
    });

    // Обновить список файлов при нажатии Enter
    connect(dirPathEdit1, &QLineEdit::returnPressed, this, [this]() {
        fileTable1->setModel(updateFileTable(dirPathEdit1->text()));
        lastDirPath1 = dirPathEdit1->text();
    });
    connect(dirPathEdit2, &QLineEdit::returnPressed, this, [this]() {
        fileTable2->setModel(updateFileTable(dirPathEdit2->text()));
        lastDirPath2 = dirPathEdit2->text();
    });

    // Начать сканирование
    connect(startSearchButton, &QPushButton::clicked, this, [this]() {
        searchForDuplicates();
        progressBar->setValue(100);
    });

    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    // INITIALIZE TABLES
    fileTable1->setModel(updateFileTable(dirPathEdit1->text()));
    fileTable1->header()->resizeSection(0, 190);
    fileTable1->header()->resizeSection(1, 45);

    fileTable2->setModel(updateFileTable(dirPathEdit2->text()));
    fileTable2->header()->resizeSection(0, 190);
    fileTable2->header()->resizeSection(1, 45);

    searchForDuplicates();
    resultTable->header()->resizeSection(0, 225);
    resultTable->header()->resizeSection(1, 225);
    resultTable->header()->resizeSection(2, 45);

    progressBar->setValue(100);
    updateStatusBar(tr("Ready"));
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

    updateStatusBar(tr("%1 files in %2").arg(model->rowCount()).arg(dirPath));

    return model;
}

QAbstractItemModel *Widget::updateDuplicatesTable(const QVector<QPair<QVector<QString>, size_t>> &duplicates)
{
    QStandardItemModel *model = new QStandardItemModel(0, 3, this);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("File 1"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("File 2"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Size"));

    for (const auto& duplicatePair : duplicates)
    {
        const QVector<QString>& filePaths = duplicatePair.first;
        size_t fileSize = duplicatePair.second;

        if (filePaths.size() < 2)
            continue;

        int row = model->rowCount();
        model->insertRow(row);

        QString file1 = filePaths[0].split("/").back();
        QString file2 = filePaths[1].split("/").back();

        model->setData(model->index(row, 0), file1);
        model->setData(model->index(row, 1), file2);
        int sizeKB = fileSize / 1024 + 1;
        model->setData(model->index(row, 2), QString::number(sizeKB) + " KB");
    }
    return model;
}

void Widget::updateStatusBar(const QString &message)
{
    statusBar->showMessage(message);
}

void Widget::searchForDuplicates()
{
    auto start = std::chrono::high_resolution_clock::now();
    updateStatusBar(tr("Scanning for duplicate files..."));
    progressBar->setValue(0);

    QVector<QPair<QVector<QString>, size_t>> duplicates;

    size_t maxFileSizeBytes = sizeFilterEdit->text().toInt() * 1000000;
    DirCompare comparator(lastDirPath1, lastDirPath2, maxFileSizeBytes);
    connect(&comparator, &DirCompare::updateProgress, this, [this](int value) {
        progressBar->setValue(value);
    });
    //duplicates = comparator.findDuplicatesByBinaryMultithreaded();

    QFuture<QVector<QPair<QVector<QString>, size_t>>> future = QtConcurrent::run([&comparator]{
        return comparator.findDuplicatesByBinaryMultithreaded();
    });
    future.waitForFinished();
    duplicates = future.result();

    resultTable->setModel(updateDuplicatesTable(duplicates));

    auto end = std::chrono::high_resolution_clock::now();
    auto len = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    updateStatusBar(tr("Found %1 pairs of duplicates. (%2 ms)").arg(duplicates.size()).arg(len.count()));
}
