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

#include <emscripten.h>

// Screen Wake Lock API. Browsers release the lock whenever the page is
// hidden, so it is requested again on every visibility change. Browsers
// without the API (or pages not served over HTTPS) simply do without.
// clang-format off
EM_JS(void, cumtempore_request_wake_lock, (), {
    if (!('wakeLock' in navigator)) {
        return;
    }
    const request = () => {
        if (document.visibilityState !== 'visible') {
            return;
        }
        if (globalThis.cumtemporeWakeLock && !globalThis.cumtemporeWakeLock.released) {
            return;
        }
        navigator.wakeLock.request('screen')
            .then((lock) => { globalThis.cumtemporeWakeLock = lock; })
            .catch((err) => { console.log('cumTempore: wake lock unavailable: ' + err); });
    };
    if (!globalThis.cumtemporeWakeLockListener) {
        globalThis.cumtemporeWakeLockListener = true;
        document.addEventListener('visibilitychange', request);
    }
    request();
});
// clang-format on

void Platform::keepScreenOn()
{
    cumtempore_request_wake_lock();
}
