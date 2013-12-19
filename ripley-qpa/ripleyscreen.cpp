#include "ripleyscreen.h"
#include "ripleyintegration.h"
#include "ripleydevice.h"

#include <QDebug>

RipleyScreen::RipleyScreen(uint32_t crtc,
                           uint32_t encoder,
                           uint32_t connector,
                           drmModeModeInfo mode,
                           QRect geometry,
                           uint32_t depth,
                           QImage::Format format,
                           QSize physicalSize)
    : QPlatformScreen()
    , m_crtc(crtc)
    , m_encoder(encoder)
    , m_connector(connector)
    , m_mode(mode)
    , m_geometry(geometry)
    , m_depth(depth)
    , m_format(format)
    , m_physicalSize(physicalSize)
{
    m_oldCrtc = drmModeGetCrtc(RipleyIntegration::instance()->device()->fd(),
                               m_crtc);
}

RipleyScreen::~RipleyScreen()
{
    if (m_oldCrtc) {
        drmModeSetCrtc(RipleyIntegration::instance()->device()->fd(),
                       m_oldCrtc->crtc_id, m_oldCrtc->buffer_id,
                       m_oldCrtc->x, m_oldCrtc->y,
                       &m_connector, 1, &m_oldCrtc->mode);
        drmModeFreeCrtc(m_oldCrtc);
    }
}

void RipleyScreen::setGeometry(const QRect &rect)
{
    m_geometry = rect;
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

void RipleyScreen::setupCrtc(gbm_bo *bo)
{
    const int fd = RipleyIntegration::instance()->device()->fd();
    uint32_t handle = gbm_bo_get_handle(bo).u32;
    uint32_t stride = gbm_bo_get_stride(bo);

    uint32_t buf_id;
    drmModeAddFB(fd, m_geometry.width(), m_geometry.height(), 24, 32, stride, handle, &buf_id);
    drmModeSetCrtc(fd, m_crtc, buf_id, m_geometry.x(), m_geometry.y(), &m_connector, 1, &m_mode);
    bo_buf[handle] = buf_id;
}

void RipleyScreen::swapBuffers(gbm_bo *bo)
{
    const int fd = RipleyIntegration::instance()->device()->fd();
    uint32_t handle = gbm_bo_get_handle(bo).u32;
    uint32_t stride = gbm_bo_get_stride(bo);

    uint32_t buf_id;
    uint32_t handles[4], pitches[4], offsets[4];
    handles[0] = handle;
    pitches[0] = stride;
    offsets[0] = 0;

    int ret = drmModeAddFB(fd, m_geometry.width(), m_geometry.height(), 24, 32, stride, handle, &buf_id);
    if (ret < 0) {
        qErrnoWarning("Couldn't add FB");
        return;
    }

    bo_buf[handle] = buf_id;

    ret = drmModeSetCrtc(fd, m_crtc, buf_id, 0, 0, &m_connector, 1, &m_mode);
    if (ret < 0) {
        qErrnoWarning("Couldn't set CRTC");
    }
}

void RipleyScreen::destroyBufferObject(gbm_bo *bo)
{
    uint32_t handle = gbm_bo_get_handle(bo).u32;
    if (bo_buf.contains(handle)) {
        uint32_t buf_id = bo_buf.take(handle);
        drmModeRmFB(RipleyIntegration::instance()->device()->fd(), buf_id);
    }
}
