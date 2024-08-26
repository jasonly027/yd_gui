import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YdGui as Yd

Item {
    id: root

    required property var model
    required property int index

    implicitHeight: columnLayout.implicitHeight
    implicitWidth: columnLayout.implicitWidth

    ColumnLayout {
        id: columnLayout

        anchors.fill: root
        spacing: 0

        Rectangle {
            id: videoCardBg

            Layout.fillWidth: true
            Layout.minimumWidth: 0
            color: Yd.Theme.videoCardBg
            implicitHeight: videoCard.implicitHeight + (videoCard.anchors.margins * 2)
            implicitWidth: videoCard.implicitWidth
            objectName: "videoDelegate"
            topLeftRadius: Yd.Constants.boxRadius
            topRightRadius: Yd.Constants.boxRadius

            Item {
                id: videoCard

                implicitHeight: rowLayout.implicitHeight
                implicitWidth: rowLayout.implicitWidth

                anchors {
                    fill: videoCardBg
                    margins: Yd.Constants.boxPadding
                }
                RowLayout {
                    id: rowLayout

                    anchors.fill: videoCard
                    spacing: 10

                    Image {
                        id: thumbnailImage

                        Layout.preferredHeight: sourceSize.height
                        asynchronous: true
                        source: root.model.info.thumbnail
                        sourceSize.height: rowLayout.implicitHeight
                        visible: root.model.info.thumbnail !== ""

                        Label {
                            id: secondsText

                            color: Yd.Theme.darkMode ? "white" : "black"
                            leftPadding: 3
                            rightPadding: 3
                            text: {
                                const secsInAHour = 3600;
                                const secsInAMinute = 60;
                                const hrs = Math.floor(root.model.info.seconds / secsInAHour);
                                const mins = Math.floor((root.model.info.seconds - (hrs * secsInAHour)) / secsInAMinute);
                                const secs = root.model.info.seconds - (hrs * secsInAHour) - (mins * secsInAMinute);
                                if (hrs !== 0)
                                    return `${hrs}:${mins}:${secs}`;
                                return `${mins}:${secs}`;
                            }
                            visible: root.model.info.seconds > 0

                            background: Rectangle {
                                color: Yd.Theme.darkMode ? "black" : "white"
                                opacity: 0.6
                                radius: Yd.Constants.boxRadius / 2
                            }

                            anchors {
                                bottom: thumbnailImage.bottom
                                margins: 10
                                right: thumbnailImage.right
                            }
                        }
                    }
                    ColumnLayout {
                        id: infosLayout

                        Layout.minimumHeight: removeAndDownloadItem.Layout.minimumHeight
                        spacing: 10

                        ColumnLayout {
                            id: titleAuthorLinkLayout

                            spacing: 0

                            Yd.TitleAudioText {
                                id: titleAudioText

                                Layout.fillWidth: true
                                text: root.model.info.title !== "" ? root.model.info.title : "Untitled"
                            }
                            Text {
                                id: authorText

                                Layout.fillWidth: true
                                color: Yd.Theme.neutral
                                elide: Text.ElideRight
                                text: root.model.info.author !== "" ? root.model.info.author : "No Author"
                            }
                            Loader {
                                id: linkTextLoader

                                Layout.fillWidth: true
                                Layout.maximumWidth: implicitWidth
                                active: visible
                                visible: root.model.info.url !== ""

                                sourceComponent: Yd.LinkText {
                                    id: linkText

                                    text: root.model.info.url
                                }
                            }
                        }
                        RowLayout {
                            id: formatThumbnailSelectorLayout

                            spacing: 10

                            Loader {
                                id: formatComboBoxLoader

                                Layout.fillWidth: true
                                Layout.maximumWidth: implicitWidth
                                active: visible
                                visible: root.model.info.formats.length > 0

                                sourceComponent: Yd.FormatComboBox {
                                    id: formatComboxBox

                                    formats: root.model.info.formats
                                    selectedFormat: root.model.selectedFormat
                                }
                            }
                            CheckBox {
                                id: thumbnailCheckBox

                                Layout.fillWidth: true
                                Layout.maximumWidth: implicitWidth
                                Layout.minimumWidth: 0
                                checked: root.model.downloadThumbnail
                                palette.windowText: Yd.Theme.darkMode ? "white" : "black"
                                text: qsTr("Download Thumbnail")

                                onCheckedChanged: root.model.downloadThumbnail = checked
                            }
                        }
                    }
                    Item {
                        id: removeAndDownloadItem

                        Layout.fillHeight: true
                        Layout.minimumHeight: removeButton.implicitHeight + downloadButton.implicitHeight
                        implicitHeight: downloadButton.implicitHeight
                        implicitWidth: downloadButton.implicitWidth + 10

                        Yd.RemoveButton {
                            id: removeButton

                            onClicked: Yd.VideoListModel.removeVideo(root.index)

                            anchors {
                                right: removeAndDownloadItem.right
                                top: removeAndDownloadItem.top
                            }
                        }
                        Yd.DownloadButton {
                            id: downloadButton

                            index: root.index
                            state: root.model.state

                            anchors {
                                bottom: removeAndDownloadItem.bottom
                                right: removeAndDownloadItem.right
                            }
                        }
                    }
                }
            }
        }
        Yd.ProgressBar {
            Layout.fillWidth: true
            activeBar.bottomLeftRadius: Yd.Constants.boxRadius
            activeBar.bottomRightRadius: value >= 1 ? Yd.Constants.boxRadius : 0
            bottomLeftRadius: Yd.Constants.boxRadius
            bottomRightRadius: Yd.Constants.boxRadius
            value: root.model.progress
        }
    }
    Yd.DropShadow {
        target: columnLayout
    }
}
