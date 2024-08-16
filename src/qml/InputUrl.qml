import QtQuick
import QtQuick.Effects
import QtQuick.Controls.Basic
import QtQuick.Layouts
import YdGui as Yd

Item {
    id: root

    property int pixelSize: Qt.application.font.pixelSize

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
            font {
                pixelSize: root.pixelSize
                weight: Font.DemiBold
            }
            horizontalAlignment: Text.AlignHCenter
            leftPadding: 20
            rightPadding: 20
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
                pixelSize: root.pixelSize
            }
        }
    }
    Yd.DropShadow {
        target: rowLayout
    }
}
