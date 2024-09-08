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

        Rectangle {
            id: tab

            Layout.fillHeight: true
            bottomLeftRadius: Yd.Constants.boxRadius
            color: Yd.Theme.inputBarTab
            implicitHeight: tabText.implicitHeight + tabText.anchors.topMargin + tabText.anchors.bottomMargin
            implicitWidth: tabText.implicitWidth + tabText.anchors.leftMargin + tabText.anchors.rightMargin
            topLeftRadius: Yd.Constants.boxRadius

            border {
                color: Yd.Theme.inputBar
                width: 2
            }
            MouseArea {
                id: labelMouseArea

                anchors.fill: tab
                hoverEnabled: true
            }
            BusyIndicator {
                hoverEnabled: false
                running: visible
                visible: Yd.Downloader.isFetching

                anchors {
                    fill: parent
                    margins: 4
                }
            }
            Text {
                id: tabText

                color: (input.hovered || labelMouseArea.containsMouse) ? Yd.Theme.primary : Yd.Theme.inputBarIcon
                text: qsTr("Link")
                visible: !Yd.Downloader.isFetching

                font {
                    pixelSize: Qt.application.font.pixelSize * 1.1
                    weight: Font.DemiBold
                }
                anchors {
                    bottomMargin: 10
                    centerIn: parent
                    leftMargin: Yd.Constants.boxPadding
                    rightMargin: Yd.Constants.boxPadding
                    topMargin: 10
                }
            }
        }
        TextField {
            id: input

            Layout.fillHeight: true
            Layout.fillWidth: true
            color: Qt.darker(Yd.Theme.inputBar)
            placeholderTextColor: color
            enabled: !Yd.Downloader.isFetching && Yd.Downloader.programExists && _database.valid
            hoverEnabled: true
            inputMethodHints: Qt.ImhUrlCharactersOnly
            placeholderText: {
                if (!Yd.Downloader.programExists)
                    return qsTr("yt-dlp not found. Verify path in app settings is correct.");
                if (!_database.valid)
                    return qsTr("History failed to load. Please restart.");
                return qsTr("Click to paste URL");
            }

            background: Rectangle {
                bottomRightRadius: Yd.Constants.boxRadius
                color: Yd.Theme.inputBar
                topRightRadius: Yd.Constants.boxRadius
            }

            onAccepted: Yd.Downloader.fetchInfo(text)

            Connections {
                function onIsFetchingChanged() {
                    if (!Yd.Downloader.isFetching)
                        input.clear();
                }

                target: Yd.Downloader
            }
            MouseArea {
                id: inputMouseArea

                anchors.fill: parent
                cursorShape: Qt.IBeamCursor
                enabled: input.text === ""

                onClicked: {
                    input.paste();
                    input.text = input.text.trim();
                    input.forceActiveFocus();
                }
            }
            font {
                italic: input.length === 0
                pixelSize: Qt.application.font.pixelSize
            }
        }
    }
    Yd.DropShadow {
        target: rowLayout
    }
}
