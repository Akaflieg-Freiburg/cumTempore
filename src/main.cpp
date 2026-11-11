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
#include <QQuickWindow>
#include <QStyleHints>
#include <QTimer>
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
    QFontDatabase::addApplicationFont(u":/fonts/B612-Regular.ttf"_s);
    QFontDatabase::addApplicationFont(u":/fonts/B612-Bold.ttf"_s);

    // Developer aid: CUMTEMPORE_COLOR_SCHEME=dark|light overrides the system
    // color scheme
    auto const colorScheme = qEnvironmentVariable("CUMTEMPORE_COLOR_SCHEME");
    if (colorScheme == u"dark"_s)
    {
        QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Dark);
    }
    else if (colorScheme == u"light"_s)
    {
        QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Light);
    }

#if defined(Q_OS_WINDOWS)
    QQuickStyle::setStyle(u"Universal"_s);
#endif

    // Start QML engine
    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
    engine.loadFromModule("CumTempore", "Main");

    // Developer aid: CUMTEMPORE_SCREENSHOT=<file.png> saves a screenshot of the
    // main window after a short delay and quits. Used for checking the UI in
    // headless environments and for producing store screenshots.
    auto const screenshotPath = qEnvironmentVariable("CUMTEMPORE_SCREENSHOT");
    if (!screenshotPath.isEmpty())
    {
        QTimer::singleShot(1500, &app, [&engine, screenshotPath]() {
            if (auto* window = qobject_cast<QQuickWindow*>(engine.rootObjects().value(0)))
            {
                window->grabWindow().save(screenshotPath);
            }
            QCoreApplication::quit();
        });
    }

#if defined(Q_OS_ANDROID)
    QNativeInterface::QAndroidApplication::hideSplashScreen(1);
#endif

    return QGuiApplication::exec();
}
