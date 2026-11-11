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

import QtCore
import QtQuick

// Countdown timer page: duration entry while idle, progress ring while
// running or paused.
Item {
    id: page

    // Background color requested from the window: red in overtime
    readonly property color pageColor: countdown.phase === CountdownTimer.Overtime ? Style.overtime : Style.background

    readonly property alias timer: countdown

    // Conversion between the keypad digits (HHMMSS, no leading zeros) and
    // milliseconds. Minutes and seconds above 59 are allowed while typing and
    // are normalized when the timer starts, as in Google's Clock app.
    function msFromDigits(digits: string) : real {
        var p = ("000000" + digits).slice(-6)
        return ((parseInt(p.slice(0, 2)) * 60 + parseInt(p.slice(2, 4))) * 60 + parseInt(p.slice(4, 6))) * 1000
    }

    function digitsFromMs(ms: real) : string {
        var total = Math.max(0, Math.round(ms / 1000))
        var h = Math.min(99, Math.floor(total / 3600))
        var m = Math.floor((total % 3600) / 60)
        var s = total % 60
        var pad = (n) => (n < 10 ? "0" : "") + n
        return String(pad(h) + pad(m) + pad(s)).replace(/^0+/, "")
    }

    function start() {
        setView.digits = digitsFromMs(countdown.durationMs)
        countdown.start()
    }

    // Keyboard support for desktop computers. Returns true if the key was
    // handled.
    function handleKey(key: int) : bool {
        if (countdown.state === CountdownTimer.Idle) {
            if (key >= Qt.Key_0 && key <= Qt.Key_9) {
                setView.appendDigits(String(key - Qt.Key_0))
                return true
            }
            if (key === Qt.Key_Backspace) {
                setView.backspace()
                return true
            }
            if ((key === Qt.Key_Return || key === Qt.Key_Enter || key === Qt.Key_Space) && countdown.durationMs > 0) {
                page.start()
                return true
            }
            return false
        }
        switch (key) {
        case Qt.Key_Space:
        case Qt.Key_Return:
        case Qt.Key_Enter:
            countdown.toggle()
            return true
        case Qt.Key_Escape:
        case Qt.Key_Delete:
        case Qt.Key_Backspace:
            countdown.reset()
            return true
        case Qt.Key_Plus:
            countdown.addMinute()
            return true
        }
        return false
    }

    CountdownTimer {
        id: countdown

        durationMs: page.msFromDigits(setView.digits)
        onDurationMsChanged: settings.durationMs = durationMs
    }

    Settings {
        id: settings

        category: "Timer"
        property real durationMs: 0
    }

    Component.onCompleted: setView.digits = digitsFromMs(settings.durationMs)

    TimerSetView {
        id: setView

        anchors.fill: parent
        visible: countdown.state === CountdownTimer.Idle

        onStartRequested: page.start()
    }

    TimerRunningView {
        anchors.fill: parent
        visible: countdown.state !== CountdownTimer.Idle

        timer: countdown
    }
}
