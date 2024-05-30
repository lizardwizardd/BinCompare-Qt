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


DirCompare::DirCompare(const QString &dirPath1, const QString &dirPath2)
{
    readDirs(dirPath1, dirPath2);
}

void DirCompare::readDirs(const QString &dirPath1, const QString &dirPath2)
{
    QDirIterator it1(dirPath1, QDir::Files, QDirIterator::NoIteratorFlags);
    while (it1.hasNext())
    {
        it1.next();
        QFileInfo fileInfo = it1.fileInfo();
        dirFiles1.emplace_back(fileInfo.absoluteFilePath(), fileInfo.size());
    }

    QDirIterator it2(dirPath2, QDir::Files, QDirIterator::NoIteratorFlags);
    while (it2.hasNext())
    {
        it2.next();
        QFileInfo fileInfo = it2.fileInfo();
        dirFiles2.emplace_back(fileInfo.absoluteFilePath(), fileInfo.size());
    }
}

QVector<QPair<QVector<QString>, size_t>> DirCompare::findDuplicatesByBinary()
{
    QVector<QPair<QVector<QString>, size_t>> duplicates;

    for (const auto& file1 : dirFiles1)
    {
        for (const auto& file2 : dirFiles2)
        {
            if (file1.second != file2.second)
                continue;

            if (compareFilesBinary(file1.first, file2.first))
            {
                QVector<QString> duplicateNames = {file1.first, file2.first};
                duplicates.emplace_back(duplicateNames, file1.second);
            }
        }
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
