import QtQuick
import QtQuick.Controls
import YdGui as Yd

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

            // onClicked: Downloader.fetch_info("https://www.youtube.com/watch?v=_9GfTyIcDLU")
            onClicked: Yd.Downloader.test_enqueue()
        }
    }

    // Connections {
    //     target: Downloader

    //     function onStandardOutputPushed(data) {
    //         console.log("COUT", data);
    //     }

    //     function onStandardErrorPushed(data) {
    //         console.log("[CERR]", data);
    //     }
    // }
    ListView {
        width: 300
        height: 500
        model: Yd.VideoListModel

        delegate: Text {
            text: "This video is " + model.info.title
            font.pointSize: 20

        }
    }
}
