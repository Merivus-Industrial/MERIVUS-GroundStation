message("Adding MERIVUS Custom Plugin")

CUSTOM_QGC_VERSION = 0.1.0

DEFINES -= DAILY_BUILD

DEFINES -= APP_VERSION_STR=\"\\\"$$APP_VERSION_STR\\\"\"
DEFINES += APP_VERSION_STR=\"\\\"$$CUSTOM_QGC_VERSION\\\"\"

DEFINES += CUSTOMHEADER=\"\\\"CustomPlugin.h\\\"\"
DEFINES += CUSTOMCLASS=CustomPlugin

TARGET   = MERIVUS
DEFINES += QGC_APPLICATION_NAME='"\\\"MERIVUS\\\""'
DEFINES += QGC_ORG_NAME=\"\\\"MERIVUS\\\"\"
DEFINES += QGC_ORG_DOMAIN=\"\\\"com.merivus\\\"\"

QGC_APP_NAME        = "MERIVUS"
QGC_BINARY_NAME     = "MERIVUS"
QGC_ORG_NAME        = "MERIVUS"
QGC_ORG_DOMAIN      = "com.merivus"
QGC_ANDROID_PACKAGE = "com.merivus.qgroundcontrol"
QGC_APP_DESCRIPTION = "MERIVUS Ground Control"
QGC_APP_COPYRIGHT   = "Copyright (C) 2026 MERIVUS. All rights reserved."

RESOURCES += \
    $$PWD/custom.qrc

QML_IMPORT_PATH += \
   $$PWD/res

SOURCES += \
    $$PWD/src/CustomPlugin.cc \
    $$PWD/src/Swarm/SwarmController.cc

HEADERS += \
    $$PWD/src/CustomPlugin.h \
    $$PWD/src/Swarm/SwarmController.h

INCLUDEPATH += \
    $$PWD/src \
    $$PWD/src/Swarm

# Keep MSVC builds from failing on non-ASCII comments in upstream/source files.
win32-msvc {
    QMAKE_CXXFLAGS += /utf-8 /wd4819
    QMAKE_CFLAGS += /utf-8 /wd4819
    QMAKE_CXXFLAGS_WARN_ON -= /WX
    QMAKE_CFLAGS_WARN_ON -= /WX
    QMAKE_CXXFLAGS -= /WX
    QMAKE_CFLAGS -= /WX
    QMAKE_CXXFLAGS += /Zm500
    QMAKE_CFLAGS += /Zm500
}
