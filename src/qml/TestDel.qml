import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YdGui as Yd

Rectangle {
    id: root

    property Yd.videoInfo info: model.info
    required property var model
    property real progress: model.progress

    color: "red"
    implicitHeight: 100
    implicitWidth: 300
    objectName: "testDel"

    Text {
        anchors.centerIn: parent
        color: "white"
        text: `Title: ${root.info.title}\nProgress is ${root.progress}`

        MouseArea {
            anchors.fill: parent

            onClicked: {
                console.log("area clicked");
                root.progress = 0.5;
            }
        }
    }
}
