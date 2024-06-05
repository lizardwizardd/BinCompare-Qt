#include "../inc/dircompare.h"

#include <iostream>
#include <fstream>
#include <exception>

#include <QDirIterator>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QString>
#include <QException>
#include <QDebug>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QThread>
#include <QAtomicInt>


DirCompare::DirCompare(const QString &dirPath1, const QString &dirPath2, const size_t fileSizeFilter)
{
    readDirs(dirPath1, dirPath2, fileSizeFilter);
}

void DirCompare::readDirs(const QString &dirPath1, const QString &dirPath2, const size_t fileSizeFilter)
{
    QDirIterator it1(dirPath1, QDir::Files, QDirIterator::NoIteratorFlags);
    while (it1.hasNext())
    {
        it1.next();
        QFileInfo fileInfo = it1.fileInfo();

        if (fileInfo.size() > fileSizeFilter)
            continue;

        dirFiles1.emplace_back(fileInfo.absoluteFilePath(), fileInfo.size());
    }

    QDirIterator it2(dirPath2, QDir::Files, QDirIterator::NoIteratorFlags);
    while (it2.hasNext())
    {
        it2.next();
        QFileInfo fileInfo = it2.fileInfo();

        if (fileInfo.size() > fileSizeFilter)
            continue;

        dirFiles2.emplace_back(fileInfo.absoluteFilePath(), fileInfo.size());
    }
}

QVector<QPair<QVector<QString>, size_t>> DirCompare::findDuplicatesByBinary()
{
    QVector<QPair<QVector<QString>, size_t>> duplicates;

    const int totalOperations = dirFiles1.size() * dirFiles2.size();
    int currentOperation = 0;

    for (int i = 0; i < dirFiles1.size(); ++i)
    {
        for (int j = 0; j < dirFiles2.size(); ++j)
        {
            currentOperation++;

            if (dirFiles1[i].second != dirFiles2[j].second)
                continue;

            if (compareFilesBinary(dirFiles1[i].first, dirFiles2[j].first))
            {
                QVector<QString> duplicateNames = {dirFiles1[i].first, dirFiles2[j].first};
                duplicates.emplace_back(duplicateNames, dirFiles1[i].second);
            }

            int progress = static_cast<int>((static_cast<double>(currentOperation) / totalOperations) * 100);
            emit updateProgress(progress);
        }
    }

    return duplicates;
}

QVector<QPair<QVector<QString>, size_t>> DirCompare::findDuplicatesByBinaryMultithreaded()
{
    QVector<QPair<QVector<QString>, size_t>> duplicates;
    QAtomicInt currentOperation(0);

    size_t maxSameSizePairs = 0;
    for (const auto& file1 : dirFiles1)
    {
        for (const auto& file2 : dirFiles2)
        {
            if (file1.second == file2.second)
            {
                maxSameSizePairs++;
            }
        }
    }
    duplicates.reserve(maxSameSizePairs);

    const int totalOperations = dirFiles1.size() * dirFiles2.size();

    // Возвращает вектор с идентичными файлами в заданном промежут
    auto processFiles = [&](QPair<qsizetype, qsizetype> rangePair) -> QVector<QPair<QVector<QString>, size_t>> {
        QVector<QPair<QVector<QString>, size_t>> duplicatesFound;
        for (qsizetype i = rangePair.first; i < rangePair.second; ++i)
        {
            for (qsizetype j = 0; j < dirFiles2.size(); ++j)
            {
                currentOperation++;

                if (dirFiles1.at(i).second != dirFiles2.at(j).second)
                    continue;

                if (compareFilesBinary(dirFiles1[i].first, dirFiles2[j].first))
                {
                    QVector<QString> duplicateNames = {dirFiles1[i].first, dirFiles2[j].first};
                    duplicatesFound.emplace_back(duplicateNames, dirFiles1[i].second);
                }
            }

            int progress = static_cast<int>((static_cast<double>(currentOperation._q_value) / totalOperations) * 100);
            emit updateProgress(progress);
        }
        return duplicatesFound;
    };

    static const int THREAD_COUNT = QThread::idealThreadCount();
    qsizetype regionSize = dirFiles1.size() / THREAD_COUNT;
    QVector<QFuture<QVector<QPair<QVector<QString>, size_t>>>> futures;

    qsizetype i = 0; // для последнего цикла
    if (regionSize != 0)
    {
        for (; i < dirFiles1.size() - regionSize; i += regionSize)
        {
            futures.append(QtConcurrent::run(processFiles, qMakePair(i, i + regionSize)));
        }
    }
    futures.append(QtConcurrent::run(processFiles, qMakePair(i, dirFiles1.size())));

    for (auto &future : futures)
    {
        future.waitForFinished();
        duplicates.append(future.result());
    }

    return duplicates;
}

bool DirCompare::compareFilesBinary(const QString &filePath1, const QString &filePath2)
{
    QFile file1(filePath1);
    QFile file2(filePath2);

    if (!file1.open(QIODevice::ReadOnly) || !file2.open(QIODevice::ReadOnly))
    {
        qDebug() << "Error opening file";
        return false;
    }

    char buffer1[bufferSize];
    char buffer2[bufferSize];

    while (!file1.atEnd() && !file2.atEnd())
    {
        qint64 bytesRead1 = file1.read(buffer1, bufferSize);
        qint64 bytesRead2 = file2.read(buffer2, bufferSize);

        if (bytesRead1 != bytesRead2 || memcmp(buffer1, buffer2, bytesRead1) != 0)
        {
            return false;
        }
    }

    return file1.atEnd() && file2.atEnd();
}
