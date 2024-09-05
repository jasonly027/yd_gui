import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YdGui as Yd

Dialog {
    id: root

    anchors.centerIn: parent
    modal: true
    objectName: "clearHistoryDialog"
    padding: Yd.Constants.boxPadding
    parent: Overlay.overlay
    onAccepted: Yd.VideoListModel.removeAllVideos()

    background: Rectangle {
        id: rootBackground

        color: Yd.Theme.settingsBg
        radius: Yd.Constants.boxRadius

        border {
            color: Qt.darker(rootBackground.color, 1.1)
            width: 2
        }
    }
    contentItem: ColumnLayout {
        id: columnLayout

        spacing: 15

        Text {
            id: header

            Layout.alignment: Qt.AlignCenter
            color: Yd.Theme.darkMode ? "white" : "black"
            font.pixelSize: Qt.application.font.pixelSize * 1.2
            font.weight: Font.Medium
            text: qsTr("Clear History?")
        }
        Text {
            id: body

            Layout.alignment: Qt.AlignCenter
            Layout.preferredWidth: buttons.implicitWidth
            color: Yd.Theme.neutral
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("This will also remove any currently downloading videos")
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.WordWrap
        }
        Item {
            id: buttons

            Layout.alignment: Qt.AlignCenter
            implicitHeight: rowLayout.height
            implicitWidth: rowLayout.width * 1.5

            RowLayout {
                id: rowLayout

                anchors.centerIn: buttons
                spacing: Yd.Constants.spacing

                Yd.RaisedButton {
                    Layout.alignment: Qt.AlignCenter
                    color: Yd.Theme.clearHistoryBtn
                    id: "clearButton"
                    text: qsTr("Clear")

                    onClicked: root.accept()
                }
                Yd.RaisedButton {
                    Layout.alignment: Qt.AlignCenter
                    color: Yd.Theme.cancelBtn
                    id: "cancelButton"
                    text: qsTr("Cancel")

                    onClicked: root.reject()
                }
            }
        }
    }
}
