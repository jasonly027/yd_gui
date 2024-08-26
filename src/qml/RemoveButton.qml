import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YdGui as Yd

Text {
    id: root

    property alias mouseArea: mouseArea

    signal clicked(MouseEvent ev)

    color: mouseArea.containsMouse ? Yd.Theme.primary : Yd.Theme.neutral
    objectName: "removeButton"
    opacity: 0.6
    text: "\ue122"

    font {
        family: "typicons"
        pixelSize: Yd.Constants.iconSizeMedium
    }
    MouseArea {
        id: mouseArea

        anchors.fill: removeButton
        cursorShape: Qt.PointingHandCursor
        hoverEnabled: true

        onClicked: (ev) => root.clicked(ev)
    }
}
