#ifndef RIPLEYWINDOW_H
#define RIPLEYWINDOW_H

#include <QtGui/qpa/qplatformwindow.h>

#include <gbm.h>

#define EGL_EGLEXT_PROTOTYPES 1
#include <EGL/egl.h>

class RipleyWindow : public QPlatformWindow
{
public:
    RipleyWindow(QWindow *window);
    EGLSurface *eglSurface() const;

    void swapBuffers();

private:
    EGLSurface *m_eglSurface;
};

#endif // RIPLEYWINDOW_H
