#include <QPainter>
#include <QOpenGLPaintDevice>
#include <QScreen>
#include "ripleybackingstore.h"
#include "ripleywindow.h"

RipleyBackingStore::RipleyBackingStore(QWindow *window)
    : QPlatformBackingStore(window)
{
}

void RipleyBackingStore::beginPaint(const QRegion &r)
{
    m_dirty |= r;
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
}

void RipleyBackingStore::resize(const QSize &size, const QRegion &staticContents)
{
    Q_UNUSED(staticContents);
    m_image = QImage(size, QImage::Format_RGB32);
}
