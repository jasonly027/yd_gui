import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YdGui as Yd

Rectangle {
    id: root

    color: Yd.Theme.videosListBg
    objectName: "videosListContent"
    topLeftRadius: Yd.Constants.boxRadius
    topRightRadius: Yd.Constants.boxRadius

    Connections {
        function onRequestDownloadVideo(video) {
            Yd.Downloader.enqueue_video(video);
        }

        target: Yd.VideoListModel
    }
    ListView {
        id: listView

        anchors.fill: parent
        boundsBehavior: Flickable.StopAtBounds
        clip: true
        model: Yd.VideoListModel
        spacing: Yd.Constants.boxPadding

        delegate: Yd.VideoDelegate {
            width: ListView.view.width
        }
        footer: Item {
            enabled: false
            height: listView.spacing
        }
        header: Item {
            enabled: false
            height: listView.spacing
        }

        anchors {
            leftMargin: spacing
            rightMargin: spacing
        }
    }
}
