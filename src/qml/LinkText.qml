import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YdGui as Yd

Yd.TextTrailingIcon {
    id: root

    color: mouseArea.containsMouse ? Qt.darker(Yd.Theme.secondary, 1.1) : Yd.Theme.secondary
    icon: "\ue06c"
    objectName: "linkText"
    spacing: 2
    elide: Text.ElideRight
    trailingIcon.font.pixelSize: Yd.Constants.iconSizeSmall

    MouseArea {
        id: mouseArea
        anchors.fill: root
        cursorShape: Qt.PointingHandCursor
        onClicked: Qt.openUrlExternally(root.text)
        hoverEnabled: true
    }
}
