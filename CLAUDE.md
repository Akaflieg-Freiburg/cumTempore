# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

cumTempore is a small Qt 6/QML app for keeping time during lectures and talks: a
full-window wall clock and a countdown timer, switched by swiping. GPL-3.0-or-later,
published by Akaflieg Freiburg. Sibling project and template: "Add Times"
(`../addhoursandminutes`). Requirements are in `REQ.md`.

- App ID `de.akaflieg_freiburg.cavok.cum_tempore`; Apple platforms use
  `de.akafliegfreiburg.cumtempore` (no underscores allowed). Both are set once in the
  top-level `CMakeLists.txt` (`APP_ID`, `APP_ID_APPLE`).
- Clock font: Airbus B612 (proportional, but with tabular digits, so clocks do not jitter; B612 Mono was rejected because its colon sits at the left edge of a full-width cell) (`3rdParty/fonts/`, SIL OFL), registered in `src/main.cpp`.
- Languages: English source + cs de es fr it pl pt (`APP_LANGUAGES` in the top-level
  `CMakeLists.txt`).

## Build

Qt ≥ 6.9, CMake ≥ 3.21 (presets need ≥ 3.25), C++20. Qt kits are located through
environment variables: `Qt6_DIR_LINUX`, `Qt6_DIR_BASE` (Android kits live in
`$Qt6_DIR_BASE/android_*`), `Qt6_DIR_WASM`, plus `ANDROID_SDK_ROOT`, `ANDROID_NDK_ROOT`
and `QT_ANDROID_KEYSTORE_*` for signed Android builds.

```bash
cmake --preset linux-debug && cmake --build --preset linux-debug   # clang + ASan/UBSan
ctest --preset linux-debug                                         # unit tests
cmake --build --preset linux-debug --target all_qmllint            # QML lint
build/linux-debug/src/cumtempore
```

Other presets: `linux-release`, `android` (signed release APK/AAB), `android-debug`
(debug-signed, installable), `wasm`. Build output goes to `build/<preset>/`. The WASM kit
needs exactly the Emscripten version Qt was built with (Qt 6.12: 5.0.5); CMake stops on a
mismatch.

Other targets: `update_translations` (lupdate, no source locations), `generatedSources`
(renders all icons from `metadata/*.svg`; needs rsvg-convert and Pillow; results are
committed), `apk`/`aab` on Android.

## Checking the UI without a display

Developer aids in `src/main.cpp`, used for visual checks and store screenshots:

```bash
QT_QPA_PLATFORM=offscreen XDG_CONFIG_HOME=/tmp/cfg \
  CUMTEMPORE_KEYS="Right 1 3 0 0 0 Return" CUMTEMPORE_SCREENSHOT=/tmp/shot.png \
  CUMTEMPORE_SCREENSHOT_DELAY=2500 build/linux-debug/src/cumtempore
```

`CUMTEMPORE_KEYS` replays key presses (QKeySequence names), `CUMTEMPORE_COLOR_SCHEME=dark|light`
overrides the color scheme. Use a separate `XDG_CONFIG_HOME` so that test runs do not touch
the real settings. Limitations of the offscreen platform: it ignores the color scheme
override, and Material popups render without background; use the real platform (Wayland)
for those. The debug preset uses ASan; leak reports on exit come from Qt's GTK theme plugin.

## Architecture

- `src/main.cpp`: translators, fonts, keep-screen-on, loads `CumTempore/Main.qml`.
- `src/qml/`: QML module `CumTempore`. `Main.qml` is a plain `Window` (not
  `ApplicationWindow`, which swallows safe-area margins) with a desktop `MenuBar` or a
  floating burger menu on mobile/WASM, and a `SwipeView` with `WallClockPage` and
  `TimerPage`. `Style.qml` is the colour singleton. Menus/dialogs bind palettes explicitly
  because Qt does not push colour-scheme changes into existing popups.
- `src/timer/CountdownTimer`: timer logic in C++, unit-tested in `tests/`. Stores the
  absolute end time in wall-clock milliseconds (monotonic clocks stop while phones sleep).
- `src/platform/KeepScreenOn_<platform>`: one implementation per OS, chosen in
  `src/CMakeLists.txt`.
- Platform files: `src/android/` + `src/AndroidManifest.xml.in` (no `<uses-sdk>`, no
  `package=`, keep the `%%INSERT_…%%` placeholders; XML comments must not contain `--`),
  `src/ios/`, `src/macos/`, `src/windows/`, `metadata/` (Linux desktop/AppStream, icon SVGs).

## Release infrastructure

- CI: `.github/workflows/`. `quality.yml` (sanitizer build with warnings as errors, ctest,
  qmllint, .ts files up to date), one workflow per platform uploading to the
  `developerBuilds` pre-release, `pages.yml` (WASM + Hugo site, deploys from `main`). The
  Qt version is pinned in `.github/qt-version`.
- fastlane: `fastlane/Fastfile`; store texts in `fastlane/metadata/{android,ios}/<locale>/`.
  The Mac App Store lane needs `src/macos/cumtempore_for_Mac_App_Store.provisionprofile`.
- Website: Hugo, `website/` (custom minimal layout, no theme). Preview with
  `cd website && hugo server`.

## Versioning

`project(cumtempore VERSION x.y.z)` in the top-level `CMakeLists.txt` is the single source
of truth. Android versionCode = `100000*major + 1000*minor + patch`; keep this formula
in sync with `fastlane/Fastfile`. `CHANGELOG.md` headings (`## x.y.z - date`) are parsed by
fastlane.

## Git conventions

Day-to-day work happens on `develop`; `main` is the release branch.
