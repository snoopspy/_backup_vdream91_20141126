#include <VNet>
#include <VDebugNew>

#ifdef linux
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif // linux

// ----------------------------------------------------------------------------
// Ip
// ----------------------------------------------------------------------------
Ip::Ip(const QString s)
{
  this->value = ntohl(inet_addr(qPrintable(s)));
}

void Ip::clear()
{
  value = 0;
}

QString Ip::str()
{
  quint32 _temp = htonl(value);
  QString res = inet_ntoa(*((in_addr*)&_temp));
  return res;
}

QString Ip::qformat(QString format)
{
  return ::qformat(qPrintable(format), value);
}

// ----------------------------------------------------------------------------
// VNetInstance
// ----------------------------------------------------------------------------
VNetInstance::VNetInstance()
{
  #ifdef WIN32
    WSADATA wsaData;
    WSAStartup(0x0202, &wsaData);
  #endif // WIN32
}

VNetInstance::~VNetInstance()
{
  #ifdef WIN32
    //
    // If WSACleanup is called with QT IDE debugging, the following exception occurs.
    // So do not call WSACleanup.
    // ---------------------------
    // Exception Triggered
    // ---------------------------
    // <p>The inferior stopped because it triggered an exception.<p>Stopped in thread 0 by:
    // Exception at 0x75aad09c, code: 0xc0000005: read access violation at: 0x0, flags=0x0.
    //
    // WSACleanup();
  #endif // WIN32
}

VNetInstance& VNetInstance::instance()
{
  static VNetInstance g_instance;
  return  g_instance;
}

// ----------------------------------------------------------------------------
// VNet
// ----------------------------------------------------------------------------
VNet::VNet(void* owner) : VRwObject(owner)
{
  VNetInstance::instance();
}

VNet::~VNet()
{
}

bool VNet::isIPV4Address(const QString host)
{
  int i, index;
  bool bDigitOK;
  int nDigit;

  int len = (int)host.length();
  if (len == 0)
    goto _false;
  i = 0;
  for (index = 1; index <= 4; index++)
  {
    bDigitOK = false;
    nDigit = 0;
    while (i < len)
    {
      char ch = host.at(i).toLatin1();
      if (host[i] < '0' || ch > '9')
        break;
      bDigitOK = true;
      nDigit = nDigit * 10 + (ch - '0');
      i++;
      if (nDigit > 255)
        goto _false;
    }
    if (!bDigitOK)
      goto _false;
    if (nDigit > 255)
      goto _false;
    if (index < 4)
    {
      if (host[i] != '.')
      {
        goto _false;
      }
      i++; // skip '.'
    } else {
      if (i != len)
      {
        goto _false;
      }
    }
  }
  return true;
_false:
  return false;
}

Ip VNet::resolve(QString host)
{
  hostent *pHostEnt;
  char *pAddr;
  struct in_addr addr;
  int lastError;

  if (isIPV4Address(host))
  {
    return Ip(host);
  }

  pHostEnt = gethostbyname(qPrintable(host));
  if (pHostEnt == NULL)
  {
    goto _error;
  }
  pAddr = *pHostEnt->h_addr_list;
  if (pAddr == NULL)
  {
    goto _error;
  }

  //addr.s_addr = ntohl(*((uint32_t*)pAddr));
#ifdef WIN32
  addr.s_addr = *((ULONG*)pAddr);
#endif // WIN32
#ifdef linux
  addr.s_addr = *((in_addr_t*)pAddr);
#endif // linux
  return ntohl(*((Ip*)&addr));
_error:
  lastError = GetLastError();
  {
    LOG_ERROR("[VNet.cpp] -------------------------------------");
    LOG_ERROR("[VNet.cpp] VNet::resolve host=%s GetLastError=%d", qPrintable(host), lastError);
    LOG_ERROR("[VNet.cpp] -------------------------------------");
  }
  return quint32(0);
}

#ifdef GTEST
#include <gtest/gtest.h>

TEST( Net, ipTest )
{
  Ip ip = (QString)"127.0.0.1";
  QString s1 = ip.str();
  QString s2 = "127.0.0.1";
  EXPECT_EQ( s1, s2 );
}

TEST( Net, isIPV4AddressTest )
{
  EXPECT_TRUE( VNet::isIPV4Address("1.2.3.4") );
  EXPECT_FALSE( VNet::isIPV4Address("1.2.3.4.5") );
  EXPECT_FALSE( VNet::isIPV4Address("www.google.com") );
  EXPECT_FALSE( VNet::isIPV4Address("www.gilgil.net") );
}

class MyNet : public VNet // to initialize VNet
{
protected:
  virtual bool doOpen()  { return true; }
  virtual bool doClose() { return true; }
  virtual int  doRead(char* buf,  int size) {  Q_UNUSED(buf) Q_UNUSED(size) return 0; }
  virtual int  doWrite(char* buf, int size) {  Q_UNUSED(buf) Q_UNUSED(size) return 0; }
};

TEST( Net, resolveTest )
{
  MyNet myNet;
  {
    Ip ip = (quint32)0x7F000001;
    //Ip ip = VNet::resolve("127.0.0.1");
    LOG_DEBUG("ip for 127.0.0.1 is %s", qPrintable(ip.str()));
    EXPECT_EQ( ip, 0x7F000001 );
  }

  {
    Ip ip = VNet::resolve("www.google.com");
    LOG_DEBUG("ip for www.google.com is %s", qPrintable(ip.str()));
    EXPECT_TRUE( ip != 0 );
  }
}

#endif // GTEST
