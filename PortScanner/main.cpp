#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

int main ( int argc, char **argv )
{
  if ( argc < 3 )
  {
    std::cerr << "Usage: " << argv[0] << "<ip> <port>" << std::endl;
    return 1;
  }

  std::string ipAddress{ argv[1] };
  int port{ std::atoi ( argv[2] ) };

  int createdSocket = socket ( AF_INET, SOCK_STREAM, 0 );

  if ( createdSocket < 0 )
  {
    std::cerr << "Error creating socket" << std::endl;
    return 1;
  }

  struct sockaddr_in serverAddress;

  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons ( port );

  if ( inet_pton ( serverAddress.sin_family, ipAddress.c_str (), &serverAddress.sin_addr ) <= 0 )
  {
    std::cerr << "Invalid IP address" << std::endl;
    close ( createdSocket );
    return 1;
  }

  if ( connect ( createdSocket, reinterpret_cast<sockaddr *> ( &serverAddress ), sizeof ( serverAddress ) ) == 0 )
  {
    std::cout << ipAddress << ":" << port << "is open";
    close ( createdSocket );
    return 0;
  }

  else
  {
    close ( createdSocket );
    std::cout << ipAddress << ":" << port << "isn't open";
  }

  return 0;
}
