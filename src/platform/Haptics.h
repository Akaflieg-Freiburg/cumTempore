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

#pragma once

#include <QObject>
#include <QtQml/qqmlregistration.h>

/*! \brief Haptic feedback for key presses, available in QML as "Haptics"
 *
 *  On Android, the device vibrates very briefly; on iOS, it plays a light
 *  impact. On other platforms, this does nothing. The implementation is
 *  platform specific, see the files Haptics_<platform>.* in this directory.
 */
class Haptics : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    using QObject::QObject;

    /*! \brief Gives haptic feedback for a key press */
    Q_INVOKABLE void keyPress();
};
