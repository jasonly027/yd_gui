import QtQuick
import QtQuick.Dialogs
import QtCore
import QtQuick.Controls.Basic
import QtQuick.Layouts
import YdGui as Yd

Item {
    id: root

    property int pixelSize: Qt.application.font.pixelSize

    ToolTip.delay: Yd.Constants.toolTipDelay
    ToolTip.text: qsTr("Change path to yt-dlp")
    ToolTip.visible: mouseArea.containsMouse
    implicitHeight: rowLayout.implicitHeight
    implicitWidth: rowLayout.implicitWidth
    objectName: "inputYtdlp"

    FileDialog {
        id: fileDialog

        acceptLabel: qsTr("Select")
        title: qsTr("Select yt-dlp")

        onAccepted: {
            _settings.ytdlp = selectedFile;
            Yd.Downloader.checkProgram();
        }
    }
    MouseArea {
        id: mouseArea

        anchors.fill: rowLayout
        cursorShape: Qt.PointingHandCursor
        hoverEnabled: true

        onClicked: {
            fileDialog.open();
        }
    }
    RowLayout {
        id: rowLayout

        anchors.fill: root
        spacing: 0

        Label {
            id: label

            Layout.fillHeight: true
            bottomPadding: 5
            color: mouseArea.containsMouse ? Yd.Theme.primary : Yd.Theme.inputBarIcon
            horizontalAlignment: Text.AlignHCenter
            leftPadding: 10
            rightPadding: 10
            text: "\ue053"
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
                pixelSize: Yd.Constants.iconSizeLarge
            }
        }
        Label {
            id: dir

            Layout.fillHeight: true
            Layout.fillWidth: true
            color: mouseArea.containsMouse ? Qt.darker(Yd.Theme.primary) : Qt.darker(Yd.Theme.neutral)
            elide: Text.ElideMiddle
            font.pixelSize: root.pixelSize
            horizontalAlignment: Text.AlignHCenter
            padding: 10
            text: _settings.ytdlpStr
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
