
cmake_minimum_required (VERSION 3.2)

include (${CMAKE_ROOT}/Modules/ExternalProject.cmake)

include_directories (${CMAKE_BINARY_DIR}/include)
link_directories (${CMAKE_BINARY_DIR}/lib)

configure_file (cmd.sh.in cmd.sh @ONLY)
set (CMD ${CMAKE_BINARY_DIR}/cmd.sh)

#########
## zlib
#########

if (WIN32)
  ExternalProject_Add (zlib_Project
    URL http://www.zlib.net/zlib-1.2.11.tar.gz
    PREFIX ${CMAKE_BINARY_DIR}
    CONFIGURE_COMMAND
    BUILD_COMMAND ${CMD} make -fwin32/Makefile.gcc
    INSTALL_COMMAND ${CMD} make install -fwin32/Makefile.gcc
    BUILD_IN_SOURCE 1
    INSTALL_DIR ${CMAKE_BINARY_DIR}
  )
else ()
  ExternalProject_Add (zlib_Project
    URL http://www.zlib.net/zlib-1.2.11.tar.gz
    PREFIX ${CMAKE_BINARY_DIR}
    CONFIGURE_COMMAND ${CMD} ./configure
      --prefix=${CMAKE_BINARY_DIR}
      --static
    BUILD_COMMAND ${CMD} make
    INSTALL_COMMAND ${CMD} make install
    BUILD_IN_SOURCE 1
    INSTALL_DIR ${CMAKE_BINARY_DIR}
  )
endif ()

#########
## libpng
#########

ExternalProject_Add (libpng_Project
  URL http://prdownloads.sourceforge.net/libpng/libpng-1.6.28.tar.xz?download
  PREFIX ${CMAKE_BINARY_DIR}
  CONFIGURE_COMMAND ${CMD} ./configure
    --prefix=${CMAKE_BINARY_DIR}
    --disable-shared
    --enable-static
  BUILD_COMMAND ${CMD} make
  INSTALL_COMMAND ${CMD} make install
  BUILD_IN_SOURCE 1
  INSTALL_DIR ${CMAKE_BINARY_DIR}
  DEPENDS zlib_Project
)

#########
## FFmpeg
#########

ExternalProject_Add (FFmpeg_Project
  URL http://www.ffmpeg.org/releases/ffmpeg-3.2.4.tar.xz
  PREFIX ${CMAKE_BINARY_DIR}
  CONFIGURE_COMMAND ${CMD} ./configure
    --prefix=${CMAKE_BINARY_DIR}
    --disable-shared
    --enable-static
    --disable-iconv
    --disable-programs
    --disable-doc
    --disable-yasm
    --disable-avdevice
    --disable-avfilter
    --disable-network
    --disable-everything
    --enable-decoder=cinepak,vorbis
    --enable-demuxer=matroska
    --enable-protocol=file
    --disable-crystalhd
    --disable-xvmc
    --disable-vaapi
    --disable-videotoolbox
    --disable-vdpau
    --disable-vda
    --disable-nvenc
    --disable-dxva2
    --disable-d3d11va
    --disable-audiotoolbox
    --disable-zlib
    --disable-bzlib
    --disable-lzma
    --disable-xlib
    --disable-sdl2
    --disable-libxcb
    --disable-libxcb-shm
    --disable-libxcb-xfixes
    --disable-libxcb-shape
  BUILD_COMMAND ${CMD} make
  INSTALL_COMMAND ${CMD} make install
  BUILD_IN_SOURCE 1
  INSTALL_DIR ${CMAKE_BINARY_DIR}
)

#########
## libass
#########

# ExternalProject_Add (libass_Project
#   URL https://github.com/libass/libass/releases/download/0.13.6/libass-0.13.6.tar.xz
#   PREFIX ${CMAKE_BINARY_DIR}
#   CONFIGURE_COMMAND ./configure
#     --prefix=${CMAKE_BINARY_DIR}
#     --disable-shared
#     --enable-static
#     --disable-fontconfig
#     --disable-directwrite
#     --disable-coretext
#     --disable-harfbuzz
#     --disable-asm
#     --disable-rasterizer
#     --disable-require-system-font-provider
#   BUILD_COMMAND make
#   INSTALL_COMMAND make install
#   BUILD_IN_SOURCE 1
#   INSTALL_DIR ${CMAKE_BINARY_DIR}
# )

#######
## SDL2
#######

ExternalProject_Add (SDL2_Project
  URL https://www.libsdl.org/release/SDL2-2.0.5.tar.gz
  PREFIX ${CMAKE_BINARY_DIR}
  CONFIGURE_COMMAND ${CMD} ./configure
    --prefix=${CMAKE_BINARY_DIR}
    --disable-shared
    --enable-static
    --disable-joystick
    --disable-haptic
  BUILD_COMMAND ${CMD} make
  INSTALL_COMMAND ${CMD} make install
  BUILD_IN_SOURCE 1
  INSTALL_DIR ${CMAKE_BINARY_DIR}
  DEPENDS zlib_Project
)

#############
## SDL2_image
#############

ExternalProject_Add (SDL2_image_Project
  URL https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.1.tar.gz
  PREFIX ${CMAKE_BINARY_DIR}
  CONFIGURE_COMMAND ${CMD} ./configure
    --prefix=${CMAKE_BINARY_DIR}
    --disable-shared
    --enable-static
    --disable-sdl-test
    --disable-bmp
    --disable-gif
    --disable-jpg
    --disable-jpg-shared
    --disable-lbm
    --disable-pcx
    --enable-png
    --disable-png-shared
    --disable-pnm
    --disable-tga
    --disable-tif
    --disable-tif-shared
    --disable-xcf
    --disable-xpm
    --disable-xv
    --disable-webp
    --disable-webp-shared
  BUILD_COMMAND ${CMD} make
  INSTALL_COMMAND ${CMD} make install
  BUILD_IN_SOURCE 1
  INSTALL_DIR ${CMAKE_BINARY_DIR}
  DEPENDS SDL2_Project libpng_Project
)

#############
## SDL2_mixer
#############

ExternalProject_Add (SDL2_mixer_Project
  URL https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.1.tar.gz
  PREFIX ${CMAKE_BINARY_DIR}
  CONFIGURE_COMMAND ${CMD} ./configure
    --prefix=${CMAKE_BINARY_DIR}
    --disable-shared
    --enable-static
    --disable-sdltest
    --disable-music-cmd
    --enable-music-wave
    --disable-music-mod
    --disable-music-mod-modplug
    --disable-music-mod-modplug-shared
    --disable-music-mod-mikmod
    --disable-music-mod-mikmod-shared
    --enable-music-midi
    --enable-music-midi-timidity
    --enable-music-midi-native
    --disable-music-midi-fluidsynth
    --disable-music-midi-fluidsynth-shared
    --disable-music-ogg
    --disable-music-ogg-tremor
    --disable-music-ogg-shared
    --disable-music-flac
    --disable-music-flac-shared
    --disable-music-mp3
    --disable-music-mp3-smpeg
    --disable-music-mp3-smpeg-shared
    --disable-smpegtest
    --disable-music-mp3-mad-gpl
  BUILD_COMMAND ${CMD} make
  INSTALL_COMMAND ${CMD} make install
  BUILD_IN_SOURCE 1
  INSTALL_DIR ${CMAKE_BINARY_DIR}
  DEPENDS SDL2_Project
)

##################
## SDL_kitchensink
##################

ExternalProject_Add (SDL_kitchensink_Project
  GIT_REPOSITORY https://github.com/Skywalker13/SDL_kitchensink.git
  PREFIX ${CMAKE_BINARY_DIR}
  CMAKE_COMMAND ${CMD} cmake
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
  INSTALL_DIR ${CMAKE_BINARY_DIR}
  DEPENDS SDL2_Project FFmpeg_Project
)

##############
## planetblupi
##############

ExternalProject_Add (planetblupi_Project
  URL ${CMAKE_SOURCE_DIR}/planetblupi
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/planetblupi
  PREFIX ${CMAKE_BINARY_DIR}
  CMAKE_COMMAND ${CMD} cmake
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
  INSTALL_DIR ${CMAKE_BINARY_DIR}
  DEPENDS SDL2_Project SDL2_image_Project SDL2_mixer_Project SDL_kitchensink_Project libpng_Project
)
