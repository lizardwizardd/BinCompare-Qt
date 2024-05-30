#pragma once

#include <string>
#include <vector>

#include <QPair>
#include <QString>
#include <QVector>

#include "xxhash32.h"

class DirCompare
{
public:
    DirCompare(int seed = 0);
    DirCompare(const QString& dirPath1, const QString& dirPath2, int seed = 0);
    ~DirCompare() = default;

    void readDirs(const QString& dirPath1, const QString& dirPath2);

    // Возвращает группы одинаковых файлов
    QVector<QPair<QVector<QString>, size_t>> findDuplicatesByHash();

    // Возвращает пары одинаковых файлов
    QVector<QPair<QVector<QString>, size_t>> findDuplicatesByBinary();

private:
    XXHash32 hasher;
    QVector<QPair<QString, size_t>> dirFiles1; // <путь, размер>
    QVector<QPair<QString, size_t>> dirFiles2; // <путь, размер>

    static const size_t bufferSize = 4096;

    uint32_t calculateFileHash(const QString& filePath); // возвращает 0 при ошибке
    bool compareFilesBinary(const QString& filePath1, const QString& filePath2);
};
