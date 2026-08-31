################################################################################
#
# (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
#
# QGroundControl is licensed according to the terms in the file
# COPYING.md in the root of the source code directory.
#
################################################################################

#
#-- Depends on gstreamer, which can be found at: http://gstreamer.freedesktop.org/download/
#

LinuxBuild {
    QT += x11extras waylandclient
    CONFIG += link_pkgconfig
    packagesExist(gstreamer-1.0) {
        PKGCONFIG   += gstreamer-1.0  gstreamer-video-1.0 gstreamer-gl-1.0 egl
        CONFIG      += VideoEnabled
    }
} else:MacBuild {
    #- gstreamer framework installed by the gstreamer devel installer
    GST_ROOT = /Library/Frameworks/GStreamer.framework
    exists($$GST_ROOT) {
        CONFIG      += VideoEnabled
        INCLUDEPATH += $$GST_ROOT/Headers
        LIBS        += -F/Library/Frameworks -framework GStreamer
        QMAKE_LIBDIR += $$GST_ROOT/Versions/1.0/lib/
    }
} else:iOSBuild {
    #- gstreamer framework installed by the gstreamer iOS SDK installer (default to home directory)
    GST_ROOT = $$(HOME)/Library/Developer/GStreamer/iPhone.sdk/GStreamer.framework
    exists($$GST_ROOT) {
        CONFIG      += VideoEnabled
        INCLUDEPATH += $$GST_ROOT/Headers
        LIBS        += -F$$(HOME)/Library/Developer/GStreamer/iPhone.sdk -framework GStreamer -framework AVFoundation -framework CoreMedia -framework CoreVideo -framework VideoToolbox -liconv -lresolv
    }
} else:WindowsBuild {
    # Prefer the path exported by the official MSVC x64 installer. PATH alone
    # is not enough because qmake needs the development headers and import libs.
    GST_ROOT = $$(GSTREAMER_1_0_ROOT_MSVC_X86_64)

    isEmpty(GST_ROOT) {
        # Kept for compatibility with older GStreamer Windows installers.
        GST_ROOT = $$(GSTREAMER_1_0_ROOT_X86_64)
    }

    isEmpty(GST_ROOT) {
        GST_ROOT = $$(ProgramFiles)/gstreamer/1.0/msvc_x86_64
    }

    !exists($$GST_ROOT/include/gstreamer-1.0/gst/gst.h) {
        GST_ROOT = c:/gstreamer/1.0/msvc_x86_64
    }

    !exists($$GST_ROOT/include/gstreamer-1.0/gst/gst.h) {
        # GitHub Actions installs dependencies on the D drive.
        GST_ROOT = d:/gstreamer/1.0/msvc_x86_64
    }

    exists($$GST_ROOT/include/gstreamer-1.0/gst/gst.h):exists($$GST_ROOT/lib/gstreamer-1.0.lib) {
        message("Using GStreamer from $$GST_ROOT")
        CONFIG      += VideoEnabled

        LIBS        += -L$$GST_ROOT/lib -lgstreamer-1.0 -lgstgl-1.0 -lgstvideo-1.0 -lgstbase-1.0
        LIBS        += -lglib-2.0 -lintl -lgobject-2.0

        INCLUDEPATH += \
            $$GST_ROOT/include \
            $$GST_ROOT/include/gstreamer-1.0 \
            $$GST_ROOT/include/glib-2.0 \
            $$GST_ROOT/lib/gstreamer-1.0/include \
            $$GST_ROOT/lib/glib-2.0/include

        DESTDIR_WIN = $$replace(DESTDIR, "/", "\\")
        GST_ROOT_WIN = $$replace(GST_ROOT, "/", "\\")

        !exists($$GST_ROOT/lib/gstreamer-1.0/gstrtsp.dll) {
            error("GStreamer RTSP plugin is missing. Modify the Runtime installer and select Complete.")
        }
        !exists($$GST_ROOT/lib/gstreamer-1.0/gstrtp.dll) {
            error("GStreamer RTP plugin is missing. Modify the Runtime installer and select Complete.")
        }
        !exists($$GST_ROOT/libexec/gstreamer-1.0/gst-plugin-scanner.exe) {
            error("GStreamer plugin scanner is missing. Install the complete MSVC x64 Runtime package.")
        }
        !exists($$GST_ROOT/lib/gstreamer-1.0/gstlibav.dll) {
            warning("GStreamer libav plugin is missing; Force software decoder will be unavailable.")
        }

        # Never merge a newly selected GStreamer version with plugins left by
        # an older build. The application loads plugins only from this folder.
        QMAKE_POST_LINK += $$escape_expand(\\n) if exist \"$$DESTDIR_WIN\\gstreamer-plugins\" rmdir /S /Q \"$$DESTDIR_WIN\\gstreamer-plugins\" $$escape_expand(\\n)
        QMAKE_POST_LINK += if exist \"$$DESTDIR_WIN\\libexec\\gstreamer-1.0\" rmdir /S /Q \"$$DESTDIR_WIN\\libexec\\gstreamer-1.0\" $$escape_expand(\\n)

        # Copy main GStreamer runtime files
        QMAKE_POST_LINK += $$escape_expand(\\n) xcopy \"$$GST_ROOT_WIN\\bin\*.dll\" \"$$DESTDIR_WIN\" /S/Y $$escape_expand(\\n)

        # Copy GStreamer plugins
        QMAKE_POST_LINK += $$escape_expand(\\n) xcopy \"$$GST_ROOT_WIN\\lib\\gstreamer-1.0\\*.dll\" \"$$DESTDIR_WIN\\gstreamer-plugins\\\" /Y $$escape_expand(\\n)

        # Keep plugin discovery out of the application process. Without the
        # scanner, a faulty hardware plugin can crash QGC during gst_init().
        QMAKE_POST_LINK += $$escape_expand(\\n) xcopy \"$$GST_ROOT_WIN\\libexec\\gstreamer-1.0\\gst-plugin-scanner.exe\" \"$$DESTDIR_WIN\\libexec\\gstreamer-1.0\\\" /Y $$escape_expand(\\n)
    } else {
        warning("GStreamer MSVC x64 development files were not found under $$GST_ROOT")
    }
} else:AndroidBuild {
    #- gstreamer assumed to be installed in $$PWD/../../gstreamer-1.0-android-universal-1.18.6/***
    contains(ANDROID_TARGET_ARCH, armeabi-v7a) {
        GST_ROOT = $$PWD/../../gstreamer-1.0-android-universal-1.18.6/armv7
    } else:contains(ANDROID_TARGET_ARCH, arm64-v8a) {
        GST_ROOT = $$PWD/../../gstreamer-1.0-android-universal-1.18.6/arm64
    } else:contains(ANDROID_TARGET_ARCH, x86_64) {
        GST_ROOT = $$PWD/../../gstreamer-1.0-android-universal-1.18.6/x86_64
    } else {
        message(Unknown ANDROID_TARGET_ARCH $$ANDROID_TARGET_ARCH)
        GST_ROOT = $$PWD/../../gstreamer-1.0-android-universal-1.18.6/x86
    }
    exists($$GST_ROOT) {
        QMAKE_CXXFLAGS  += -pthread
        CONFIG          += VideoEnabled

        # We want to link these plugins statically
        LIBS += -L$$GST_ROOT/lib/gstreamer-1.0 \
            -lgstvideo-1.0 \
            -lgstcoreelements \
            -lgstplayback \
            -lgstudp \
            -lgstrtp \
            -lgstrtsp \
            -lgstx264 \
            -lgstlibav \
            -lgstsdpelem \
            -lgstvideoparsersbad \
            -lgstrtpmanager \
            -lgstisomp4 \
            -lgstmatroska \
            -lgstmpegtsdemux \
            -lgstandroidmedia \
            -lgstopengl \
            -lgsttcp

        # Rest of GStreamer dependencies
        LIBS += -L$$GST_ROOT/lib \
            -lgraphene-1.0 -ljpeg -lpng16 \
            -lgstfft-1.0 -lm  \
            -lgstnet-1.0 -lgio-2.0 \
            -lgstphotography-1.0 -lgstgl-1.0 -lEGL \
            -lgstaudio-1.0 -lgstcodecparsers-1.0 -lgstbase-1.0 \
            -lgstreamer-1.0 -lgstrtp-1.0 -lgstpbutils-1.0 -lgstrtsp-1.0 -lgsttag-1.0 \
            -lgstvideo-1.0 -lavformat -lavcodec -lavutil -lx264 -lavfilter -lswresample \
            -lgstriff-1.0 -lgstcontroller-1.0 -lgstapp-1.0 \
            -lgstsdp-1.0 -lbz2 -lgobject-2.0 -lgstmpegts-1.0 \
            -Wl,--export-dynamic -lgmodule-2.0 -pthread -lglib-2.0 -lorc-0.4 -liconv -lffi -lintl \

        INCLUDEPATH += \
            $$GST_ROOT/include/gstreamer-1.0 \
            $$GST_ROOT/lib/gstreamer-1.0/include \
            $$GST_ROOT/include/glib-2.0 \
            $$GST_ROOT/lib/glib-2.0/include
    }
}

VideoEnabled {

    message("Including support for video streaming")

    DEFINES += \
        QGC_GST_STREAMING

    INCLUDEPATH += \
        $$PWD

    iOSBuild {
        OBJECTIVE_SOURCES += \
            $$PWD/gst_ios_init.m
    }

    HEADERS += \
        $$PWD/GStreamer.h \
        $$PWD/GstVideoReceiver.h \
        $$PWD/VideoReceiver.h

    SOURCES += \
        $$PWD/gstqgcvideosinkbin.c \
        $$PWD/gstqgc.c \
        $$PWD/GStreamer.cc \
        $$PWD/GstVideoReceiver.cc

    include($$PWD/../../qmlglsink.pri)
} else {
    LinuxBuild|MacBuild|iOSBuild|WindowsBuild|AndroidBuild {
        message("Skipping support for video streaming (GStreamer libraries not installed)")
        message("Installation instructions here: https://github.com/mavlink/qgroundcontrol/blob/master/src/VideoReceiver/README.md")
    } else {
        message("Skipping support for video streaming (Unsupported platform)")
    }
}
