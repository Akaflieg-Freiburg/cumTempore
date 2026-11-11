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
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusObjectPath>
#include <QVariantMap>

using namespace Qt::Literals::StringLiterals;

namespace {
bool inhibited = false;
} // namespace

// Asks the desktop to inhibit idle (screen saver, screen blanking), first
// through the XDG desktop portal, which works on GNOME, KDE and inside
// Flatpak, then through the older org.freedesktop.ScreenSaver interface.
// Both inhibitions last until the D-Bus connection closes, i.e. until the
// app exits.
void Platform::keepScreenOn()
{
    if (inhibited)
    {
        return;
    }
    auto bus = QDBusConnection::sessionBus();
    if (!bus.isConnected())
    {
        return;
    }
    auto const reason = QCoreApplication::translate("Platform", "Showing the clock");

    QDBusInterface portal(u"org.freedesktop.portal.Desktop"_s,
                          u"/org/freedesktop/portal/desktop"_s,
                          u"org.freedesktop.portal.Inhibit"_s,
                          bus);
    if (portal.isValid())
    {
        constexpr uint flagIdle = 8;
        QDBusReply<QDBusObjectPath> const reply = portal.call(u"Inhibit"_s, QString(), flagIdle,
                                                              QVariantMap{{u"reason"_s, reason}});
        if (reply.isValid())
        {
            inhibited = true;
            return;
        }
    }

    QDBusInterface screenSaver(u"org.freedesktop.ScreenSaver"_s,
                               u"/org/freedesktop/ScreenSaver"_s,
                               u"org.freedesktop.ScreenSaver"_s,
                               bus);
    if (screenSaver.isValid())
    {
        QDBusReply<uint> const reply = screenSaver.call(u"Inhibit"_s, QCoreApplication::applicationName(), reason);
        inhibited = reply.isValid();
    }
}
