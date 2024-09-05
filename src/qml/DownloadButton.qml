import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YdGui as Yd

Yd.RaisedButton {
    id: root

    readonly property var __stateValues: {
        switch (state) {
        case Yd.ManagedVideo.DownloadState.kAdded:
            return {
                color: Yd.Theme.downloadBtnAdded,
                icon: "\ue065",
                fn: function () {
                    Yd.VideoListModelSortedProxy.downloadVideo(index);
                },
                toolTip: qsTr("Enqueue video")
            };
        case Yd.ManagedVideo.DownloadState.kQueued:
            return {
                color: Yd.Theme.downloadBtnAdded,
                icon: "\ue0c9",
                fn: function () {
                    Yd.VideoListModelSortedProxy.cancelDownload(index);
                },
                toolTip: qsTr("Queued")
            };
        case Yd.ManagedVideo.DownloadState.kDownloading:
            return {
                color: Yd.Theme.downloadBtnDownloading,
                icon: "\ue13c",
                fn: function () {
                    Yd.VideoListModelSortedProxy.cancelDownload(index);
                },
                toolTip: qsTr("Downloading")
            };
        case Yd.ManagedVideo.DownloadState.kComplete:
            return {
                color: Yd.Theme.downloadBtnDone,
                icon: "\ue11e",
                fn: function () {
                    Yd.VideoListModelSortedProxy.downloadVideo(index);
                },
                toolTip: qsTr("Complete")
            };
        }
        console.log("Unknown state");
        return null;
    }
    required property int index
    required property int state

    ToolTip.delay: Yd.Constants.toolTipDelay
    ToolTip.text: __stateValues["toolTip"]
    ToolTip.visible: button.hoverHandler.hovered
    color: __stateValues["color"]
    objectName: "downloadButton"
    text: __stateValues["icon"]
    textColor: "white"

    onClicked: __stateValues["fn"]()

    button {
        bottomPadding: Yd.Constants.boxPadding / 2
        leftPadding: Yd.Constants.boxPadding / 1.5
        rightPadding: Yd.Constants.boxPadding / 1.5
        topPadding: Yd.Constants.boxPadding / 2

        font {
            family: "typicons"
            pixelSize: Qt.application.font.pixelSize * 2.5
        }
    }
}
