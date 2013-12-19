#include "ripleyintegration.h"
#include "ripleywindow.h"
#include "ripleydevice.h"
#include "ripleyhelpers.h"

#include <QtPlatformSupport/private/qeglconvenience_p.h>


RipleyWindow::RipleyWindow(QWindow *window)
    : QPlatformWindow(window)
    , m_gbmSurface(0)
    , m_eglSurface(0)
{
}

void RipleyWindow::setGeometry(const QRect &rect)
{
    RipleyIntegration *ri = RipleyIntegration::instance();
    EGLDisplay display = ri->device()->eglDisplay();

    if (m_eglSurface)
        eglDestroySurface(display, m_eglSurface);

    if (m_gbmSurface)
        gbm_surface_destroy(m_gbmSurface);

    m_gbmSurface = gbm_surface_create(ri->device()->gbmDevice(),
                                      rect.width(), rect.height(),
                                      GBM_BO_FORMAT_ARGB8888,
                                      GBM_BO_USE_RENDERING | GBM_BO_USE_SCANOUT);
    if (!m_gbmSurface)
        qFatal("Could not create gbm buffer object");

    EGLConfig config = q_configFromGLFormat(display, tweakSurfaceFormat(format()), true);
    m_eglSurface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)m_gbmSurface, NULL);
    if (!m_eglSurface)
        qFatal("Could not create egl window surface");
}

gbm_surface *RipleyWindow::gbmSurface() const
{
    return m_gbmSurface;
}

EGLSurface RipleyWindow::eglSurface() const
{
    return m_eglSurface;
}
