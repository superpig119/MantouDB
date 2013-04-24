// Implementation of the ServerSocket class

#include "ServerSocket.h"
#include "SocketException.h"
#include <iostream>

using namespace std;

ServerSocket::ServerSocket ( int port )
{
  if ( ! Socket::create() )
    {
		cout << this->m_sock << "cannot create" << endl;
      throw SocketException ( "Could not create server socket." );
    }

  if ( ! Socket::bind ( port ) )
    {
		cout << this->m_sock << "cannot bind" << endl;

      throw SocketException ( "Could not bind to port." );
    }

  if ( ! Socket::listen() )
    {
		cout << this->m_sock << "cannot listen" << endl;
      throw SocketException ( "Could not listen to socket." );
    }

}



ServerSocket::~ServerSocket()
{
}


const ServerSocket& ServerSocket::operator << ( const std::string& s ) const
{
  if ( ! Socket::send ( s ) )
    {
      throw SocketException ( "Could not write to socket." );
    }

  return *this;

}


const ServerSocket& ServerSocket::operator >> ( std::string& s ) const
{
  if ( ! Socket::recv ( s ) )
    {
      throw SocketException ( "Could not read from socket." );
    }

  return *this;
}

void ServerSocket::accept ( ServerSocket& sock )
{
  if ( ! Socket::accept ( sock ) )
    {
		cout << "cannot accept!" << endl;
      throw SocketException ( "Could not accept socket." );
    }
}
