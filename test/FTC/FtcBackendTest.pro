QT = core testlib network
CONFIG += console testcase c++14
CONFIG -= app_bundle
TARGET = ftc-backend-test
INCLUDEPATH += $$PWD/standalone $$PWD/../../src/Vehicle $$PWD/../../libs/mavlink/include/mavlink/v2.0
SOURCES += $$PWD/FtcBackendTest.cpp $$PWD/FtcLiveProbe.cpp $$PWD/../../src/Vehicle/VehicleFtcStatusFactGroup.cc
HEADERS += $$PWD/../../src/Vehicle/VehicleFtcStatusFactGroup.h
msvc:QMAKE_CXXFLAGS += /utf-8
