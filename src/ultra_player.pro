#-------------------------------------------------
#
# Project created by QtCreator 2015-08-31T23:44:31
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = UltraPlayer
TEMPLATE = app

TARGET_NAME = $${TARGET}.$${TEMPLATE}

SOURCES += main.cpp \
        ui/mainwindow.cpp \
        core/player.cpp \
    core/device.cpp \
    core/message.cpp \
    core/bass_player.cpp

HEADERS  += ui/mainwindow.h \
            core/player.h \
    core/device.h \
    core/message.h \
    core/bass_player.h \
    core/custom_events.h

FORMS    += ui/mainwindow.ui

DESTDIR = ../bin
OBJECTS_DIR = ../obj
MOC_DIR = ../obj
RCC_DIR = ../obj
UI_DIR = ../obj
OUT_PWD = ../obj

target.path = ../

## Additional library files
win32 {
    LIB_DIR = ../lib/windows
}
unix {
    LIB_DIR = ../lib/unix
}
macx {
    LIB_DIR = ../lib/macosx
}
MY_LIB_FILES += $${LIB_DIR}/*
LIBS += -L$$LIB_DIR
LIBS += -lbass

## Copy those library files to bin directory
win32 {
    CP = copy /Y
    INSTALL_DIR = $${DESTDIR}
}
unix {
    CP = cp
    INSTALL_DIR = $${DESTDIR}
}
macx {
    CP = cp
    INSTALL_DIR = $${DESTDIR}/$${TARGET_NAME}/Contents/MacOS
}
QMAKE_POST_LINK += $${CP} $${MY_LIB_FILES} $${INSTALL_DIR}
