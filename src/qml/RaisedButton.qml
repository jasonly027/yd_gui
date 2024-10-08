import QtQuick
import QtQuick.Controls.Basic
import YdGui as Yd

Item {
    id: root

    property alias button: button
    property alias color: background.color
    property alias shadowColor: shadow.color
    property int shadowHeight: 5
    property alias text: button.text
    property alias textColor: button.palette.buttonText

    signal clicked(MouseEvent ev)

    implicitHeight: dropShadowTarget.implicitHeight
    implicitWidth: dropShadowTarget.implicitWidth
    objectName: "raisedButton"
    state: "up"

    states: [
        State {
            name: "up"

            PropertyChanges {
                target: button
                y: 0
            }
            PropertyChanges {
                anchors.bottomMargin: -shadowHeight
                target: shadow
            }
        },
        State {
            name: "down"

            PropertyChanges {
                target: button
                y: shadowHeight
            }
            PropertyChanges {
                anchors.bottomMargin: 0
                target: shadow
            }
        }
    ]
    transitions: [
        Transition {
            from: "up"
            reversible: true
            to: "down"

            PropertyAnimation {
                duration: 40
                properties: "y"
                target: button
            }
            PropertyAnimation {
                duration: 40
                properties: "anchors.bottomMargin"
                target: shadow
            }
        }
    ]

    Item {
        id: dropShadowTarget

        implicitHeight: button.implicitHeight + shadowHeight
        implicitWidth: button.implicitWidth

        Button {
            id: button

            readonly property alias hoverHandler: hoverHandler

            onClicked: (ev) => root.clicked(ev)

            bottomPadding: 5
            font {
                weight: Font.Medium
                pixelSize: Qt.application.font.pixelSize * 1.1
            }
            leftPadding: Yd.Constants.boxPadding
            palette.buttonText: Yd.Theme.darkMode ? (hoverHandler.hovered ? Qt.darker("white", 1.1) : "white") : (hoverHandler.hovered ? Qt.darker("white", 6) : "black")
            rightPadding: Yd.Constants.boxPadding
            topPadding: 5

            background: Rectangle {
                id: background

                radius: Yd.Constants.buttonRadius
            }

            HoverHandler {
                id: hoverHandler

                cursorShape: Qt.PointingHandCursor
            }
            Connections {
                function onCanceled() {
                    root.state = "up";
                }
                function onPressed() {
                    root.state = "down";
                }
                function onReleased() {
                    root.state = "up";
                }
            }
        }
        Rectangle {
            id: shadow

            anchors.bottom: button.bottom
            anchors.bottomMargin: -shadowHeight
            anchors.fill: button
            color: Qt.darker(background.color, 1.3)
            radius: Yd.Constants.buttonRadius
            z: -1
        }
    }
    Yd.DropShadow {
        target: dropShadowTarget
    }
}
