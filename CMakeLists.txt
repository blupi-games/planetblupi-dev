
cmake_minimum_required (VERSION 3.2)

include (${CMAKE_ROOT}/Modules/ExternalProject.cmake)

include_directories (${CMAKE_BINARY_DIR}/include)
link_directories (${CMAKE_BINARY_DIR}/lib)

set (STATIC_BUILD true)

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
## argagg
#########

ExternalProject_Add (argagg_Project
  URL https://github.com/vietjtnguyen/argagg/archive/0.4.6.tar.gz
  PREFIX ${CMAKE_BINARY_DIR}
  CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}"
)

##########
## libcurl
##########

ExternalProject_Add (libcurl_Project
  URL https://curl.haxx.se/download/curl-7.55.0.tar.xz
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
  DEPENDS zlib_Project
)

###########
## libiconv
###########

if (NOT APPLE)
  ExternalProject_Add (libiconv_Project
    URL https://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.15.tar.gz
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

set (gettext_DEPENDS)
if (NOT APPLE)
  set (gettext_DEPENDS ${gettext_DEPENDS} libiconv_Project)
endif ()

ExternalProject_Add (gettext_Project
  URL http://mirror.switch.ch/ftp/mirror/gnu/gettext/gettext-0.19.8.1.tar.xz
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

#########
## libpng
#########

ExternalProject_Add (libpng_Project
  URL http://prdownloads.sourceforge.net/libpng/libpng-1.6.28.tar.xz
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
  BUILD_COMMAND ${CMD} make
  INSTALL_COMMAND ${CMD} make install
  BUILD_IN_SOURCE 1
  INSTALL_DIR ${CMAKE_BINARY_DIR}
)

############
## libasound
############

if (UNIX AND NOT APPLE)
  ExternalProject_Add (libasound_Project
    URL ftp://ftp.alsa-project.org/pub/lib/alsa-lib-1.1.4.1.tar.bz2
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
endif ()

#########
## libogg
#########

ExternalProject_Add (libogg_Project
  URL http://downloads.xiph.org/releases/ogg/libogg-1.3.3.tar.xz
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
  URL http://downloads.xiph.org/releases/vorbis/libvorbis-1.3.5.tar.xz
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

if (UNIX AND NOT APPLE)
  ExternalProject_Add (libsndfile_Project
    URL http://www.mega-nerd.com/libsndfile/files/libsndfile-1.0.28.tar.gz
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

if (UNIX AND NOT APPLE)
  ExternalProject_Add (libpulse_Project
    URL https://freedesktop.org/software/pulseaudio/releases/pulseaudio-11.0.tar.xz
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
endif ()

#######
## SDL2
#######

if (UNIX AND NOT APPLE)
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
                      --disable-pulseaudio-shared)
elseif (MINGW)
  set (SDL2_CONFIGURE --enable-directx
                      --enable-render-d3d
                      --enable-video-opengl)
elseif (APPLE)
  set (SDL2_CONFIGURE --enable-video-opengl
                      --enable-video-cocoa)
endif ()

set (SDL2_DEPENDS zlib_Project)
if (UNIX AND NOT APPLE)
  set (SDL2_DEPENDS ${SDL2_DEPENDS} libasound_Project libpulse_Project)
endif ()

ExternalProject_Add (SDL2_Project
  URL http://hg.libsdl.org/SDL/archive/3d0bbfe683a8.tar.bz2
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

set (SDL2_mixer_DEPENDS SDL2_Project libogg_Project libvorbis_Project)
if (NOT APPLE)
  set (SDL2_mixer_DEPENDS ${SDL2_mixer_DEPENDS} libiconv_Project)
endif ()

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

##################
## SDL_kitchensink
##################

ExternalProject_Add (SDL_kitchensink_Project
  URL ${CMAKE_SOURCE_DIR}/SDL_kitchensink
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/SDL_kitchensink
  PREFIX ${CMAKE_BINARY_DIR}
  CMAKE_COMMAND ${CMD} cmake
  CMAKE_ARGS -DDISABLE_SHARED=true
             -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
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
if (UNIX AND NOT APPLE)
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
  CMAKE_COMMAND ${CMD} cmake
  CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
             -DAPPIMAGE_APPRUN_PROGRAM=${APPIMAGE_APPRUN_PROGRAM}
             -DAPPIMAGE_ASSISTANT_PROGRAM=${APPIMAGE_TOOL_PROGRAM}
             -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
             -DPB_HTTP_VERSION_CHECK=yes
             -DSTATIC_BUILD=${STATIC_BUILD}
             -DSIGN_APP=${SIGN_APP}
  INSTALL_DIR ${CMAKE_BINARY_DIR}
  BUILD_ALWAYS 1
  DEPENDS ${planetblupi_DEPS}
)

add_custom_target (dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

set (CPACK_SOURCE_GENERATOR "TBZ2")
set (CPACK_SOURCE_PACKAGE_FILE_NAME "planetblupi-dev")
set (CPACK_SOURCE_IGNORE_FILES "/build/;/Debug/;/Release/;/.git/;~$;${CPACK_SOURCE_IGNORE_FILES}")
include (CPack)
