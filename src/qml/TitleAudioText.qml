import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YdGui as Yd

Yd.TextTrailingIcon {
    id: root

    property bool audio: true

    color: Yd.Theme.darkMode ? "white" : "dark"
    icon: audio ? "\ue132" : "\ue133"
    objectName: "titleAudioText"
    trailingIcon.font.pixelSize: Yd.Constants.iconSizeMedium

    title.font {
        pixelSize: Qt.application.font.pixelSize * 1.1
        weight: Font.Medium
    }
    trailingIcon {
        ToolTip.text: audio ? qsTr("Audio available") : qsTr("No audio available")
        ToolTip.visible: audioHoverHandler.hovered
    }
    HoverHandler {
        id: audioHoverHandler

        parent: root.trailingIcon
    }
}
