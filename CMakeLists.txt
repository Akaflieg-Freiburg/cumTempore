cmake_minimum_required(VERSION 3.21)

project(cumtempore
    VERSION 0.1.0
    DESCRIPTION "Clock and timer for lectures and talks"
    HOMEPAGE_URL "https://akaflieg-freiburg.github.io/cumTempore"
    LANGUAGES CXX
)

# Identifiers used throughout the build. Apple platforms do not allow
# underscores in bundle identifiers, hence the separate Apple ID.
set(APP_ID de.akaflieg_freiburg.cavok.cum_tempore)
set(APP_ID_APPLE de.akafliegfreiburg.cumtempore)
set(DISPLAY_NAME "cumTempore")
math(EXPR PROJECT_VERSION_CODE "100000*${PROJECT_VERSION_MAJOR}+1000*${PROJECT_VERSION_MINOR}+${PROJECT_VERSION_PATCH}")

# The language list is used by qt_add_translations and by the Apple lproj
# handling in src/CMakeLists.txt. Keep in sync with fastlane/metadata.
set(APP_LANGUAGES cs de es fr it pl pt)

option(QTDEPLOY "Generate and run Qt deployment scripts on install" OFF)
option(CUMTEMPORE_BUILD_TESTS "Build unit tests" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.9 REQUIRED COMPONENTS Core Gui LinguistTools Qml Quick QuickControls2 Svg)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME MATCHES "BSD")
    find_package(Qt6 REQUIRED COMPONENTS DBus)
endif ()

set(QT_QML_GENERATE_QMLLS_INI ON)
qt_standard_project_setup(REQUIRES 6.9 I18N_TRANSLATED_LANGUAGES ${APP_LANGUAGES})
qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

# Common compile options, applied per target via this interface library
add_library(cumtempore_warnings INTERFACE)
if (MSVC)
    target_compile_options(cumtempore_warnings INTERFACE /W4 /permissive-)
else ()
    target_compile_options(cumtempore_warnings INTERFACE -Wall -Wextra -Wpedantic)
endif ()

add_subdirectory(src)
add_subdirectory(metadata)
add_subdirectory(generatedSources)

if (CUMTEMPORE_BUILD_TESTS AND NOT (ANDROID OR IOS OR EMSCRIPTEN))
    enable_testing()
    add_subdirectory(tests)
endif ()
