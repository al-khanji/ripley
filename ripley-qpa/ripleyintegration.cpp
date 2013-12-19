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

#include <xf86drm.h>
#include <xf86drmMode.h>

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
    return ri;
}

RipleyIntegration::RipleyIntegration(int fd)
    : QPlatformIntegration()
    , m_device(new RipleyDevice(fd, this))
    , current_bo(0)
    , next_bo(0)
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

void RipleyIntegration::addScreen(RipleyScreen *screen)
{
    qDebug("INTEGRATION ADD SCREEN");

    static int x = 0;
    QRect geometry = screen->geometry();

    geometry.translate(x, 0);
    x += geometry.width();
    screen->setGeometry(geometry);

    m_geometry += geometry;

    foreach (RipleyWindow *window, m_windows)
        window->setGeometry(m_geometry.boundingRect());

    m_screens.append(screen);
    screenAdded(screen);
}

static void bo_destroy_user_data(struct gbm_bo *bo, void *)
{
    RipleyIntegration::instance()->destroyBufferObject(bo);
}

void RipleyIntegration::setupFrameBuffers(RipleyWindow *rw)
{
    current_bo = gbm_surface_lock_front_buffer(rw->gbmSurface());
    if (!current_bo)
        qFatal("Could not lock front buffer");

    foreach (RipleyScreen *rs, m_screens)
        rs->setupCrtc(current_bo);

    gbm_bo_set_user_data(current_bo, (void *)rw, bo_destroy_user_data);
}

void RipleyIntegration::swapBuffers(RipleyWindow *rw)
{
    next_bo = gbm_surface_lock_front_buffer(rw->gbmSurface());
    foreach (RipleyScreen *rs, m_screens)
        rs->swapBuffers(next_bo);

    gbm_bo_set_user_data(next_bo, (void *)rw, bo_destroy_user_data);

    gbm_surface_release_buffer(rw->gbmSurface(), current_bo);
    current_bo = next_bo;
    next_bo = 0;
    if (!gbm_surface_has_free_buffers(rw->gbmSurface()))
        qFatal("NO FREE BUFFERS");
}

void RipleyIntegration::destroyBufferObject(gbm_bo *bo)
{
    foreach (RipleyScreen *screen, m_screens)
        screen->destroyBufferObject(bo);
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
    qDebug("RipleyIntegration::createPlatformWindow");
    RipleyWindow *w = new RipleyWindow(window);
    w->setGeometry(m_geometry.boundingRect());

    RipleyIntegration *that = const_cast<RipleyIntegration *>(this);
    that->m_windows.append(w);

    return w;
}
