#include "ripleydevice.h"
#include "ripleyintegration.h"
#include "ripleyscreen.h"

#include <xf86drm.h>

RipleyDevice::RipleyDevice(int fd, QObject *parent)
    : QObject(parent)
    , m_fd(fd)
    , m_notifier(new QSocketNotifier(m_fd, QSocketNotifier::Read, this))
{
    connect(m_notifier, SIGNAL(activated(int)), SLOT(socketNotified()));
    m_notifier->setEnabled(true);

    m_gbmDevice = gbm_create_device(m_fd);
    if (!m_gbmDevice)
        qFatal("Could not create gbm device");

    m_eglDisplay = eglGetDisplay(m_gbmDevice);

    if (m_eglDisplay == EGL_NO_DISPLAY) {
        qWarning("Could not open EGL display");
        qFatal("EGL error");
    }

    EGLint major;
    EGLint minor;
    if (!eglInitialize(m_eglDisplay, &major, &minor)) {
        qWarning("Could not initialize EGL display");
        qFatal("EGL error");
    }

//    EGLContext ctx;

//    EGLBoolean ok = eglBindAPI(EGL_OPENGL_ES_API);
//    if (!ok)
//        qWarning("Could not bind API!");

//    ctx = eglCreateContext(m_eglDisplay, NULL, EGL_NO_CONTEXT, NULL);
//    ok = eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, ctx);
//    if (!ok)
//        qWarning("Could not bind null surface on startup!");
}

RipleyDevice::~RipleyDevice()
{
    eglTerminate(m_eglDisplay);
    gbm_device_destroy(m_gbmDevice);
}

void RipleyDevice::scanConnectors()
{
    drmModeResPtr res = drmModeGetResources(m_fd);
    if (!res)
        qFatal("Could not get card resources");

    for (int i = 0; i < res->count_connectors; i++) {
        drmModeConnectorPtr connector = drmModeGetConnector(m_fd, res->connectors[i]);
        if (!connector)
            qFatal("Could not get connector %d", res->connectors[i]);

        if (connector->connection == DRM_MODE_CONNECTED
                && connector->count_modes > 0)
            checkConnector(res, connector);

        drmModeFreeConnector(connector);
    }

    drmModeFreeResources(res);
}

void RipleyDevice::checkConnector(drmModeResPtr res,
                                  drmModeConnectorPtr connector)
{
    if (connector->encoder_id) {
        drmModeEncoderPtr encoder = drmModeGetEncoder(m_fd, connector->encoder_id);
        if (encoder->crtc_id && !m_crtcs.contains(encoder->crtc_id)) {
            drmModeCrtcPtr crtc = drmModeGetCrtc(m_fd, encoder->crtc_id);
            if (!crtc)
                qFatal("Could not get crtc 1");
            addScreen(connector, encoder, crtc);
            drmModeFreeCrtc(crtc);
        }
        drmModeFreeEncoder(encoder);
    } else {
        for (int i = 0; i < connector->count_encoders; i++) {
            drmModeEncoderPtr encoder = drmModeGetEncoder(m_fd, connector->encoders[i]);
            if (!encoder)
                qFatal("Could not get encoder %d", connector->encoders[i]);

            for (int j = 0; j < res->count_crtcs; j++) {
                if (!encoder->possible_crtcs & (1 << j))
                    continue;

                if (m_crtcs.contains(res->crtcs[j]))
                    continue;

                drmModeCrtcPtr crtc = drmModeGetCrtc(m_fd, res->crtcs[j]);
                if (!crtc)
                    qFatal("Could not get crtc 2");

                addScreen(connector, encoder, crtc);
                drmModeFreeCrtc(crtc);
                break;
            }

            drmModeFreeEncoder(encoder);
        }
    }
}

//Fallback mode (taken from Wayland DRM demo compositor)
static drmModeModeInfo builtin_1024x768 = {
    63500, //clock
    1024, 1072, 1176, 1328, 0,
    768, 771, 775, 798, 0,
    59920,
    DRM_MODE_FLAG_NHSYNC | DRM_MODE_FLAG_PVSYNC,
    0,
    "1024x768"
};

void RipleyDevice::addScreen(drmModeConnectorPtr connector, drmModeEncoderPtr encoder, drmModeCrtcPtr crtc)
{
    drmModeModeInfoPtr mode = 0;
    for (int i = 0; i < connector->count_modes; i++) {
        if (connector->modes[i].type & DRM_MODE_TYPE_PREFERRED) {
            mode = &connector->modes[i];
            break;
        }
    }
    if (!mode) {
        qDebug("Using fallback mode");
        mode = &builtin_1024x768;
    }

    m_encoders.append(encoder->encoder_id);
    m_crtcs.append(crtc->crtc_id);

    uint32_t crtc_id = crtc->crtc_id;
    uint32_t encoder_id = encoder->encoder_id;
    uint32_t connector_id = connector->connector_id;
    drmModeModeInfo newMode = *mode;
    QRect geometry(0, 0, mode->hdisplay, mode->vdisplay);
    uint32_t depth = 32;
    QImage::Format format = QImage::Format_RGB32;
    QSize physicalSize(connector->mmWidth, connector->mmHeight);

    RipleyScreen *screen = new RipleyScreen(crtc_id, encoder_id, connector_id, newMode, geometry, depth, format, physicalSize);
    RipleyIntegration::instance()->addScreen(screen);
}

void vblank_handler(int fd,
                    unsigned int sequence,
                    unsigned int tv_sec,
                    unsigned int tv_usec,
                    void *user_data)
{
    qDebug("vblank handler");
}

void page_flip_handler(int fd,
                       unsigned int sequence,
                       unsigned int tv_sec,
                       unsigned int tv_usec,
                       void *user_data)
{
    qDebug("page flip handler");
}

void RipleyDevice::socketNotified()
{
    drmEventContext ctx = {
        DRM_EVENT_CONTEXT_VERSION,
        vblank_handler,
        page_flip_handler
    };

    drmHandleEvent(m_fd, &ctx);
    m_notifier->setEnabled(true);
}
