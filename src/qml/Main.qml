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

import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

Window {
    id: window

    // Desktop platforms get a menu bar; everywhere else a floating burger
    // button opens the menu. Note that Qt 6 reports macOS as "osx".
    readonly property bool isMacOS: Qt.platform.os === "osx"
    readonly property bool hasMenuBar: isMacOS
                                       || (Qt.platform.os === "linux")
                                       || (Qt.platform.os === "windows")

    // Height that the menu bar takes away from the window content. Zero on
    // macOS, where the menu bar is native and lives outside of the window.
    readonly property real menuBarHeight: mainMenuBar.visible ? mainMenuBar.height : 0

    // An explicit binding rather than Material.System: only explicitly themed
    // Material objects register for system color scheme updates.
    Material.theme: Style.isDark ? Material.Dark : Material.Light
    Material.accent: Style.teal

    width: 800
    height: 500 + menuBarHeight
    minimumWidth: 320
    minimumHeight: 240 + menuBarHeight

    // iOS: draw below the status bar and home indicator; the content respects
    // the safe area margins itself.
    flags: (Qt.platform.os === "ios") ? Qt.ExpandedClientAreaHint : Qt.Window

    title: Application.displayName
    color: (swipeView.currentIndex === 1) ? timerPage.pageColor : Style.background
    visible: true

    Settings {
        id: settings

        property alias x: window.x
        property alias y: window.y
        property alias width: window.width
        property alias height: window.height
        property int currentPage: 0
    }

    Component.onCompleted: swipeView.setCurrentIndex(Math.max(0, Math.min(settings.currentPage, swipeView.count-1)))

    // Menu bar for desktop computers. On macOS this becomes the native menu
    // bar. Qt moves the items whose text begins with "About" or "Quit" into the
    // application menu there and hides menus that are left empty - so the
    // "File" menu shows up on Linux and Windows only.
    //
    // The window is a plain Window, not an ApplicationWindow: the latter
    // absorbs the safe area margins and reports none of them to its content.
    MenuBar {
        id: mainMenuBar

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        visible: window.hasMenuBar

        Material.background: Style.popupWindow
        palette {
            window: Style.popupWindow
            windowText: Style.popupWindowText
            text: Style.popupText
            base: Style.popupWindow
            button: Style.popupWindow
            buttonText: Style.popupButtonText
            mid: Style.popupMid
            dark: Style.popupDark
        }

        Menu {
            title: qsTr("File")

            Material.background: Style.popupWindow
            palette {
                window: Style.popupWindow
                windowText: Style.popupWindowText
                text: Style.popupText
                base: Style.popupWindow
                button: Style.popupButton
                buttonText: Style.popupButtonText
                mid: Style.popupMid
                dark: Style.popupDark
            }

            Action {
                // On macOS this item moves into the application menu, where the
                // platform substitutes its own localized "Quit <app>". Qt only
                // recognizes the untranslated English wording.
                text: window.isMacOS ? "Quit" : qsTr("Quit")
                shortcut: StandardKey.Quit
                onTriggered: Qt.quit()
            }
        }

        Menu {
            title: qsTr("View")

            Material.background: Style.popupWindow
            palette {
                window: Style.popupWindow
                windowText: Style.popupWindowText
                text: Style.popupText
                base: Style.popupWindow
                button: Style.popupButton
                buttonText: Style.popupButtonText
                mid: Style.popupMid
                dark: Style.popupDark
            }

            Action {
                text: qsTr("Wall Clock")
                shortcut: "Ctrl+1"
                checkable: true
                checked: swipeView.currentIndex === 0
                onTriggered: swipeView.setCurrentIndex(0)
            }
            Action {
                text: qsTr("Timer")
                shortcut: "Ctrl+2"
                checkable: true
                checked: swipeView.currentIndex === 1
                onTriggered: swipeView.setCurrentIndex(1)
            }
            MenuSeparator { }
            Action {
                text: qsTr("Full Screen")
                shortcut: StandardKey.FullScreen
                checkable: true
                checked: window.visibility === Window.FullScreen
                onTriggered: window.visibility = (window.visibility === Window.FullScreen) ? Window.Windowed : Window.FullScreen
            }
        }

        // AppKit adds its search field to the menu titled "Help"
        Menu {
            title: qsTr("Help")

            Material.background: Style.popupWindow
            palette {
                window: Style.popupWindow
                windowText: Style.popupWindowText
                text: Style.popupText
                base: Style.popupWindow
                button: Style.popupButton
                buttonText: Style.popupButtonText
                mid: Style.popupMid
                dark: Style.popupDark
            }

            Action {
                text: qsTr("Help")
                shortcut: StandardKey.HelpContents
                onTriggered: helpDialog.open()
            }

            // No separator above this item: on macOS it moves into the
            // application menu, and a separator would be left dangling.
            Action {
                // Untranslated on macOS for the same reason as "Quit" above
                text: window.isMacOS ? "About" : qsTr("About")
                onTriggered: infoDialog.open()
            }
        }
    }

    // Content area, inside the safe area of the window and below the menu bar
    Item {
        id: content

        anchors.fill: parent
        anchors.topMargin: window.menuBarHeight + parent.SafeArea.margins.top
        anchors.bottomMargin: parent.SafeArea.margins.bottom
        anchors.leftMargin: parent.SafeArea.margins.left
        anchors.rightMargin: parent.SafeArea.margins.right

        SwipeView {
            id: swipeView

            anchors.fill: parent
            focus: true

            onCurrentIndexChanged: settings.currentPage = currentIndex

            WallClockPage { }
            TimerPage { id: timerPage }

            // Left/right arrow keys switch pages on desktop computers
            Keys.onLeftPressed: decrementCurrentIndex()
            Keys.onRightPressed: incrementCurrentIndex()
        }

        PageIndicator {
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter

            count: swipeView.count
            currentIndex: swipeView.currentIndex
            interactive: true
            onCurrentIndexChanged: swipeView.setCurrentIndex(currentIndex)

            delegate: Rectangle {
                required property int index

                implicitWidth: 8
                implicitHeight: 8
                radius: width / 2
                color: (window.color !== Style.background) ? Style.overtimeText : Style.teal
                opacity: index === swipeView.currentIndex ? 0.95 : 0.35
            }
        }

        // Burger menu for mobile devices and the web browser
        ToolButton {
            id: menuButton

            anchors.top: parent.top
            anchors.left: parent.left

            visible: !window.hasMenuBar

            icon.source: "/images/ic_menu_24px.svg"
            icon.color: Style.teal

            Accessible.name: qsTr("Menu")

            onClicked: mainMenu.open()

            Menu {
                id: mainMenu

                Material.background: Style.popupWindow
                palette {
                    window: Style.popupWindow
                    windowText: Style.popupWindowText
                    text: Style.popupText
                    base: Style.popupWindow
                    button: Style.popupButton
                    buttonText: Style.popupButtonText
                    mid: Style.popupMid
                    dark: Style.popupDark
                }

                MenuItem {
                    icon.source: "/images/ic_help_24px.svg"
                    text: qsTr("Help")
                    onTriggered: helpDialog.open()
                }

                MenuItem {
                    icon.source: "/images/ic_info_24px.svg"
                    text: qsTr("About")
                    onTriggered: infoDialog.open()
                }
            }
        }
    }

    LongTextDialog {
        id: helpDialog

        // qmllint disable
        title: qsTr("Help")
        text: qsTr("
<p>Swipe left or right to switch between the <strong>wall clock</strong> and
the <strong>timer</strong>. On a computer, you can also use the arrow keys.</p>

<h4>Timer</h4>

<p>Type the duration on the keypad. To set a timer for <strong>1 hour and
30 minutes</strong>, press <strong>1</strong>, <strong>3</strong>,
<strong>0</strong>, <strong>0</strong>, <strong>0</strong>. Then press the
start button.</p>

<p>The display turns dark red during the last five minutes. Once the time is
up, the screen turns red and the timer counts the overtime.</p>

<p>The screen stays on while the app is open.</p>
")
        // qmllint enable

        standardButtons: DialogButtonBox.Ok
    }

    LongTextDialog {
        id: infoDialog

        // qmllint disable
        title: Application.displayName
        text: qsTr("
<h4>Version %1</h4>

<p>A simple clock and timer for keeping time while giving lectures or talks.</p>

<ul style='margin-left:-25px;'>
<li>Simple, elegant and functional</li>
<li>No ads</li>
<li>No commercial 'pro' version</li>
<li><a href='https://akaflieg-freiburg.github.io/cumTempore/privacy'>Does not spy on you</a></li>
<li><a href='https://github.com/Akaflieg-Freiburg/cumTempore'>100% Open Source</a></li>
<li>Written without commercial interest</li>
</ul>

<p>This app is available on a variety of
platforms. <a href='https://akaflieg-freiburg.github.io/cumTempore'>
Have a look at the homepage.</a></p>

<h3>Author</h3>

<p>
Stefan Kebekus<br>
Wintererstraße 77<br>
79104 Freiburg im Breisgau<br>
Germany
</p>

<h3>License</h3>

<p>This program is licensed under the
<a href='https://www.gnu.org/licenses/gpl-3.0-standalone.html'>GNU General
Public License V3</a>.</p>

<h3>Acknowledgements</h3>

<p>This program builds on a number of open source libraries, including
<a href='https://www.qt.io'>Qt</a>. The clock font is
<a href='https://github.com/polarsys/b612'>B612</a> by Airbus, licensed under
the SIL Open Font License.</p>
").arg(Application.version)
        // qmllint enable

        standardButtons: DialogButtonBox.Ok
    }
}
