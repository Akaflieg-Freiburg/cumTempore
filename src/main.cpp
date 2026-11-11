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

#include <QFontDatabase>
#include <QGuiApplication>
#include <QIcon>
#include <QKeyEvent>
#include <QKeySequence>
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
    QGuiApplication::setWindowIcon(QIcon(u":/icon.png"_s));

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

    // Developer aid for checking the UI in headless environments and for
    // producing store screenshots:
    //   CUMTEMPORE_KEYS="Right 1 3 0 Return"  key presses sent after startup
    //   CUMTEMPORE_SCREENSHOT=<file.png>      saves a screenshot and quits
    //   CUMTEMPORE_SCREENSHOT_DELAY=<ms>      delay before the screenshot
    auto const screenshotPath = qEnvironmentVariable("CUMTEMPORE_SCREENSHOT");
    if (!screenshotPath.isEmpty())
    {
        auto* window = qobject_cast<QQuickWindow*>(engine.rootObjects().value(0));
        auto const keys = qEnvironmentVariable("CUMTEMPORE_KEYS").split(u' ', Qt::SkipEmptyParts);
        QTimer::singleShot(500, &app, [window, keys]() {
            for (auto const& keyName : keys)
            {
                auto const combination = QKeySequence::fromString(keyName)[0];
                QKeyEvent press(QEvent::KeyPress, combination.key(), combination.keyboardModifiers(), keyName.size() == 1 ? keyName : QString());
                QKeyEvent release(QEvent::KeyRelease, combination.key(), combination.keyboardModifiers(), keyName.size() == 1 ? keyName : QString());
                QCoreApplication::sendEvent(window, &press);
                QCoreApplication::sendEvent(window, &release);
            }
        });
        auto const delay = qEnvironmentVariableIntValue("CUMTEMPORE_SCREENSHOT_DELAY");
        QTimer::singleShot(delay > 0 ? delay : 1500, &app, [window, screenshotPath]() {
            if (window != nullptr)
            {
                window->grabWindow().save(screenshotPath);
            }
            QCoreApplication::quit();
        });
    }

    // Keep the screen on. Some platforms drop the request when the app goes
    // into the background, so it is renewed whenever the app becomes active.
    Platform::keepScreenOn();
    QObject::connect(&app, &QGuiApplication::applicationStateChanged, &app, [](Qt::ApplicationState state) {
        if (state == Qt::ApplicationActive)
        {
            Platform::keepScreenOn();
        }
    });

#if defined(Q_OS_ANDROID)
    QNativeInterface::QAndroidApplication::hideSplashScreen(1);
#endif

    return QGuiApplication::exec();
}
