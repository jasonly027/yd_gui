import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YdGui as Yd

Rectangle {
    id: root

    required property var model
    readonly property alias mouseArea: mouseArea
    required property string selectedFormat
    property color textColor: getTextColor()

    function getDimensionsFps(): string {
        let ratio = root.model.width / root.model.height;
        const expectedRatio = 1.77;
        const epsilon = 0.01;
        let fpsStr = (root.model.fps === 0) ? "" : `${root.model.fps}fps`;
        if ((ratio - expectedRatio) < 0.01) {
            return `${root.model.height}p${fpsStr}`;
        }
        if (fpsStr === "") {
            return `${root.model.width}x${root.model.height}`;
        }
        return `${root.model.width}x${root.model.height} ${fpsStr}`;
    }
    function getTextColor(): color {
        if (mouseArea.containsMouse) {
            return Qt.darker(Yd.Theme.primary, 1.2);
        }
        if (root.model.formatId === selectedFormat) {
            return Qt.darker(Yd.Theme.secondary, 1.2);
        }
        return Yd.Theme.darkMode ? "white" : "black";
    }

    color: mouseArea.containsMouse ? Qt.darker(Yd.Theme.formatComboBoxBg, 1.1) : Yd.Theme.formatComboBoxBg
    implicitHeight: rowLayout.implicitHeight + rowLayout.anchors.topMargin + rowLayout.anchors.bottomMargin
    implicitWidth: rowLayout.implicitWidth + rowLayout.anchors.leftMargin + rowLayout.anchors.rightMargin
    objectName: "formatDelegate"

    RowLayout {
        id: rowLayout

        anchors {
            fill: root
            leftMargin: 10
            rightMargin: 10
            topMargin: 5
            bottomMargin: 5
        }
        Text {
            id: dimensionsFpsText

            Layout.fillWidth: true
            Layout.maximumWidth: implicitWidth
            Layout.minimumWidth: 0
            color: root.textColor
            elide: Text.ElideRight
            text: getDimensionsFps()
        }
        Label {
            id: containerText

            Layout.fillWidth: true
            Layout.maximumWidth: implicitWidth
            Layout.minimumWidth: 0
            color: root.textColor
            elide: Text.ElideRight
            font.pixelSize: Qt.application.font.pixelSize * 0.8
            padding: 3
            text: root.model.container

            background: Rectangle {
                color: Qt.darker(Yd.Theme.formatComboBoxBg, 1.2)
                radius: Yd.Constants.boxRadius / 2
            }
        }
        Text {
            id: formatIdText

            Layout.fillWidth: true
            Layout.minimumWidth: 0
            elide: Text.ElideRight
            color: Yd.Theme.darkMode ? "white" : "black"
            font.italic: true
            opacity: 0.7
            text: root.model.formatId
        }
    }
    MouseArea {
        id: mouseArea

        anchors.fill: root
        hoverEnabled: true
    }
}
