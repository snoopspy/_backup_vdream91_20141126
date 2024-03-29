#include "widget.h"
#include "ui_widget.h"

// ----------------------------------------------------------------------------
// UdpServerThread
// ----------------------------------------------------------------------------
UdpServerThread::UdpServerThread(void* owner, VUdpServer* udpServer)
{
	this->widget    = (Widget*)owner;
	this->udpServer = udpServer;
}

UdpServerThread::~UdpServerThread()
{
	close();
}

void UdpServerThread::fireEvent(QEvent* event)
{
	QApplication::postEvent(widget, event);
}

void UdpServerThread::run()
{
	LOG_ASSERT(udpServer != NULL);
	VSockAddrList& sockAddrList = udpServer->sockAddrList; // for abbreviate
	VUdpSession* udpSession = udpServer->udpSession; // for abbreviate

	// fireEvent(new MsgEvent("******** connected ********")); // gilgil temp 2014.03.01
	fireEvent(new StateEvent(VState::Opened));
	while (active())
	{
		QByteArray ba;
		int readLen = udpServer->read(ba);

		//
		// Delete session
		//
		if (readLen == VERR_FAIL)
		{
			QString msg = udpSession->error.msg;
			fireEvent(new MsgEvent(msg, QThread::currentThreadId()));

			if (udpSession->error.code == 10054/*WSAECONNRESET*/) // ICMP Destination Unreachable // gilgil temp 2014.04.18
			{
				while (true)
				{
					VSockAddrList::iterator it = sockAddrList.findByInAddr(udpSession->addr.sin_addr);
					if (it == sockAddrList.end()) break;
					sockAddrList.erase(it);
				}
				LOG_DEBUG("count=%d", sockAddrList.size()); // gilgi temp 2009.08.16
			} else // other error
			{
				VSockAddrList::iterator it = sockAddrList.findBySockAddr(udpSession->addr);
				if (it != sockAddrList.end())
					sockAddrList.erase(it);
				LOG_DEBUG("count=%d", sockAddrList.size()); // gilgi temp 2009.08.16
			}
			udpSession->error.clear();
			continue;
		}

		//
		// Add session
		//
		if (sockAddrList.findBySockAddr(udpSession->addr) == sockAddrList.end())
		{
			SOCKADDR_IN* newSockAddr = new SOCKADDR_IN;
			*newSockAddr = udpSession->addr;
			sockAddrList.insert(*newSockAddr);
			LOG_DEBUG("count=%d", sockAddrList.size()); // gilgi temp 2009.08.16
		}

		if (widget->ui->chkShowHexa->checkState() == Qt::Checked)
			ba = ba.toHex();
		QString msg = ba;
		fireEvent(new MsgEvent(msg, QThread::currentThreadId()));

		if (widget->ui->chkEcho->checkState() == Qt::Checked)
		{
			if (widget->ui->chkEchoBroadcast->checkState() == Qt::Checked)
			{
				udpServer->write(ba); // do not check result
			} else
			{
				int writeLen = udpSession->write(ba);
				if (writeLen == VERR_FAIL) break;
			}
		}
	}
	// fireEvent(new MsgEvent("******** disconnected ********")); // gilgil temp 2014.03.01
	fireEvent(new CloseEvent);
}

// ----------------------------------------------------------------------------
// Widget
// ----------------------------------------------------------------------------
Widget::Widget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Widget)
{
	ui->setupUi(this);
	initializeControl();
}

Widget::~Widget()
{
	saveControl();
	finalizeControl();
	delete ui;
}

void Widget::initializeControl()
{
	netServer       = NULL;
	udpServerThread = NULL;

	move(0, 0); resize(640, 480);

	setLayout(ui->mainLayout);
	ui->mainLayout->setSpacing(0);
	ui->pteRecv->setWordWrapMode(QTextOption::NoWrap);
	ui->pteSend->setWordWrapMode(QTextOption::NoWrap);

	VObject::connect(&tcpServer, SIGNAL(runned(VTcpSession*)), this, SLOT(tcpRun(VTcpSession*)), Qt::DirectConnection);
	VObject::connect(&sslServer, SIGNAL(runned(VSslSession*)), this, SLOT(sslRun(VSslSession*)), Qt::DirectConnection);
}

void Widget::finalizeControl()
{
	on_pbClose_clicked();
}

void Widget::loadControl()
{
	this->loadFromDefaultDoc("MainWindow");
}

void Widget::saveControl()
{
	this->saveToDefaultDoc("MainWindow");
}

void Widget::setControl(VState state)
{
	LOG_DEBUG(""); // gilgil temp 2014.02.28
	if (state == VState::None)
	{
		int currentIndex = ui->tabOption->currentIndex();
		switch (currentIndex)
		{
			case 0: netServer = &tcpServer; break;
			case 1: netServer = &udpServer; break;
			case 2: netServer = &sslServer; break;
		}
		state = netServer->state();
	}

	ui->pbOpen->setEnabled(state == VState::Closed);
	ui->pbClose->setEnabled(state != VState::Closed);
	ui->tabOption->setEnabled(state == VState::Closed);
	ui->pbSend->setEnabled(state == VState::Opened);
}

void Widget::tcpRun(VTcpSession* tcpSession)
{
	myRun(tcpSession);
}

void Widget::sslRun(VSslSession* sslSession)
{
	myRun(sslSession);
}

void Widget::fireEvent(QEvent* event)
{
	QApplication::postEvent(this, event);
}

void Widget::myRun(VNetSession* netSession)
{
	LOG_ASSERT(netSession != NULL);

	fireEvent(new MsgEvent("******** connected ********", QThread::currentThreadId()));

	while (true)
	{
		QByteArray ba;

		int readLen = netSession->read(ba);
		if (readLen == VERR_FAIL) break;

		if (ui->chkShowHexa->checkState() == Qt::Checked)
			ba = ba.toHex();
		QString msg = ba;
		fireEvent(new MsgEvent(msg, QThread::currentThreadId()));

		if (ui->chkEcho->checkState() == Qt::Checked)
		{
			if (ui->chkEchoBroadcast->checkState() == Qt::Checked)
			{
				netServer->write(ba); // do not check result
			} else
			{
				int writeLen = netSession->write(ba);
				if (writeLen == VERR_FAIL) break;
			}
		}
	}

	fireEvent(new MsgEvent("******** disconnected ********", QThread::currentThreadId())); // gilgil temp 2014.03.01
	// fireEvent(new CloseEvent); // gilgil temp 2014.03.01
}

bool Widget::event(QEvent* event)
{
	StateEvent* stateEvent = dynamic_cast<StateEvent*>(event);
	if (stateEvent != NULL)
	{
		setControl();
		return true;
	}

	MsgEvent* msgEvent = dynamic_cast<MsgEvent*>(event);
	if (msgEvent != NULL)
	{
		showMessage(msgEvent);
		return true;
	}

	CloseEvent* closeEvent = dynamic_cast<CloseEvent*>(event);
	if (closeEvent != NULL)
	{
		ui->pbClose->click();
		return true;
	}

	return QWidget::event(event);
}

void Widget::showEvent(QShowEvent* showEvent)
{
	loadControl();
	setControl();
	QWidget::showEvent(showEvent);
}

void Widget::showMessage(MsgEvent* event)
{
	static Qt::HANDLE lastThreadId = 0;
	if (lastThreadId != 0 && lastThreadId != event->threadId) event->msg = QString("\r\n") + event->msg;
	lastThreadId = event->threadId;
	ui->pteRecv->insertPlainText(event->msg);
	ui->pteRecv->ensureCursorVisible();
}

void Widget::load(VXml xml)
{
	{
		VXml coordXml = xml.findChild("coord");
		if (!coordXml.isNull())
		{
			QRect rect = geometry();
			rect.setLeft  ((coordXml.getInt("left",   0)));
			rect.setTop   ((coordXml.getInt("top",    0)));
			rect.setWidth ((coordXml.getInt("width",  640)));
			rect.setHeight((coordXml.getInt("height", 480)));
			setGeometry(rect);
		}
	}

	ui->chkShowHexa->setCheckState((Qt::CheckState)xml.getInt("showHexa", (int)ui->chkShowHexa->checkState()));
	ui->chkSendHexa->setCheckState((Qt::CheckState)xml.getInt("sendHexa", (int)ui->chkSendHexa->checkState()));
	ui->chkEcho->setCheckState((Qt::CheckState)xml.getInt("echo", (int)ui->chkEcho->checkState()));
	ui->chkEchoBroadcast->setCheckState((Qt::CheckState)xml.getInt("echoBroadcast", (int)ui->chkEchoBroadcast->checkState()));
	ui->tabOption->setCurrentIndex(xml.getInt("currentIndex", 0));
	ui->leTcpPort->setText(xml.getStr("tcpPort", ui->leTcpPort->text()));
	ui->leUdpPort->setText(xml.getStr("udpPort", ui->leUdpPort->text()));
	ui->leSslPort->setText(xml.getStr("sslPort", ui->leSslPort->text()));
	ui->pteSend->insertPlainText(xml.getStr("sendText", ui->pteSend->toPlainText()));

	{
		VXml sizesXml = xml.findChild("sizes");
		QList<int> sizes;
		if (!sizesXml.isNull())
		{
			QStringList strList = sizesXml.getStr("splitter").split(",");
			foreach (QString s, strList) sizes << s.toInt();
			ui->splitter->setSizes(sizes);
		}
	}

	tcpServer.load(xml.gotoChilds("netClient/tcpServer"));
	udpServer.load(xml.gotoChilds("netClient/udpServer"));
	sslServer.load(xml.gotoChilds("netClient/sslServer"));
}

void Widget::save(VXml xml)
{
	{
		VXml coordXml = xml.gotoChild("coord");
		QRect rect = geometry();
		coordXml.setInt("left",   rect.left());
		coordXml.setInt("top",    rect.top());
		coordXml.setInt("width",  rect.width());
		coordXml.setInt("height", rect.height());
	}

	xml.setInt("showHexa", (int)ui->chkShowHexa->checkState());
	xml.setInt("sendHexa", (int)ui->chkSendHexa->checkState());
	xml.setInt("echo", (int)ui->chkEcho->checkState());
	xml.setInt("echoBroadcast", (int)ui->chkEchoBroadcast->checkState());
	xml.setInt("currentIndex", ui->tabOption->currentIndex());
	xml.setStr("tcpPort", ui->leTcpPort->text());
	xml.setStr("udpPort", ui->leUdpPort->text());
	xml.setStr("sslPort", ui->leSslPort->text());
	xml.setStr("sendText", ui->pteSend->toPlainText());

	{
		VXml sizesXml = xml.gotoChild("sizes");
		QList<int> sizes = ui->splitter->sizes();
		QString strList;
		strList.clear(); foreach (int size, sizes) strList += QString::number(size) + ",";
		strList = strList.left(strList.count() - 1);
		sizesXml.setStr("splitter", strList);
	}

	tcpServer.save(xml.gotoChilds("netClient/tcpServer"));
	udpServer.save(xml.gotoChilds("netClient/udpServer"));
	sslServer.save(xml.gotoChilds("netClient/sslServer"));
}

void Widget::on_pbOpen_clicked()
{
	int currentIndex = ui->tabOption->currentIndex();
	switch (currentIndex)
	{
		case 0:
			tcpServer.port = ui->leTcpPort->text().toInt();
			netServer = &tcpServer;
			break;
		case 1:
			udpServer.port = ui->leUdpPort->text().toInt();
			netServer = &udpServer;
			break;
		case 2:
			sslServer.port = ui->leSslPort->text().toInt();
			netServer = &sslServer;
			break;
	}

	bool res = netServer->open();
	if (!res)
	{
		QString msg = netServer->error.msg;
		MsgEvent msgEvent(msg + "\r\n", QThread::currentThreadId());
		showMessage(&msgEvent);
		return;
	}
	if (netServer == &udpServer)
	{
		SAFE_DELETE(udpServerThread);
		udpServerThread = new UdpServerThread(this, &udpServer);
		udpServerThread->open();
	}

	setControl();
}

void Widget::on_pbClose_clicked()
{
	LOG_ASSERT(netServer != NULL);
	netServer->close();
	SAFE_DELETE(udpServerThread);
	setControl();
}

void Widget::on_pbClear_clicked()
{
	ui->pteRecv->clear();
}

void Widget::on_tbTcpAdvance_clicked()
{
	if (tcpServer.optionDoAll(this))
	{
		ui->leTcpPort->setText(QString::number(tcpServer.port));
	}
}

void Widget::on_tbUdpAdvence_clicked()
{
	if (udpServer.optionDoAll(this))
	{
		ui->leTcpPort->setText(QString::number(udpServer.port));
	}
}

void Widget::on_tbSslAdvanced_clicked()
{
	if (sslServer.optionDoAll(this))
	{
		ui->leTcpPort->setText(QString::number(sslServer.port));
	}
}

void Widget::on_pbSend_clicked()
{
	if (netServer == NULL) return;
	QByteArray ba = qPrintable(ui->pteSend->toPlainText());
	ba = ba.replace("\n", "\r\n");
	if (ui->chkSendHexa->checkState() == Qt::Checked) ba = ba.fromHex(ba);
	netServer->write(ba);
}
