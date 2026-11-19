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

#include "StatusBar.h"

#include <QCoreApplication>
#include <QJniObject>

// Clears the "light status bar" appearance, which makes the system draw dark
// icons. Window decorations must be changed on the Android UI thread. Calls
// posted from here run after Qt's own handling of a color scheme change,
// which happens on the same thread.
void Platform::useWhiteStatusBarIcons()
{
    QNativeInterface::QAndroidApplication::runOnAndroidMainThread([]() {
        QJniObject const activity = QNativeInterface::QAndroidApplication::context();
        if (!activity.isValid())
        {
            return;
        }
        QJniObject const window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");
        if (!window.isValid())
        {
            return;
        }

        if (QNativeInterface::QAndroidApplication::sdkVersion() >= 30)
        {
            QJniObject const controller = window.callObjectMethod("getInsetsController", "()Landroid/view/WindowInsetsController;");
            if (!controller.isValid())
            {
                return;
            }
            constexpr int APPEARANCE_LIGHT_STATUS_BARS = 8; // android.view.WindowInsetsController
            controller.callMethod<void>("setSystemBarsAppearance", "(II)V", 0, APPEARANCE_LIGHT_STATUS_BARS);
        }
        else
        {
            QJniObject const decorView = window.callObjectMethod("getDecorView", "()Landroid/view/View;");
            if (!decorView.isValid())
            {
                return;
            }
            constexpr int SYSTEM_UI_FLAG_LIGHT_STATUS_BAR = 0x2000; // android.view.View
            auto const flags = decorView.callMethod<jint>("getSystemUiVisibility", "()I");
            decorView.callMethod<void>("setSystemUiVisibility", "(I)V", flags & ~SYSTEM_UI_FLAG_LIGHT_STATUS_BAR);
        }
    });
}
