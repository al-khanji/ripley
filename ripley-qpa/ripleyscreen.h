#ifndef RIPLEYSCREEN_H
#define RIPLEYSCREEN_H

#include <QtGui/qpa/qplatformscreen.h>
#include <xf86drmMode.h>

class RipleyScreen : public QPlatformScreen
{
public:
    explicit RipleyScreen(uint32_t crtc,
                          uint32_t connector,
                          drmModeModeInfo mode,
                          QRect geometry,
                          uint32_t depth,
                          QImage::Format format,
                          QSize physicalSize);

    virtual QRect geometry() const;
    virtual int depth() const;
    virtual QImage::Format format() const;
    virtual QSizeF physicalSize() const;


private:
    uint32_t m_crtc;
    uint32_t m_connector;
    drmModeModeInfo m_mode;
    QRect m_geometry;
    uint32_t m_depth;
    QImage::Format m_format;
    QSize m_physicalSize;
};

#endif // RIPLEYSCREEN_H
