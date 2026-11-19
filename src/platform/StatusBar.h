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

namespace Platform {

/*! \brief Shows the Android status bar icons in white
 *
 *  The status bar is teal in light and dark mode alike (see the
 *  SplashScreenTheme in src/android/res and Main.qml), so its icons must be
 *  white. Qt resets the icon color to match the system color scheme when the
 *  app starts and whenever the scheme changes, so main() calls this function
 *  at startup, on every color scheme change and whenever the app becomes
 *  active. The function is idempotent. Android only.
 */
void useWhiteStatusBarIcons();

} // namespace Platform
