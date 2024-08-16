import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YdGui as Yd

ApplicationWindow {
    id: root

    maximumHeight: 900
    maximumWidth: 700
    minimumHeight: 900
    minimumWidth: 700
    visible: true

    color: Yd.Theme.bg
    // header: Rectangle {
    //     color: "gray"
    //     height: 400
    //     width: root.width
    // }
    menuBar: Pane {
        focusPolicy: Qt.ClickFocus
        implicitHeight: menuBar.implicitHeight
        implicitWidth: menuBar.implicitWidth

        background: Rectangle {
            color: Yd.Theme.bg
        }

        ColumnLayout {
            id: menuBar

            spacing: 20

            Item {
                id: menuBarTopSpacer

            }
            Yd.InputUrl {
                id: inputUrl

                Layout.alignment: Qt.AlignCenter
                Layout.minimumWidth: implicitWidth
                Layout.preferredWidth: root.width * 0.8
            }
            Item {
                id: queueButtonsAndSettings

                Layout.preferredWidth: root.width
                implicitHeight: queueButtons.height

                RowLayout {
                    id: queueButtons

                    anchors.centerIn: parent
                    spacing: 10

                    Yd.RaisedButton {
                        color: Yd.Theme.primary
                        id: downloadAll
                        text: qsTr("Download All")
                    }
                    Yd.RaisedButton {
                        color: Yd.Theme.cancelBtn
                        id: cancelAll
                        text: qsTr("Cancel All")
                    }
                }
                Yd.SettingsDrawer {
                    id: settingsDrawer

                    drawerWidth: Math.min(root.width, drawerImplicitWidth)
                    anchors.right: queueButtonsAndSettings.right
                    anchors.verticalCenter: queueButtonsAndSettings.verticalCenter
                }
            }
        }
    }
}
