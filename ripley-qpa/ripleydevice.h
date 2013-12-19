#ifndef RIPLEYDEVICE_H
#define RIPLEYDEVICE_H

#include <QObject>
#include <QSocketNotifier>

#include <xf86drmMode.h>

extern "C" {
#include <gbm.h>
}

#define EGL_EGLEXT_PROTOTYPES 1
#include <EGL/egl.h>

class RipleyDevice : public QObject
{
    Q_OBJECT
public:
    explicit RipleyDevice(int fd, QObject *parent = 0);
    virtual ~RipleyDevice();

    int fd() const {
        return m_fd;
    }
    EGLDisplay eglDisplay() const {
        return m_eglDisplay;
    }
    gbm_device *gbmDevice() const {
        return m_gbmDevice;
    }

    void scanConnectors();
    void checkConnector(drmModeResPtr res,
                        drmModeConnectorPtr connector);
    void addScreen(drmModeConnectorPtr connector,
                   drmModeEncoderPtr encoder,
                   drmModeCrtcPtr crtc);

private slots:
    void socketNotified();

private:
    int m_fd;
    QSocketNotifier *m_notifier;

    EGLDisplay m_eglDisplay;
    EGLContext m_eglContext;
    gbm_device *m_gbmDevice;

    QList<uint32_t> m_encoders;
    QList<uint32_t> m_crtcs;
};

#endif // RIPLEYDEVICE_H
