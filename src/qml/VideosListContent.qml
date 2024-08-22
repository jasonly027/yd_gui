import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YdGui as Yd

Rectangle {
    id: root

    color: Yd.Theme.videosListBg
    objectName: "videosListContent"
    radius: Yd.Constants.boxRadius

    ListView {
        id: listView

        anchors.fill: parent
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
