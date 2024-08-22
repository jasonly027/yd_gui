import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YdGui as Yd

Item {
    id: root

    required property int createdAt
    required property Yd.videoInfo info
    required property string progress
    required property string selectedFormat

    implicitHeight: videoCardBg.implicitHeight
    implicitWidth: videoCardBg.implicitWidth

    ColumnLayout {

        Rectangle {
            id: videoCardBg

            anchors.fill: root
            color: Yd.Theme.videoCardBg
            implicitHeight: videoCard.implicitHeight + (videoCard.anchors.margins * 2)
            implicitWidth: videoCard.implicitWidth
            objectName: "videoDelegate"
            radius: Yd.Constants.boxRadius

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
                        source: root.info.thumbnail
                        sourceSize.height: rowLayout.implicitHeight

                        Label {
                            id: secondsText

                            color: Yd.Theme.darkMode ? "white" : "black"
                            text: root.info.seconds

                            background: Rectangle {
                                color: Yd.Theme.darkMode ? "black" : "white"
                                opacity: 0.5
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

                        spacing: 10

                        ColumnLayout {
                            id: titleAuthorLinkLayout

                            spacing: 0

                            Yd.TitleAudioText {
                                id: titleAudioText

                                Layout.fillWidth: true
                                audio: root.info.audioAvailable
                                text: root.info.title
                            }
                            Text {
                                id: authorText

                                Layout.fillWidth: true
                                color: Yd.Theme.neutral
                                elide: Text.ElideRight
                                text: root.info.author
                            }
                            Yd.LinkText {
                                id: linkText

                                Layout.fillWidth: true
                                Layout.maximumWidth: implicitWidth
                                text: root.info.url
                            }
                        }
                        RowLayout {
                            id: formatThumbnailSelectorLayout

                            Yd.FormatComboBox {
                                id: formatComboxBox

                                Layout.fillWidth: true
                                Layout.maximumWidth: implicitWidth
                                formats: root.info.formats
                                selectedFormat: root.selectedFormat
                            }
                            CheckBox {
                                id: thumbnailCheckbox

                                Layout.fillWidth: true
                                Layout.maximumWidth: implicitWidth
                                Layout.minimumWidth: 0
                                palette.windowText: Yd.Theme.darkMode ? "white" : "black"
                                text: qsTr("Download Thumbnail")
                            }
                        }
                    }
                    Item {
                        id: removeAndDownloadItem

                        Layout.fillHeight: true
                        implicitHeight: downloadButton.implicitHeight
                        implicitWidth: downloadButton.implicitWidth + 10

                        Text {
                            id: removeButton

                            color: removeButtonMouseArea.containsMouse ? Yd.Theme.primary : Yd.Theme.neutral
                            opacity: 0.6
                            text: "\ue122"

                            font {
                                family: "typicons"
                                pixelSize: Yd.Constants.iconSizeMedium
                            }
                            anchors {
                                right: removeAndDownloadItem.right
                                top: removeAndDownloadItem.top
                            }
                            MouseArea {
                                id: removeButtonMouseArea

                                anchors.fill: removeButton
                                cursorShape: Qt.PointingHandCursor
                                hoverEnabled: true
                            }
                        }
                        Yd.RaisedButton {
                            id: downloadButton

                            button.bottomPadding: Yd.Constants.boxPadding / 2
                            button.leftPadding: Yd.Constants.boxPadding / 1.5
                            button.rightPadding: Yd.Constants.boxPadding / 1.5
                            button.topPadding: Yd.Constants.boxPadding / 2
                            color: Yd.Theme.downloadBtn
                            text: "\ue065"
                            textColor: "white"

                            button.font {
                                family: "typicons"
                                pixelSize: Qt.application.font.pixelSize * 2.5
                            }
                            anchors {
                                bottom: removeAndDownloadItem.bottom
                                right: removeAndDownloadItem.right
                            }
                        }
                    }
                }
            }
        }
    }
}
