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

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget() = default;

private:
    QLineEdit* dirPath1 = nullptr;
    QLineEdit* dirPath2 = nullptr;
    QPushButton* browseButton1 = nullptr;
    QPushButton* browseButton2 = nullptr;
    QTableWidget* fileTable1 = nullptr;
    QTableWidget* fileTable2 = nullptr;
    QTableWidget* resultTable = nullptr;
    QStatusBar* statusBar = nullptr;
    QProgressBar* progressBar = nullptr;
    QCompleter* pathCompleter = nullptr;

private slots:
    void browseDirectory(QLineEdit* lineEdit, const QString& startDir);
};

#endif // WIDGET_H
