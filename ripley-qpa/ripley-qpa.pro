TARGET = qripley

TEMPLATE = lib
CONFIG += plugin

target.path = $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
TARGET = $$qtLibraryTarget($$TARGET)

load(qt_targets)
load(qt_common)

QT += core-private gui-private platformsupport-private opengl-private
DEFINES += MESA_EGL_NO_X11_HEADERS __GBM__
CONFIG += link_pkgconfig egl qpa/genericunixfontdatabase
PKGCONFIG += libdrm libudev egl gbm glesv2

SOURCES =  main.cpp \
    ripleyintegration.cpp \
    ripleywindow.cpp \
    ripleybackingstore.cpp \
    ripleyopenglcontext.cpp \
    ripleydevice.cpp \
    ripleyscreen.cpp

HEADERS += \
    ripleyintegration.h \
    ripleywindow.h \
    ripleybackingstore.h \
    ripleyopenglcontext.h \
    ripleydevice.h \
    ripleyscreen.h \
    ripleyhelpers.h

OTHER_FILES += \
    ripley-qpa.json
