#pragma once

#include <QPair>
#include <QString>
#include <QVector>
#include <QObject>

#include <string>
#include <cstdint>
#include <vector>


class DirCompare : public QObject
{
    Q_OBJECT

public:
    DirCompare() = default;
    DirCompare(const QString& dirPath1, const QString& dirPath2, const size_t fileSizeFilter = SIZE_MAX);
    ~DirCompare() = default;

    void readDirs(const QString& dirPath1, const QString& dirPath2, const size_t fileSizeFilter = SIZE_MAX);
    void setMaxFileSize(const size_t size);

    // Возвращает пары одинаковых файлов
    QVector<QPair<QVector<QString>, size_t>> findDuplicatesByBinary();

signals:
    void updateProgress(int progress); // прогресс выполнения поиска в процентах

private:
    QVector<QPair<QString, size_t>> dirFiles1; // <путь, размер>
    QVector<QPair<QString, size_t>> dirFiles2; // <путь, размер>

    static const size_t bufferSize = 4096;

    bool compareFilesBinary(const QString& filePath1, const QString& filePath2);
};
