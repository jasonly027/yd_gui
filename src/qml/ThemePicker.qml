import QtQuick.Effects
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YdGui as Yd

Item {
    id: root

    readonly property list<string> themes: ["darkPurple", "lightPurple"]

    implicitHeight: gridLayout.implicitHeight
    implicitWidth: gridLayout.implicitWidth
    objectName: "themePicker"

    GridLayout {
        id: gridLayout

        columnSpacing: 20
        columns: 2
        rowSpacing: 30

        Repeater {
            model: root.themes

            delegate: Item {
                id: delegateRoot

                required property string modelData

                implicitHeight: thumbnail.height
                implicitWidth: thumbnail.width

                Image {
                    id: thumbnail

                    source: `qrc:/images/${delegateRoot.modelData}`
                    sourceSize.width: width
                    width: 160

                    Rectangle {
                        anchors.fill: thumbnail
                        border.color: Yd.Theme.secondary
                        border.width: 5
                        radius: Yd.Constants.boxRadius
                        color: "transparent"
                    }
                }

                MouseArea {
                    anchors.fill: delegateRoot
                    onClicked: _settings.theme = modelData
                }
            }
        }
    }
}
