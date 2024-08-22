import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YdGui as Yd

ColumnLayout {
    id: root

    property color color: Yd.Theme.neutral
    property alias lineWidth: underline.width
    property alias pixelSize: title.font.pixelSize
    property alias text: title.text

    objectName: "header"
    spacing: 5

    Text {
        id: title

        Layout.alignment: Qt.AlignCenter
        color: root.color

        font {
            pixelSize: Qt.application.font.pixelSize * 1.1
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
