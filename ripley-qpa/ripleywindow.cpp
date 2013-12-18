#include "ripleywindow.h"

RipleyWindow::RipleyWindow(QWindow *window)
    : QPlatformWindow(window)
{
}

EGLSurface *RipleyWindow::eglSurface() const
{
    return NULL;
}

void RipleyWindow::swapBuffers()
{

}
