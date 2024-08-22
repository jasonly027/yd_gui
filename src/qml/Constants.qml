pragma Singleton

import QtQuick

QtObject {
    readonly property int boxRadius: 10
    readonly property int boxPadding: 20
    readonly property int spacing: 20
    readonly property int buttonRadius: 15
    readonly property int iconSizeLarge: Qt.application.font.pixelSize * 1.8
    readonly property int iconSizeMedium: Qt.application.font.pixelSize * 1.5
    readonly property int iconSizeSmall: Qt.application.font.pixelSize * 1.2
}
