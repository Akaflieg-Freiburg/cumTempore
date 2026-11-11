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
// The Basic style can be customized on every platform, unlike the native
// macOS and Windows styles.
import QtQuick.Controls.Basic
import QtQuick.Controls.impl

// Round button with a text or an icon, used for the keypad and the timer
// controls. The diameter follows the size given by the layout.
Button {
    id: button

    // Identifies the button for flash(), e.g. the keypad key it represents
    property string key: ""
    property color fillColor: Style.key
    property color textColor: Style.foreground

    // Diameter of the circle
    readonly property real diameter: Math.min(width - leftInset - rightInset, height - topInset - bottomInset)

    focusPolicy: Qt.NoFocus
    flat: true

    font.family: Style.clockFont
    font.pixelSize: Math.max(8, diameter * 0.36)

    icon.width: diameter * 0.45
    icon.height: diameter * 0.45
    icon.color: textColor

    // Show the button as pressed for a moment, used as visual feedback for
    // physical key presses
    function flash() {
        down = true
        flashTimer.restart()
    }

    Timer {
        id: flashTimer

        interval: 100
        onTriggered: button.down = undefined
    }

    contentItem: IconLabel {
        spacing: 0
        display: button.display
        icon: button.icon
        text: button.text
        font: button.font
        color: button.textColor
    }

    background: Rectangle {
        x: button.leftInset + (button.width - button.leftInset - button.rightInset - width) / 2
        y: button.topInset + (button.height - button.topInset - button.bottomInset - height) / 2
        width: button.diameter
        height: button.diameter
        radius: width / 2
        color: button.fillColor
        opacity: button.enabled ? 1.0 : 0.4

        Rectangle {
            anchors.fill: parent
            radius: parent.radius
            color: Style.foreground
            opacity: button.down ? 0.2 : (button.hovered ? 0.08 : 0.0)
        }
    }
}
