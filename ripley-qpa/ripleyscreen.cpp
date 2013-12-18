#include "ripleyscreen.h"
#include "ripleyintegration.h"
#include "ripleydevice.h"

#include <xf86drmMode.h>

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
{
//    drmModeSetCrtc(RipleyIntegration::instance()->device()->fd(),
//                   m_crtc,
//                   RipleyIntegration::instance()->device()->bufferId(),
//                   m_geometry.x(),
//                   m_geometry.y(),
//                   &m_connector,
//                   1,
//                   m_mode);
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
