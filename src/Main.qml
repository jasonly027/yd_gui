import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YdGui as Yd

ApplicationWindow {
    id: root

    color: Yd.Theme.bg
    height: 700
    palette.toolTipBase: Yd.Theme.darkMode ? "black" : "white"
    palette.toolTipText: Yd.Theme.darkMode ? "white" : "black"
    visible: true
    width: 900

    footer: Pane {
        id: footerPane

        focusPolicy: Qt.ClickFocus
        implicitHeight: 100
        implicitWidth: root.width

        background: Rectangle {
            color: "transparent"
        }
    }
    menuBar: Pane {
        id: menuBarPane

        focusPolicy: Qt.ClickFocus
        padding: 0

        background: Rectangle {
            color: "transparent"
        }

        ColumnLayout {
            id: menuBarLayout

            anchors.fill: parent
            spacing: 20

            Item {
                enabled: false
            }
            Yd.InputUrl {
                id: inputUrl

                Layout.alignment: Qt.AlignCenter
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.minimumWidth: implicitWidth
                Layout.rightMargin: 20
            }
            Item {
                id: queueButtonsAndSettings

                Layout.fillWidth: true
                Layout.minimumWidth: implicitWidth
                implicitHeight: queueButtons.implicitHeight
                implicitWidth: Math.max(queueButtons.implicitWidth + settingsDrawer.implicitWidth, settingsDrawer.fullImplicitWidth)

                RowLayout {
                    id: queueButtons

                    anchors.centerIn: queueButtonsAndSettings
                    spacing: 10

                    Yd.RaisedButton {
                        id: downloadAll

                        color: Yd.Theme.primary
                        text: qsTr("Download All")
                    }
                    Yd.RaisedButton {
                        id: cancelAll

                        color: Yd.Theme.cancelBtn
                        text: qsTr("Cancel All")
                    }
                }
                Yd.SettingsDrawer {
                    id: settingsDrawer

                    anchors.right: queueButtonsAndSettings.right
                    anchors.verticalCenter: queueButtonsAndSettings.verticalCenter
                    drawerWidth: Math.min(root.width, drawerImplicitWidth)
                    maxDrawerHeight: root.height * 0.7
                }
            }
        }
    }

    Pane {
        id: videosListPane

        anchors.fill: parent
        focusPolicy: Qt.ClickFocus
        padding: 0

        background: Rectangle {
            color: "transparent"
        }

        Yd.VideosListContent {
            id: videosList

            anchors.fill: parent
            anchors.margins: 20
        }
    }
}
