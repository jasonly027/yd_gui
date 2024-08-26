import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YdGui as Yd

Yd.RaisedButton {
    id: root

    readonly property var __colorIconFn: {
        switch (state) {
        case Yd.ManagedVideo.DownloadState.kAdded:
            return {
                color: Yd.Theme.downloadBtnAdded,
                icon: "\ue065",
                fn: function () {
                    console.log("kAdded");
                    Yd.VideoListModel.downloadVideo(index);
                }
            };
        case Yd.ManagedVideo.DownloadState.kQueued:
            return {
                color: Yd.Theme.downloadBtnAdded,
                icon: "\ue0c9",
                fn: function () {
                    console.log("kQueued");
                    Yd.VideoListModel.cancelDownload(index);
                }
            };
        case Yd.ManagedVideo.DownloadState.kDownloading:
            return {
                color: Yd.Theme.downloadBtnDownloading,
                icon: "\ue13c",
                fn: function () {
                    console.log("kDownloading");
                    Yd.VideoListModel.cancelDownload(index);
                }
            };
        case Yd.ManagedVideo.DownloadState.kComplete:
            return {
                color: Yd.Theme.downloadBtnDone,
                icon: "\ue11e",
                fn: function () {
                    console.log("kComplete");
                    Yd.VideoListModel.downloadVideo(index);
                }
            };
        }
        console.log("Unknown state");
        return null;
    }
    required property int index
    required property int state

    color: __colorIconFn["color"]
    objectName: "downloadButton"
    text: __colorIconFn["icon"]
    textColor: "white"

    onClicked: __colorIconFn["fn"]()

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
