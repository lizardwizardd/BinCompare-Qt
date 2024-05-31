#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QHeaderView>
#include <QStatusBar>
#include <QProgressBar>
#include <QCompleter>
#include <QFileSystemModel>
#include <QStandardPaths>
#include <QTreeView>
#include <QAbstractTableModel>
#include <QRadioButton>


class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget() = default;

private slots:
    void browseDirectory(QLineEdit* lineEdit, const QString& startDir);
    QAbstractItemModel* updateFileTable(const QString& dirPath);
    QAbstractItemModel* updateDuplicatesTable(const QVector<QPair<QVector<QString>, size_t>>& duplicates);

    void updateStatusBar(const QString& message);
    void searchForDuplicates();

private:
    QLineEdit* dirPathEdit1 = nullptr;
    QLineEdit* dirPathEdit2 = nullptr;
    QPushButton* browseButton1 = nullptr;
    QPushButton* browseButton2 = nullptr;
    QTreeView* fileTable1 = nullptr;
    QTreeView* fileTable2 = nullptr;
    QPushButton* startSearchButton = nullptr;
    QLineEdit* sizeFilterEdit = nullptr;
    QTreeView* resultTable = nullptr;
    QStatusBar* statusBar = nullptr;
    QProgressBar* progressBar = nullptr;
    QCompleter* pathCompleter = nullptr;

    QString lastDirPath1;
    QString lastDirPath2;
};
