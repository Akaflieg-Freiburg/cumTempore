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

#include "Haptics.h"

#include <QCoreApplication>
#include <QJniObject>

// Vibrates for 10 ms, like the sister app "Add Times". Needs the permission
// android.permission.VIBRATE.
void Haptics::keyPress()
{
    QJniObject const context = QNativeInterface::QAndroidApplication::context();
    if (!context.isValid())
    {
        return;
    }

    QJniObject vibrator;
    if (QNativeInterface::QAndroidApplication::sdkVersion() >= 31)
    {
        QJniObject const manager = context.callObjectMethod("getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;",
                                                            QJniObject::fromString(QStringLiteral("vibrator_manager")).object<jstring>());
        if (manager.isValid())
        {
            vibrator = manager.callObjectMethod("getDefaultVibrator", "()Landroid/os/Vibrator;");
        }
    }
    else
    {
        vibrator = context.callObjectMethod("getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;",
                                            QJniObject::fromString(QStringLiteral("vibrator")).object<jstring>());
    }
    if (!vibrator.isValid() || !vibrator.callMethod<jboolean>("hasVibrator"))
    {
        return;
    }

    constexpr jlong DURATION_MS = 10;
    constexpr jint DEFAULT_AMPLITUDE = -1; // android.os.VibrationEffect
    QJniObject const effect = QJniObject::callStaticObjectMethod("android/os/VibrationEffect", "createOneShot", "(JI)Landroid/os/VibrationEffect;",
                                                                 DURATION_MS, DEFAULT_AMPLITUDE);
    if (!effect.isValid())
    {
        return;
    }
    vibrator.callMethod<void>("vibrate", "(Landroid/os/VibrationEffect;)V", effect.object());
}
