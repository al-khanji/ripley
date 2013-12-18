#ifndef COMPOSITOR_H
#define COMPOSITOR_H

#include <QQuickWindow>
#include <QWaylandCompositor>

class QWaylandSurface;
class CompositorPrivate;

class Compositor : public QQuickWindow, public QWaylandCompositor
{
    Q_OBJECT
    Q_PROPERTY(QObject * model READ model CONSTANT)
    Q_PROPERTY(QObject * clipboard READ clipboard CONSTANT)

public:
    explicit Compositor();
    ~Compositor();

    QObject *model() const;

    QObject *clipboard() const;

protected:
    void resizeEvent(QResizeEvent *);
    void surfaceCreated(QWaylandSurface *surface);
    void retainedSelectionReceived(QMimeData *mimeData);

private slots:
    void surfaceMapped();
    void surfaceUnmapped();
    void surfaceDestroyed(QObject *);
    void clipboardChanged();

private:
    void frameFinished();

    CompositorPrivate *d;
};

#endif // COMPOSITOR_H
