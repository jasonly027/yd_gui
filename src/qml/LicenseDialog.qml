import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YdGui as Yd

Dialog {
    id: root

    anchors.centerIn: parent
    modal: true
    objectName: "licenseDialog"
    padding: Yd.Constants.boxPadding
    parent: Overlay.overlay

    background: Rectangle {
        id: rootBackground

        color: Yd.Theme.settingsBg
        radius: Yd.Constants.boxRadius

        border {
            color: Qt.darker(rootBackground.color, 1.1)
            width: 2
        }
    }
    contentItem: ColumnLayout {
        id: columnLayout

        spacing: 15

        Text {
            id: header

            Layout.alignment: Qt.AlignCenter
            color: Yd.Theme.darkMode ? "white" : "black"
            font.pixelSize: Qt.application.font.pixelSize * 1.2
            font.weight: Font.Medium
            text: qsTr("Qt LGPL3 License")
        }
        ScrollView {
            Layout.fillHeight: true
            Layout.minimumHeight: 0
            Flickable {
                boundsBehavior: Flickable.StopAtBounds
                contentHeight: body.implicitHeight
                contentWidth: body.implicitWidth
                clip: true

                TextArea {
                    id: body

                    readOnly: true
                    text: _qt_legal
                }
            }
        }
    }
}
