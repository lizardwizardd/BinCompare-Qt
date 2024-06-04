#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QLocale>
#include <QTranslator>

#include "../inc/dircompare.h"

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);
    qmlRegisterType<DirCompare>("my.dircompare", 1, 0, "DirCompare");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/BinCompare/qml/widget.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &a, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return a.exec();
}
