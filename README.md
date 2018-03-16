# Planet Blupi development bundle

This bundle is the main repository for building the _Planet Blupi_ game. If
you are an official distribution packager, maybe you can directly use the
`https://github.com/blupi-games/planetblupi.git` repository instead of this
one but **it's not the recommended way** and in the case of a distribution,
it needs some improvements in the `CMakeLists.txt` file (based on [CMake][6]).

The game is built with static linking as much as possible. The goal is to
limit the linking of dynamic libraries which are not available natively on
the host operating system of most users. There is only one exception on Windows
about the use of the dynamic `libwinpthread-1.dll` library. Most dependencies
are built via the `CMakeLists.txt` file provided here (libpng, FFmpeg, SDL2,
and more with the appropriate flags). For example, FFmpeg is built only with
the necessary codecs.

For Linux, the release is packaged in an [AppImage][1]. The user can download
the image and adds the executable flag. Then the game is ready to play (no root
needed, no dependencies to install). In the case of Darwin, it's mostly like
Linux but with a `.app` in a `DMG` image. Just open the `DMG` and the game can
be played immediatly (the user can copy the game in the `/Applications`
directory if he wants). Only Windows is a bit different. Even if the game is
standalone, the installation is provided via an [NSIS][2] installer.

> Note that the `DMG` bundling and the [NSIS][2] building is directly provided
> by [CMake][6]. In the case of [AppImage][1], it's handled by [CMake][6] too
> but via a custom `.cmake` file available in the `planetblupi/cmake` directory.

## Prepare environment

```sh
git clone --recursive https://github.com/blupi-games/planetblupi-dev.git
cd planetblupi-dev
```

The source-code is written in C++, only [GCC][4] and [Clang][5] are officially
supported. The clone must be `recursive` because the development use git
submodules.

* **source-code**: `planetblupi/`
* **source-assets**: `planetblupi-data/`
* **website**: `planetblupi-www/` (see <http://www.blupi.org>)

### Linux

You need the usual development packages for C++ ([GCC][4]) with pkg-config and
[CMake][6].

### Darwin

You need the development commandline tools provided by Apple via Xcode (Clang),
pkg-config and [CMake][6]. It should be possible to use [GCC][4] via
[Homebrew][7] or [MacPorts][8].

### Windows with [MSYS2][3]

In the case of Windows, it's not recommended to use MSVC because you will have
some difficulties to have all dependencies. The best way is using [MSYS2][3]
with it's great package manager (pacman).

```sh
# Update your toolchain
pacman -Syuu

# Install main development packages
pacman -S --noconfirm --needed make pkg-config mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake

# Install NSIS64 for Release packaging
pacman -S mingw-w64-x86_64-nsis
```

## How to build

On Linux and Darwin, create a `Debug` or `Release` directory.

```sh
# For debugging
mkdir Debug
cd Debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j

# For releasing
mkdir Release
cd Release
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j
```

In the case of Windows with [MSYS2][3], it's a bit different because you need to
specify the right generator (`MSYS Makefiles`).

```sh
# For debugging
mkdir Debug
cd Debug
cmake -G"MSYS Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
make -j

# For releasing
mkdir Release
cd Release
cmake -G"MSYS Makefiles" -DCMAKE_BUILD_TYPE=Release ..
make -j
```

## Packaging

If you have built a release, you will find the output in the appropriate
directory according to the platform.

* **Linux**: look at `Release/linux-appimage/planetblupi.AppImage`
* **Darwin**: look at `Release/src/planetblupi_Project/planetblupi-X.Y.Z.dmg`
* **Windows**: look at `Release/src/planetblupi_Project/planetblupi-X.Y.Z.exe`

# Licenses

_Planet Blupi_ and all resource files are licensed to the GPLv3+ license.

| Project               | License        | Description                                                                                 |
| --------------------- | -------------- | ------------------------------------------------------------------------------------------- |
| [argagg][9]           | MIT            | simple C++11 command line arg ument parser                                                  |
| [FFmpeg][10]          | LGPLv2.1       | complete, cross-platform solution to record, convert and stream audio and video             |
| GNU/[gettext][11]     | GPLv3          | an i18n and l10n system commonly used for writing multilingual programs                     |
| [libasound][12]       | LGPLv2.1       | provides audio and MIDI functionality to the Linux operating system                         |
| [libcurl][13]         | MIT/X derivate | a free and easy-to-use client-side URL transfer library                                     |
| GNU/[libiconv][14]    | GPLv3          | a standardized API used to convert between different character encodings                    |
| [libogg][22]          | own license    | a free, open container format maintained by the Xiph.Org Foundation                         |
| [libpng][15]          | own license    | the official PNG reference library                                                          |
| [libpulse][16]        | LGPLv2.1       | API for the PulseAudio network-capable sound server program                                 |
| [SDL_kitchensink][17] | MIT            | Simple SDL2 / FFmpeg library for audio/video playback written in C99                        |
| [libsndfile][23]      | LGPLv3         | a C library for reading and writing files containing sampled sound                          |
| [libvorbis][22]       | own license    | a free and open-source software project headed by the Xiph.Org Foundation                   |
| [SDL2][18]            | zlib license   | a cross-platform software library designed to provide a HAL to computer multimedia hardware |
| [SDL2_image][19]      | zlib license   | an image loading library that is used with the SDL library                                  |
| [SDL2_mixer][20]      | zlib license   | a sound mixing library that is used with the SDL library                                    |
| [zlib][21]            | own license    | a Massively Spiffy Yet Delicately Unobtrusive Compression Library                           |

[1]: http://appimage.org
[2]: http://nsis.sourceforge.net
[3]: http://www.msys2.org
[4]: https://gcc.gnu.org
[5]: https://clang.llvm.org
[6]: https://cmake.org
[7]: https://brew.sh
[8]: https://macports.org
[9]: https://github.com/vietjtnguyen/argagg
[10]: https://www.ffmpeg.org
[11]: https://www.gnu.org/software/gettext/
[12]: https://www.alsa-project.org
[13]: https://curl.haxx.se/libcurl/
[14]: https://www.gnu.org/software/libiconv/
[15]: http://www.libpng.org/pub/png/libpng.html
[16]: https://freedesktop.org/software/pulseaudio/doxygen/
[17]: https://github.com/katajakasa/SDL_kitchensink
[18]: https://www.libsdl.org
[19]: https://www.libsdl.org/projects/SDL_image/
[20]: https://www.libsdl.org/projects/SDL_mixer/
[21]: https://zlib.net/
[22]: https://xiph.org/downloads/
[23]: http://www.mega-nerd.com/libsndfile/
