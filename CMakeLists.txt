
cmake_minimum_required (VERSION 3.2)

include (${CMAKE_ROOT}/Modules/ExternalProject.cmake)

include_directories (${CMAKE_BINARY_DIR}/include)
link_directories (${CMAKE_BINARY_DIR}/lib)

option (STATIC_BUILD "Build Planet Blupi statically" ON)
option (ANDROID "Enable when building for Android" OFF)

# It's an hack in order to be able to link statically planetblupi on darwin.
if (APPLE)
  set (ISAPPLE 1)
  set (CMD_LDFLAGS "-L${CMAKE_BINARY_DIR}/lib -framework AudioToolBox -framework AudioUnit -framework CoreAudio -framework CoreFoundation -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo -framework Carbon")

  if (OLD_SDK)
    set (OSX_SDK_VERSION 10.9)
    set (CMAKE_OSX_SYSROOT "/Developer/SDKs/MacOSX${OSX_SDK_VERSION}.sdk/")
    set (CMAKE_OSX_DEPLOYMENT_TARGET ${OSX_SDK_VERSION})
    set (CMD_CXXFLAGS "-isysroot ${CMAKE_OSX_SYSROOT} -mmacosx-version-min=${OSX_SDK_VERSION} ${CMAKE_CXX_FLAGS}")
    set (CMD_CFLAGS "-isysroot ${CMAKE_OSX_SYSROOT} -mmacosx-version-min=${OSX_SDK_VERSION} ${CMAKE_C_FLAGS}")
  endif ()
  set (CMD_CPPFLAGS "-I${CMAKE_BINARY_DIR}/include")
endif ()

if (ANDROID)
  # It's really hard to get non-CMake projects build with Android NDK directly, so we'll start by
  # generating a standalone toolchain (at configure time)
  execute_process(COMMAND ${ANDROID_NDK}/build/tools/make-standalone-toolchain.sh
    --platform=${ANDROID_NATIVE_API_LEVEL}
    --toolchain=${ANDROID_TOOLCHAIN_NAME}
    --install-dir=${CMAKE_BINARY_DIR}/toolchain
    --arch=${ANDROID_SYSROOT_ABI}
    --stl=libc++ ) # TODO: Should probably use ANDROID_STL instead of hardcoding, but the names are different (libc++ vs c++_static)

  # Prepare all cross-compilation options that need to be passed to child CMake projects
  set (ADDITIONAL_CMAKE_OPTIONS
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DANDROID_TOOLCHAIN=${ANDROID_TOOLCHAIN}
    -DANDROID_ABI=${ANDROID_ABI}
    -DANDROID_STL=${ANDROID_STL})

  # Prepare all cross-compilation options needed for configure-based projects
  set (ADDITIONAL_CONFIGURE_OPTIONS
    --host=${ANDROID_HEADER_TRIPLE}
  )
  set (ADDITIONAL_TOOL_OPTIONS
    CC=clang
    CXX=clang++
  )
  set (CMD_PATH "${CMAKE_BINARY_DIR}/toolchain/bin")
  set (CMD_CHOST "${ANDROID_HEADER_TRIPLE}")

  set (CMD_LDFLAGS "-L${CMAKE_BINARY_DIR}/lib")
  set (CMD_CPPFLAGS "-I${CMAKE_BINARY_DIR}/include")
endif()

configure_file (cmd.sh.in cmd.sh @ONLY)
set (CMD ${CMAKE_BINARY_DIR}/cmd.sh)

macro (DLEXT in out)
  if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/externals/${out}")
    file (DOWNLOAD "${in}" "${CMAKE_CURRENT_SOURCE_DIR}/externals/${out}" SHOW_PROGRESS)
  endif ()
endmacro (DLEXT)

DLEXT (http://www.zlib.net/zlib-1.2.11.tar.gz zlib.tar.gz)
DLEXT (https://github.com/vietjtnguyen/argagg/archive/0.4.6.tar.gz argagg.tar.gz)
DLEXT (https://curl.haxx.se/download/curl-7.55.0.tar.xz curl.tar.xz)
DLEXT (http://mirror.switch.ch/ftp/mirror/gnu/gettext/gettext-0.19.8.1.tar.xz gettext.tar.xz)
DLEXT (ftp://ftp-osl.osuosl.org/pub/libpng/src/libpng16/libpng-1.6.34.tar.xz libpng.tar.xz)
DLEXT (http://www.ffmpeg.org/releases/ffmpeg-3.4.1.tar.xz ffmpeg.tar.xz)
DLEXT (http://downloads.xiph.org/releases/ogg/libogg-1.3.3.tar.xz libogg.tar.xz)
DLEXT (http://downloads.xiph.org/releases/vorbis/libvorbis-1.3.5.tar.xz libvorbis.tar.xz)
DLEXT (https://www.libsdl.org/release/SDL2-2.0.8.tar.gz SDL2.tar.bz2)
DLEXT (https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.3.tar.gz SDL2_image.tar.gz)
DLEXT (https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.2.tar.gz SDL2_mixer.tar.gz)

if (NOT APPLE)
  DLEXT (https://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.15.tar.gz libiconv.tar.gz)
endif ()

if (UNIX AND NOT APPLE)
  DLEXT (ftp://ftp.alsa-project.org/pub/lib/alsa-lib-1.1.4.1.tar.bz2 alsa-lib.tar.bz2)
  DLEXT (http://www.mega-nerd.com/libsndfile/files/libsndfile-1.0.28.tar.gz libsndfile.tar.gz)
  DLEXT (https://freedesktop.org/software/pulseaudio/releases/pulseaudio-11.1.tar.xz pulseaudio.tar.xz)
  DLEXT (http://www.linuxfromscratch.org/patches/blfs/svn/pulseaudio-11.1-glibc_2.27_fix-1.patch pulseaudio.patch)
endif ()

#########
## zlib
#########

if (WIN32)
  ExternalProject_Add (zlib_Project
    URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/zlib.tar.gz
    PREFIX ${CMAKE_BINARY_DIR}
    CONFIGURE_COMMAND
    BUILD_COMMAND ${CMD} make -fwin32/Makefile.gcc
    INSTALL_COMMAND ${CMD} make install -fwin32/Makefile.gcc
    BUILD_IN_SOURCE 1
    INSTALL_DIR ${CMAKE_BINARY_DIR}
  )
else ()
  ExternalProject_Add (zlib_Project
    URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/zlib.tar.gz
    PREFIX ${CMAKE_BINARY_DIR}
    CONFIGURE_COMMAND ${ADDITIONAL_TOOL_OPTIONS} ${CMD} ./configure
      --prefix=${CMAKE_BINARY_DIR}
      --static
    BUILD_COMMAND ${CMD} make
    INSTALL_COMMAND ${CMD} make install
    BUILD_IN_SOURCE 1
    INSTALL_DIR ${CMAKE_BINARY_DIR}
  )
endif ()

#########
## argagg
#########

ExternalProject_Add (argagg_Project
  URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/argagg.tar.gz
  PREFIX ${CMAKE_BINARY_DIR}
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR} ${ADDITIONAL_CMAKE_OPTIONS}
)

##########
## libcurl
##########

ExternalProject_Add (libcurl_Project
  URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/curl.tar.xz
  PREFIX ${CMAKE_BINARY_DIR}
  CONFIGURE_COMMAND ${ADDITIONAL_TOOL_OPTIONS} ${CMD} ./configure
    --prefix=${CMAKE_BINARY_DIR}
    --disable-shared
    --enable-static
    --enable-http
    --disable-ftp
    --disable-file
    --disable-ldap
    --disable-ldaps
    --disable-rtsp
    --disable-proxy
    --disable-dict
    --disable-telnet
    --disable-tftp
    --disable-pop3
    --disable-imap
    --disable-smb
    --disable-smtp
    --disable-gopher
    --disable-manual
    --disable-libcurl-option
    --enable-ipv6
    --disable-sspi
    --disable-ntlm-wb
    --disable-unix-socket
    --without-winssl
    --without-darwinssl
    --without-ssl
    --without-gnutls
    --without-polarssl
    --without-mbedtls
    --without-cyassl
    --without-axtls
    --without-libpsl
    --without-libmetalink
    --without-libssh2
    --without-librtmp
    --without-nghttp2
    --without-libidn2
    ${ADDITIONAL_CONFIGURE_OPTIONS}
  BUILD_COMMAND ${CMD} make
  INSTALL_COMMAND ${CMD} make install
  BUILD_IN_SOURCE 1
  INSTALL_DIR ${CMAKE_BINARY_DIR}
  DEPENDS zlib_Project
)

###########
## libiconv
###########

if (NOT APPLE)
  ExternalProject_Add (libiconv_Project
    URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/libiconv.tar.gz
    PREFIX ${CMAKE_BINARY_DIR}
    CONFIGURE_COMMAND ${ADDITIONAL_TOOL_OPTIONS} ${CMD} ./configure
      --prefix=${CMAKE_BINARY_DIR}
      --disable-shared
      --enable-static
      ${ADDITIONAL_CONFIGURE_OPTIONS}
    BUILD_COMMAND ${CMD} make
    INSTALL_COMMAND ${CMD} make install
    BUILD_IN_SOURCE 1
    INSTALL_DIR ${CMAKE_BINARY_DIR}
  )
endif ()

##########
## gettext
##########

set (gettext_DEPENDS)
if (NOT APPLE)
  set (gettext_DEPENDS ${gettext_DEPENDS} libiconv_Project)
endif ()

ExternalProject_Add (gettext_Project
  URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/gettext.tar.xz
  PREFIX ${CMAKE_BINARY_DIR}
  CONFIGURE_COMMAND cd gettext-runtime && ${ADDITIONAL_TOOL_OPTIONS} ${CMD} ./configure
    --prefix=${CMAKE_BINARY_DIR}
    --disable-shared
    --enable-static
    ${ADDITIONAL_CONFIGURE_OPTIONS}
  BUILD_COMMAND ${CMD} make -C gettext-runtime
  INSTALL_COMMAND ${CMD} make -C gettext-runtime install
  BUILD_IN_SOURCE 1
  INSTALL_DIR ${CMAKE_BINARY_DIR}
  DEPENDS ${gettext_DEPENDS}
)

#########
## libpng
#########

ExternalProject_Add (libpng_Project
  URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/libpng.tar.xz
  PREFIX ${CMAKE_BINARY_DIR}
  CONFIGURE_COMMAND ${ADDITIONAL_TOOL_OPTIONS} ${CMD} ./configure
    --prefix=${CMAKE_BINARY_DIR}
    --disable-shared
    --enable-static
    ${ADDITIONAL_CONFIGURE_OPTIONS}
  BUILD_COMMAND ${CMD} make
  INSTALL_COMMAND ${CMD} make install
  BUILD_IN_SOURCE 1
  INSTALL_DIR ${CMAKE_BINARY_DIR}
  DEPENDS zlib_Project
)

#########
## FFmpeg
#########

if (ANDROID)
  set (ADDITIONAL_FFMPEG_CONFIGURE_OPTIONS
    --enable-cross-compile
    --target-os=android
    --arch=${ANDROID_SYSROOT_ABI}
    --cross-prefix=${CMAKE_BINARY_DIR}/toolchain/bin/${ANDROID_HEADER_TRIPLE}-)
    # TODO: We should really be using clang (gcc is deprecated in android NDK) but it doesn't work :(
    # Try building with:
    #--toolchain=clang-usan
    #--as=${CMAKE_BINARY_DIR}/toolchain/bin/${ANDROID_HEADER_TRIPLE}-gcc # Workaround for missing as in clang, looks quite stupid but it works
endif ()

ExternalProject_Add (FFmpeg_Project
  URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/ffmpeg.tar.xz
  PREFIX ${CMAKE_BINARY_DIR}
  CONFIGURE_COMMAND ${CMD} ./configure
    --prefix=${CMAKE_BINARY_DIR}
    --disable-shared
    --enable-static
    --disable-iconv
    --disable-programs
    --disable-doc
    --disable-x86asm
    --disable-avdevice
    --disable-avfilter
    --disable-network
    --disable-everything
    --enable-decoder=cinepak,msvideo1,vorbis
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
    --disable-jack
    --disable-sndio
    ${ADDITIONAL_FFMPEG_CONFIGURE_OPTIONS}
  BUILD_COMMAND ${CMD} make
  INSTALL_COMMAND ${CMD} make install
  BUILD_IN_SOURCE 1
  INSTALL_DIR ${CMAKE_BINARY_DIR}
)

############
## libasound
############

if (UNIX AND NOT APPLE AND NOT ANDROID)
  ExternalProject_Add (libasound_Project
    URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/alsa-lib.tar.bz2
    PREFIX ${CMAKE_BINARY_DIR}
    CONFIGURE_COMMAND ${ADDITIONAL_TOOL_OPTIONS} ${CMD} ./configure
      --prefix=${CMAKE_BINARY_DIR}
      --disable-shared
      --enable-static
      --disable-old-symbols
      --disable-python
      ${ADDITIONAL_CONFIGURE_OPTIONS}
    BUILD_COMMAND ${CMD} make
    INSTALL_COMMAND ${CMD} make install
    BUILD_IN_SOURCE 1
    INSTALL_DIR ${CMAKE_BINARY_DIR}
  )
endif ()

#########
## libogg
#########

ExternalProject_Add (libogg_Project
  URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/libogg.tar.xz
  PREFIX ${CMAKE_BINARY_DIR}
  CONFIGURE_COMMAND ${CMD} ./configure
    --prefix=${CMAKE_BINARY_DIR}
    --disable-shared
    --enable-static
  BUILD_COMMAND ${CMD} make
  INSTALL_COMMAND ${CMD} make install INSTALLDIR=${CMAKE_BINARY_DIR}
  BUILD_IN_SOURCE 1
  INSTALL_DIR ${CMAKE_BINARY_DIR}
)

############
## libvorbis
############

ExternalProject_Add (libvorbis_Project
  URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/libvorbis.tar.xz
  PREFIX ${CMAKE_BINARY_DIR}
  CONFIGURE_COMMAND ${CMD} ./configure
    --prefix=${CMAKE_BINARY_DIR}
    --disable-shared
    --enable-static
  BUILD_COMMAND ${CMD} make
  INSTALL_COMMAND ${CMD} make install INSTALLDIR=${CMAKE_BINARY_DIR}
  BUILD_IN_SOURCE 1
  INSTALL_DIR ${CMAKE_BINARY_DIR}
  DEPENDS libogg_Project
)

#############
## libsndfile
#############

if (UNIX AND NOT APPLE AND NOT ANDROID)
  ExternalProject_Add (libsndfile_Project
    URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/libsndfile.tar.gz
    PREFIX ${CMAKE_BINARY_DIR}
    CONFIGURE_COMMAND ${CMD} ./configure
      --prefix=${CMAKE_BINARY_DIR}
      --disable-shared
      --enable-static
      --enable-alsa
      --disable-sqlite
      --disable-octave
      --disable-full-suite
    BUILD_COMMAND ${CMD} make
    INSTALL_COMMAND ${CMD} make install INSTALLDIR=${CMAKE_BINARY_DIR}
    BUILD_IN_SOURCE 1
    INSTALL_DIR ${CMAKE_BINARY_DIR}
    DEPENDS libasound_Project libogg_Project libvorbis_Project
  )
endif ()

###########
## libpulse
###########

if (UNIX AND NOT APPLE AND NOT ANDROID)
  ExternalProject_Add (libpulse_Project
    URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/pulseaudio.tar.xz
    PREFIX ${CMAKE_BINARY_DIR}
    CONFIGURE_COMMAND patch -p1 < ${CMAKE_CURRENT_SOURCE_DIR}/externals/pulseaudio.patch && ${ADDITIONAL_TOOL_OPTIONS} ${CMD} ./configure
      --prefix=${CMAKE_BINARY_DIR}
      --disable-shared
      --enable-static
      --enable-alsa
      --disable-nls
      --disable-x11
      --disable-tests
      --disable-esound
      --disable-waveout
      --disable-gconf
      --disable-glib2
      --disable-gtk3
      --disable-jack
      --disable-asyncns
      --disable-avahi
      --disable-openssl
      --disable-tcpwrap
      --disable-lirc
      --disable-dbus
      --disable-udev
      --disable-bluez4
      --disable-bluez5
      --disable-hal-compat
      --disable-ipv6
      --disable-webrtc-aec
      --disable-systemd-daemon
      --disable-systemd-login
      --disable-systemd-journal
      --disable-manpages
      --disable-default-build-tests
      --disable-legacy-database-entry-format
      --enable-static-bins
      --without-caps
      --without-fftw
      --without-speex
      --without-soxr
      --with-database=simple
      ${ADDITIONAL_CONFIGURE_OPTIONS}
    BUILD_COMMAND ${CMD} make
    INSTALL_COMMAND ${CMD} make install INSTALLDIR=${CMAKE_BINARY_DIR}
    BUILD_IN_SOURCE 1
    INSTALL_DIR ${CMAKE_BINARY_DIR}
    DEPENDS libasound_Project libsndfile_Project
  )
endif ()

#######
## SDL2
#######

if (UNIX AND NOT APPLE AND NOT ANDROID)
  set (SDL2_CONFIGURE --enable-video-x11
                      --enable-x11-shared
                      --enable-video-wayland
                      --enable-wayland-shared
                      --enable-video-opengl
                      --enable-oss
                      --enable-alsa
                      --disable-alsa-shared
                      --disable-sndio
                      --disable-sndio-shared
                      --enable-pulseaudio
                      --disable-pulseaudio-shared
                      --disable-jack
                      --disable-jack-shared)
elseif (MINGW)
  set (SDL2_CONFIGURE --enable-directx
                      --enable-render-d3d
                      --enable-video-opengl)
elseif (APPLE)
  set (SDL2_CONFIGURE --enable-video-opengl
                      --enable-video-cocoa)
elseif (ANDROID)
  set (SDL2_CONFIGURE --disable-pulseaudio
                      --disable-jack
                      --disable-jack-shared)
endif ()

set (SDL2_DEPENDS zlib_Project)
if (UNIX AND NOT APPLE AND NOT ANDROID)
  set (SDL2_DEPENDS ${SDL2_DEPENDS} libasound_Project libpulse_Project)
endif ()

ExternalProject_Add (SDL2_Project
  URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/SDL2.tar.bz2
  PREFIX ${CMAKE_BINARY_DIR}
  CONFIGURE_COMMAND ${ADDITIONAL_TOOL_OPTIONS} ${CMD} ./configure
    --prefix=${CMAKE_BINARY_DIR}
    --disable-shared
    --enable-static
    #--disable-joystick
    --enable-joystick # TODO: Default SDL2 android skeleton requires joystick to build, could be probably fixed by commenting out some things
    ${SDL2_CONFIGURE}
    ${ADDITIONAL_CONFIGURE_OPTIONS}
  BUILD_COMMAND ${CMD} make
  INSTALL_COMMAND ${CMD} make install
  BUILD_IN_SOURCE 1
  INSTALL_DIR ${CMAKE_BINARY_DIR}
  DEPENDS ${SDL2_DEPENDS}
)

#############
## SDL2_image
#############

ExternalProject_Add (SDL2_image_Project
  URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/SDL2_image.tar.gz
  PREFIX ${CMAKE_BINARY_DIR}
  # SDL_MAIN_HANDLED fixes test program linking on Android
  # TODO: Is there a better way to handle this?
  CONFIGURE_COMMAND CFLAGS=-DSDL_MAIN_HANDLED ${ADDITIONAL_TOOL_OPTIONS} ${CMD} ./configure
    --prefix=${CMAKE_BINARY_DIR}
    --disable-shared
    --enable-static
    --disable-sdltest
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
    ${ADDITIONAL_CONFIGURE_OPTIONS}
  BUILD_COMMAND ${CMD} make
  INSTALL_COMMAND ${CMD} make install
  BUILD_IN_SOURCE 1
  INSTALL_DIR ${CMAKE_BINARY_DIR}
  DEPENDS SDL2_Project libpng_Project
)

#############
## SDL2_mixer
#############

set (SDL2_mixer_DEPENDS SDL2_Project libogg_Project libvorbis_Project)
if (NOT APPLE)
  set (SDL2_mixer_DEPENDS ${SDL2_mixer_DEPENDS} libiconv_Project)
endif ()

ExternalProject_Add (SDL2_mixer_Project
  URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/SDL2_mixer.tar.gz
  PREFIX ${CMAKE_BINARY_DIR}
  # SDL_MAIN_HANDLED fixes test program linking on Android
  # TODO: Is there a better way to handle this?
  CONFIGURE_COMMAND CFLAGS=-DSDL_MAIN_HANDLED ${ADDITIONAL_TOOL_OPTIONS} ${CMD} ./configure
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
    --enable-music-ogg
    --disable-music-ogg-tremor
    --disable-music-ogg-shared
    --disable-music-flac
    --disable-music-flac-shared
    --disable-music-mp3
    --disable-music-mp3-smpeg
    --disable-music-mp3-smpeg-shared
    --disable-smpegtest
    --disable-music-mp3-mad-gpl
    ${ADDITIONAL_CONFIGURE_OPTIONS}
  BUILD_COMMAND ${CMD} make
  INSTALL_COMMAND ${CMD} make install
  BUILD_IN_SOURCE 1
  INSTALL_DIR ${CMAKE_BINARY_DIR}
  DEPENDS ${SDL2_mixer_DEPENDS}
)

##################
## SDL_kitchensink
##################

ExternalProject_Add (SDL_kitchensink_Project
  URL ${CMAKE_SOURCE_DIR}/SDL_kitchensink
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/SDL_kitchensink
  PREFIX ${CMAKE_BINARY_DIR}
  CMAKE_COMMAND ${CMD} ${CMAKE_COMMAND}
  CMAKE_ARGS -DDISABLE_SHARED=true
             -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
             ${ADDITIONAL_CMAKE_OPTIONS}
  INSTALL_DIR ${CMAKE_BINARY_DIR}
  BUILD_ALWAYS 1
  DEPENDS SDL2_Project FFmpeg_Project
)

##############
## planetblupi
##############

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  set (APPIMAGE_APPRUN_BIN     "AppRun-x86_64")
  set (APPIMAGE_APPRUN_URL     "https://github.com/probonopd/AppImageKit/releases/download/continuous/${APPIMAGE_APPRUN_BIN}")
  set (APPIMAGE_APPRUN_PROGRAM "${CMAKE_BINARY_DIR}/bin/${APPIMAGE_APPRUN_BIN}")

  set (APPIMAGE_TOOL_BIN     "appimagetool-x86_64.AppImage")
  set (APPIMAGE_TOOL_URL     "https://github.com/probonopd/AppImageKit/releases/download/continuous/${APPIMAGE_TOOL_BIN}")
  set (APPIMAGE_TOOL_PROGRAM "${CMAKE_BINARY_DIR}/bin/${APPIMAGE_TOOL_BIN}")

  if (NOT EXISTS "${APPIMAGE_APPRUN_PROGRAM}")
    file (DOWNLOAD "${APPIMAGE_APPRUN_URL}" "${APPIMAGE_APPRUN_PROGRAM}")
  endif ()

  if (NOT EXISTS "${APPIMAGE_TOOL_PROGRAM}")
    file (DOWNLOAD "${APPIMAGE_TOOL_URL}" "${APPIMAGE_TOOL_PROGRAM}")
  endif ()

  execute_process (COMMAND /bin/chmod 0755 "${APPIMAGE_APPRUN_PROGRAM}")
  execute_process (COMMAND /bin/chmod 0755 "${APPIMAGE_TOOL_PROGRAM}")
endif ()

set (planetblupi_DEPS
  argagg_Project
  libcurl_Project
  SDL2_Project
  SDL2_image_Project
  SDL2_mixer_Project
  SDL_kitchensink_Project
  libpng_Project
  gettext_Project
  libvorbis_Project
)
if (UNIX AND NOT APPLE AND NOT ANDROID)
  list (APPEND planetblupi_DEPS
    libasound_Project
    libpulse_Project
  )
endif ()

ExternalProject_Add (planetblupi_Project
  URL ${CMAKE_SOURCE_DIR}/planetblupi
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/planetblupi
  PREFIX ${CMAKE_BINARY_DIR}
  CMAKE_COMMAND ${CMD} ${CMAKE_COMMAND}
  CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
             -DAPPIMAGE_APPRUN_PROGRAM=${APPIMAGE_APPRUN_PROGRAM}
             -DAPPIMAGE_ASSISTANT_PROGRAM=${APPIMAGE_TOOL_PROGRAM}
             -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
             -DPB_HTTP_VERSION_CHECK=yes
             -DSTATIC_BUILD=${STATIC_BUILD}
             -DSIGN_APP=${SIGN_APP}
             -DANDROID=${ANDROID}
             ${ADDITIONAL_CMAKE_OPTIONS}
  INSTALL_DIR ${CMAKE_BINARY_DIR}
  BUILD_ALWAYS 1
  DEPENDS ${planetblupi_DEPS}
)

if (ANDROID)
    #find_package (PkgConfig REQUIRED)
    #pkg_search_module (SDL2 REQUIRED sdl2)

    add_library(main SHARED android/SDL_android_main.c)
    add_dependencies (main planetblupi_Project)
    target_link_libraries (main PUBLIC planetblupi)

    #add_dependencies (main SDL2_Project)
    target_link_libraries (main PUBLIC -lintl -liconv -lSDL2 -lSDL2_image -lSDL2_mixer -lcurl -lSDL_kitchensink_static -lpng -lavformat -lavcodec -lavutil -lswscale -lswresample -lz -lGLESv1_CM -lGLESv2 -llog -landroid ${CMAKE_CXX_STANDARD_LIBRARIES_INIT})
endif ()

add_custom_target (dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

set (CPACK_SOURCE_GENERATOR "TXZ")
set (CPACK_SOURCE_PACKAGE_FILE_NAME "planetblupi-dev")
set (CPACK_SOURCE_IGNORE_FILES "/build/;/Debug/;/Release/;/planetblupi-data/;/planetblupi-www/;/.git/;~$;${CPACK_SOURCE_IGNORE_FILES}")
include (CPack)
