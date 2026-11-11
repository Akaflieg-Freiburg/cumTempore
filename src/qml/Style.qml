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

pragma Singleton

import QtQuick

QtObject {
    // Font family of the clock and timer displays (bundled, see main.cpp)
    readonly property string clockFont: "B612"

    readonly property real fontPixelSize: (Qt.platform.os === "android") ? 1.6*Application.font.pixelSize : 1.2*Application.font.pixelSize

    // Follows the system color scheme; Qt.Unknown deliberately counts as light
    readonly property bool isDark: Application.styleHints.colorScheme === Qt.Dark

    // Window background
    readonly property color background: isDark ? "black" : "white"
    // Brand color: wall clock digits, running timer, buttons
    readonly property color teal: isDark ? "#4db6ac" : "teal"
    // Text and icons on top of a teal button
    readonly property color tealText: isDark ? "black" : "white"
    // Timer in its last minutes
    readonly property color warning: isDark ? "#e57373" : "darkred"
    // Timer in overtime: background and text
    readonly property color overtime: isDark ? "#b71c1c" : "#d32f2f"
    readonly property color overtimeText: "white"
    // Secondary text and inactive elements (keypad digits, idle ring)
    readonly property color foreground: isDark ? "#eff0f2" : "#202124"
    readonly property color subdued: isDark ? "#5f6368" : "#c4c7c5"
    // Keypad key background
    readonly property color key: isDark ? "#2a2a2a" : "#eef0f1"
    // Teal-tinted text on the background, used for hyperlinks
    readonly property color accent: teal

    // Palette for menus and dialogs, mirroring the light/dark palettes of the
    // Qt Quick "Basic" style. Explicit palette bindings on the popups are
    // required because Qt does not push a system color scheme change into
    // popups that already exist (their default palette is a snapshot).
    readonly property color popupWindow: isDark ? "#1e1e1e" : "white"
    readonly property color popupWindowText: isDark ? "#d4d6d8" : "#26282a"
    readonly property color popupText: isDark ? "#eff0f2" : "#353637"
    readonly property color popupButton: isDark ? "#2f2f2f" : "#e0e0e0"
    readonly property color popupButtonText: isDark ? "#d4d6d8" : "#26282a"
    readonly property color popupMid: isDark ? "#626262" : "#bdbdbd"
    readonly property color popupDark: isDark ? "#c8c9cb" : "#353637"
}
