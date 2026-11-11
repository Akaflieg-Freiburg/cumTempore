import QtQuick

// Placeholder, replaced by the countdown timer
Item {
    // Background color requested from the window
    property color pageColor: Style.background

    Text {
        anchors.centerIn: parent
        color: Style.teal
        font.family: Style.clockFont
        font.pixelSize: 48
        text: "00:00"
    }
}
