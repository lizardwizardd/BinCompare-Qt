#include "widget.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include <istream>
#include <string>
#include <fstream>
#include <iostream>
#include "xxhash32.h"

void f(const QString& filepath)
{
    XXHash32 hasher(0);
    QFile file(filepath);

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Error opening file:" << filepath;
        return;
    }

    char buffer[4096];

    while (!file.atEnd())
    {
        qint64 bytesRead = file.read(buffer, sizeof(buffer));
        if (bytesRead > 0)
        {
            hasher.add(buffer, static_cast<uint64_t>(bytesRead));
        }
    }

    file.close();

    qDebug() << hasher.hash();
}


int main(int argc, char *argv[])
{
    f("C:\\Games\\Balatro-SteamGG.net\\Balatro.exe");
    f("C:\\Games\\Balatro-SteamGG.net\\Balatro — копия.exe");


    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "BinCompare_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    Widget w;
    w.show();
    return a.exec();
}
