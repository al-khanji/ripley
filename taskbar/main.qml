import QtQuick 2.1
import QtQuick.Window 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import DesktopEntry 1.0

ApplicationWindow {

    visible: true
    width: 800
    height: 100

    toolBar: ToolBar {
        RowLayout {
            anchors.fill: parent
            Repeater {
                model: DesktopModel
                Button {

                    style: ButtonStyle {
                        background: Rectangle {
                            radius: 5
                            color: control.pressed || control.checked ? "#333" : "#ccc"
                            anchors.fill: parent
                        }
                        label: Column {
                            Image {
                                anchors.horizontalCenter: parent.horizontalCenter
                                width: 96
                                height: 96
                                source: control.iconSource
                            }
                            Text {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: control.text
                                color: control.pressed || control.checked ? "#fff" : "#333"
                            }
                        }
                    }

                    iconSource: model.icon
                    checkable: true
                    checked: model.process.state > 0
                    text: model.name
                    tooltip: model.comment
                    onClicked: checked ? process.start() : process.terminate()
                    Connections {
                        target: model.process
                        onStateChanged: console.log('state', model.process.state)
                    }
                }
            }
        }
    }
}
