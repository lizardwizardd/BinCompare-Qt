#pragma once

#include <QObject>
#include <QAbstractItemModel>


class TableUpdater : public QObject
{
    Q_OBJECT
public:
    explicit TableUpdater(QObject *parent = nullptr);

    Q_INVOKABLE QAbstractItemModel* updateFileTable(const QString& dirPath);
    Q_INVOKABLE QAbstractItemModel* updateDuplicatesTable(const QString& dir1,
                const QString& dir2, const QString& sizeFilterMB, bool useMultithreading);

signals:
    void updateStatusBar(const QString& message);
    void updateProgressBar(const int progress);
};
