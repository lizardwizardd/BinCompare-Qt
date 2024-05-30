#include "dircompare.h"

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


DirCompare::DirCompare(int seed) :
    hasher(seed)
{}

DirCompare::DirCompare(const QString& dirPath1, const QString& dirPath2, int seed) :
    hasher(seed)
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

QVector<QPair<QVector<QString>, size_t>> DirCompare::findDuplicatesByHash()
{
    QVector<QPair<QVector<QString>, size_t>> duplicates;
    std::unordered_map<size_t, QVector<QString>> sizeGroups;

    // Сгруппировать по размерам
    for (const auto& file : dirFiles1)
        sizeGroups[file.second].append(file.first);

    for (const auto& file : dirFiles2)
        sizeGroups[file.second].append(file.first);

    // Подсчитать хэши файлов для каждой группы с >1 файлов
    for (const auto& sizeGroupEntry : sizeGroups)
    {
        if (sizeGroupEntry.second.size() < 2)
            continue;

        std::unordered_map<uint32_t, QVector<QString>> hashGroups;

        // Сгруппировать файлы с одинкавым хэшем
        for (const auto& file : sizeGroupEntry.second)
        {
            uint32_t fileHash = calculateFileHash(file);
            if (fileHash != 0)
            {
                hashGroups[fileHash].append(file);
            }
        }

        // Внести группы в duplicates
        for (const auto& hashEntry : hashGroups)
        {
            if (hashEntry.second.size() > 1)
            {
                duplicates.emplace_back(hashEntry.second, sizeGroupEntry.first);
            }
        }
    }

    return duplicates;
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

uint32_t DirCompare::calculateFileHash(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Error opening file" << filePath;
        return 0;
    }

    char buffer[bufferSize];

    while (!file.atEnd())
    {
        qint64 bytesRead = file.read(buffer, bufferSize);
        if (bytesRead > 0)
        {
            hasher.add(buffer, static_cast<uint64_t>(bytesRead));
        }
    }

    file.close();

    return hasher.hash();
}

bool DirCompare::compareFilesBinary(const QString &filePath1, const QString &filePath2)
{
    QFile file1(filePath1);
    QFile file2(filePath2);

    if (!file1.open(QIODevice::ReadOnly) || !file2.open(QIODevice::ReadOnly))
    {
        qDebug() << "Error opening file for comparison";
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
