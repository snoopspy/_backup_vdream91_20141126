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
	../include/etc/vint.h \
	../include/common/vxml.h \
	../include/common/vmemoryleak.h \
	../include/common/vapp.h \
	../include/common/vstring.h \
	../include/common/vobject.h \
	../include/common/vmetaclass.h \
	../include/common/veventhandler.h \
	../include/common/vrwobject.h \
	../include/common/vsecurelib.h \
	../include/common/vlinklibrary.h \
	../include/common/vsystem.h \
	../include/common/vfactory.h \
	../include/common/vthread.h \
	../include/common/vcommon.h \
	../include/common/vperformance.h \
	../include/common/vtick.h \
	../include/common/vlinux.h \
	../include/common/vxmldoc.h \
	../include/common/vfile.h \
	../include/common/vdebugnew.h \
	../include/common/vwindows.h \
	../include/common/vobjectwidget.h \
	../include/common/vexception.h \
	../include/common/verror.h \
	../include/net/vnet.h \
	../include/net/vudpclient.h \
	../include/net/vnetserver.h \
	../include/net/vtcpserver.h \
	../include/net/http/vhttpresponse.h \
	../include/net/http/vwebproxywidget.h \
	../include/net/http/vhttpheader.h \
	../include/net/http/vhttpchunkbody.h \
	../include/net/http/vhttpcommon.h \
	../include/net/http/vhttprequest.h \
	../include/net/http/vwebproxy.h \
	../include/net/vsslsession.h \
	../include/net/vnetclient.h \
	../include/net/vsslclient.h \
	../include/net/vtcpclient.h \
	../include/net/vsslcommon.h \
	../include/net/vsslserver.h \
	../include/net/vtcpsession.h \
	../include/net/vnetsession.h \
	../include/net/vudpsession.h \
	../include/net/vnetfactory.h \
	../include/net/vudpserver.h \
	../include/log/vlogudp.h \
	../include/log/vlogstdout.h \
	../include/log/vlog.h \
	../include/log/vloglist.h \
	../include/log/vlogfactory.h \
	../include/log/vlognull.h \
	../include/log/vlogdbwin32.h \
	../include/log/vlogfile.h \
	../include/log/vlogstderr.h \
	../include/log/vloghttp.h \
	../include/etc/vdatachange.h \
	../include/etc/vmyobject.h \
	../include/etc/vdatafind.h \
	../include/etc/vlistwidget.h \
	../include/graph/vgraphlink.h \
	../include/graph/vgraphstream.h \
	../include/graph/vgraph.h \
	../include/graph/vgraphnode.h
	../include/etc/vmyobj.h

SOURCES += \
	../include/common/vdesign.cpp \
	../include/etc/vint.cpp \
	../include/common/vcommon.cpp \
	../include/common/vperformance.cpp \
	../include/common/vrwobject.cpp \
	../include/common/vthread.cpp \
	../include/common/vobjectwidget.cpp \
	../include/common/vxmldoc.cpp \
	../include/common/vsystem.cpp \
	../include/common/vsecurelib.cpp \
	../include/common/verror.cpp \
	../include/common/vtick.cpp \
	../include/common/vstring.cpp \
	../include/common/vxml.cpp \
	../include/common/vobject.cpp \
	../include/common/vdebugnew.cpp \
	../include/common/vmemoryleak.cpp \
	../include/common/vexception.cpp \
	../include/common/vmetaclass.cpp \
	../include/common/vapp.cpp \
	../include/common/vfile.cpp \
	../include/common/veventhandler.cpp \
	../include/net/vudpsession.cpp \
	../include/net/vtcpclient.cpp \
	../include/net/vudpclient.cpp \
	../include/net/vnetserver.cpp \
	../include/net/vnetclient.cpp \
	../include/net/vtcpserver.cpp \
	../include/net/vnet.cpp \
	../include/net/vnetfactory.cpp \
	../include/net/http/vwebproxywidget.cpp \
	../include/net/http/vwebproxy.cpp \
	../include/net/http/vhttpresponse.cpp \
	../include/net/http/vhttpchunkbody.cpp \
	../include/net/http/vhttpheader.cpp \
	../include/net/http/vhttprequest.cpp \
	../include/net/vtcpsession.cpp \
	../include/net/vsslserver.cpp \
	../include/net/vudpserver.cpp \
	../include/net/vsslcommon.cpp \
	../include/net/vsslclient.cpp \
	../include/net/vsslsession.cpp \
	../include/log/vlog.cpp \
	../include/log/vlogstderr.cpp \
	../include/log/vlogfactory.cpp \
	../include/log/vlogudp.cpp \
	../include/log/vlogdbwin32.cpp \
	../include/log/vlognull.cpp \
	../include/log/vlogstdout.cpp \
	../include/log/vloglist.cpp \
	../include/log/vloghttp.cpp \
	../include/log/vlogfile.cpp \
	../include/etc/vdatachange.cpp \
	../include/etc/vdatafind.cpp \
	../include/etc/vlistwidget.cpp \
	../include/etc/vmyobject.cpp \
	../include/graph/vgraphnode.cpp \
	../include/graph/vgraphlink.cpp \
	../include/graph/vgraph.cpp \
	../include/graph/vgraphstream.cpp \
	../include/etc/vmyobj.cpp
