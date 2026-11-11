#
# Application target
#

qt_add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE
    main.cpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    PROJECT_VERSION="${PROJECT_VERSION}"
    APP_ID="${APP_ID}"
)

target_include_directories(${PROJECT_NAME} PRIVATE platform timer)

# Platform-specific code that keeps the screen on, see platform/KeepScreenOn.h
target_sources(${PROJECT_NAME} PRIVATE platform/KeepScreenOn.h)
if (ANDROID)
    target_sources(${PROJECT_NAME} PRIVATE platform/KeepScreenOn_android.cpp)
elseif (IOS)
    enable_language(OBJCXX)
    target_sources(${PROJECT_NAME} PRIVATE platform/KeepScreenOn_ios.mm)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework UIKit")
elseif (APPLE)
    target_sources(${PROJECT_NAME} PRIVATE platform/KeepScreenOn_macos.cpp)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework IOKit" "-framework CoreFoundation")
elseif (WIN32)
    target_sources(${PROJECT_NAME} PRIVATE platform/KeepScreenOn_windows.cpp)
elseif (EMSCRIPTEN)
    target_sources(${PROJECT_NAME} PRIVATE platform/KeepScreenOn_wasm.cpp)
else ()
    target_sources(${PROJECT_NAME} PRIVATE platform/KeepScreenOn_linux.cpp)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::DBus)
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Gui
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Svg
    cumtempore_warnings
)

set_source_files_properties(qml/Style.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(${PROJECT_NAME}
    URI CumTempore
    VERSION 1.0
    QML_FILES
        qml/CenteringDialog.qml
        qml/CircleButton.qml
        qml/DecoratedScrollView.qml
        qml/LongTextDialog.qml
        qml/Main.qml
        qml/ProgressRing.qml
        qml/Style.qml
        qml/TimerPage.qml
        qml/TimerRunningView.qml
        qml/TimerSetView.qml
        qml/WallClockPage.qml
    SOURCES
        timer/CountdownTimer.cpp
        timer/CountdownTimer.h
)

qt_add_resources(${PROJECT_NAME} "images"
    PREFIX "/"
    FILES
        images/ic_backspace_24px.svg
        images/ic_help_24px.svg
        images/ic_info_24px.svg
        images/ic_menu_24px.svg
        images/ic_pause_24px.svg
        images/ic_play_arrow_24px.svg
        images/ic_replay_24px.svg
)

# Fonts: Airbus B612 (tabular digits), SIL Open Font License, see 3rdParty/fonts/OFL.txt
qt_add_resources(${PROJECT_NAME} "fonts"
    PREFIX "/fonts"
    BASE "${PROJECT_SOURCE_DIR}/3rdParty/fonts"
    FILES
        "${PROJECT_SOURCE_DIR}/3rdParty/fonts/B612-Regular.ttf"
        "${PROJECT_SOURCE_DIR}/3rdParty/fonts/B612-Bold.ttf"
)

# Qt Quick Controls looks for this file at :/qtquickcontrols2.conf
qt_add_resources(${PROJECT_NAME} "controlsconf"
    PREFIX "/"
    FILES
        qtquickcontrols2.conf
)


#
# Installation for Linux, macOS, Windows
# see https://www.qt.io/blog/deploying-to-linux-with-cmake
#

qt_generate_deploy_qml_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

if (CMAKE_SYSTEM_NAME MATCHES "^(Darwin|Linux|Windows)$")
    include(GNUInstallDirs)
    install(TARGETS ${PROJECT_NAME}
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    if (QTDEPLOY)
        install(SCRIPT ${deploy_script})
    endif ()
endif ()
