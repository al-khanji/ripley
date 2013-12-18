#include "ripleyintegration.h"
#include "ripleyopenglcontext.h"
#include "ripleydevice.h"
#include "ripleywindow.h"

#include <QtGui/QOpenGLContext>
#include <QtPlatformSupport/private/qeglconvenience_p.h>
#include <QtGui/qpa/qplatformsurface.h>
#include <QtGui/qpa/qplatformwindow.h>

RipleyOpenGLContext::RipleyOpenGLContext(QOpenGLContext *context)
    : QPlatformOpenGLContext()
{
    EGLDisplay display = RipleyIntegration::instance()->device()->eglDisplay();
    EGLConfig config = q_configFromGLFormat(display, context->format(), true);
    m_format = q_glFormatFromConfig(display, config);

    //Initialize EGLContext
    static EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, context->format().majorVersion(),
        EGL_NONE
    };

    eglBindAPI(EGL_OPENGL_ES_API);
    m_eglContext = eglCreateContext(display, config, 0, contextAttribs);
    if (m_eglContext == EGL_NO_CONTEXT) {
        qWarning("QKmsContext::QKmsContext(): eglError: %x, this: %p",
                 eglGetError(), this);
        m_eglContext = 0;
    }
}

bool RipleyOpenGLContext::isValid() const
{
    return m_eglContext != EGL_NO_CONTEXT;
}

bool RipleyOpenGLContext::makeCurrent(QPlatformSurface *surface)
{
    EGLDisplay display = RipleyIntegration::instance()->device()->eglDisplay();

    QPlatformWindow *window = static_cast<QPlatformWindow *>(surface);
    RipleyWindow *rwindow = static_cast<RipleyWindow *>(window);

    EGLSurface eglSurface = rwindow->eglSurface();
    bool ok = eglMakeCurrent(display, eglSurface, eglSurface, m_eglContext);

    if (!ok)
        qWarning("RipleyOpenGLContext::makeCurrent(): eglError: %x, this: %p",
                 eglGetError(), this);

    return true;
}

void RipleyOpenGLContext::doneCurrent()
{
    bool ok = eglMakeCurrent(RipleyIntegration::instance()->device()->eglDisplay(),
                             EGL_NO_SURFACE, EGL_NO_SURFACE,
                             EGL_NO_CONTEXT);
    if (!ok)
        qWarning("RipleyOpenGLContext::doneCurrent(): eglError: %x, this: %p",
                 eglGetError(), this);

}

void RipleyOpenGLContext::swapBuffers(QPlatformSurface *surface)
{
    RipleyWindow *window = static_cast<RipleyWindow *>(surface);
    window->swapBuffers();
}

void (*RipleyOpenGLContext::getProcAddress(const QByteArray &procName)) ()
{
    return eglGetProcAddress(procName.data());
}


EGLContext RipleyOpenGLContext::eglContext() const
{
    return m_eglContext;
}

QSurfaceFormat RipleyOpenGLContext::format() const
{
    return m_format;
}
