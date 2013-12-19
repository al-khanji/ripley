#ifndef RIPLEYOPENGLCONTEXT_H
#define RIPLEYOPENGLCONTEXT_H

#include <QtPlatformSupport/private/qeglplatformcontext_p.h>

class RipleyOpenGLContext : public QEGLPlatformContext
{
public:
    explicit RipleyOpenGLContext(QOpenGLContext *context);

    virtual void swapBuffers(QPlatformSurface *surface);

protected:
    virtual EGLSurface eglSurfaceForPlatformSurface(QPlatformSurface *surface);
};

#endif // RIPLEYOPENGLCONTEXT_H
