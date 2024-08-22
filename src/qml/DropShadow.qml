import QtQuick
import QtQuick.Effects
import YdGui as Yd

MultiEffect {
    required property Item target

    anchors.fill: target
    blurMax: 12
    objectName: "dropShadow"
    shadowBlur: 0.3
    shadowEnabled: true
    shadowOpacity: Yd.Theme.darkMode ? 0.5 : 0.3
    shadowVerticalOffset: 1
    source: target
}
