import QtQuick

Window {
    id: window

    width: 640
    height: 480
    visible: true
    title: Application.displayName

    Text {
        anchors.fill: parent
        anchors.margins: 16
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        fontSizeMode: Text.Fit
        font.family: "B612 Mono"
        font.pixelSize: 1000
        minimumPixelSize: 8
        color: "teal"
        text: "12:34"
    }
}
