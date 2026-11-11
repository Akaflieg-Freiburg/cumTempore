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
- Clock font: Airbus B612 Mono (`3rdParty/fonts/`, SIL OFL), registered in `src/main.cpp`.
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

Other presets: `linux-release`, `android`, `wasm`. Build output goes to `build/<preset>/`.

## Versioning

`project(cumtempore VERSION x.y.z)` in the top-level `CMakeLists.txt` is the single source
of truth. Android versionCode = `100000*major + 1000*minor + patch`; keep this formula
in sync with `fastlane/Fastfile`. `CHANGELOG.md` headings (`## x.y.z - date`) are parsed by
fastlane.

## Git conventions

Day-to-day work happens on `develop`; `main` is the release branch.
