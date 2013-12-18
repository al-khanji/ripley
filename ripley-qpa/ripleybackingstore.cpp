#include <QScreen>
#include "ripleybackingstore.h"

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
}

void RipleyBackingStore::resize(const QSize &size, const QRegion &staticContents)
{
    m_image = QImage(size, QImage::Format_RGB32);
}
