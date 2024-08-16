import QtQuick
import QtQuick.Effects

MultiEffect {
    required property Item target

    anchors.fill: target
    blurMax: 12
    shadowEnabled: true
    shadowOpacity: 0.5
    shadowVerticalOffset: 2
    source: target
}
