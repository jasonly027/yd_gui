import QtQuick
import QtQml.Models
import QtQuick.Controls
import QtQuick.Layouts
import YdGui as Yd

Item {
    id: root

    property int __widestFormatText: 0
    required property list<Yd.videoFormat> formats
    required property string selectedFormat

    implicitHeight: rowLayout.implicitHeight
    implicitWidth: __widestFormatText + icon.implicitWidth + rowLayout.spacing
    objectName: "formatComboBox"

    RowLayout {
        id: rowLayout

        anchors.fill: root
        spacing: 0

        Yd.FormatDelegate {
            id: comboBox

            function getSelectedFormatModel(): var {
                for (let i = 0; i < root.formats.length; ++i) {
                    if (root.formats[i].formatId === root.selectedFormat) {
                        return root.formats[i];
                    }
                }
                return null;
            }

            Layout.fillWidth: true
            bottomLeftRadius: Yd.Constants.boxRadius
            color: Yd.Theme.formatComboBoxBg
            model: comboBox.getSelectedFormatModel()
            selectedFormat: root.selectedFormat
            textColor: Yd.Theme.darkMode ? "white" : "black"
            topLeftRadius: Yd.Constants.boxRadius
        }
        Label {
            id: icon

            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumWidth: implicitWidth
            Layout.minimumWidth: 0
            clip: true
            color: Yd.Theme.darkMode ? "white" : "black"
            rightPadding: 10
            text: "\ue01d"
            verticalAlignment: Text.AlignVCenter

            background: Rectangle {
                bottomRightRadius: Yd.Constants.boxRadius
                color: Yd.Theme.formatComboBoxBg
                topRightRadius: Yd.Constants.boxRadius
            }

            font {
                family: "typicons"
                pixelSize: Yd.Constants.iconSizeSmall
            }
        }
    }
    MouseArea {
        id: mouseArea

        anchors.fill: rowLayout

        onClicked: menu.open()

        Menu {
            id: menu

            closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
            padding: 0
            width: mouseArea.width

            Instantiator {
                id: instantiator

                model: root.formats

                delegate: Yd.FormatDelegate {
                    selectedFormat: root.selectedFormat

                    Component.onCompleted: {
                        if (implicitWidth > root.__widestFormatText) {
                            root.__widestFormatText = implicitWidth;
                        }
                    }
                    mouseArea.onClicked: {
                        root.selectedFormat = model.formatId;
                        menu.close();
                    }
                }

                onObjectAdded: (index, object) => menu.insertItem(index, object)
                onObjectRemoved: (index, object) => menu.removeItem(object)
            }
        }
    }
}
