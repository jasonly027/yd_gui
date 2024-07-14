import QtQuick
import QtQuick.Controls
import YdGui

Window {
    height: 480
    title: qsTr("Hello World")
    visible: true
    width: 640

    Rectangle {
        id: spinningBox

        anchors.centerIn: parent
        color: "blue"
        height: 100
        width: 100

        NumberAnimation on rotation {
            duration: 2000
            from: 0
            loops: Animation.Infinite
            to: 360
        }

        MouseArea {
            anchors.fill: parent

            onClicked: Downloader.fetch_info("https://www.youtube.com/watch?v=_9GfTyIcDLU")
        }
    }
}
