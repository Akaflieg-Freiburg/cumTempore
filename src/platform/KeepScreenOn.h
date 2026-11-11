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

/*! \brief Keeps the screen on and disables the screen saver
 *
 *  The implementation is platform specific, see the files
 *  KeepScreenOn_<platform>.* in this directory. The function is idempotent;
 *  main() calls it at startup and whenever the application becomes active,
 *  because some platforms (iOS, web browsers) drop the request when the app
 *  goes into the background.
 */
void keepScreenOn();

} // namespace Platform
