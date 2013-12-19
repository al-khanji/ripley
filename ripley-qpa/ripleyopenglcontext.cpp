#include "ripleyintegration.h"
#include "ripleyopenglcontext.h"
#include "ripleydevice.h"
#include "ripleywindow.h"
#include "ripleyhelpers.h"

#include <QtGui/QOpenGLContext>
#include <QtPlatformSupport/private/qeglconvenience_p.h>
#include <QtGui/qpa/qplatformsurface.h>
#include <QtGui/qpa/qplatformwindow.h>

RipleyOpenGLContext::RipleyOpenGLContext(QOpenGLContext *context)
    : QPlatformOpenGLContext()
{
    EGLDisplay display = RipleyIntegration::instance()->device()->eglDisplay();
    EGLConfig config = q_configFromGLFormat(display, tweakSurfaceFormat(context->format()), true);

    static EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, context->format().majorVersion(),
        EGL_NONE
    };

    m_format = q_glFormatFromConfig(display, config);
    m_eglContext = eglCreateContext(display, config, 0, contextAttribs);

    if (m_eglContext == EGL_NO_CONTEXT)
        qWarning("RipleyOpenGLContext::RipleyOpenGLContext(): eglError: %x, this: %p",
                 eglGetError(), this);
}

RipleyOpenGLContext::~RipleyOpenGLContext()
{
    EGLDisplay display = RipleyIntegration::instance()->device()->eglDisplay();

    if (m_eglContext != EGL_NO_CONTEXT)
        eglDestroyContext(display, m_eglContext);
}

bool RipleyOpenGLContext::isValid() const
{
    return m_eglContext != EGL_NO_CONTEXT;
}

bool RipleyOpenGLContext::makeCurrent(QPlatformSurface *surface)
{
    RipleyWindow *rw = static_cast<RipleyWindow *>(surface);

    EGLDisplay display = RipleyIntegration::instance()->device()->eglDisplay();
    EGLSurface eglSurface = rw->eglSurface();
    bool ok = eglMakeCurrent(display, eglSurface, eglSurface, m_eglContext);

    if (!ok)
        qWarning("RipleyOpenGLContext::makeCurrent(): eglError: %x, this: %p",
                 eglGetError(), this);

    return ok;
}

void RipleyOpenGLContext::doneCurrent()
{
    bool ok = eglMakeCurrent(RipleyIntegration::instance()->device()->eglDisplay(),
                             EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (!ok)
        qWarning("RipleyOpenGLContext::doneCurrent(): eglError: %x, this: %p",
                 eglGetError(), this);

}

void RipleyOpenGLContext::swapBuffers(QPlatformSurface *surface)
{
    Q_ASSERT(surface);
    RipleyWindow *rw = static_cast<RipleyWindow *>(surface);

    Q_ASSERT(rw);
    Q_ASSERT(rw->gbmSurface());

    eglSwapBuffers(RipleyIntegration::instance()->device()->eglDisplay(),
                   rw->eglSurface());
    RipleyIntegration::instance()->swapBuffers(rw->gbmSurface());
}

void (*RipleyOpenGLContext::getProcAddress(const QByteArray &procName)) ()
{
    return eglGetProcAddress(procName.data());
}


QSurfaceFormat RipleyOpenGLContext::format() const
{
    return m_format;
}
