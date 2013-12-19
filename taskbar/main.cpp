#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qqml.h>

#include "desktopmodel.h"

static QObject *desktopModel(QQmlEngine *engine, QJSEngine *)
{
    return new DesktopModel;
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterUncreatableType<DesktopProcess>("DesktopEntry", 1, 0, "DesktopProcess", "DesktopProcess cannot be instantiated directly.");
    qmlRegisterSingletonType<DesktopModel>("DesktopEntry", 1, 0, "DesktopModel", &desktopModel);

    QQmlApplicationEngine engine("main.qml");

    return app.exec();
}
