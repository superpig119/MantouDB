#include "transport.h"
#include <string>

class TCPTransport: public Transport
{
public:
   TCPTransport();
   virtual ~TCPTransport();

public:
   virtual int open(int& port, bool rendezvous = false, bool reuseaddr = false);

   virtual int listen();
   virtual TCPTransport* accept(std::string& ip, int& port);
   virtual int connect(const std::string& ip, int port);
   virtual int close();

   virtual int send(const char* data, int size);
   virtual int recv(char* data, int size);
   virtual long long sendfile(std::fstream& ifs, long long offset, long long size);
   virtual long long  recvfile(std::fstream& ofs, long long offset, long long size);

   virtual bool isConnected();
   virtual long long getRealSndSpeed();
   virtual int getLocalAddr(std::string& ip, int& port);

private:
#ifndef WIN32
   int m_iSocket;
#else
   SOCKET m_iSocket;
#endif
   bool m_bConnected;
};


