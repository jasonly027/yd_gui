import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YdGui as Yd

Item {
    id: root

    property color color: Yd.Theme.neutral
    property alias lineWidth: underline.width
    property alias pixelSize: title.font.pixelSize
    required property string text

    implicitHeight: columnLayout.implicitHeight
    implicitWidth: columnLayout.implicitWidth
    objectName: "header"

    ColumnLayout {
        id: columnLayout

        spacing: 5

        Text {
            id: title

            Layout.alignment: Qt.AlignCenter
            color: root.color
            text: root.text

            font {
                pixelSize: Qt.application.font.pixelSize * 1.2
                weight: Font.DemiBold
            }
        }
        Rectangle {
            id: underline

            Layout.alignment: Qt.AlignCenter
            color: root.color
            implicitHeight: 2
            implicitWidth: title.implicitWidth + 150
            radius: 2
        }
    }
}
