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

#include "KeepScreenOn.h"

#include <QCoreApplication>
#include <QJniObject>

// Sets FLAG_KEEP_SCREEN_ON on the activity window. Window flags must be
// changed on the Android UI thread. The flag stays set across pause/resume.
void Platform::keepScreenOn()
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
        constexpr int FLAG_KEEP_SCREEN_ON = 128; // android.view.WindowManager.LayoutParams
        window.callMethod<void>("addFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
    });
}
