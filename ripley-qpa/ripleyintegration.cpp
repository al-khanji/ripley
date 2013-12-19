#include "ripleyintegration.h"
#include "ripleybackingstore.h"
#include "ripleyopenglcontext.h"
#include "ripleywindow.h"
#include "ripleydevice.h"
#include "ripleyscreen.h"

#include <QtGui/private/qpixmap_raster_p.h>
#include <QtPlatformSupport/private/qdevicediscovery_p.h>
#include <QtCore/private/qcore_unix_p.h>
#include <QtPlatformSupport/private/qgenericunixeventdispatcher_p.h>
#include <QtPlatformSupport/private/qgenericunixfontdatabase_p.h>

RipleyIntegration *RipleyIntegration::m_instance = 0;

RipleyIntegration *RipleyIntegration::create()
{
    QDeviceDiscovery *disc = QDeviceDiscovery::create(QDeviceDiscovery::Device_DRM | QDeviceDiscovery::Device_DRM_PrimaryGPU, 0);
    if (!disc) {
        qDebug("Could not start device discovery");
        return 0;
    }

    QStringList devices = disc->scanConnectedDevices();
    if (devices.isEmpty()) {
        qDebug("No candidate devices found");
        return 0;
    }

    QStringListIterator iter(devices);
    int fd = -1;
    while (fd < 0 && iter.hasNext()) {
        QString s = iter.next();
        fd = QT_OPEN(s.toLocal8Bit(), O_RDWR);
    }

    if (fd < 0) {
        qDebug("Could not open any devices");
        return 0;
    }

    RipleyIntegration *ri = new RipleyIntegration(fd);
    connect(disc, &QDeviceDiscovery::deviceDetected,
            ri, &RipleyIntegration::deviceDetected);
    connect(disc, &QDeviceDiscovery::deviceRemoved,
            ri, &RipleyIntegration::deviceRemoved);
    connect(disc, &QDeviceDiscovery::deviceChanged,
            ri, &RipleyIntegration::deviceChanged);
    return ri;
}

RipleyIntegration::RipleyIntegration(int fd)
    : QPlatformIntegration()
    , m_device(new RipleyDevice(fd, this))
{
    Q_ASSERT(!m_instance);
    m_instance = this;

    m_device->scanConnectors();
}

RipleyIntegration::~RipleyIntegration()
{

}

bool RipleyIntegration::hasCapability(QPlatformIntegration::Capability cap) const
{
    return QPlatformIntegration::hasCapability(cap);
}

QPlatformPixmap *RipleyIntegration::createPlatformPixmap(QPlatformPixmap::PixelType type) const
{
    return new QRasterPlatformPixmap(type);
}

QPlatformOpenGLContext *RipleyIntegration::createPlatformOpenGLContext(QOpenGLContext *context) const
{
    return new RipleyOpenGLContext(context);
}

QPlatformFontDatabase *RipleyIntegration::fontDatabase() const
{
    return new QGenericUnixFontDatabase;
}

QAbstractEventDispatcher *RipleyIntegration::createEventDispatcher() const
{
    return createUnixEventDispatcher();
}

void RipleyIntegration::addScreen(uint32_t crtc,
                                  uint32_t connector,
                                  drmModeModeInfo mode,
                                  QRect geometry,
                                  uint32_t depth,
                                  QImage::Format format,
                                  QSize physicalSize)
{
    static int x = 0;

    geometry.setX(x);
    x += geometry.width();
    m_geometry += geometry;

    foreach (RipleyWindow *window, m_windows)
        window->setGeometry(m_geometry.boundingRect());

    qDebug() << "Found a connector of size" << geometry << physicalSize;

    RipleyScreen *rs = new RipleyScreen(crtc, connector, mode, geometry, depth, format, physicalSize);
    m_screens.append(rs);
    screenAdded(rs);
}

void RipleyIntegration::swapBuffers(gbm_surface *surface)
{
qDebug("SWAPPING BUFFERS");

    gbm_bo *locked = gbm_surface_lock_front_buffer(surface);
    if (!locked) {
        qDebug("Could not lock surface front buffer");
        return;
    }

    uint32_t handle = gbm_bo_get_handle(locked).u32;
    uint32_t stride = gbm_bo_get_stride(locked);

    foreach (RipleyScreen *rs, m_screens)
        rs->setupCrtc(handle, stride);

    gbm_surface_release_buffer(surface, locked);
}

void RipleyIntegration::deviceDetected(const QString &deviceNode)
{
    qDebug("DEVICE DETECTED %s", deviceNode.toLocal8Bit().constData());
}

void RipleyIntegration::deviceRemoved(const QString &deviceNode)
{
    qDebug("DEVICE REMOVED %s", deviceNode.toLocal8Bit().constData());
}

void RipleyIntegration::deviceChanged(const QString &deviceNode)
{
    qDebug("DEVICE CHANGED %s", deviceNode.toLocal8Bit().constData());
}

QPlatformBackingStore *RipleyIntegration::createPlatformBackingStore(QWindow *window) const
{
    return new RipleyBackingStore(window);
}

QPlatformWindow *RipleyIntegration::createPlatformWindow(QWindow *window) const
{
    RipleyWindow *w = new RipleyWindow(window);
    w->setGeometry(m_geometry.boundingRect());
    const_cast<RipleyIntegration *>(this)->m_windows.append(w);
    return w;
}
