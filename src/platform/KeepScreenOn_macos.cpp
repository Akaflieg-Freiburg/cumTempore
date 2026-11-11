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

#include <IOKit/pwr_mgt/IOPMLib.h>

namespace {
IOPMAssertionID assertionID = kIOPMNullAssertionID;
} // namespace

// Holds a power management assertion for the lifetime of the app. macOS
// releases it automatically when the process exits.
void Platform::keepScreenOn()
{
    if (assertionID != kIOPMNullAssertionID)
    {
        return;
    }
    IOPMAssertionCreateWithName(kIOPMAssertionTypePreventUserIdleDisplaySleep,
                                kIOPMAssertionLevelOn,
                                CFSTR("cumTempore keeps the clock visible"),
                                &assertionID);
}
