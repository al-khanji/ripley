#ifndef RIPLEYOPENGLCONTEXT_H
#define RIPLEYOPENGLCONTEXT_H

#include <QtGui/qpa/qplatformopenglcontext.h>

#define EGL_EGLEXT_PROTOTYPES 1
#include <EGL/egl.h>

class RipleyWindow;

class RipleyOpenGLContext : public QPlatformOpenGLContext
{
public:
    explicit RipleyOpenGLContext(QOpenGLContext *context);
    virtual ~RipleyOpenGLContext();

    bool makeCurrent(QPlatformSurface *surface);
    void doneCurrent();
    void swapBuffers(QPlatformSurface *surface);
    void (*getProcAddress(const QByteArray &procName)) ();

    bool isValid() const;

    QSurfaceFormat format() const;

private:
    EGLContext m_eglContext;
    QSurfaceFormat m_format;
};

#endif // RIPLEYOPENGLCONTEXT_H
