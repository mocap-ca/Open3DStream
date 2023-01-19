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

#ifndef O3DS_UDP_SOCKET_H
#define O3DS_UDP_SOCKET_H

#include "socket_exception.h"
#include "socket_util.h"
#include "socket_address.h"

namespace O3DS
{
	class UdpSocket : public Socket
	{
	public:
		UdpSocket();
		UdpSocket(SOCKET s);
		~UdpSocket();

		virtual void CreateSocket() override;

		//! Set broadcast destination - stored in m_sendDest
		/*! Socket must be UDP */
		void     SetDestination(const char*, unsigned short port);

		//! Set broadcast destination - stored in m_sendDest
		/*! Socket must be UDP */
		void     SetDestination(struct sockaddr_in& in, unsigned short port);

		//! Send a datagram to the destination specified by SetDestination()
		/*! uses m_sendDest as desination
			Socket must be UDP
		*/
		void     SendTo(const void* data, size_t len);


		size_t GetDatagram(char* buf, int len);

		SocketAddress m_sendDest;

		SocketAddress m_from;

	};
}

#endif