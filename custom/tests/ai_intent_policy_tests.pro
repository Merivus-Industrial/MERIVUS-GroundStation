QT += core
CONFIG += console c++17
CONFIG -= app_bundle
TEMPLATE = app
TARGET = ai_intent_policy_tests

INCLUDEPATH += \
    $$PWD/../src/Ai

SOURCES += \
    $$PWD/AiIntentPolicyTest.cc \
    $$PWD/../src/Ai/ActionProposal.cc \
    $$PWD/../src/Ai/AiCommandPolicy.cc \
    $$PWD/../src/Ai/AiSchemaValidator.cc

HEADERS += \
    $$PWD/../src/Ai/ActionProposal.h \
    $$PWD/../src/Ai/AiCommandPolicy.h \
    $$PWD/../src/Ai/AiSchemaValidator.h

win32-msvc {
    QMAKE_CXXFLAGS += /utf-8 /wd4819
    QMAKE_CXXFLAGS_WARN_ON -= /WX
}