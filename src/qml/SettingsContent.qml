import QtQuick
import QtQuick.Controls
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
    CheckBox {
        id: downloadThumbnailCheckBox

        Layout.alignment: Qt.AlignCenter
        checked: _settings.downloadThumbnail
        onCheckedChanged: _settings.downloadThumbnail = checked
        palette.windowText: Yd.Theme.neutral
        text: qsTr("Download thumbnail by default")
    }
    Yd.RaisedButton {
        id: clearHistoryButton

        Layout.alignment: Qt.AlignCenter
        color: Yd.Theme.secondary
        text: qsTr("Clear History")

        onClicked: clearHistory.open()

        Yd.ClearHistoryDialog {
            id: clearHistory

        }
    }
    Item {
        enabled: false
    }
}
