import QtQuick 2.0
import QtQuick.Controls 1.1
//import QtQml 2.2
import QtQuick.Compositor 1.0
import QtQuick.Layouts 1.1
//import QtQuick.Window 2.1

Row {
    id: root

    property alias model: surfaces.model
    property alias delegate: surfaces.delegate

    add: Transition {
        NumberAnimation { properties: "scale, opacity"; from: 0.3; to: 1.0; easing.type: Easing.InOutQuad; duration: 800 }
        RotationAnimation { properties: "rotation"; from: 0; to: 360; duration: 800 }
    }

    Repeater {
        id: surfaces

        Item {
            height: compositor.height
            width: compositor.width / 2

            WaylandSurfaceItem {
                id: surfaceItem
                surface: model.surface
                height: parent.height
                width: parent.width - (parent.width - handle.x)

                Component.onCompleted: surfaceItem.surface.requestSize(Qt.size(surfaceItem.width, surfaceItem.height))
            }

            Rectangle {
                id: handle
                x: parent.width - width
                height: parent.height
                width: visible ? 40 : 0
                color: "lightgray"
                opacity: surfaces.count > 1 && index != surfaces.count - 1 ? 1.0 : 0.0
                visible: opacity > 0.0
                Behavior on opacity { NumberAnimation { easing.type: Easing.InOutQuad } }
                Behavior on width { NumberAnimation { easing.type: Easing.InOutQuad } }

                MouseArea {
                    id: mouse

                    cursorShape: Qt.SplitHCursor

                    anchors.fill: parent
                    drag {
                        target: parent
                        axis: Drag.XAxis
                        minimumX: Math.round(parent.width / 4)
                        maximumX: compositor.width - parent.width
                    }

                    onReleased: surfaceItem.surface.requestSize(Qt.size(surfaceItem.width, surfaceItem.height))
                }
            }
        }
    }
}
