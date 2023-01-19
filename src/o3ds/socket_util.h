/*
Open 3D Stream

Copyright 2022 Alastair Macleod

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef O3DS_SOCKET_UTIL_H
#define O3DS_SOCKET_UTIL_H

#include "socket_exception.h"


#ifdef _WIN32
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <Ws2tcpip.h>
#  include <winsock2.h>
#  include "Wininet.h"
#  define SOCKET_WOULDBLOCK WSAEWOULDBLOCK
#  define SOCKET_CONNRESET  WSAECONNRESET
#  define SOCKET_NULL       NULL
#  define CLOSESOCKET       closesocket
#  define SOCKLENTYPE       int
#else
#  include <sys/socket.h>
#  include <sys/types.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <netdb.h>
#  include <errno.h>
#  include <unistd.h>
#  include <fcntl.h>

#  define SOCKET             int
#  define SOCKET_WOULDBLOCK  EWOULDBLOCK
#  define SOCKET_NULL        0
#  define INVALID_SOCKET     -1
#  define SOCKET_ERROR       -1
#  define CLOSESOCKET        close
#  define SOCKADDR           sockaddr
#  define SOCKLENTYPE        socklen_t
#endif


namespace O3DS
{
	class Socket
	{
	public:
		Socket();
		Socket(SOCKET);
		virtual ~Socket();

		bool Valid() { return m_socket != SOCKET_NULL && m_socket != INVALID_SOCKET; }

		virtual void CreateSocket();

		void SetBlocking(bool block);
		int  GetError();
		void DestroySocket();

		//! Bind the server to a port. 
		/*! Throws a SocketException if bind fails
			This method is called by listen, so likely does not need to be called directly
		 */
		void     Bind(unsigned short port);

		//! The platform socket object
		SOCKET       m_socket;

		bool deleteOnClose;
	};

#ifdef WIN32
	static bool wsok = false;
	static WSADATA gWS;
	static void Cleanup() { WSACleanup(); }
#endif

}


#endif