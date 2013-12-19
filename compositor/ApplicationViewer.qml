import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Compositor 1.0
import QtQuick.Layouts 1.1

ScrollView {
    id: root

     Row {
        id: row

        add: Transition {
            NumberAnimation { properties: "scale, opacity"; from: 0.3; to: 1.0; easing.type: Easing.InOutQuad; duration: 800 }
            RotationAnimation { properties: "rotation"; from: 0; to: 360; duration: 800 }
        }

        Repeater {
            id: surfaces

            model: compositor.model

            Item {

                height: compositor.height - (root.frameVisible ? 50 : 0)
                width: surfaceItem.width + handle.width

                WaylandSurfaceItem {
                    id: surfaceItem
                    surface: model.surface
                    height: parent.height
                    width: compositor.width - (compositor.width - handle.x)

                    Component.onCompleted: surfaceItem.surface.requestSize(Qt.size(surfaceItem.width, surfaceItem.height))
                }

                Rectangle {
                    id: handle
                    x: compositor.width - width
                    height: parent.height
                    width: visible ? 20 : 0
                    color: "lightgray"
                    opacity: surfaces.count > 1
                    visible: opacity > 0.0
                    Behavior on opacity { NumberAnimation { easing.type: Easing.InOutQuad } }
                    Behavior on width { NumberAnimation { easing.type: Easing.InOutQuad } }

                    MouseArea {
                        id: splitter

                        anchors {
                            top: parent.top; left: parent.left
                            right: parent.right; bottom: parent.verticalCenter
                        }

                        cursorShape: Qt.SplitHCursor

                        drag {
                            target: parent; axis: Drag.XAxis
                            minimumX: Math.round(parent.width / 4)
                            maximumX: compositor.width - parent.width
                        }

                        onReleased: surfaceItem.surface.requestSize(Qt.size(surfaceItem.width, surfaceItem.height))
                    }
                }
            }
        }
    }
}
