#ifndef WIDGET_H
#define WIDGET_H

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

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget() = default;

private slots:
    void browseDirectory(QLineEdit* lineEdit, const QString& startDir);
    QAbstractItemModel* updateFileTable(const QString& dirPath);
    void updateStatusBar(const QString& message);

private:
    QLineEdit* dirPath1 = nullptr;
    QLineEdit* dirPath2 = nullptr;
    QPushButton* browseButton1 = nullptr;
    QPushButton* browseButton2 = nullptr;
    QTreeView* fileTable1 = nullptr;
    QTreeView* fileTable2 = nullptr;
    QTreeView* resultTable = nullptr;
    QStatusBar* statusBar = nullptr;
    QProgressBar* progressBar = nullptr;
    QCompleter* pathCompleter = nullptr;
};

#endif // WIDGET_H
