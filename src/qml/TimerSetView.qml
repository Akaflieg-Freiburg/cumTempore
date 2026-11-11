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

pragma ComponentBehavior: Bound

import QtQuick

// Duration entry in the style of Google's Clock app: digits typed on the
// keypad shift in from the right into "00h 00m 00s".
Item {
    id: view

    // Up to six digits, HHMMSS, without leading zeros
    property string digits: ""

    signal startRequested()

    readonly property bool landscape: width > 1.2 * height
    readonly property real margin: Math.min(width, height) * 0.04

    function appendDigits(newDigits: string) {
        var d = (view.digits + newDigits).replace(/^0+/, "")
        if (d.length <= 6)
            view.digits = d
        keyRepeater.flashKey(newDigits)
    }

    function backspace() {
        view.digits = view.digits.slice(0, -1)
        keyRepeater.flashKey("⌫")
    }

    // "00h 00m 00s" as styled text. Digits that have been typed are shown in
    // the brand color, the leading zeros that have not been typed are grey.
    function displayText() : string {
        var padded = ("000000" + view.digits).slice(-6)
        var firstTyped = 6 - view.digits.length
        var units = [qsTr("h", "abbreviation for hours"),
                     qsTr("m", "abbreviation for minutes"),
                     qsTr("s", "abbreviation for seconds")]
        var result = ""
        for (var group = 0; group < 3; group++) {
            for (var i = 2*group; i < 2*group+2; i++) {
                var c = (i >= firstTyped) ? Style.teal : Style.subdued
                result += "<font color='" + c + "'>" + padded[i] + "</font>"
            }
            var uc = (2*group+1 >= firstTyped) ? Style.teal : Style.subdued
            result += "<font color='" + uc + "'><small>" + units[group] + "</small></font>"
            if (group < 2)
                result += " "
        }
        return result
    }

    // Time display; on top in portrait, top left in landscape
    Item {
        id: displayArea

        x: 0
        y: 0
        width: view.landscape ? view.width * 0.5 : view.width
        height: view.landscape ? view.height * 0.6 : view.height * 0.25

        Text {
            anchors.fill: parent
            anchors.margins: view.margin
            anchors.topMargin: view.margin * 2

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            textFormat: Text.StyledText
            fontSizeMode: Text.Fit
            font.family: Style.clockFont
            font.pixelSize: 400
            minimumPixelSize: 8

            text: view.displayText()

            Accessible.role: Accessible.StaticText
            Accessible.name: view.digits === "" ? qsTr("No duration set") : text
        }
    }

    // Keypad; below the display in portrait, on the right in landscape
    Item {
        id: keypadArea

        x: view.landscape ? view.width * 0.5 : 0
        y: view.landscape ? 0 : displayArea.height
        width: view.landscape ? view.width * 0.5 : view.width
        height: view.landscape ? view.height : view.height * 0.58

        // Cell size of the 3×4 key grid, capped so that keys do not become
        // huge on large screens
        readonly property real cell: Math.min(width / 3, height / 4, 14 * Style.fontPixelSize)

        Grid {
            anchors.centerIn: parent
            columns: 3

            Repeater {
                id: keyRepeater

                model: ["1", "2", "3", "4", "5", "6", "7", "8", "9", "00", "0", "⌫"]

                function flashKey(key: string) {
                    for (var i = 0; i < count; i++) {
                        var b = itemAt(i) as CircleButton
                        if (b && b.key === key)
                            b.flash()
                    }
                }

                delegate: CircleButton {
                    required property string modelData
                    readonly property bool isBackspace: modelData === "⌫"

                    key: modelData
                    width: keypadArea.cell
                    height: keypadArea.cell
                    topInset: keypadArea.cell * 0.05
                    bottomInset: keypadArea.cell * 0.05
                    leftInset: keypadArea.cell * 0.05
                    rightInset: keypadArea.cell * 0.05

                    text: isBackspace ? "" : modelData
                    icon.source: isBackspace ? "/images/ic_backspace_24px.svg" : ""
                    display: isBackspace ? CircleButton.IconOnly : CircleButton.TextOnly
                    fillColor: isBackspace ? "transparent" : Style.key
                    enabled: isBackspace ? view.digits !== "" : true

                    Accessible.name: isBackspace ? qsTr("Delete") : modelData

                    onClicked: isBackspace ? view.backspace() : view.appendDigits(modelData)
                }
            }
        }
    }

    // Start button
    Item {
        x: 0
        y: view.landscape ? displayArea.height : displayArea.height + keypadArea.height
        width: displayArea.width
        height: view.height - y

        CircleButton {
            anchors.centerIn: parent
            width: Math.min(parent.height * 0.8, keypadArea.cell * 1.1)
            height: width

            visible: view.digits !== ""
            fillColor: Style.teal
            textColor: Style.tealText
            icon.source: "/images/ic_play_arrow_24px.svg"
            display: CircleButton.IconOnly

            Accessible.name: qsTr("Start")

            onClicked: view.startRequested()
        }
    }
}
