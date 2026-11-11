---
title: "Download"
---

**cumTempore** is available for Android, iOS, macOS, Windows, and runs directly
in your web browser.

### In your web browser

[Open cumTempore in your browser.](../app/cumtempore.html) The web version keeps
the screen on in browsers that support the Screen Wake Lock API, which includes
Chrome, Edge, Safari and recent versions of Firefox.

### Mobile and desktop apps

The app is being prepared for Google Play, the App Store and the Mac App Store.
Until then, test builds of the current development version are available on the
[GitHub release page](https://github.com/Akaflieg-Freiburg/cumTempore/releases/tag/developerBuilds):

- Android (arm64): `cumTempore-android-arm64v8a.apk`, for sideloading
- macOS (Intel and Apple silicon): `cumTempore-macOS-universal.dmg`
- Windows (64-bit): `cumTempore-windows64.zip`
- iOS simulator: `cumTempore-iphonesimulator.zip`, for developers

These builds are not signed and may show warnings when you install them.

### Add to your website

Add the following code to your website in order to embed a fully functional
copy of **cumTempore**.

```html
<iframe
   src="https://akaflieg-freiburg.github.io/cumTempore/app/cumtempore.html"
   width="320"
   height="480"
   title="cumTempore">
   <p>Your browser cannot show embedded frames. Open the app via
   <a href="https://akaflieg-freiburg.github.io/cumTempore/app/cumtempore.html">this link</a>.</p>
</iframe>
```

### Source code

Developers who are interested in compiling **cumTempore** themselves find all
relevant sources on [GitHub](https://github.com/Akaflieg-Freiburg/cumTempore).
This program builds on the Qt library. The source code for Qt can be found at
[this URL](https://cplx.vm.uni-freiburg.de/storage/QtSources).
