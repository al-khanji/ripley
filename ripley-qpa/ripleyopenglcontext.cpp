#include "ripleyhelpers.h"
#include "ripleyintegration.h"
#include "ripleyopenglcontext.h"
#include "ripleywindow.h"
#include "ripleydevice.h"

#include <QtGui/QOpenGLContext>
#include <QtPlatformSupport/private/qeglplatformcontext_p.h>

RipleyOpenGLContext::RipleyOpenGLContext(QOpenGLContext *context)
    : QEGLPlatformContext(tweakSurfaceFormat(context->format()),
                          NULL,
                          RipleyIntegration::instance()->device()->eglDisplay())
{

}

void RipleyOpenGLContext::swapBuffers(QPlatformSurface *surface)
{
    QEGLPlatformContext::swapBuffers(surface);
    RipleyIntegration::instance()->swapBuffers(static_cast<RipleyWindow *>(surface));
}

EGLSurface RipleyOpenGLContext::eglSurfaceForPlatformSurface(QPlatformSurface *surface)
{
    return static_cast<RipleyWindow *>(surface)->eglSurface();
}
