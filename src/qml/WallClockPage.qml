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

// Local time, as large as the page allows
Item {
    id: page

    property date now: new Date()

    Timer {
        // Ticking every second keeps the minute change accurate to within a
        // second, at negligible cost.
        interval: 1000
        repeat: true
        running: true
        triggeredOnStart: true
        onTriggered: page.now = new Date()
    }

    Text {
        anchors.fill: parent
        anchors.margins: Math.min(parent.width, parent.height) * 0.05

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        // Text.Fit shrinks the font from this (absurdly large) size until the
        // text fits the item.
        fontSizeMode: Text.Fit
        font.family: Style.clockFont
        font.pixelSize: 2000
        minimumPixelSize: 8

        color: Style.teal
        text: page.now.toLocaleTimeString(Qt.locale(), Locale.ShortFormat)

        Accessible.role: Accessible.StaticText
        Accessible.name: text
    }
}
