import QtQuick
import QtQuick.Dialogs
import QtCore
import QtQuick.Controls.Basic
import QtQuick.Layouts
import YdGui as Yd

Item {
    id: root

    property int pixelSize: Qt.application.font.pixelSize

    implicitHeight: rowLayout.implicitHeight
    implicitWidth: rowLayout.implicitWidth
    objectName: "inputDownloadDir"

    FolderDialog {
        id: folderDialog

        acceptLabel: "Select"
        currentFolder: _settings.downloadDirValidated()
        title: qsTr("Select a download destination")

        onAccepted: {
            _settings.downloadDir = selectedFolder;
            currentFolder = _settings.downloadDirValidated();
        }
        onRejected: {
            currentFolder = _settings.downloadDirValidated();
        }
    }
    Item {
        id: container

        implicitHeight: rowLayout.implicitHeight
        implicitWidth: rowLayout.implicitWidth

        MouseArea {
            id: mouseArea

            anchors.fill: container
            cursorShape: Qt.PointingHandCursor
            hoverEnabled: true

            onClicked: folderDialog.open()
        }
        RowLayout {
            id: rowLayout

            spacing: 0
            width: root.width

            Label {
                id: label

                Layout.fillHeight: true
                bottomPadding: 5
                color: mouseArea.containsMouse ? Yd.Theme.primary : Yd.Theme.inputBarIcon
                horizontalAlignment: Text.AlignHCenter
                leftPadding: 10
                rightPadding: 10
                text: "\ue07e"
                topPadding: 5
                verticalAlignment: Text.AlignVCenter

                background: Rectangle {
                    id: labelBox

                    bottomLeftRadius: Yd.Constants.boxRadius
                    color: Yd.Theme.inputBarTab
                    topLeftRadius: Yd.Constants.boxRadius

                    border {
                        color: Yd.Theme.inputBar
                        width: 2
                    }
                }

                font {
                    family: "typicons"
                    pixelSize: Yd.Constants.iconSize
                }
            }
            Label {
                id: dir

                Layout.fillHeight: true
                Layout.fillWidth: true
                color: mouseArea.containsMouse ? Qt.darker(Yd.Theme.primary) : Qt.darker(Yd.Theme.neutral)
                font.pixelSize: root.pixelSize
                horizontalAlignment: Text.AlignHCenter
                padding: 10
                text: _settings.downloadDirStr
                elide: Text.ElideMiddle
                verticalAlignment: Text.AlignVCenter

                background: Rectangle {
                    bottomRightRadius: Yd.Constants.boxRadius
                    color: Yd.Theme.inputBar
                    topRightRadius: Yd.Constants.boxRadius
                }
            }
        }
        Yd.DropShadow {
            target: rowLayout
        }
    }
}
