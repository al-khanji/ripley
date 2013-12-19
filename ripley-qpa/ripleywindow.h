#ifndef RIPLEYWINDOW_H
#define RIPLEYWINDOW_H

#include <QtGui/qpa/qplatformwindow.h>

#include <xf86drmMode.h>

extern "C" {
#include <gbm.h>
}

#define EGL_EGLEXT_PROTOTYPES 1
#include <EGL/egl.h>


class RipleyWindow : public QPlatformWindow
{
public:
    RipleyWindow(QWindow *window);

    virtual void setGeometry(const QRect &rect);

    gbm_surface *gbmSurface() const;
    EGLSurface eglSurface() const;

private:
    gbm_surface *m_gbmSurface;
    EGLSurface m_eglSurface;
};

#endif // RIPLEYWINDOW_H
