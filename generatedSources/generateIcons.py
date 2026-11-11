#!/usr/bin/env python3
"""Render all platform icons from the SVG sources in metadata/.

Usage: generateIcons.py <source dir> <app id>

Needs rsvg-convert and Pillow. The results are committed to git, so that
regular builds do not depend on these tools; re-run the CMake target
'generatedSources' after changing the SVG files.
"""

import io
import pathlib
import subprocess
import sys

from PIL import Image

source = pathlib.Path(sys.argv[1])
app_id = sys.argv[2]
metadata = source / "metadata"
out = source / "generatedSources"


def render(svg: pathlib.Path, size: int) -> Image.Image:
    png = subprocess.run(["rsvg-convert", "-w", str(size), "-h", str(size), str(svg)],
                         check=True, capture_output=True).stdout
    return Image.open(io.BytesIO(png)).convert("RGBA")


def save(image: Image.Image, path: pathlib.Path, **kwargs) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    image.save(path, **kwargs)
    print("Generated", path.relative_to(source))


main_svg = metadata / f"{app_id}.svg"
angular_svg = metadata / f"{app_id}_angular.svg"
foreground_svg = metadata / f"{app_id}_foreground.svg"

# Window icon, Linux desktop icon
save(render(main_svg, 512), out / "metadata" / f"{app_id}.png")

# Android launcher icons: legacy icon, and the foreground layer of the
# adaptive icon (108 dp). The background layer is a color resource.
for density, factor in {"mdpi": 1, "hdpi": 1.5, "xhdpi": 2, "xxhdpi": 3, "xxxhdpi": 4}.items():
    res = out / "android" / "res" / f"mipmap-{density}"
    save(render(main_svg, round(48 * factor)), res / "ic_launcher.png")
    save(render(foreground_svg, round(108 * factor)), res / "ic_launcher_foreground.png")

# iOS: a single 1024 px icon (the App Store rejects icons with alpha
# channel), and the image on the launch screen. These go directly into the
# asset catalog.
assets = source / "src" / "ios" / "Assets.xcassets"
save(render(angular_svg, 1024).convert("RGB"), assets / "AppIcon.appiconset" / "AppIcon.png")
save(render(main_svg, 512), assets / "Launch.imageset" / "Launch.png")

# macOS: icon on the standard grid (824 px artwork on a 1024 px canvas)
canvas = Image.new("RGBA", (1024, 1024), (0, 0, 0, 0))
artwork = render(main_svg, 824)
canvas.paste(artwork, (100, 100), artwork)
save(canvas, out / "macos" / "cumtempore.icns")

# Windows
save(render(main_svg, 256), out / "windows" / "cumtempore.ico",
     sizes=[(16, 16), (24, 24), (32, 32), (48, 48), (64, 64), (128, 128), (256, 256)])

# Website and WebAssembly loader
save(render(main_svg, 192), out / "web" / "icon-192.png")
save(render(main_svg, 32), out / "web" / "favicon.png")
