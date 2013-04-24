#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "tcptransport.h"
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <fstream>
#include <unistd.h>

using namespace std;

TCPTransport::TCPTransport():
m_iSocket(0),
m_bConnected(false)
{

}

TCPTransport::~TCPTransport()
{
   close();
}

int TCPTransport::open(int& port, bool /*rendezvous*/, bool reuseaddr)
{
   struct addrinfo hints, *local;

   memset(&hints, 0, sizeof(struct addrinfo));//把hints置零
   hints.ai_flags = AI_PASSIVE;	//Socket address is intended for `bind'.
   hints.ai_family = AF_INET;	
   hints.ai_socktype = SOCK_STREAM;

   stringstream service;
   service << port;

   if (0 != getaddrinfo(NULL, service.str().c_str(), &hints, &local)) //返回0成功 3为填的数据 4为返回结果
      return -1;

   m_iSocket = socket(local->ai_family, local->ai_socktype, local->ai_protocol);

   int reuse = reuseaddr;
   ::setsockopt(m_iSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));

   if (::bind(m_iSocket, local->ai_addr, local->ai_addrlen) < 0)
   {
      freeaddrinfo(local);
      return -1;
   }

   freeaddrinfo(local);

   return 0;
}

int TCPTransport::listen()
{
   return ::listen(m_iSocket, 1024);
}

TCPTransport* TCPTransport::accept(string& ip, int& port)
{
   TCPTransport* t = new TCPTransport;

   sockaddr_in addr;
   socklen_t addrlen = sizeof(sockaddr_in);
   if ((t->m_iSocket = ::accept(m_iSocket, (sockaddr*)&addr, &addrlen)) < 0)
   {
      delete t;
      return NULL;
   }

   char clienthost[NI_MAXHOST];
   char clientport[NI_MAXSERV];
   getnameinfo((sockaddr*)&addr, addrlen, clienthost, sizeof(clienthost), clientport, sizeof(clientport), NI_NUMERICHOST|NI_NUMERICSERV);

   ip = clienthost;
   port = atoi(clientport);

   t->m_bConnected = true;

   return t;
}

int TCPTransport::connect(const string& host, int port)
{
   if (m_bConnected)
      return 0;

   addrinfo hints, *peer;

   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_flags = AI_PASSIVE;
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;

   stringstream portstr;
   portstr << port;

   if (0 != getaddrinfo(host.c_str(), portstr.str().c_str(), &hints, &peer))
   {
      return -1;
   }

   if (::connect(m_iSocket, peer->ai_addr, peer->ai_addrlen) < 0)
   {
      freeaddrinfo(peer);
      return -1;
   }

   freeaddrinfo(peer);

   m_bConnected = true;

   return 0;
}

int TCPTransport::close()
{
   if (!m_bConnected)
      return 0;

   m_bConnected = false;

   return ::close(m_iSocket);
}

int TCPTransport::send(const char* data, int size)
{
   if (!m_bConnected)
   {
	   cout << "socket不存在" << endl;
		return -1;
   }
   int ts = size;
   while (ts > 0)
   {
      int s = ::send(m_iSocket, data + size - ts, ts, 0);
      if (s <= 0)
	  {
		  cout << "s:" << s << endl;
         return -1;
	  }
      ts -= s;
   }

   return size;
}

int TCPTransport::recv(char* data, int size)
{
   if (!m_bConnected)
   {
	   cout << "recv socket不存在" << endl;
      return -1;
   }

   int tr = size;
 /*  while (tr > 0)
   {*/
      int r = ::recv(m_iSocket, data + size - tr, tr, 0);
      if (r <= 0)
	  {
	//	  cout << "recv r:" << r << endl;
         return -1;
	  }
      tr -= r;
//	  cout << "tr:" << tr << endl;
  // }

   return size;
}

long long TCPTransport::sendfile(std::fstream& ifs, long long offset, long long size)
{
   if (!m_bConnected)
      return -1;

   if (ifs.bad() || ifs.fail())
      return -1;

   ifs.seekg(offset);

   int block = 1000000;
   char* buf = new char[block];
   long long sent = 0;
   while (sent < size)
   {
      int unit = int((size - sent) > block ? block : size - sent);
      ifs.read(buf, unit);
      send(buf, unit);
      sent += unit;
   }

   delete [] buf;

   return sent;
}

long long TCPTransport::recvfile(std::fstream& ofs, long long offset, long long size)
{
   if (!m_bConnected)
      return -1;

   if (ofs.bad() || ofs.fail())
      return -1;

   ofs.seekp(offset);

   int block = 1000000;
   char* buf = new char[block];
   long long recd = 0;
   while (recd < size)
   {
      int unit = int((size - recd) > block ? block : size - recd);
      recv(buf, unit);
      ofs.write(buf, unit);
      recd += unit;
   }

   delete [] buf;

   return recd;
}

bool TCPTransport::isConnected()
{
   return m_bConnected;
}

long long TCPTransport::getRealSndSpeed()
{
   return -1;
}

int TCPTransport::getLocalAddr(std::string& ip, int& port)
{
   sockaddr_in addr;
   socklen_t size = sizeof(sockaddr_in);

   if (::getsockname(m_iSocket, (sockaddr*)&addr, &size) < 0)
      return -1;

   char clienthost[NI_MAXHOST];
   char clientport[NI_MAXSERV];
   getnameinfo((sockaddr*)&addr, size, clienthost, sizeof(clienthost), clientport, sizeof(clientport), NI_NUMERICHOST|NI_NUMERICSERV);

   ip = clienthost;
   port = atoi(clientport);

   return 0;
}
