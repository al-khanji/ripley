#ifndef RIPLEYBACKINGSTORE_H
#define RIPLEYBACKINGSTORE_H

#include <QtGui/qpa/qplatformbackingstore.h>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLShaderProgram>

class RipleyBackingStore : public QPlatformBackingStore
{
public:
    explicit RipleyBackingStore(QWindow *window);

    void beginPaint(const QRegion &r);
    virtual QPaintDevice *paintDevice();

    virtual void flush(QWindow *window, const QRegion &region, const QPoint &offset);
    virtual void resize(const QSize &size, const QRegion &staticContents);

private:
    QImage m_image;
    QRegion m_dirty;
};

#endif // RIPLEYBACKINGSTORE_H
