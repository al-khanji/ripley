#include "ripleyintegration.h"
#include <qpa/qplatformintegrationplugin.h>

class RipleyIntegrationPlugin : public QPlatformIntegrationPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QPA.QPlatformIntegrationFactoryInterface.5.2" FILE "ripley-qpa.json")
public:
    QPlatformIntegration *create(const QString&, const QStringList&);
};

QPlatformIntegration *RipleyIntegrationPlugin::create(const QString& system, const QStringList& paramList)
{
    Q_UNUSED(paramList);

    if (system.toLower() == "ripley")
        return RipleyIntegration::create();

    return 0;
}

#include "main.moc"

