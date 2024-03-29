QT += core xml

#-------------------------------------------------
# VDREAM_LIB_NAME
#-------------------------------------------------
VDREAM_LIB_NAME=vdream
CONFIG(debug, debug|release) {
	VDREAM_LIB_NAME=$${VDREAM_LIB_NAME}_d
}

contains(QT, gui) {
	VDREAM_LIB_NAME=$${VDREAM_LIB_NAME}_gui
}

android-g++ {
	VDREAM_LIB_NAME=$${VDREAM_LIB_NAME}_android
}

#-------------------------------------------------
# debug and release
#-------------------------------------------------
CONFIG(debug, debug|release) {
	DEFINES += _DEBUG
}
CONFIG(release, debug|release) {
	DEFINES += _RELEASE
}

#-------------------------------------------------
# vdream
#-------------------------------------------------
VDREAM_PATH            =   $$PWD/..
INCLUDEPATH           +=   $${VDREAM_PATH}/include
INCLUDEPATH           +=   $${VDREAM_PATH}/lib
DEPENDPATH            +=   $${VDREAM_PATH}
CONFIG(VDREAM_LIB_BUILD) {
} else {
	LIBS                       += -L$${VDREAM_PATH}/lib -l$${VDREAM_LIB_NAME}
	win32:PRE_TARGETDEPS       +=  $${VDREAM_PATH}/lib/$${VDREAM_LIB_NAME}.lib
	linux-g++:PRE_TARGETDEPS   +=  $${VDREAM_PATH}/lib/lib$${VDREAM_LIB_NAME}.a
	android-g++:PRE_TARGETDEPS +=  $${VDREAM_PATH}/lib/lib$${VDREAM_LIB_NAME}.so
}

#-------------------------------------------------
# openssl
#-------------------------------------------------
win32 {
	contains(QMAKE_TARGET.arch, x86_64) {
		OPENSSL_PATH = C:/OpenSSL-Win64
	} else {
		OPENSSL_PATH = C:/OpenSSL-Win32
	}
	INCLUDEPATH   += $${OPENSSL_PATH}/include
	LIBS          += -L$${OPENSSL_PATH}/lib
	LIBS          += -llibeay32 -lssleay32
}
linux {
	LIBS          += -lssl -lcrypto
}
