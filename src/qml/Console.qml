import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import YdGui as Yd

Rectangle {
    id: root

    readonly property int previewHeight: previewLayout.implicitHeight + columnLayout.spacing + previewLayout.Layout.topMargin

    signal newMessage(string msg)

    color: Yd.Theme.consoleBg
    implicitHeight: columnLayout.implicitHeight
    implicitWidth: columnLayout.implicitWidth
    objectName: "console"

    onNewMessage: msg => {
        previewText.text = msg.replace(/\n$/, "");
        text.text += msg;
    }

    Connections {
        function onErrorPushed(err) {
            root.newMessage(err);
        }

        target: _settings
    }
    Connections {
        function onErrorPushed(err) {
            root.newMessage(err);
        }

        target: _database
    }
    Connections {
        function onStandardErrorPushed(err) {
            root.newMessage(err);
        }

        target: Yd.Downloader
    }
    ColumnLayout {
        id: columnLayout

        anchors.fill: root
        spacing: 10

        RowLayout {
            id: previewLayout

            Layout.leftMargin: 20
            Layout.rightMargin: 20
            Layout.topMargin: 10
            spacing: 10

            Text {
                id: documentIcon

                ToolTip.delay: Yd.Constants.toolTipDelay
                ToolTip.text: qsTr("Console Messages")
                ToolTip.visible: documentIconHoverHandler.hovered
                color: Yd.Theme.neutral
                text: "\ue062"

                font {
                    family: "typicons"
                    pixelSize: Yd.Constants.iconSizeMedium
                }
                HoverHandler {
                    id: documentIconHoverHandler

                }
            }
            Text {
                id: previewText

                Layout.alignment: Qt.AlignTop
                Layout.fillWidth: true
                color: Yd.Theme.neutral
                elide: Text.ElideRight
                maximumLineCount: 1
            }
        }
        Rectangle {
            id: seperator

            Layout.fillWidth: true
            Layout.leftMargin: 20
            Layout.rightMargin: 20
            color: Yd.Theme.neutral
            implicitHeight: 2
            radius: Yd.Constants.boxRadius
        }
        ScrollView {
            id: textScrollView

            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.leftMargin: 20
            Layout.rightMargin: 20

            Flickable {
                id: textFlickable

                boundsBehavior: Flickable.StopAtBounds
                clip: true
                contentHeight: text.implicitHeight
                contentWidth: text.implicitWidth

                TextArea {
                    id: text

                    color: Yd.Theme.neutral
                    readOnly: true

                    background: Rectangle {
                        color: "transparent"
                    }
                }
            }
        }
    }
}
