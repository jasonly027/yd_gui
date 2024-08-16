import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import YdGui as Yd

Item {
    id: root

    implicitHeight: columnLayout.implicitHeight
    implicitWidth: columnLayout.implicitWidth
    objectName: "settingsContent"

    ColumnLayout {
        id: columnLayout

        spacing: 20

        Yd.InputDownloadDir {
            id: inputDownloadDir

            Layout.preferredWidth: themesHeader.width + 150
            Layout.alignment: Qt.AlignCenter
        }
        Yd.Header {
            id: themesHeader

            Layout.alignment: Qt.AlignCenter
            text: qsTr("Themes")
        }
        Yd.ThemePicker {
            id: themePicker

            Layout.alignment: Qt.AlignCenter
        }
        Yd.RaisedButton {
            id: clearHistoryButton

            Layout.alignment: Qt.AlignCenter
            color: Yd.Theme.secondary
            text: qsTr("Clear History")
        }
    }
}
