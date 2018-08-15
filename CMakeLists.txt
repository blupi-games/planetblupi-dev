
cmake_minimum_required (VERSION 3.2)

include (${CMAKE_ROOT}/Modules/ExternalProject.cmake)

include_directories (${CMAKE_BINARY_DIR}/include)
link_directories (${CMAKE_BINARY_DIR}/lib)

option (STATIC_BUILD "Build Planet Blupi statically" ON)
option (EMSCRIPTEN "Build Planet Blupi for emscripten toolchain" OFF)
option (ASMJS "Build for asm.js instead of WebAssembly (slower)" OFF)

if (EMSCRIPTEN)
  set (BUILD_JS ON)
  set (PLATFORM emscripten)
elseif (APPLE)
  set (BUILD_DARWIN ON)
  set (PLATFORM darwin)
elseif (UNIX)
  set (BUILD_LINUX ON)
  set (PLATFORM linux)
elseif (MINGW OR WIN32)
  set (BUILD_WINDOWS ON)
  set (PLATFORM windows)
endif ()

# It's an hack in order to be able to link statically planetblupi on darwin.
if (BUILD_DARWIN)
  set (CMD_LDFLAGS "-L${CMAKE_BINARY_DIR}/lib -framework AudioToolBox -framework AudioUnit -framework CoreAudio -framework CoreFoundation -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo -framework Carbon")

  if (OLD_SDK)
    set (OSX_SDK_VERSION 10.9)
    set (CMAKE_OSX_SYSROOT "/Developer/SDKs/MacOSX${OSX_SDK_VERSION}.sdk/")
    set (CMAKE_OSX_DEPLOYMENT_TARGET ${OSX_SDK_VERSION})
    set (CMD_CXXFLAGS "-isysroot ${CMAKE_OSX_SYSROOT} -mmacosx-version-min=${OSX_SDK_VERSION} ${CMAKE_CXX_FLAGS}")
    set (CMD_CFLAGS "-isysroot ${CMAKE_OSX_SYSROOT} -mmacosx-version-min=${OSX_SDK_VERSION} ${CMAKE_C_FLAGS}")
  endif ()
  set (CMD_CPPFLAGS "-I${CMAKE_BINARY_DIR}/include")
elseif (BUILD_JS)
  set (EMSCRIPTEN_FLAGS
   "-s USE_ZLIB=1 -s USE_LIBPNG=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=['png'] -s USE_OGG=1 -s USE_VORBIS=1 -s TOTAL_MEMORY=256MB"
  )
  if (ASMJS)
    set (EMSCRIPTEN_FLAGS "${EMSCRIPTEN_FLAGS} -s WASM=0")
  else ()
    set (EMSCRIPTEN_FLAGS "${EMSCRIPTEN_FLAGS} -s WASM=1")
  endif ()
  set (CMD_CXXFLAGS ${EMSCRIPTEN_FLAGS})
  set (CMD_CFLAGS ${EMSCRIPTEN_FLAGS})
  set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${EMSCRIPTEN_FLAGS})
  set (CMAKE_C_FLAGS ${CMAKE_C_FLAGS} ${EMSCRIPTEN_FLAGS})
endif ()

configure_file (cmd.sh.in cmd.sh @ONLY)
set (CMD ${CMAKE_BINARY_DIR}/cmd.sh)

###############
## Dependencies
###############

macro (DLEXT in out)
  if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/externals/${out}")
    file (DOWNLOAD "${in}" "${CMAKE_CURRENT_SOURCE_DIR}/externals/${out}" SHOW_PROGRESS)
  endif ()
endmacro (DLEXT)

set (HAS_KITCHENSINK ON)

set (HAS_ARGAGG ON)
set (ARGAGG_VER 0.4.6)
DLEXT (https://github.com/vietjtnguyen/argagg/archive/${ARGAGG_VER}.tar.gz argagg-${ARGAGG_VER}.tar.gz)

set (HAS_FFMPEG ON)
set (FFMPEG_VER 3.4.1)
DLEXT (http://www.ffmpeg.org/releases/ffmpeg-${FFMPEG_VER}.tar.xz ffmpeg-${FFMPEG_VER}.tar.xz)

set (HAS_SDLMIXER ON)
if (BUILD_JS)
  set (SDLMIXER_VER 2.0.1)
else ()
  set (SDLMIXER_VER 2.0.2)
endif ()
DLEXT (https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-${SDLMIXER_VER}.tar.gz SDL2_mixer-${SDLMIXER_VER}.tar.gz)

if (NOT BUILD_JS)
  set (HAS_ZLIB ON)
  set (ZLIB_VER 1.2.11)
  DLEXT (http://www.zlib.net/zlib-${ZLIB_VER}.tar.gz zlib-${ZLIB_VER}.tar.gz)

  set (HAS_CURL ON)
  set (CURL_VER 7.55.0)
  DLEXT (https://curl.haxx.se/download/curl-${CURL_VER}.tar.xz curl-${CURL_VER}.tar.xz)

  set (HAS_GETTEXT ON)
  set (GETTEXT_VER 0.19.8.1)
  DLEXT (http://mirror.switch.ch/ftp/mirror/gnu/gettext/gettext-${GETTEXT_VER}.tar.xz gettext-${GETTEXT_VER}.tar.xz)

  set (HAS_PNG ON)
  set (PNG_VER 1.6.34)
  DLEXT (ftp://ftp-osl.osuosl.org/pub/libpng/src/libpng16/libpng-${PNG_VER}.tar.xz libpng-${PNG_VER}.tar.xz)

  set (HAS_OGG ON)
  set (OGG_VER 1.3.3)
  DLEXT (http://downloads.xiph.org/releases/ogg/libogg-${OGG_VER}.tar.xz libogg-${OGG_VER}.tar.xz)

  set (HAS_VORBIS ON)
  set (VORBIS_VER 1.3.5)
  DLEXT (http://downloads.xiph.org/releases/vorbis/libvorbis-${VORBIS_VER}.tar.xz libvorbis-${VORBIS_VER}.tar.xz)

  set (HAS_SDL ON)
  set (SDL_VER 2.0.8)
  DLEXT (https://www.libsdl.org/release/SDL2-${SDL_VER}.tar.gz SDL2-${SDL_VER}.tar.bz2)

  set (HAS_SDLIMAGE ON)
  set (SDLIMAGE_VER 2.0.3)
  DLEXT (https://www.libsdl.org/projects/SDL_image/release/SDL2_image-${SDLIMAGE_VER}.tar.gz SDL2_image-${SDLIMAGE_VER}.tar.gz)
endif ()

if (BUILD_LINUX AND BUILD_WINDOWS)
  set (HAS_ICONV ON)
  set (ICONV_VER 1.15)
  DLEXT (https://ftp.gnu.org/pub/gnu/libiconv/libiconv-${ICONV_VER}.tar.gz libiconv-${ICONV_VER}.tar.gz)
endif ()

if (BUILD_LINUX)
  set (HAS_ALSA ON)
  set (ALSA_VER 1.1.4.1)
  DLEXT (ftp://ftp.alsa-project.org/pub/lib/alsa-lib-${ALSA_VER}.tar.bz2 alsa-lib-${ALSA_VER}.tar.bz2)

  set (HAS_SNDFILE ON)
  set (SNDFILE_VER 1.0.28)
  DLEXT (http://www.mega-nerd.com/libsndfile/files/libsndfile-${SNDFILE_VER}.tar.gz libsndfile-${SNDFILE_VER}.tar.gz)

  set (HAS_PULSEAUDIO ON)
  set (PULSEAUDIO_VER 12.2)
  DLEXT (https://freedesktop.org/software/pulseaudio/releases/pulseaudio-${PULSEAUDIO_VER}.tar.xz pulseaudio-${PULSEAUDIO_VER}.tar.xz)
endif ()

message (STATUS "Build projects for ${PLATFORM}")
if (HAS_ZLIB)
  message (STATUS " * zlib (${ZLIB_VER})")
endif ()
if (HAS_GETTEXT)
  message (STATUS " * gettext (${GETTEXT_VER})")
endif ()
if (HAS_ARGAGG)
  message (STATUS " * argagg (${ARGAGG_VER})")
endif ()
if (HAS_CURL)
  message (STATUS " * libcurl (${CURL_VER})")
endif ()
if (HAS_ICONV)
  message (STATUS " * libiconv (${ICONV_VER})")
endif ()
if (HAS_PNG)
  message (STATUS " * libpng (${PNG_VER})")
endif ()
if (HAS_FFMPEG)
  message (STATUS " * FFmpeg (${FFMPEG_VER})")
endif ()
if (HAS_SDL)
  message (STATUS " * SDL2 (${SDL_VER})")
endif ()
if (HAS_SDLIMAGE)
  message (STATUS " * SDL2_image (${SDLIMAGE_VER})")
endif ()
if (HAS_SDLMIXER)
  message (STATUS " * SDL2_mixer (${SDLMIXER_VER})")
endif ()
if (HAS_KITCHENSINK)
  message (STATUS " * SDL_kitchensink")
endif ()
if (HAS_ALSA)
  message (STATUS " * libasound (${ALSA_VER})")
endif ()
if (HAS_SNDFILE)
  message (STATUS " * libsndfile (${SNDFILE_VER})")
endif ()
if (HAS_PULSEAUDIO)
  message (STATUS " * libpulse (${PULSEAUDIO_VER})")
endif ()
if (HAS_OGG)
  message (STATUS " * libogg (${OGG_VER})")
endif ()
if (HAS_VORBIS)
  message (STATUS " * libvorbis (${VORBIS_VER})")
endif ()

set (planetblupi_DEPS)

#######
## zlib
#######

if (HAS_ZLIB)
  if (BUILD_WINDOWS)
    ExternalProject_Add (zlib_Project
      URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/zlib-${ZLIB_VER}.tar.gz
      PREFIX ${CMAKE_BINARY_DIR}
      CONFIGURE_COMMAND
      BUILD_COMMAND ${CMD} make -fwin32/Makefile.gcc
      INSTALL_COMMAND ${CMD} make install -fwin32/Makefile.gcc
      BUILD_IN_SOURCE 1
      INSTALL_DIR ${CMAKE_BINARY_DIR}
    )
  else ()
    ExternalProject_Add (zlib_Project
      URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/zlib-${ZLIB_VER}.tar.gz
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
endif ()

#########
## argagg
#########

if (HAS_ARGAGG)
  ExternalProject_Add (argagg_Project
    URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/argagg-${ARGAGG_VER}.tar.gz
    PREFIX ${CMAKE_BINARY_DIR}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}
               -DARGAGG_BUILD_EXAMPLES=off
               -DARGAGG_BUILD_TESTS=off
               -DARGAGG_BUILD_DOCS=off
  )

  set (planetblupi_DEPS ${planetblupi_DEPS} argagg_Project)
endif ()

##########
## libcurl
##########

if (HAS_CURL)
  set (libcurl_DEPENDS)
  if (HAS_ZLIB)
    set (libcurl_DEPENDS ${libcurl_DEPENDS} zlib_Project)
  endif ()

  ExternalProject_Add (libcurl_Project
    URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/curl-${CURL_VER}.tar.xz
    PREFIX ${CMAKE_BINARY_DIR}
    CONFIGURE_COMMAND ${CMD} ./configure
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
    BUILD_COMMAND ${CMD} make
    INSTALL_COMMAND ${CMD} make install
    BUILD_IN_SOURCE 1
    INSTALL_DIR ${CMAKE_BINARY_DIR}
    DEPENDS ${libcurl_DEPENDS}
  )

  set (planetblupi_DEPS ${planetblupi_DEPS} libcurl_Project)
endif ()

###########
## libiconv
###########

if (HAS_ICONV)
  ExternalProject_Add (libiconv_Project
    URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/libiconv-${ICONV_VER}.tar.gz
    PREFIX ${CMAKE_BINARY_DIR}
    CONFIGURE_COMMAND ${CMD} ./configure
      --prefix=${CMAKE_BINARY_DIR}
      --disable-shared
      --enable-static
    BUILD_COMMAND ${CMD} make
    INSTALL_COMMAND ${CMD} make install
    BUILD_IN_SOURCE 1
    INSTALL_DIR ${CMAKE_BINARY_DIR}
  )
endif ()

##########
## gettext
##########

if (HAS_GETTEXT)
  set (gettext_DEPENDS)
  if (HAS_ICONV)
    set (gettext_DEPENDS ${gettext_DEPENDS} libiconv_Project)
  endif ()

  ExternalProject_Add (gettext_Project
    URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/gettext-${GETTEXT_VER}.tar.xz
    PREFIX ${CMAKE_BINARY_DIR}
    CONFIGURE_COMMAND cd gettext-runtime && ${CMD} ./configure
      --prefix=${CMAKE_BINARY_DIR}
      --disable-shared
      --enable-static
    BUILD_COMMAND ${CMD} make -C gettext-runtime
    INSTALL_COMMAND ${CMD} make -C gettext-runtime install
    BUILD_IN_SOURCE 1
    INSTALL_DIR ${CMAKE_BINARY_DIR}
    DEPENDS ${gettext_DEPENDS}
  )

  set (planetblupi_DEPS ${planetblupi_DEPS} gettext_Project)
endif ()

#########
## libpng
#########

if (HAS_PNG)
  ExternalProject_Add (libpng_Project
    URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/libpng-${PNG_VER}.tar.xz
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

  set (planetblupi_DEPS ${planetblupi_DEPS} libpng_Project)
endif ()

#########
## FFmpeg
#########

if (HAS_FFMPEG)
  set (FFMPEG_CONFIGURE)
  if (BUILD_JS)
    set (FFMPEG_CONFIGURE
      --cc=emcc
      --enable-cross-compile
      --target-os=none
      --arch=x86_64
      --disable-asm
      --disable-pthreads
    )
  endif ()

  ExternalProject_Add (FFmpeg_Project
    URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/ffmpeg-${FFMPEG_VER}.tar.xz
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
      ${FFMPEG_CONFIGURE}
    BUILD_COMMAND ${CMD} make
    INSTALL_COMMAND ${CMD} make install
    BUILD_IN_SOURCE 1
    INSTALL_DIR ${CMAKE_BINARY_DIR}
  )
endif ()

############
## libasound
############

if (HAS_ALSA)
  ExternalProject_Add (libasound_Project
    URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/alsa-lib-${ALSA_VER}.tar.bz2
    PREFIX ${CMAKE_BINARY_DIR}
    CONFIGURE_COMMAND ${CMD} ./configure
      --prefix=${CMAKE_BINARY_DIR}
      --disable-shared
      --enable-static
      --disable-old-symbols
      --disable-python
    BUILD_COMMAND ${CMD} make
    INSTALL_COMMAND ${CMD} make install
    BUILD_IN_SOURCE 1
    INSTALL_DIR ${CMAKE_BINARY_DIR}
  )

  set (planetblupi_DEPS ${planetblupi_DEPS} libasound_Project)
endif ()

#########
## libogg
#########

if (HAS_OGG)
  ExternalProject_Add (libogg_Project
    URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/libogg-${OGG_VER}.tar.xz
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
endif ()

############
## libvorbis
############

if (HAS_VORBIS)
  ExternalProject_Add (libvorbis_Project
    URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/libvorbis-${VORBIS_VER}.tar.xz
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

  set (planetblupi_DEPS ${planetblupi_DEPS} libvorbis_Project)
endif ()

#############
## libsndfile
#############

if (HAS_SNDFILE)
  ExternalProject_Add (libsndfile_Project
    URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/libsndfile-${SNDFILE_VER}.tar.gz
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

if (HAS_PULSEAUDIO)
  ExternalProject_Add (libpulse_Project
    URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/pulseaudio-${PULSEAUDIO_VER}.tar.xz
    PREFIX ${CMAKE_BINARY_DIR}
    CONFIGURE_COMMAND ${CMD} ./configure
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
    BUILD_COMMAND ${CMD} make
    INSTALL_COMMAND ${CMD} make install INSTALLDIR=${CMAKE_BINARY_DIR}
    BUILD_IN_SOURCE 1
    INSTALL_DIR ${CMAKE_BINARY_DIR}
    DEPENDS libasound_Project libsndfile_Project
  )

  set (planetblupi_DEPS ${planetblupi_DEPS} libpulse_Project)
endif ()

#######
## SDL2
#######

if (HAS_SDL)
  if (BUILD_LINUX)
    set (SDL2_CONFIGURE
      --enable-video-x11
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
      --disable-jack-shared
    )
  elseif (BUILD_WINDOWS)
    set (SDL2_CONFIGURE
      --enable-directx
      --enable-render-d3d
      --enable-video-opengl
    )
  elseif (BUILD_DARWIN)
    set (SDL2_CONFIGURE
      --enable-video-opengl
      --enable-video-cocoa
    )
  endif ()

  set (SDL2_DEPENDS)
  if (HAS_ZLIB)
    set (SDL2_DEPENDS zlib_Project)
  endif ()
  if (HAS_ALSA)
    set (SDL2_DEPENDS ${SDL2_DEPENDS} libasound_Project)
  endif ()
  if (HAS_PULSEAUDIO)
    set (SDL2_DEPENDS ${SDL2_DEPENDS} libpulse_Project)
  endif ()

  ExternalProject_Add (SDL2_Project
    URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/SDL2-${SDL_VER}.tar.bz2
    PREFIX ${CMAKE_BINARY_DIR}
    CONFIGURE_COMMAND ${CMD} ./configure
      --prefix=${CMAKE_BINARY_DIR}
      --disable-shared
      --enable-static
      --disable-joystick
      --disable-haptic
      ${SDL2_CONFIGURE}
    BUILD_COMMAND ${CMD} make
    INSTALL_COMMAND ${CMD} make install
    BUILD_IN_SOURCE 1
    INSTALL_DIR ${CMAKE_BINARY_DIR}
    DEPENDS ${SDL2_DEPENDS}
  )
endif ()

#############
## SDL2_image
#############

if (HAS_SDLIMAGE)
  ExternalProject_Add (SDL2_image_Project
    URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/SDL2_image-${SDLIMAGE_VER}.tar.gz
    PREFIX ${CMAKE_BINARY_DIR}
    CONFIGURE_COMMAND ${CMD} ./configure
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
    BUILD_COMMAND ${CMD} make
    INSTALL_COMMAND ${CMD} make install
    BUILD_IN_SOURCE 1
    INSTALL_DIR ${CMAKE_BINARY_DIR}
    DEPENDS SDL2_Project libpng_Project
  )

  set (planetblupi_DEPS ${planetblupi_DEPS} SDL2_image_Project)
endif ()

#############
## SDL2_mixer
#############

if (HAS_SDLMIXER)
  set (SDL2_mixer_DEPENDS)
  if (HAS_SDL)
    set (SDL2_mixer_DEPENDS ${SDL2_mixer_DEPENDS} SDL2_Project)
  endif ()
  if (HAS_OGG)
    set (SDL2_mixer_DEPENDS ${SDL2_mixer_DEPENDS} libogg_Project)
  endif ()
  if (HAS_VORBIS)
    set (SDL2_mixer_DEPENDS ${SDL2_mixer_DEPENDS} libvorbis_Project)
  endif ()
  if (HAS_ICONV)
    set (SDL2_mixer_DEPENDS ${SDL2_mixer_DEPENDS} libiconv_Project)
  endif ()

  ExternalProject_Add (SDL2_mixer_Project
    URL ${CMAKE_CURRENT_SOURCE_DIR}/externals/SDL2_mixer-${SDLMIXER_VER}.tar.gz
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
    BUILD_COMMAND ${CMD} make
    INSTALL_COMMAND ${CMD} make install
    BUILD_IN_SOURCE 1
    INSTALL_DIR ${CMAKE_BINARY_DIR}
    DEPENDS ${SDL2_mixer_DEPENDS}
  )

  set (planetblupi_DEPS ${planetblupi_DEPS} SDL2_mixer_Project)
endif ()

##################
## SDL_kitchensink
##################

if (HAS_KITCHENSINK)
  set (SDL_kitchensink_DEPENDS FFmpeg_Project)
  if (HAS_SDL)
    set (SDL_kitchensink_DEPENDS ${SDL_kitchensink_DEPENDS} SDL2_Project)
  endif ()

  ExternalProject_Add (SDL_kitchensink_Project
    URL ${CMAKE_SOURCE_DIR}/SDL_kitchensink
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/SDL_kitchensink
    PREFIX ${CMAKE_BINARY_DIR}
    CMAKE_COMMAND ${CMD} cmake
    CMAKE_ARGS -DDISABLE_SHARED=true
               -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
               -DCMAKE_C_FLAGS=${EMSCRIPTEN_FLAGS}
    INSTALL_DIR ${CMAKE_BINARY_DIR}
    BUILD_ALWAYS 1
    DEPENDS ${SDL_kitchensink_DEPENDS}
  )

  set (planetblupi_DEPS ${planetblupi_DEPS} SDL_kitchensink_Project)
endif ()

##############
## planetblupi
##############

if (BUILD_LINUX)
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

ExternalProject_Add (planetblupi_Project
  URL ${CMAKE_SOURCE_DIR}/planetblupi
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/planetblupi
  PREFIX ${CMAKE_BINARY_DIR}
  CMAKE_COMMAND ${CMD} cmake
  CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
             -DAPPIMAGE_APPRUN_PROGRAM=${APPIMAGE_APPRUN_PROGRAM}
             -DAPPIMAGE_ASSISTANT_PROGRAM=${APPIMAGE_TOOL_PROGRAM}
             -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
             -DPB_HTTP_VERSION_CHECK=${HAS_CURL}
             -DSTATIC_BUILD=${STATIC_BUILD}
             -DSIGN_APP=${SIGN_APP}
             -DEMSCRIPTEN_FLAGS=${EMSCRIPTEN_FLAGS}
             -DBUILD_JS=${BUILD_JS}
             -DBUILD_LINUX=${BUILD_LINUX}
             -DBUILD_DARWIN=${BUILD_DARWIN}
             -DBUILD_WINDOWS=${BUILD_WINDOWS}
  INSTALL_DIR ${CMAKE_BINARY_DIR}
  BUILD_ALWAYS 1
  DEPENDS ${planetblupi_DEPS}
)

add_custom_target (dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

set (CPACK_SOURCE_GENERATOR "TXZ")
set (CPACK_SOURCE_PACKAGE_FILE_NAME "planetblupi-dev")
set (CPACK_SOURCE_IGNORE_FILES "/build/;/Debug/;/Release/;/planetblupi-data/;/planetblupi-www/;/.git/;~$;${CPACK_SOURCE_IGNORE_FILES}")
include (CPack)
