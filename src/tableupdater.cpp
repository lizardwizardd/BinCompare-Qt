#include "../inc/tableupdater.h"

#include <QStandardItemModel>
#include <QDirIterator>
#include <QFuture>
#include <QtConcurrent>
#include <chrono>

#include "../inc/dircompare.h"

TableUpdater::TableUpdater(QObject *parent)
    : QObject{parent}
{}

QAbstractItemModel* TableUpdater::updateFileTable(const QString &dirPath)
{
    emit updateStatusBar(tr("Searching for files in %1...").arg(dirPath));

    QString processedPath(dirPath);
    if (processedPath.startsWith("file:///"))
    {
        processedPath = processedPath.sliced(8, processedPath.size() - 8);
    }

    QStandardItemModel *model = new QStandardItemModel(0, 2, this);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Size"));

    QDirIterator it(processedPath, QDir::Files, QDirIterator::NoIteratorFlags);
    while (it.hasNext())
    {
        it.next();
        QFileInfo fileInfo = it.fileInfo();
        int row = model->rowCount();

        model->insertRow(row);
        model->setData(model->index(row, 0), fileInfo.fileName());
        model->setData(model->index(row, 1), fileInfo.size());
    }

    emit updateStatusBar(tr("%1 files in %2").arg(model->rowCount()).arg(processedPath));

    return model;
}

QAbstractItemModel* TableUpdater::updateDuplicatesTable(const QString& dir1, const QString& dir2,
                                             const QString& sizeFilterMB, bool useMultithreading)
{
    auto start = std::chrono::high_resolution_clock::now();
    emit updateStatusBar(tr("Scanning for duplicate files..."));

    QString processedPath1(dir1);
    if (processedPath1.startsWith("file:///"))
    {
        processedPath1 = processedPath1.sliced(8, processedPath1.size() - 8);
    }

    QString processedPath2(dir2);
    if (processedPath2.startsWith("file:///"))
    {
        processedPath2 = processedPath2.sliced(8, processedPath2.size() - 8);
    }

    // Search for duplicates
    QVector<QPair<QVector<QString>, size_t>> duplicates;
    size_t maxFileSizeBytes = sizeFilterMB.toInt() * 1000 * 1000;
    DirCompare comparator(processedPath1, processedPath2, maxFileSizeBytes);
    connect(&comparator, &DirCompare::updateProgress, this, [this](int value) {
        emit updateProgressBar(value);
    });

    if (useMultithreading)
    {
        QFuture<QVector<QPair<QVector<QString>, size_t>>> future = QtConcurrent::run([&comparator]{
            return comparator.findDuplicatesByBinaryMultithreaded();
        });
        future.waitForFinished();
        duplicates = future.result();
    }
    else
    {
        duplicates = comparator.findDuplicatesByBinary();
    }

    // Create model
    QStandardItemModel* model = new QStandardItemModel(0, 3, this);
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
        qDebug() << file1 << file2 << sizeKB << "KB";
        model->setData(model->index(row, 2), QString::number(sizeKB) + " KB");
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto len = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    emit updateStatusBar(tr("Found %1 pairs of duplicates. (%2 ms)").arg(duplicates.size()).arg(len.count()));

    return model;
}
