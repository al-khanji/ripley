#include "ripleyintegration.h"
#include "ripleywindow.h"
#include "ripleydevice.h"
#include "ripleyhelpers.h"

#include <QDebug>
#include <QtPlatformSupport/private/qeglconvenience_p.h>


RipleyWindow::RipleyWindow(QWindow *window)
    : QPlatformWindow(window)
    , m_gbmSurface(0)
    , m_eglSurface(EGL_NO_SURFACE)
{
    window->setSurfaceType(QSurface::OpenGLSurface);
}

void RipleyWindow::setGeometry(const QRect &rect)
{
    qDebug("SET GEOMETRY");

    QPlatformWindow::setGeometry(rect);

    RipleyIntegration *ri = RipleyIntegration::instance();
    EGLDisplay display = ri->device()->eglDisplay();

    if (m_eglSurface)
        eglDestroySurface(display, m_eglSurface);

    if (m_gbmSurface)
        gbm_surface_destroy(m_gbmSurface);

    qDebug() << "Creating gbm surface of size" << rect.size();

    m_gbmSurface = gbm_surface_create(ri->device()->gbmDevice(),
                                      rect.width(), rect.height(),
                                      GBM_BO_FORMAT_XRGB8888,
                                      GBM_BO_USE_RENDERING | GBM_BO_USE_SCANOUT);

    if (!m_gbmSurface)
        qFatal("Could not create gbm buffer object");

    QSurfaceFormat format = tweakSurfaceFormat(window()->requestedFormat());
    EGLConfig config = q_configFromGLFormat(display, format, false);
    m_eglSurface = eglCreateWindowSurface(display, config, (EGLNativeWindowType) m_gbmSurface, NULL);
    if (!m_eglSurface)
        qFatal("Could not create egl window surface");

    qDebug() << "Surface format:" << format;
    qDebug("EGL Config follows");
    q_printEglConfig(display, config);

    glClear(GL_COLOR_BUFFER_BIT);
    eglSwapBuffers(display, m_eglSurface);
}

gbm_surface *RipleyWindow::gbmSurface() const
{
    return m_gbmSurface;
}

EGLSurface RipleyWindow::eglSurface() const
{
    return m_eglSurface;
}
