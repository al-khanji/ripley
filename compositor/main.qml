import QtQuick 2.1
import QtQuick.Compositor 1.0
import Compositor 1.0

Compositor {
    id: compositor
    visible: true

    width: 800
    height: 600


    ListView {
        anchors.fill: parent
        model: compositor.model
        delegate: WaylandSurfaceItem { surface: model.surface }
    }
}


