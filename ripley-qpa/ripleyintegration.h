#ifndef RIPLEYINTEGRATION_H
#define RIPLEYINTEGRATION_H

#include <QObject>
#include <QtGui/qpa/qplatformintegration.h>
#include <QtGui/QRegion>

class RipleyDevice;
class RipleyScreen;
class RipleyWindow;

#include <xf86drmMode.h>
#include <gbm.h>

class RipleyIntegration : public QObject, public QPlatformIntegration
{
public:
    static RipleyIntegration *create();

    RipleyIntegration(int fd);
    virtual ~RipleyIntegration();

    virtual bool hasCapability(Capability cap) const;

    virtual QPlatformPixmap *createPlatformPixmap(QPlatformPixmap::PixelType type) const;
    virtual QPlatformWindow *createPlatformWindow(QWindow *window) const ;
    virtual QPlatformBackingStore *createPlatformBackingStore(QWindow *window) const ;
    virtual QPlatformOpenGLContext *createPlatformOpenGLContext(QOpenGLContext *context) const;

    virtual QPlatformFontDatabase *fontDatabase() const;

    virtual QAbstractEventDispatcher *createEventDispatcher() const;

    void addScreen(uint32_t crtc,
                   uint32_t connector,
                   drmModeModeInfo mode,
                   QRect geometry,
                   uint32_t depth,
                   QImage::Format format,
                   QSize physicalSize);

    static RipleyIntegration *instance() {
        return m_instance;
    }

    RipleyDevice *device() const {
        return m_device.data();
    }

    void swapBuffers(gbm_surface *surface);

public slots:
    void deviceDetected(const QString &deviceNode);
    void deviceRemoved(const QString &deviceNode);
    void deviceChanged(const QString &deviceNode);

private:
    static RipleyIntegration *m_instance;

    QScopedPointer<RipleyDevice> m_device;
    QRegion m_geometry;
    QList<RipleyScreen*> m_screens;
    QList<RipleyWindow*> m_windows;
};

#endif // RIPLEYINTEGRATION_H
