# cumTempore

I would like to vibe-code a simple app that I can use to keep the time while giving university lectures or talks.

## Platforms

- Android
- iOS
- macOS
- Windows
- WebAssembly

## User Experience

A simple app, design similar to "Add Times".

- A swipe view, where the user can choose between a wall clock and a timer
- Minimal menu with "About", "Help", and "Quit" on platforms where appropriate

### Wall Clock

- In light mode: white background, local time in teal, local time display fills window. Appropriate colors in dark mode.

### Timer

- GUI design very similar to Google's Clock app.
- While running: teal is standard, dark red when remaining time is <= 5 mins, red display with white text when overtime (text then counts upwards). 

### General

- Screen always stays on, screen savers disabled
- Translated to all languages that Enroute supports, plus Portuguese
- App remembers GUI state (wall clock / timer) on restart, remembers timer value last set
- Clocks use the airbus B361 font

## Tech Stack

- Qt similar to "Add Times"

## Buildsystem

- CMake similar to "Add Times", but ensure using best practices
- GitHub actions for quality check as appropriate
- GitHub actions that produce macOS binary, iOS binary for emulator, Android binary (arm64) for emulator and sideloading, windows binary, WebAssembly site.
- fastlane script for deplyment, similar to Enroute or "Add Times".

## Other

- Website, similar to "Add Times", but I am unsure what site generator to best use
- Icon similar to Add Times
