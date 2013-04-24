// Definition of the ClientSocket class

#ifndef ClientSocket_class
#define ClientSocket_class

#include "Socket.h"
#include <iostream>

using namespace std;

class ClientSocket : public Socket
{
 public:

  ClientSocket( std::string host, int port );
  ClientSocket(){};
	
  virtual ~ClientSocket(){};

  const ClientSocket& operator << ( const std::string& ) const;
  const ClientSocket& operator >> ( std::string& ) const;

};


#endif
