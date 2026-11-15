# Astree

Astree is a free, open source, optical design software.

Its computes, optimize and shows the light analysis of optical devices,.

Optical Surfaces can be conical or polynomial,having reflections, transmissions, stops, void, etc.

It also have an optimizer to enhance your design.

The source code is covered by the GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later.

It use Qt for the gui.

Feel free to contribute!

## Building from source

The project is built with CMake 3.20 or newer and Qt 6.2 or newer (Widgets).
Configure and build from the repository root (the GUI is enabled by default):

```sh
cmake -S . -B build
cmake --build build --config Release
```

On Linux, `cmake --install build` installs the application and its desktop
metadata. CMake must be able to locate the Qt 6 installation. To build just the
engine and tests without Qt, configure with `-DASTREE_BUILD_GUI=OFF`.

Windows binaries can be downloaded at: 
http://edeforas.free.fr
or
in the bin/ folder

