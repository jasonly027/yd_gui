import QtQuick
import QtQuick.Effects
import QtQuick.Controls.Basic
import QtQuick.Layouts
import YdGui as Yd

Item {
    id: root

    implicitHeight: rowLayout.implicitHeight
    implicitWidth: rowLayout.implicitWidth
    objectName: "inputUrl"

    RowLayout {
        id: rowLayout

        spacing: 0
        width: root.width

        Label {
            id: label

            Layout.fillHeight: true
            bottomPadding: 10
            color: (input.hovered || labelMouseArea.containsMouse) ? Yd.Theme.primary : Yd.Theme.inputBarIcon
            horizontalAlignment: Text.AlignHCenter
            leftPadding: Yd.Constants.boxPadding
            rightPadding: Yd.Constants.boxPadding
            text: qsTr("Link")
            topPadding: 10
            verticalAlignment: Text.AlignVCenter

            background: Rectangle {
                id: labelBox

                bottomLeftRadius: Yd.Constants.boxRadius
                color: Yd.Theme.inputBarTab
                topLeftRadius: Yd.Constants.boxRadius

                border {
                    color: Yd.Theme.inputBar
                    width: 2
                }
            }

            font {
                pixelSize: Qt.application.font.pixelSize * 1.1
                weight: Font.DemiBold
            }
            MouseArea {
                id: labelMouseArea

                anchors.fill: label
                hoverEnabled: true
            }
        }
        TextField {
            id: input

            Layout.fillHeight: true
            Layout.fillWidth: true
            hoverEnabled: true
            placeholderText: qsTr("Click to paste URL")

            background: Rectangle {
                bottomRightRadius: Yd.Constants.boxRadius
                color: Yd.Theme.inputBar
                topRightRadius: Yd.Constants.boxRadius
            }

            font {
                italic: input.length == 0
                pixelSize: Qt.application.font.pixelSize
            }
        }
    }
    Yd.DropShadow {
        target: rowLayout
    }
}
