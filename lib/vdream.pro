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
	../include/common/vdesign.h \
	../_vdream90/include/etc/vint.h \
	../_vdream90/include/common/vxml.h \
	../_vdream90/include/common/vmemoryleak.h \
	../_vdream90/include/common/vapp.h \
	../_vdream90/include/common/vstring.h \
	../_vdream90/include/common/vobject.h \
	../_vdream90/include/common/vmetaclass.h \
	../_vdream90/include/common/veventhandler.h \
	../_vdream90/include/common/vrwobject.h \
	../_vdream90/include/common/vsecurelib.h \
	../_vdream90/include/common/vlinklibrary.h \
	../_vdream90/include/common/vsystem.h \
	../_vdream90/include/common/vfactory.h \
	../_vdream90/include/common/vthread.h \
	../_vdream90/include/common/vcommon.h \
	../_vdream90/include/common/vperformance.h \
	../_vdream90/include/common/vtick.h \
	../_vdream90/include/common/vlinux.h \
	../_vdream90/include/common/vxmldoc.h \
	../_vdream90/include/common/vfile.h \
	../_vdream90/include/common/vdebugnew.h \
	../_vdream90/include/common/vwindows.h \
	../_vdream90/include/common/vobjectwidget.h \
	../_vdream90/include/common/vexception.h \
	../_vdream90/include/common/verror.h \
	../_vdream90/include/net/vnet.h \
	../_vdream90/include/net/vudpclient.h \
	../_vdream90/include/net/vnetserver.h \
	../_vdream90/include/net/vtcpserver.h \
	../_vdream90/include/net/http/vhttpresponse.h \
	../_vdream90/include/net/http/vwebproxywidget.h \
	../_vdream90/include/net/http/vhttpheader.h \
	../_vdream90/include/net/http/vhttpchunkbody.h \
	../_vdream90/include/net/http/vhttpcommon.h \
	../_vdream90/include/net/http/vhttprequest.h \
	../_vdream90/include/net/http/vwebproxy.h \
	../_vdream90/include/net/vsslsession.h \
	../_vdream90/include/net/vnetclient.h \
	../_vdream90/include/net/vsslclient.h \
	../_vdream90/include/net/vtcpclient.h \
	../_vdream90/include/net/vsslcommon.h \
	../_vdream90/include/net/vsslserver.h \
	../_vdream90/include/net/vtcpsession.h \
	../_vdream90/include/net/vnetsession.h \
	../_vdream90/include/net/vudpsession.h \
	../_vdream90/include/net/vnetfactory.h \
	../_vdream90/include/net/vudpserver.h \
	../_vdream90/include/log/vlogudp.h \
	../_vdream90/include/log/vlogstdout.h \
	../_vdream90/include/log/vlog.h \
	../_vdream90/include/log/vloglist.h \
	../_vdream90/include/log/vlogfactory.h \
	../_vdream90/include/log/vlognull.h \
	../_vdream90/include/log/vlogdbwin32.h \
	../_vdream90/include/log/vlogfile.h \
	../_vdream90/include/log/vlogstderr.h \
	../_vdream90/include/log/vloghttp.h \
	../_vdream90/include/etc/vdatachange.h \
	../_vdream90/include/etc/vmyobject.h \
	../_vdream90/include/etc/vdatafind.h \
	../_vdream90/include/etc/vlistwidget.h \
	../_vdream90/include/graph/vgraphlink.h \
	../_vdream90/include/graph/vgraphstream.h \
	../_vdream90/include/graph/vgraph.h \
	../_vdream90/include/graph/vgraphnode.h
	../include/etc/vmyobj.h

SOURCES += \
	../include/common/vdesign.cpp \
	../include/etc/vint.cpp \
	../include/common/vcommon.cpp \
	../_vdream90/include/common/vperformance.cpp \
	../_vdream90/include/common/vrwobject.cpp \
	../_vdream90/include/common/vthread.cpp \
	../_vdream90/include/common/vobjectwidget.cpp \
	../_vdream90/include/common/vxmldoc.cpp \
	../_vdream90/include/common/vsystem.cpp \
	../_vdream90/include/common/vsecurelib.cpp \
	../_vdream90/include/common/verror.cpp \
	../_vdream90/include/common/vtick.cpp \
	../_vdream90/include/common/vstring.cpp \
	../_vdream90/include/common/vxml.cpp \
	../_vdream90/include/common/vobject.cpp \
	../_vdream90/include/common/vdebugnew.cpp \
	../_vdream90/include/common/vmemoryleak.cpp \
	../_vdream90/include/common/vexception.cpp \
	../_vdream90/include/common/vmetaclass.cpp \
	../_vdream90/include/common/vapp.cpp \
	../_vdream90/include/common/vfile.cpp \
	../_vdream90/include/common/veventhandler.cpp \
	../_vdream90/include/net/vudpsession.cpp \
	../_vdream90/include/net/vtcpclient.cpp \
	../_vdream90/include/net/vudpclient.cpp \
	../_vdream90/include/net/vnetserver.cpp \
	../_vdream90/include/net/vnetclient.cpp \
	../_vdream90/include/net/vtcpserver.cpp \
	../_vdream90/include/net/vnet.cpp \
	../_vdream90/include/net/vnetfactory.cpp \
	../_vdream90/include/net/http/vwebproxywidget.cpp \
	../_vdream90/include/net/http/vwebproxy.cpp \
	../_vdream90/include/net/http/vhttpresponse.cpp \
	../_vdream90/include/net/http/vhttpchunkbody.cpp \
	../_vdream90/include/net/http/vhttpheader.cpp \
	../_vdream90/include/net/http/vhttprequest.cpp \
	../_vdream90/include/net/vtcpsession.cpp \
	../_vdream90/include/net/vsslserver.cpp \
	../_vdream90/include/net/vudpserver.cpp \
	../_vdream90/include/net/vsslcommon.cpp \
	../_vdream90/include/net/vsslclient.cpp \
	../_vdream90/include/net/vsslsession.cpp \
	../_vdream90/include/log/vlog.cpp \
	../_vdream90/include/log/vlogstderr.cpp \
	../_vdream90/include/log/vlogfactory.cpp \
	../_vdream90/include/log/vlogudp.cpp \
	../_vdream90/include/log/vlogdbwin32.cpp \
	../_vdream90/include/log/vlognull.cpp \
	../_vdream90/include/log/vlogstdout.cpp \
	../_vdream90/include/log/vloglist.cpp \
	../_vdream90/include/log/vloghttp.cpp \
	../_vdream90/include/log/vlogfile.cpp \
	../_vdream90/include/etc/vdatachange.cpp \
	../_vdream90/include/etc/vdatafind.cpp \
	../_vdream90/include/etc/vlistwidget.cpp \
	../_vdream90/include/etc/vmyobject.cpp \
	../_vdream90/include/graph/vgraphnode.cpp \
	../_vdream90/include/graph/vgraphlink.cpp \
	../_vdream90/include/graph/vgraph.cpp \
	../_vdream90/include/graph/vgraphstream.cpp \
	../include/etc/vmyobj.cpp
