pragma Singleton

import QtQuick

Item {
    id: root

    readonly property color bg: current.bg
    readonly property color cancelBtn: current.cancelBtn
    readonly property color clearHistoryBtn: current.clearHistoryBtn
    readonly property color consoleBg: current.consoleBg
    property var current: themes[_settings.theme] ?? themes["darkPurple"]
    readonly property bool darkMode: current.darkMode
    readonly property color downloadAllBtn: current.downloadAllBtn
    readonly property color downloadBtnAdded: current.downloadBtnAdded
    readonly property color downloadBtnDone: current.downloadBtnDone
    readonly property color downloadBtnDownloading: current.downloadBtnDownloading
    readonly property color dropDownBg: current.dropDownBg
    readonly property color error: current.error
    readonly property color formatComboBoxBg: current.formatComboBoxBg
    readonly property color iconPrimary: current.iconPrimary
    readonly property color iconSecondary: current.iconSecondary
    readonly property color inputBar: current.inputBar
    readonly property color inputBarIcon: current.inputBarIcon
    readonly property color inputBarTab: current.inputBarTab
    readonly property color link: current.link
    readonly property color neutral: current.neutral
    readonly property color primary: current.primary
    readonly property color secondary: current.secondary
    readonly property color settingsBg: current.settingsBg
    readonly property var themes: {
        "darkPurple": themeObj.createObject(null, {
            bg: "#2b2d31",
            cancelBtn: "#999999",
            clearHistoryBtn: "#ffa3b0",
            consoleBg: "#232428",
            darkMode: true,
            downloadAllBtn: "#d0b1ff",
            downloadBtnAdded: "#ffa3b0",
            downloadBtnDownloading: "#d0b1ff",
            downloadBtnDone: "#B994EC",
            dropDownBg: "#383a40",
            error: "#cc4125",
            formatComboBoxBg: "#383a40",
            iconPrimary: "#ffffff",
            iconSecondary: "#919193",
            inputBar: "#999999",
            inputBarIcon: "#999999",
            inputBarTab: "#232428",
            link: "#ffa3b0",
            neutral: "#999999",
            primary: "#d0b1ff",
            secondary: "#ffa3b0",
            settingsBg: "#232428",
            videoCardBg: "#2b2d31",
            videosListBg: "#313338"
        }),
        "lightPurple": themeObj.createObject(null, {
            bg: "#ffffff",
            cancelBtn: "#999999",
            clearHistoryBtn: "#ffa3b0",
            consoleBg: "#d9d9d9",
            darkMode: false,
            downloadAllBtn: "#d0b1ff",
            downloadBtnAdded: "#ffa3b0",
            downloadBtnDownloading: "#d0b1ff",
            downloadBtnDone: "#B994EC",
            dropDownBg: "#d9d9d9",
            error: "#cc4125",
            formatComboBoxBg: "#d9d9d9",
            iconPrimary: "#404040",
            iconSecondary: "#6d6d6d",
            inputBar: "#eeeeee",
            inputBarIcon: "#2b2d31",
            inputBarTab: "#ffffff",
            link: "#ffa3b0",
            neutral: "#999999",
            primary: "#d0b1ff",
            secondary: "#ffa3b0",
            settingsBg: "#ffffff",
            videoCardBg: "#ffffff",
            videosListBg: "#eeeeee"
        }),
        "garden": themeObj.createObject(null, {
            bg: "#281103",
            cancelBtn: "#D18022",
            clearHistoryBtn: "#117A34",
            consoleBg: "#1C0400",
            darkMode: true,
            downloadAllBtn: "#117A34",
            downloadBtnAdded: "#C0D16D",
            downloadBtnDownloading: "#E39B52",
            downloadBtnDone: "#FF6542",
            dropDownBg: "#38082B",
            error: "#B10000",
            formatComboBoxBg: "#38082B",
            iconPrimary: "#ffffff",
            iconSecondary: "#919193",
            inputBar: "#9FB17B",
            inputBarIcon: "#9FB17B",
            inputBarTab: "#444A30",
            link: "#28C638",
            neutral: "#FF7C78",
            primary: "#D18022",
            secondary: "#117A34",
            settingsBg: "#4A0C00",
            videoCardBg: "#281103",
            videosListBg: "#442500"
        })
    }
    readonly property color videoCardBg: current.videoCardBg
    readonly property color videosListBg: current.videosListBg

    objectName: "themeObject"

    Component {
        id: themeObj

        QtObject {
            required property color bg
            required property color cancelBtn
            required property color clearHistoryBtn
            required property color consoleBg
            required property bool darkMode
            required property color downloadAllBtn
            required property color downloadBtnAdded
            required property color downloadBtnDone
            required property color downloadBtnDownloading
            required property color dropDownBg
            required property color error
            required property color formatComboBoxBg
            required property color iconPrimary
            required property color iconSecondary
            required property color inputBar
            required property color inputBarIcon
            required property color inputBarTab
            required property color link
            required property color neutral
            required property color primary
            required property color secondary
            required property color settingsBg
            required property color videoCardBg
            required property color videosListBg
        }
    }
}
