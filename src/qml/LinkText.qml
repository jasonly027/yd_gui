import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YdGui as Yd

Yd.TextTrailingIcon {
    id: root

    ToolTip.delay: Yd.Constants.toolTipDelay
    ToolTip.text: qsTr("Original link")
    ToolTip.visible: mouseArea.containsMouse
    color: mouseArea.containsMouse ? Qt.darker(Yd.Theme.link, 1.1) : Yd.Theme.link
    elide: Text.ElideRight
    icon: "\ue06c"
    objectName: "linkText"
    spacing: 2
    trailingIcon.font.pixelSize: Yd.Constants.iconSizeSmall

    MouseArea {
        id: mouseArea

        anchors.fill: root
        cursorShape: Qt.PointingHandCursor
        hoverEnabled: true

        onClicked: Qt.openUrlExternally(root.text)
    }
}
