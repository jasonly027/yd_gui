import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YdGui as Yd

Item {
    id: root

    property color color: "white"
    property alias elide: title.elide
    property alias icon: trailingIcon.text
    property int spacing: 5
    property alias text: title.text
    property alias title: title
    property alias trailingIcon: trailingIcon

    implicitHeight: rowLayout.implicitHeight
    implicitWidth: rowLayout.implicitWidth
    objectName: "textTrailingtrailingIcon"

    RowLayout {
        id: rowLayout

        anchors.fill: root
        spacing: root.spacing

        Text {
            id: title

            Layout.fillWidth: true
            Layout.maximumWidth: implicitWidth
            Layout.minimumWidth: 0
            color: root.color
            elide: Text.ElideMiddle
            maximumLineCount: 1
        }
        Text {
            id: trailingIcon

            clip: true
            color: root.color
            text: root.icon

            font {
                family: "typicons"
                pixelSize: Yd.Constants.iconSizeSmall
                weight: Font.Medium
            }
        }
        Item {
            Layout.fillWidth: true
            enabled: false
        }
    }
}
