#include "compositor.h"
#include "compositormodel.h"

#include <QGuiApplication>
#include <QWaylandSurface>
#include <QWaylandInputPanel>
#include <QClipboard>
#include <QMimeData>
#include <QLoggingCategory>
Q_LOGGING_CATEGORY(COMPOSITOR, "compositor")

class CompositorPrivate
{
friend class Compositor;
    QList<QWaylandSurface *> surfaces;
    QSet<WaylandClient *> clients;
    CompositorModel model;
    QSet<QWaylandSurface *> inputPanels;
};

Compositor::Compositor()
    : QQuickWindow()
    , QWaylandCompositor(this, 0, DefaultExtensions|TextInputExtension)
    , d(new CompositorPrivate)
{
    setClientFullScreenHint(true);
    connect(this, &QQuickWindow::frameSwapped, this, &Compositor::frameFinished);

    COMPOSITOR().setEnabled(QtDebugMsg, true);

    connect(QGuiApplication::clipboard(), SIGNAL(changed(QClipboard::Mode)), SLOT(clipboardChanged()));

    setRetainedSelectionEnabled(true);
}

Compositor::~Compositor()
{
    delete d;
}

QObject *Compositor::model() const
{
    return &d->model;
}

QObject *Compositor::clipboard() const
{
    return QGuiApplication::clipboard();
}

void Compositor::resizeEvent(QResizeEvent *event)
{
    QQuickWindow::resizeEvent(event);
    QWaylandCompositor::setOutputGeometry(QRect(0, 0, width(), height()));
}

void Compositor::retainedSelectionReceived(QMimeData *mimeData)
{
    qCDebug(COMPOSITOR) << "Retained selection received:" << mimeData;
}

void Compositor::surfaceCreated(QWaylandSurface *surface)
{
    qCDebug(COMPOSITOR) << "Surface created:" << surface << surface->title() << surface->className();
    connect(surface, SIGNAL(mapped()), SLOT(surfaceMapped()));
    connect(surface, SIGNAL(unmapped()), SLOT(surfaceUnmapped()));
    connect(surface, SIGNAL(destroyed(QObject*)), SLOT(surfaceDestroyed(QObject*)));
    d->clients.insert(surface->client());
}

void Compositor::surfaceMapped()
{
    if (QWaylandSurface *surface = qobject_cast<QWaylandSurface*>(sender())) {
        qCDebug(COMPOSITOR) << "Surface mapped:" << surface << surface->title() << surface->className();
        if (!surface->hasShellSurface())
            return;

        d->model.append(CompositorItem(surface));
    }
}

void Compositor::surfaceUnmapped()
{
    if (QWaylandSurface *surface = qobject_cast<QWaylandSurface*>(sender())) {
        qCDebug(COMPOSITOR) << "Surface unmapped:" << surface;
        d->model.remove(surface);
    }
}

void Compositor::surfaceDestroyed(QObject *object)
{
    qCDebug(COMPOSITOR) << "Surface destroyed:" << object;
    d->model.remove(static_cast<QWaylandSurface *>(object));
}

void Compositor::frameFinished()
{
    QWaylandCompositor::frameFinished();
}

void Compositor::clipboardChanged()
{
    qCDebug(COMPOSITOR) << "Clipboard changed, overriding selection";
    overrideSelection(QGuiApplication::clipboard()->mimeData());
}
