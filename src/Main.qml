import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtCore
import YdGui as Yd

ApplicationWindow {
    id: root

    color: Yd.Theme.bg
    height: 700
    palette.toolTipBase: Yd.Theme.darkMode ? "black" : "white"
    palette.toolTipText: Yd.Theme.darkMode ? "white" : "black"
    visible: true
    width: 900

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
                Layout.maximumWidth: videosList.width
            }
            Item {
                id: queueButtonsAndSettings

                Layout.fillWidth: true
                implicitHeight: queueButtons.implicitHeight
                implicitWidth: Math.max(queueButtons.implicitWidth + settingsDrawer.implicitWidth, settingsDrawer.fullImplicitWidth)

                RowLayout {
                    id: queueButtons

                    anchors.centerIn: queueButtonsAndSettings
                    spacing: 10

                    Yd.RaisedButton {
                        id: downloadAll

                        color: Yd.Theme.downloadAllBtn
                        text: qsTr("Download All")

                        onClicked: Yd.VideoListModel.downloadAllVideos()
                    }
                    Yd.RaisedButton {
                        id: cancelAll

                        color: Yd.Theme.cancelBtn
                        text: qsTr("Cancel All")

                        onClicked: Yd.VideoListModel.cancelAllDownloads()
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

    Connections {
        function onInfoPushed(info) {
            _database.addVideo(info);
        }

        target: Yd.Downloader
    }
    Settings {
        property alias windowHeight: root.height
        property alias windowWidth: root.width
        property alias windowX: root.x
        property alias windowY: root.y
    }
    SplitView {
        id: splitView

        anchors.fill: parent
        orientation: Qt.Vertical

        handle: Rectangle {
            id: handleDelegate

            implicitHeight: 0
            implicitWidth: splitView.width

            containmentMask: Item {
                height: errorConsole.previewHeight
                width: splitView.width
            }
        }

        Pane {
            id: videosListPane

            SplitView.fillHeight: true
            focusPolicy: Qt.ClickFocus
            padding: 0

            background: Rectangle {
                color: "transparent"
            }

            Yd.VideosListContent {
                id: videosList

                anchors {
                    fill: parent
                    leftMargin: 20
                    rightMargin: 20
                    topMargin: 20
                }
            }
        }
        Pane {
            id: consolePane

            SplitView.minimumHeight: errorConsole.previewHeight
            SplitView.preferredHeight: errorConsole.previewHeight
            focusPolicy: Qt.ClickFocus
            padding: 0

            background: Rectangle {
                color: "transparent"
            }

            Yd.Console {
                id: errorConsole

                anchors.fill: parent
            }
        }
    }
}
