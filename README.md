# Astree

Astree is a free, open source, optical design software.

Its computes, optimize and shows the light analysis of optical devices,.

Optical Surfaces can be conical or polynomial,having reflections, transmissions, stops, void, etc.

It also have an optimizer to enhance your design.

The source code is covered by the GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later.

Feel free to contribute!

## Building from source

The project is built with CMake 3.20 or newer. The Dear ImGui desktop
application is enabled by default; CMake downloads GLFW and Dear ImGui during
configuration:

```sh
cmake -S . -B build
cmake --build build --config Release
```

To build just the engine and tests without a desktop application, configure
with `-DASTREE_BUILD_IMGUI=OFF`.

The legacy Qt desktop application remains available in `deprecated_gui/`.
To build it instead, install Qt 6.2 or newer (Widgets) and configure with:

```sh
cmake -S . -B build-qt -DASTREE_BUILD_GUI=ON -DASTREE_BUILD_IMGUI=OFF
cmake --build build-qt --config Release
```

On Linux, `cmake --install build-qt` installs the Qt application and its desktop
metadata.

The Dear ImGui app uses GLFW and OpenGL. It defaults to a dark, blue-accented
theme and a system sans-serif font. Its View menu includes theme selection and
automatic display-aware scaling, with manual UI scale presets from 75% to 200%.
On Windows, its native title bar switches between dark and light with the
selected theme.
On Windows, Open, Import, and Save As use the native file dialogs; the
built-in picker remains available on platforms without this native integration
and if the native dialog cannot be opened. The Windows ImGui app starts without
opening a console window.

Windows binaries can be downloaded at: 
http://edeforas.free.fr
or
in the bin/ folder

