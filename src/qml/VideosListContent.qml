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
    Connections {
        function onVideosPushed(videos) {
            Yd.VideoListModel.appendVideos(videos);
        }

        target: _database
    }
    ListView {
        id: listView

        property date __lastCheckedPos: new Date()
        property real __position: visibleArea.yPosition / (1.0 - visibleArea.heightRatio)

        anchors.fill: parent
        boundsBehavior: Flickable.StopAtBounds
        clip: true
        model: Yd.VideoListModelSortedProxy
        spacing: Yd.Constants.boxPadding

        Component.onCompleted: {
            Yd.VideoListModelSortedProxy.setModel(Yd.VideoListModel);
        }

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

        on__PositionChanged: {
            const now = new Date();
            if (now - __lastCheckedPos > 3000 && __position > 0.99) {
                __lastCheckedPos = now;
                Yd.VideoListModel.paginate();
            }
        }

        anchors {
            leftMargin: spacing
            rightMargin: spacing
        }
    }
}
