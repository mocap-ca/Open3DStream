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

#ifndef O3DS_TCP_H
#define O3DS_TCP_H

#include <string>
#include <vector>

#include "socket_exception.h"
#include "socket_util.h"


namespace O3DS
{
	class TcpSocket : public Socket
	{
	public:
		TcpSocket();
		TcpSocket(SOCKET s);
		~TcpSocket();

		// Socket Creation/Destruction
		
		//! Calls socket() - throws SocketException
		virtual void CreateSocket() override;

		//! open a connection to the host/port
		void     Connect(const char* host, unsigned short port);

		//! open a connection to the host/port
		void     Connect(sockaddr_in* host, unsigned short port);

		void     Disconnect();         //!< shut down a connection
		void     DisconnectSend();     //!< shut down a connection for sending data
		void     DisconnectReceive();  //!< shut down a connection for reading data

		//! Listen for a connection.
		/*! Set the socket options for blocking, calles Bind then Listen.
			If blocking is false, function should return right away
			Throws a SocketException if Bind or Listen fails */
		void     Listen(int port, bool nonBlocking, int maxConnections);

		//! Accept an incoming connection.  
		/*! Will poll the listening socket and wait for one second for an incoming connection.  The
			reason for doing this rather than using a blocking socket is on unix, the accept will
			not unblock when the socket is destoryed, so putting this in a thread and checking the
			shutdown status is required.
		*/
		//! \retval true if a connection was made
		bool     Accept(SOCKET& s, int timeout = 0);

		//! Puts data on the wire.
		void     Send(std::vector<char>& data);  

		//! Puts data on the wire
		void     Send(void* data, int len);  

		//! Receive data
		/*! Will keep reading in data till TIMEOUT has been exceeded or connection is closed. */
		bool     Receive(std::vector<char>& data);

		//! Receive a specific length of data
		void     Receive(void* buf, int len, int* recd);

		//! Recieve a specific length of data, and keep going till its all read.
		void     Receive(void* buf, int len);

		//! Receive everything up to a token
		/*! Will keep reading in data till TIMEOUT has been exceed or the token is read */
		bool     ReceiveToToken(void* data, int len, const char token, int* bytesRead = NULL);



	};

}



#endif