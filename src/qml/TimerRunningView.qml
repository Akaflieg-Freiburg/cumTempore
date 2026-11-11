/***************************************************************************
 *   Copyright (C) 2026 by Stefan Kebekus                                  *
 *   stefan.kebekus@math.uni-freiburg.de                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <https://www.gnu.org/licenses/>. *
 ***************************************************************************/

import QtQuick

// Running or paused timer: remaining time inside a progress ring, with
// controls below (portrait) or to the right (landscape).
Item {
    id: view

    required property CountdownTimer timer

    readonly property bool landscape: width > 1.2 * height
    readonly property bool overtime: timer.phase === CountdownTimer.Overtime
    readonly property color accentColor: {
        switch (timer.phase) {
        case CountdownTimer.Warning:
            return Style.warning
        case CountdownTimer.Overtime:
            return Style.overtimeText
        }
        return Style.teal
    }

    // Formats seconds as h:mm:ss or m:ss. Overtime (negative values) gets a
    // minus sign, as in Google's Clock app; the number counts upwards.
    function format(seconds: int) : string {
        var a = Math.abs(seconds)
        var h = Math.floor(a / 3600)
        var m = Math.floor((a % 3600) / 60)
        var s = a % 60
        var pad = (n) => (n < 10 ? "0" : "") + n
        var str = (h > 0) ? h + ":" + pad(m) + ":" + pad(s) : m + ":" + pad(s)
        return (seconds < 0 ? "−" : "") + str
    }

    readonly property real controlsSize: landscape ? Math.min(width * 0.22, height / 3.4)
                                                   : Math.min(height * 0.18, width / 3.4)

    // Ring with the remaining time
    Item {
        id: ringArea

        x: 0
        y: 0
        width: view.landscape ? view.width - view.controlsSize * 1.4 : view.width
        height: view.landscape ? view.height : view.height - view.controlsSize * 1.3

        ProgressRing {
            id: ring

            anchors.centerIn: parent
            width: Math.min(parent.width, parent.height) * 0.92
            height: width

            progress: view.timer.progress
            color: view.accentColor
            trackColor: view.overtime ? Qt.rgba(1, 1, 1, 0.3) : Style.subdued
        }

        // Invisible text with the widest string of the current format. The
        // visible time copies its font size, so that the digits do not jump
        // in size as the time string gets shorter.
        Text {
            id: template

            anchors.centerIn: ring
            width: ring.width * 0.72
            height: ring.height * 0.4

            visible: false
            fontSizeMode: Text.Fit
            font.family: Style.clockFont
            font.pixelSize: 1000
            minimumPixelSize: 8
            text: (view.overtime ? "−" : "") + (Math.abs(view.timer.displaySeconds) >= 3600 ? "00:00:00" : "00:00")
        }

        Text {
            id: timeText

            anchors.centerIn: ring

            font.family: Style.clockFont
            font.pixelSize: template.fontInfo.pixelSize
            color: view.accentColor
            text: view.format(view.timer.displaySeconds)

            Accessible.role: Accessible.StaticText
            Accessible.name: text

            // Blink while paused, as in Google's Clock app
            SequentialAnimation on opacity {
                running: view.timer.state === CountdownTimer.Paused
                loops: Animation.Infinite
                alwaysRunToEnd: true
                NumberAnimation { to: 0.2; duration: 500; easing.type: Easing.InOutQuad }
                NumberAnimation { to: 1.0; duration: 500; easing.type: Easing.InOutQuad }
            }
        }
    }

    // Controls: reset, start/pause, add a minute
    Item {
        id: controlsArea

        x: view.landscape ? ringArea.width : 0
        y: view.landscape ? 0 : ringArea.height
        width: view.width - x
        height: view.height - y

        Grid {
            anchors.centerIn: parent
            columns: view.landscape ? 1 : 3
            spacing: view.controlsSize * 0.25
            verticalItemAlignment: Grid.AlignVCenter
            horizontalItemAlignment: Grid.AlignHCenter

            CircleButton {
                width: view.controlsSize * 0.75
                height: width
                fillColor: view.overtime ? Qt.rgba(1, 1, 1, 0.2) : Style.key
                textColor: view.overtime ? Style.overtimeText : Style.foreground
                icon.source: "/images/ic_replay_24px.svg"
                display: CircleButton.IconOnly
                Accessible.name: qsTr("Reset")
                onClicked: view.timer.reset()
            }

            CircleButton {
                width: view.controlsSize
                height: width
                fillColor: view.overtime ? Style.overtimeText : view.accentColor
                textColor: view.overtime ? Style.overtime : Style.tealText
                icon.source: view.timer.state === CountdownTimer.Running ? "/images/ic_pause_24px.svg" : "/images/ic_play_arrow_24px.svg"
                display: CircleButton.IconOnly
                Accessible.name: view.timer.state === CountdownTimer.Running ? qsTr("Pause") : qsTr("Resume")
                onClicked: view.timer.toggle()
            }

            CircleButton {
                width: view.controlsSize * 0.75
                height: width
                fillColor: view.overtime ? Qt.rgba(1, 1, 1, 0.2) : Style.key
                textColor: view.overtime ? Style.overtimeText : Style.foreground
                font.pixelSize: diameter * 0.26
                text: "+1:00"
                display: CircleButton.TextOnly
                Accessible.name: qsTr("Add one minute")
                onClicked: view.timer.addMinute()
            }
        }
    }
}
