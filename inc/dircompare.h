#pragma once

#include <QPair>
#include <QString>
#include <QVector>

#include <string>
#include <vector>


class DirCompare
{
public:
    DirCompare() = default;
    DirCompare(const QString& dirPath1, const QString& dirPath2);
    ~DirCompare() = default;

    void readDirs(const QString& dirPath1, const QString& dirPath2);

    // Возвращает пары одинаковых файлов
    QVector<QPair<QVector<QString>, size_t>> findDuplicatesByBinary();

private:
    QVector<QPair<QString, size_t>> dirFiles1; // <путь, размер>
    QVector<QPair<QString, size_t>> dirFiles2; // <путь, размер>

    static const size_t bufferSize = 4096;

    bool compareFilesBinary(const QString& filePath1, const QString& filePath2);
};
