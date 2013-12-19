import QtQuick 2.1
import QtQuick.Compositor 1.0
import Compositor 1.0

Compositor {
    id: compositor
    visible: true

    width: 1024
    height: 768

    ApplicationViewer { anchors.fill: parent }
}


