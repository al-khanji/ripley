#ifndef RIPLEYSCREEN_H
#define RIPLEYSCREEN_H

#include <QtGui/qpa/qplatformscreen.h>
#include <xf86drmMode.h>
#include <gbm.h>
#include <QHash>

class RipleyScreen : public QPlatformScreen
{
public:
    explicit RipleyScreen(uint32_t crtc,
                          uint32_t encoder_id,
                          uint32_t connector,
                          drmModeModeInfo mode,
                          QRect geometry,
                          uint32_t depth,
                          QImage::Format format,
                          QSize physicalSize);
    virtual ~RipleyScreen();

    void setGeometry(const QRect &rect);
    virtual QRect geometry() const;

    virtual int depth() const;
    virtual QImage::Format format() const;
    virtual QSizeF physicalSize() const;

    void setupCrtc(gbm_bo *bo);
    void swapBuffers(gbm_bo *bo);

    void destroyBufferObject(gbm_bo *bo);

private:
    drmModeCrtcPtr m_oldCrtc;
    uint32_t m_crtc;
    uint32_t m_encoder;
    uint32_t m_connector;
    drmModeModeInfo m_mode;
    QRect m_geometry;
    uint32_t m_depth;
    QImage::Format m_format;
    QSize m_physicalSize;

    QHash<uint32_t, uint32_t> bo_buf;
    bool waiting_for_flip;
};

#endif // RIPLEYSCREEN_H
