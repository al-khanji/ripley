#include <QPainter>
#include <QOpenGLPaintDevice>
#include <QScreen>

#include "ripleybackingstore.h"
#include "ripleywindow.h"
#include "ripleyintegration.h"

RipleyBackingStore::RipleyBackingStore(QWindow *window)
    : QPlatformBackingStore(window)
{
}

QPaintDevice *RipleyBackingStore::paintDevice()
{
    return &m_image;
}

void RipleyBackingStore::flush(QWindow *window, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(window)
    Q_UNUSED(region)
    Q_UNUSED(offset)

    QOpenGLContext ctx;
    ctx.setFormat(window->format());
    ctx.setScreen(window->screen());

    bool ok = ctx.create();
    if (!ok) {
        qDebug("could not create opengl context");
        return;
    }

    ok = ctx.makeCurrent(window);
    if (!ok) {
        qDebug("Could not make current");
        return;
    }

    QOpenGLPaintDevice device(window->size());
    QPainter painter(&device);

    foreach (QRect r, region.rects())
        painter.drawImage(r, m_image, r);

    ctx.swapBuffers(window);
    ctx.doneCurrent();
}

void RipleyBackingStore::resize(const QSize &size, const QRegion &staticContents)
{
    Q_UNUSED(staticContents);
    m_image = QImage(size, QImage::Format_RGB32);
}
