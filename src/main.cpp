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

#include <QFontDatabase>
#include <QGuiApplication>
#include <QIcon>
#include <QLibraryInfo>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QTranslator>

using namespace Qt::Literals::StringLiterals;

auto main(int argc, char *argv[]) -> int
{
    QGuiApplication const app(argc, argv);

    // Application parameters. The application name determines where QSettings
    // are stored and must not depend on the locale; the translated name goes
    // into applicationDisplayName instead.
    QCoreApplication::setOrganizationName(u"Akaflieg Freiburg"_s);
    QCoreApplication::setOrganizationDomain(u"akaflieg_freiburg.de"_s);
    QCoreApplication::setApplicationName(u"cumTempore"_s);
    QCoreApplication::setApplicationVersion(QStringLiteral(PROJECT_VERSION));
    QGuiApplication::setDesktopFileName(QStringLiteral(APP_ID));

    // Translators. QTranslator::load(QLocale, ...) walks through the user's
    // list of preferred UI languages and picks the first one available.
    QTranslator translator;
    if (translator.load(QLocale(), u"cumtempore"_s, u"_"_s, u":/i18n"_s))
    {
        QCoreApplication::installTranslator(&translator);
    }
    QTranslator qtTranslator;
    if (qtTranslator.load(QLocale(), u"qtbase"_s, u"_"_s, QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
    {
        QCoreApplication::installTranslator(&qtTranslator);
    }
    QGuiApplication::setApplicationDisplayName(QCoreApplication::translate("C++ Main Program", "cumTempore", "Application Name"));

    // Fonts
    QFontDatabase::addApplicationFont(u":/fonts/B612Mono-Regular.ttf"_s);
    QFontDatabase::addApplicationFont(u":/fonts/B612Mono-Bold.ttf"_s);

#if defined(Q_OS_WINDOWS)
    QQuickStyle::setStyle(u"Universal"_s);
#endif

    // Start QML engine
    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
    engine.loadFromModule("CumTempore", "Main");

#if defined(Q_OS_ANDROID)
    QNativeInterface::QAndroidApplication::hideSplashScreen(1);
#endif

    return QGuiApplication::exec();
}
