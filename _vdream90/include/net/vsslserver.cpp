#include <VSslServer>
#include <VApp>
#include <VFile>
#include <VDebugNew>

REGISTER_METACLASS(VSslServer, VNet)

// ----------------------------------------------------------------------------
// VSslServerSession
// ----------------------------------------------------------------------------
VSslServerSession::VSslServerSession(void* owner) : VSslSession(owner)
{
}

VSslServerSession::~VSslServerSession()
{
	close();
}

bool VSslServerSession::doOpen()
{
	return VSslSession::doOpen();
}

bool VSslServerSession::doClose()
{
	return VSslSession::doClose();
}

bool VSslServerSession::setup(QString fileName)
{
	// LOG_DEBUG("------------------------------------------"); // gilgil temp 2014.03.14
	LOG_DEBUG("fileName=%s", qPrintable(fileName));
	// LOG_DEBUG("------------------------------------------"); // gilgil temp 2014.03.14

	EVP_PKEY* key = VSslServer::loadKey(error, fileName);
	if (key == NULL) return false;

	X509* crt = VSslServer::loadCrt(error, fileName);
	if (crt == NULL) return false;

	bool res = VSslServer::setKeyCrtStuff(error, con, key, crt);
	if (!res) return false;

	EVP_PKEY_free(key);
	X509_free(crt);

	return true;
}

// ----------------------------------------------------------------------------
// VSslServer
// ----------------------------------------------------------------------------
VSslServer::VSslServer(void* owner) : VTcpServer(owner)
{
	VSslCommon::instance();

	methodType             = VSslMethodType::mtSSLv23;
	certificatePath        = QString("certificate") + QDir::separator();
	defaultKeyCrtFileName  = "default.pem";
	processConnectMessage  = false;
	m_meth                 = NULL;
	m_ctx                  = NULL;

	VObject::connect(this, SIGNAL(runned(VTcpSession*)), this, SLOT(myRun(VTcpSession*)), Qt::DirectConnection);
}

VSslServer::~VSslServer()
{
	close();
}

bool VSslServer::doOpen()
{
	VLock lock(stateOpenCs); // gilgil temp 2014.03.14

	//
	// Set server method
	//
	LOG_DEBUG("method=%s", qPrintable(methodType.str()));
	switch (methodType)
	{
		#ifdef WIN32 // Linux openssl does not support SSLv2_server_method
		case VSslMethodType::mtSSLv2   : m_meth = (SSL_METHOD*)SSLv2_server_method();   break;
		#endif // WIN32
		case VSslMethodType::mtSSLv3   : m_meth = (SSL_METHOD*)SSLv3_server_method();   break;
		case VSslMethodType::mtSSLv23  : m_meth = (SSL_METHOD*)SSLv23_server_method();  break;
		case VSslMethodType::mtTLSv1   : m_meth = (SSL_METHOD*)TLSv1_server_method();   break;
		case VSslMethodType::mtTLSv1_1 : m_meth = (SSL_METHOD*)TLSv1_1_server_method(); break;
		case VSslMethodType::mtTLSv1_2 : m_meth = (SSL_METHOD*)TLSv1_2_server_method(); break;
		case VSslMethodType::mtDTLSv1  : m_meth = (SSL_METHOD*)DTLSv1_server_method();  break;
		case VSslMethodType::mtNone    :
		default                        :
			SET_ERROR(VSslError, qformat("client method error(%s)", qPrintable(methodType.str())), VERR_SSL_METHOD);
			return false;
	}

	m_ctx = SSL_CTX_new(m_meth);
	if (!SSL_CTX_set_tlsext_servername_callback(m_ctx, ssl_servername_cb))
	{
		LOG_ERROR("SSL_CTX_set_tlsext_servername_callback return false");
	}
	if (!SSL_CTX_set_tlsext_servername_arg(m_ctx, this))
	{
		LOG_ERROR("SSL_CTX_set_tlsext_servername_arg return false");
	}

	if (defaultKeyCrtFileName != "")
	{
		QString fileName = certificatePath;
		QFileInfo fi(fileName);
		if (!fi.isAbsolute()) fileName = VApp::_filePath() + fileName;
		if (!fileName.endsWith('/') && !fileName.endsWith('\\'))
		{
			fileName += QDir::separator();
		}
		fileName += defaultKeyCrtFileName;
		if (!setup(fileName)) return false;
	}

	if (!VTcpServer::doOpen()) return false;

	return true;
}

bool VSslServer::doClose()
{
	VLock lock(stateCloseCs); // gilgil temp 2014.03.14

	sslSessionList.lock();
	for (int i = 0; i < sslSessionList.count(); i++)
	{
		VSslSession* session = sslSessionList.at(i);
		session->close();
	}
	sslSessionList.unlock();

	VTcpServer::doClose();

	//
	// m_meth and m_ctx
	//
	if (m_meth != NULL)
	{
		m_meth = NULL;
	}
	if (m_ctx != NULL)
	{
		SSL_CTX_free(m_ctx);
		m_ctx = NULL;
	}

	return true;
}

int VSslServer::doRead(char* buf, int size)
{
	// VLock lock(stateReadCs); // gilgil temp 2014.03.14

	Q_UNUSED(buf)
	Q_UNUSED(size)
	SET_ERROR(VError, "not readable", VERR_NOT_READABLE);
	return VERR_FAIL;
}

int VSslServer::doWrite(char* buf, int size)
{
	VLock lock(stateWriteCs);

	sslSessionList.lock();
	for (int i = 0; i < sslSessionList.count(); i++)
	{
		VSslSession* session = sslSessionList.at(i);
		session->write(buf, size);
	}
	sslSessionList.unlock();
	return size;
}

// ----- gilgil temp 2014.03.24 -----
/*
int VSslServer::ssl_servername_cb(SSL *con, int *ad, void *arg) // gilgil temp 2014.03.07
{
	int debug = 0;
	int res = 0;
	TRY
	{
		res = ssl_servername_cb_debug(con, ad, arg, &debug);
	} EXCEPT
	{
		LOG_FATAL("debug=%d %d", debug, threadTag);
	}
	return res;
}
*/
// ----------------------------------

// int VSslServer::ssl_servername_cb_debug(SSL *con, int *ad, void *arg, int* debug) // gilgil temp 2014.03.14
int VSslServer::ssl_servername_cb(SSL *con, int *ad, void *arg)
{
	Q_UNUSED(ad)

	const char* serverName = SSL_get_servername(con, TLSEXT_NAMETYPE_host_name);
	// *debug = 1000; // gilgil temp 2014.03.14

	if (serverName == NULL)
	{
		LOG_DEBUG("serverName is null");
		return SSL_TLSEXT_ERR_NOACK;
	}
	// *debug = 2500; // gilgil temp 2014.03.14
	// LOG_DEBUG("serverName=%p %s", serverName, serverName); // gilgil temp 2014.03.14
	// *debug = 3000; // gilgil temp 2014.03.14

	VSslServer* server  = (VSslServer*)(arg);
	LOG_ASSERT(server != NULL);

	VSslServerSession* session = (VSslServerSession*)SSL_get_ex_data(con, VSslSession::VSSL_SESSION_IDENTIFY_INDEX);
	LOG_ASSERT(session->con == con);

	// *debug = 500; // gilgil temp 2014.03.14
	// LOG_DEBUG("server=%p session=%p", server, session); // gilgil temp 2014.03.14

	QString path = server->certificatePath;
	QFileInfo fi(path);
	if (!fi.isAbsolute())
	{
		path = VApp::_filePath() + path;
	}
	if (!path.endsWith('/') && !path.endsWith('\\')) path += QDir::separator();

	QString fileName = path + serverName + ".pem";
	// *debug = 4000; // gilgil temp 2014.03.14
	{
		VLock lock(server->certificateCs); // protect file create critical section
		// *debug = 5000; // gilgil temp 2014.03.14
		if (!QFile::exists(fileName))
		{
			QProcess process;

			process.setWorkingDirectory(path);
			LOG_DEBUG("working directory=%s", qPrintable(process.workingDirectory())); // gilgil temp 2014.03.01

			QString command = qformat("\"%s_make_site.bat\" %s 2>&1", qPrintable(path), qPrintable(serverName));
			LOG_INFO("command=%s", qPrintable(command));

			process.start(command);
			LOG_DEBUG("pid=%llx", process.pid());

			// *debug = 6000; // gilgil temp 2014.03.14
			if(!process.waitForStarted())
			{
				LOG_FATAL("process.waitForStarted(%s) return false", qPrintable(command));
			}
			// *debug = 700; // gilgil temp 2014.03.14
			while(process.waitForReadyRead())
			{
				QByteArray ba = process.readAll();
				LOG_DEBUG("ba.size=%d", ba.size())
				LOG_DEBUG("ba.datas=%s", ba.data());
			}
			// *debug = 8000; // gilgil temp 2014.03.14
		}
		// *debug = 9000; // gilgil temp 2014.03.14
		// LOG_DEBUG("con=%p", con); // gilgil temp 2014.03.14
		// *debug = 9100; // gilgil temp 2014.03.14
		if (!session->setup(fileName))
		{
			LOG_ERROR("session->setup(%s) return false", qPrintable(fileName));
		}
		// *debug = 9500; // gilgil temp 2014.03.14
	}

	return SSL_TLSEXT_ERR_NOACK;
}

bool VSslServer::setup(QString fileName)
{
	// LOG_DEBUG("------------------------------------------"); // gilgil temp 2014.03.14
	LOG_DEBUG("fileName=%s", qPrintable(fileName));
	// LOG_DEBUG("------------------------------------------"); // gilgil temp 2014.03.14

	EVP_PKEY* key = VSslServer::loadKey(error, fileName);
	if (key == NULL) return false;

	X509* crt = VSslServer::loadCrt(error, fileName);
	if (crt == NULL) return false;

	bool res = VSslServer::setKeyCrtStuff(error, m_ctx, key, crt);
	if (!res) return false;

	EVP_PKEY_free(key);
	X509_free(crt);

	return true;
}

EVP_PKEY* VSslServer::loadKey(VError& error, QString fileName)
{
	BIO* bio = BIO_new(BIO_s_file());
	if (bio == NULL)
	{
		QString msg = "BIO_s_file return NULL";
		LOG_ERROR("%s", qPrintable(msg));
		setError<VSslError>(error, msg, VERR_IN_BIO_S_FILE);
		return NULL;
	}

	long res = BIO_read_filename(bio, qPrintable(fileName));
	if (res <= 0)
	{
		QString msg = qformat("BIO_read_filename(%s) return %d", qPrintable(fileName), res);
		LOG_ERROR("%s", qPrintable(msg));
		setError<VSslError>(error, msg, VERR_IN_BIO_READ_FILENAME);
		BIO_free(bio);
		return NULL;
	}

	EVP_PKEY* key = PEM_read_bio_PrivateKey(bio, NULL,
		NULL, // (pem_password_cb*)password_callback // gilgil temp 2008.10.29
		NULL  // &cb_data);
	);
	if (key == NULL)
	{
		QString msg = "PEM_read_bio_PrivateKey return NULL";
		LOG_ERROR("%s", qPrintable(msg));
		setError<VSslError>(error, msg, VERR_IN_PEM_READ_BIO_PRIVATEKEY);
		BIO_free(bio);
		return NULL;
	}

	BIO_free(bio);
	return key;
}

X509* VSslServer::loadCrt(VError& error, QString fileName)
{
	BIO* bio = BIO_new(BIO_s_file());
	if (bio == NULL)
	{
		QString msg = "BIO_s_file return NULL";
		LOG_ERROR("%s", qPrintable(msg));
		setError<VSslError>(error, msg, VERR_IN_BIO_S_FILE);
		BIO_free(bio);
		return NULL;
	}

	long res = BIO_read_filename(bio, qPrintable(fileName));
	if (res <= 0)
	{
		QString msg = qformat("BIO_read_filename(%s) %d", qPrintable(fileName), res);
		LOG_ERROR("%s", qPrintable(msg));
		setError<VSslError>(error, msg, VERR_IN_BIO_READ_FILENAME);
		BIO_free(bio);
		return NULL;
	}

	X509* crt = PEM_read_bio_X509_AUX(bio, NULL, NULL, NULL);
	if (crt == NULL)
	{
		QString msg = "PEM_read_bio_X509_AUX return NULL";
		LOG_ERROR("%s", qPrintable(msg));
		setError<VSslError>(error, msg, VERR_IN_PEM_READ_BIO_X509_AUX);
		BIO_free(bio);
		return NULL;
	}

	BIO_free(bio);
	return crt;
}

bool VSslServer::setKeyCrtStuff(VError& error, SSL_CTX* ctx, EVP_PKEY* key, X509* crt)
{
	LOG_ASSERT(key != NULL);
	LOG_ASSERT(crt != NULL);

	int res = SSL_CTX_use_certificate(ctx, crt);
	if (res <= 0)
	{
		setError<VSslError>(error, qformat("SSL_CTX_use_certificate return %d", res), VERR_IN_SSL_CTX_USE_CERTIFICATE);
		return false;
	}

	res = SSL_CTX_use_PrivateKey(ctx, key);
	if (res <= 0)
	{
		setError<VSslError>(error, qformat("SSL_CTX_use_PrivateKey return %d", res), VERR_SSL_CTX_USER_PRIVATEKEY);
		return false;
	}

	res = SSL_CTX_check_private_key(ctx);
	if (!res)
	{
		setError<VSslError>(error, qformat("SSL_CTX_check_private_key return %d", res), VERR_SSL_CTX_CHECK_PRIVATEKEY);
		return false;
	}

	return true;
}

bool VSslServer::setKeyCrtStuff(VError& error, SSL* con, EVP_PKEY* key, X509* crt)
{
	LOG_ASSERT(key != NULL);
	LOG_ASSERT(crt != NULL);

	// LOG_DEBUG("con=%p key=%p crt=%p", con, key, crt); // gilgil temp 2014.03.14
	int res = SSL_use_certificate(con, crt);
	if (res <= 0)
	{
		setError<VSslError>(error, qformat("SSL_use_certificate return %d", res), VERR_IN_SSL_CTX_USE_CERTIFICATE);
		return false;
	}

	res = SSL_use_PrivateKey(con, key);
	if (res <= 0)
	{
		setError<VSslError>(error, qformat("SSL_use_PrivateKey return %d", res), VERR_SSL_CTX_USER_PRIVATEKEY);
		return false;
	}

	res = SSL_check_private_key(con);
	if (!res)
	{
		setError<VSslError>(error, qformat("SSL_check_private_key return %d", res), VERR_SSL_CTX_CHECK_PRIVATEKEY);
		return false;
	}

	return true;
}

// static VCS sslAccept; // gilgil temp 2014.03.24
void VSslServer::myRun(VTcpSession* tcpSession)
{
	tag = 3000; // gilgil temp 2014.04.04
	VSslServerSession *sslSession = new VSslServerSession;
	{
		//VLock lock(sslAccept); // gilgil temp 2014.03.24
		sslSession->owner       = this;
		sslSession->tcpSession  = tcpSession;
		// sslSession->handle      = tcpSession->handle; // gilgil temp 2014.04.03
		sslSession->ctx         = m_ctx;
		if (!sslSession->open()) goto _end;
		// LOG_DEBUG("beg sslSession=%p con=%p", sslSession, sslSession->con); // gilgil temp 2014.03.07

		if (processConnectMessage)
		{
			QByteArray ba;
			int readLen = tcpSession->read(ba);
			if (readLen == VERR_FAIL) goto _end;
			tcpSession->write("HTTP/1.0 200 Connection established\r\n\r\n");
		}

		SSL_set_accept_state(sslSession->con);
		while (true)
		{
			if (SSL_is_init_finished(sslSession->con)) break;
			int res = 0;
			{
				//VLock lock(sslAccept); // gilgil temp 2014.03.24
				res = SSL_accept(sslSession->con);
			}
			if (res < 0)
			{
				LOG_DEBUG("SSL_accept return %d error=%d", res, SSL_get_error(sslSession->con, res));
				goto _end;
			}
			else if (res == 0) // may be the TLS/SSL handshake was not successful
			{
				LOG_DEBUG("SSL_accept return zero");
				goto _end;
			} else // res > 0
			{
				break;
			}
		}
	}

	sslSessionList.lock();
	sslSessionList.push_back(sslSession);
	sslSessionList.unlock();

	emit runned(sslSession);

	sslSessionList.lock();
	sslSessionList.removeOne(sslSession);
	sslSessionList.unlock();

_end:
	// LOG_DEBUG("end sslSession=%p con=%p", sslSession, sslSession->con);  // gilgil temp 2014.03.14
	delete sslSession;
	tag = 4000; // gilgil temp 2014.04.04
}

void VSslServer::load(VXml xml)
{
	VTcpServer::load(xml);

	methodType            = xml.getInt("methodType", (int)methodType);
	certificatePath       = xml.getStr("certificatePath", certificatePath);
	defaultKeyCrtFileName = xml.getStr("defaultKeyCrtFileName", defaultKeyCrtFileName);
	processConnectMessage  = xml.getBool("processConnectMessage", processConnectMessage);
}

void VSslServer::save(VXml xml)
{
	VTcpServer::save(xml);

	xml.setInt("methodType", (int)methodType);
	xml.setStr("certificatePath", certificatePath);
	xml.setStr("defaultKeyCrtFileName", defaultKeyCrtFileName);
	xml.setBool("processConnectMessage", processConnectMessage);
}

#ifdef QT_GUI_LIB
void VSslServer::optionAddWidget(QLayout* layout)
{
	VTcpServer::optionAddWidget(layout);

	QStringList methodTypes; methodTypes << "mtNone" << "mtSSLv2" << "mtSSLv3" << "mtSSLv23" << "mtTLSv1" << "mtTLSv1_1" << "mtTLSv1_2" << "mtDTLSv1";
	VOptionable::addComboBox(layout, "cbxMethodType", "Method Type", methodTypes, (int)methodType, methodType.str());
	VOptionable::addLineEdit(layout, "leCertificatePath", "Certificate Path", certificatePath);
	VOptionable::addLineEdit(layout, "leDefaultKeyCrtFileName", "Default KeyCrtFileName", defaultKeyCrtFileName);
	VOptionable::addCheckBox(layout, "chkProcessConnectMessage", "Process Connect Message", processConnectMessage);
}

void VSslServer::optionSaveDlg(QDialog* dialog)
{
	VTcpServer::optionSaveDlg(dialog);

	methodType            = (VSslMethodType)(dialog->findChild<QComboBox*>("cbxMethodType")->currentIndex());
	certificatePath       = dialog->findChild<QLineEdit*>("leCertificatePath")->text();
	defaultKeyCrtFileName = dialog->findChild<QLineEdit*>("leDefaultKeyCrtFileName")->text();
	processConnectMessage  = dialog->findChild<QCheckBox*>("chkProcessConnectMessage")->checkState() == Qt::Checked;
}
#endif // QT_GUI_LIB
