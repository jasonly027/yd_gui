import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import YdGui as Yd

ColumnLayout {
    id: root

    objectName: "settingsContent"
    spacing: Yd.Constants.spacing

    Item {
        enabled: false
    }
    Yd.InputDownloadDir {
        id: inputDownloadDir

        Layout.alignment: Qt.AlignCenter
        Layout.preferredWidth: themePicker.width
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

        button.onClicked: clearHistory.open()

        Yd.ClearHistoryDialog {
            id: clearHistory

        }
    }
    Item {
        enabled: false
    }
}
