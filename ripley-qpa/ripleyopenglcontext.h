#ifndef RIPLEYOPENGLCONTEXT_H
#define RIPLEYOPENGLCONTEXT_H

#include <QtGui/qpa/qplatformopenglcontext.h>

#define EGL_EGLEXT_PROTOTYPES 1
#include <EGL/egl.h>

class RipleyOpenGLContext : public QPlatformOpenGLContext
{
public:
    explicit RipleyOpenGLContext(QOpenGLContext *context);

    bool makeCurrent(QPlatformSurface *surface);
    void doneCurrent();
    void swapBuffers(QPlatformSurface *surface);
    void (*getProcAddress(const QByteArray &procName)) ();

    bool isValid() const;

    QSurfaceFormat format() const;

    EGLContext eglContext() const;

private:
    EGLContext m_eglContext;
    QSurfaceFormat m_format;
};

#endif // RIPLEYOPENGLCONTEXT_H
