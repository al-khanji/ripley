#ifndef RIPLEYHELPERS_H
#define RIPLEYHELPERS_H

#include <QtGui/QSurfaceFormat>

inline QSurfaceFormat tweakSurfaceFormat(const QSurfaceFormat &format)
{
    QSurfaceFormat fmt = format;
    fmt.setRedBufferSize(8);
    fmt.setGreenBufferSize(8);
    fmt.setBlueBufferSize(8);
    if (fmt.alphaBufferSize() != -1)
        fmt.setAlphaBufferSize(8);
    return fmt;
}

#endif // RIPLEYHELPERS_H
