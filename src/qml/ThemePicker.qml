import QtQuick.Effects
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YdGui as Yd

Item {
    id: root

    readonly property list<string> themes: Object.keys(Yd.Theme.themes)
    property alias columns: gridLayout.columns

    implicitHeight: gridLayout.implicitHeight
    implicitWidth: gridLayout.implicitWidth
    objectName: "themePicker"

    GridLayout {
        id: gridLayout

        columnSpacing: Yd.Constants.spacing
        columns: 2
        rowSpacing: Yd.Constants.spacing

        Repeater {
            model: root.themes

            delegate: Yd.ThemeDelegate {
            }
        }
    }
}
