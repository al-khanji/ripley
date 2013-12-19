#include <QGuiApplication>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlApplicationEngine>
#include <QWaylandSurfaceItem>
#include <QWaylandSurface>
#include "compositor.h"

#include "desktopmodel.h"

static QObject *desktopModel(QQmlEngine *engine, QJSEngine *)
{
    return new DesktopModel;
}

int main(int argc, char *argv[])
{
    qmlRegisterRevision<QWindow, 1>("Compositor", 1, 0);
    qmlRegisterType<QWaylandSurfaceItem>("QtQuick.Compositor", 1, 0, "WaylandSurfaceItem");
    qmlRegisterUncreatableType<QWaylandSurface>("QtQuick.Compositor", 1, 0, "WaylandSurface", "WaylandSurfaces are created by the compositor and cannot be instantiated directly.");
    qmlRegisterType<Compositor>("Compositor", 1, 0, "Compositor");
    qmlRegisterUncreatableType<DesktopProcess>("DesktopEntry", 1, 0, "DesktopProcess", "DesktopProcess cannot be instantiated directly.");
    qmlRegisterSingletonType<DesktopModel>("DesktopEntry", 1, 0, "DesktopModel", &desktopModel);

    QGuiApplication app(argc, argv);

    qputenv("QT_COMPOSITOR_NEGATE_INVERTED_Y", "1");

    QQmlApplicationEngine engine("main.qml");

    return app.exec();
}
