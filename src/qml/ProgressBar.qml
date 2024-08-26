import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YdGui as Yd

Rectangle {
    id: root

    property alias activeBar: activeBar
    property alias activeColor: activeBar.color
    property real value: 0

    color: "white"
    implicitHeight: 10
    objectName: "progressBar"

    Rectangle {
        id: activeBar

        color: Yd.Theme.primary
        width: root.width * value

        Behavior on width {
            NumberAnimation {
                duration: 350
                easing.type: Easing.InOutQuad
            }
        }

        anchors {
            bottom: root.bottom
            left: root.left
            top: root.top
        }
    }
}
