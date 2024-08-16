import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import YdGui as Yd

Item {
    id: root

    readonly property bool __transitionRunning: (openAnim.running || closeAnim.running)
    property color backgroundColor: Yd.Theme.settingsBg
    property alias drawerImplicitWidth: drawer.implicitWidth
    property alias drawerWidth: drawer.width
    property int transitionDuration: 500

    implicitHeight: tab.implicitHeight
    implicitWidth: tab.implicitWidth
    objectName: "settingsDrawer"
    state: "closed"

    states: [
        State {
            name: "closed"

            PropertyChanges {
                target: anchor
                x: 0
            }
        },
        State {
            name: "opened"

            PropertyChanges {
                target: anchor
                x: -drawer.width
            }
        }
    ]
    transitions: [
        Transition {
            id: openAnim

            from: "closed"
            to: "opened"

            PropertyAnimation {
                duration: root.transitionDuration
                easing.type: Easing.OutQuad
                property: "x"
                target: anchor
            }
        },
        Transition {
            id: closeAnim

            from: "opened"
            to: "closed"

            PropertyAnimation {
                duration: root.transitionDuration * 0.75
                easing.type: Easing.InQuad
                property: "x"
                target: anchor
            }
        }
    ]

    Item {
        id: anchor

        Button {
            id: tab

            bottomPadding: 15
            enabled: !root.__transitionRunning
            font.family: "typicons"
            font.pixelSize: Yd.Constants.iconSize
            hoverEnabled: true
            palette.buttonText: hovered ? Yd.Theme.primary : Yd.Theme.neutral
            text: "\ue04f"
            topPadding: 15

            background: Rectangle {
                bottomLeftRadius: 10
                color: root.backgroundColor
                topLeftRadius: 10
            }

            onClicked: drawer.opened ? drawer.close() : drawer.open()

            HoverHandler {
                cursorShape: Qt.PointingHandCursor
            }
        }
        Yd.DropShadow {
            target: tab
        }
        Rectangle {
            // There's a small pixel gap where the tab and drawer connect
            // that I believe reveals itself during transitioning because
            // of the non-linear easing used to move the shapes.
            id: patchCrevice

            color: root.backgroundColor
            implicitHeight: tab.implicitHeight
            implicitWidth: tab.implicitWidth
            visible: root.__transitionRunning
            x: tab.width / 2
            z: -1
        }
        Popup {
            id: drawer

            closePolicy: Popup.CloseOnPressOutsideParent | Popup.CloseOnReleaseOutsideParent | Popup.CloseOnEscape
            enabled: !root.__transitionRunning
            padding: 30
            x: tab.width

            background: Rectangle {
                id: drawerBackground

                anchors.fill: parent
                border.color: Qt.darker(backgroundColor, 1.1)
                border.width: 1
                bottomLeftRadius: Yd.Constants.boxRadius
                bottomRightRadius: Yd.Constants.boxRadius
                color: root.backgroundColor
            }
            contentItem: SettingsContent {
            }
            exit: Transition {
                PauseAnimation {
                    duration: root.transitionDuration
                }
            }

            onAboutToHide: root.state = "closed"
            onAboutToShow: root.state = "opened"
        }
    }
}
