import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YdGui as Yd

Item {
    id: root

    required property string modelData

    implicitHeight: thumbnail.height
    implicitWidth: thumbnail.width
    objectName: "themeDelegate"

    Image {
        id: thumbnail

        source: `qrc:/images/${root.modelData}`
        sourceSize.width: width
        width: 160

        Rectangle {
            anchors.fill: thumbnail
            color: "transparent"
            radius: Yd.Constants.boxRadius * 0.75

            border {
                color: mouseArea.containsMouse ? Yd.Theme.primary : (modelData === _settings.theme ? Yd.Theme.secondary : Yd.Theme.neutral)
                width: 4
            }
        }
    }
    MouseArea {
        id: mouseArea

        anchors.fill: root
        cursorShape: Qt.PointingHandCursor
        hoverEnabled: true

        onClicked: {
            _settings.theme = modelData;
        }
    }
}
