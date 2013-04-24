#include <string>
//#include "udt/udt.h"


struct TransOption
{
   // TODO: put rendzevous, reuseaddr, timeout, etc in this structure.
};

class Transport
{
public:
   virtual ~Transport() {};

public:
   virtual int open(int& port, bool rendezvous, bool reuseaddr) = 0;

   virtual int listen() = 0;
   virtual Transport* accept(std::string& ip, int& port) = 0;
   virtual int connect(const std::string& ip, int port) = 0;
   virtual int close() = 0;

   virtual int send(const char* buf, int size) = 0;
   virtual int recv(char* buf, int size) = 0;
   virtual long long sendfile(std::fstream& ifs, long long  offset, long long size) = 0;
   virtual long long recvfile(std::fstream& ofs, long long  offset, long long size) = 0;

   virtual bool isConnected() = 0;
   virtual long long getRealSndSpeed() = 0;
   virtual int getLocalAddr(std::string& ip, int& port) = 0;

   //virtual int setOption(const TransOption& opt) { return 0; }
};


