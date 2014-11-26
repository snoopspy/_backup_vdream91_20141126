CONFIG += VDREAM_LIB_BUILD
QT -= gui
#QT += gui widgets

include (vdream.pri)

TARGET   = $${VDREAM_LIB_NAME}
TEMPLATE = lib
CONFIG  += staticlib
DESTDIR  = ../lib
android-g++ {
} else {
	CONFIG  += staticlib
}

HEADERS += \
    ../include/etc/vmyobj.h

SOURCES += \
    ../include/etc/vmyobj.cpp
