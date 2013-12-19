#include "ripleyscreen.h"
#include "ripleyintegration.h"
#include "ripleydevice.h"

RipleyScreen::RipleyScreen(uint32_t crtc,
                           uint32_t connector,
                           drmModeModeInfo mode,
                           QRect geometry,
                           uint32_t depth,
                           QImage::Format format,
                           QSize physicalSize)
    : QPlatformScreen()
    , m_crtc(crtc)
    , m_connector(connector)
    , m_mode(mode)
    , m_geometry(geometry)
    , m_depth(depth)
    , m_format(format)
    , m_physicalSize(physicalSize)
    , m_bufId(0)
{
}


QRect RipleyScreen::geometry() const
{
    return m_geometry;
}

int RipleyScreen::depth() const
{
    return m_depth;
}

QImage::Format RipleyScreen::format() const
{
    return m_format;
}

QSizeF RipleyScreen::physicalSize() const
{
    return m_physicalSize;
}

void RipleyScreen::setupCrtc(uint32_t handle, uint32_t stride)
{
    const int fd = RipleyIntegration::instance()->device()->fd();

    if (m_bufId)
        drmModeRmFB(fd, m_bufId);

    drmModeAddFB(fd, m_geometry.width(), m_geometry.height(), 24, 32, stride, handle, &m_bufId);
    drmModeSetCrtc(fd, m_crtc, m_bufId, m_geometry.x(), m_geometry.y(), &m_connector, 1, &m_mode);
}
