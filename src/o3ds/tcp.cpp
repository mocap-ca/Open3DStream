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

#include "tcp.h"


#include <sstream>

#ifdef _WIN32
#include <Ws2tcpip.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#endif

using namespace O3DS;


TcpSocket::TcpSocket()
	: Socket()
{
	CreateSocket();	
}

TcpSocket::TcpSocket(SOCKET s)
	: Socket(s)
{
}

TcpSocket::~TcpSocket()
{
}

void TcpSocket::CreateSocket()
{
	Socket::CreateSocket();

	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (m_socket == SOCKET_NULL || m_socket == INVALID_SOCKET)
		throw SocketException("Error creating socket: ", GetError());

#ifdef _OSX
	int set = 1;
	setsockopt(m_socket, SOL_SOCKET, SO_NOSIGPIPE, (void*)&set, sizeof(int));
#endif
}



#if defined(_WIN32) 
void TcpSocket::Connect(const char* host, unsigned short port)
{

	// This is a windows unicode specific implementation which uses 
	// GetAttrInfo to populate result with a number of possible results
	// for which each are tried to connect to.
	if (m_socket == SOCKET_NULL || m_socket == INVALID_SOCKET)
		throw SocketException("Invalid socket while trying to connect");

	std::ostringstream ossport;
	ossport << (int)port;

	ADDRINFOT hints;
	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	ADDRINFOT* result = NULL;

	if (GetAddrInfo(host, ossport.str().c_str(), &hints, &result) != 0)
		throw SocketException("Could not find host");

	ADDRINFOT* current = result;

	bool okay = false;

	while (1)
	{
		if (connect(m_socket, current->ai_addr, (int)current->ai_addrlen) != SOCKET_ERROR)
		{
			okay = true;
			break;
		}
		if (result->ai_next == NULL) break;
		current = result->ai_next;
	}

	if (!okay)
	{
		m_socket = INVALID_SOCKET;
		throw SocketException("Failed to connect", GetError(), host, port);
	}
	//	delete hp?
	return;

}

#endif



void TcpSocket::Connect(sockaddr_in* host, unsigned short port)
{
	if (m_socket == SOCKET_NULL || m_socket == INVALID_SOCKET)
		throw SocketException("Invalid socket while trying to connect");

	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = host->sin_addr.s_addr;
	clientService.sin_port = htons(port);

	if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR)
	{
		m_socket = INVALID_SOCKET;
		throw SocketException("Failed to connect", GetError());
	}
	return;
}




void TcpSocket::DisconnectSend()
{
	if (m_socket == SOCKET_NULL) return;

#ifdef _WIN32
	if (shutdown(m_socket, SD_SEND) == SOCKET_ERROR)
		throw SocketException("Failed to shut down send socket", GetError());
#else
	if (shutdown(m_socket, SHUT_WR) == -1)
		throw SocketException("Failed to shut down send socket", GetError());
#endif

}


void TcpSocket::DisconnectReceive()
{
	if (m_socket == SOCKET_NULL) return;

#ifdef _WIN32
	if (shutdown(m_socket, SD_RECEIVE) == SOCKET_ERROR)
		throw SocketException("Failed to shut down receive socket", GetError());
#else
	if (shutdown(m_socket, SHUT_RD) == -1)
		throw SocketException("Failed to shut down receive socket", GetError());
#endif

}


void TcpSocket::Disconnect()
{
	if (m_socket == SOCKET_NULL) return;
	//	WSAWaitForMultipleEvents(1, (const WSAEVENT*)&evt, 0, 3000, 1);
#if defined(_WIN32)
	if (shutdown(m_socket, SD_BOTH) == SOCKET_ERROR)
		throw SocketException("Failed to shut down socket", GetError());
#elif defined(_OSX)
	if (shutdown(m_socket, SHUT_RDWR) == -1)
	{
		int err = GetError();
		if (err != 57)
			throw SocketException("Failed to close socket", GetError());
	}
#else
	if (shutdown(m_socket, SHUT_RDWR) == -1)
		throw SocketException("Failed to close socket", GetError());

#endif
}



void TcpSocket::Listen(int port, bool nonBlocking, int maxConnections)
{
	if (nonBlocking)
	{
#ifdef _WIN32
		u_long arg = 1;
		ioctlsocket(m_socket, FIONBIO, &arg);
#else
		int x;
		x = fcntl(m_socket, F_GETFL, 0);
		fcntl(m_socket, F_SETFL, x | O_NONBLOCK);
#endif
	}

	this->Bind(port);

	// listen
	if (listen(m_socket, maxConnections) == SOCKET_ERROR)
	{
		throw SocketException("Listen failed", GetError(), "inaddr_any", port);
	}

	return;
}


bool TcpSocket::Accept(SOCKET& new_sock, int timeout)
{
	fd_set fd;
	struct timeval tv;
	int retval;

	if (timeout > 0)
	{
		FD_ZERO(&fd);
		FD_SET(m_socket, &fd);

		tv.tv_sec = timeout;
		tv.tv_usec = 0;
		retval = select(m_socket + 1, &fd, NULL, NULL, &tv);

		if (retval != 1) return false;
	}

	new_sock = accept(m_socket, 0, 0);
	if (new_sock == INVALID_SOCKET)
	{
		if (GetError() == SOCKET_WOULDBLOCK) return false;
		throw SocketException("Could not accept", GetError());
	}

	return true;
}




void TcpSocket::Send(std::vector<char>& data)
{
	Send((void*)data.data(), (int)data.size());
}

void TcpSocket::Send(void* data, int len)
{
	std::ostringstream ss;
	int bytesSent = 0;

	if (m_socket == SOCKET_NULL)
		throw(SocketException("Error trying to send data while disconnected", GetError()));

	//bytesSent = send( m_socket, (char*)data, len, MSG_NOSIGNAL );
	bytesSent = send(m_socket, (char*)data, len, 0);

	if (bytesSent == SOCKET_ERROR)
	{
		int err = GetError();
		if (err == SOCKET_WOULDBLOCK)
			return;

		throw SocketException("Sending", err);
	}

	if (bytesSent != len)
	{
		ss << "Error not all data was sent: " << bytesSent << " of " << len;
		throw SocketException(ss.str(), GetError());
	}
}



void TcpSocket::Receive(void* buf, int len, int* recd)
{
	int bytesRecv = 0;
	bytesRecv = recv(m_socket, (char*)buf, len, 0);

	if (bytesRecv == 0)
		throw SocketException("Connection Closed");

	if (bytesRecv == SOCKET_ERROR)
	{
		int err = GetError();
		if (err == SOCKET_WOULDBLOCK) return;
		throw SocketException("Reading", err);
	}

	if (recd != NULL) *recd = bytesRecv;
}

void TcpSocket::Receive(void* buf, int len)
{
	int count = 0;
	int recd = 0;

	while (count < len)
	{
		Receive((char*)buf + count, len - count, &recd);
		count += recd;
	}
}

bool TcpSocket::Receive(std::vector<char>& data)
{
	char    c[10240];
	int     bytesRecv = 0;
	time_t  startTime = time(NULL);

	while (bytesRecv >= 0 && difftime(time(NULL), startTime) < 30)
	{
		Receive((void*)c, (10240)-1, &bytesRecv);

		if (bytesRecv > 0)
		{
			std::copy(c, c + bytesRecv, data.begin());
		}
	}

	if (data.size() > 0)
		return true;

	return false;
}


bool TcpSocket::ReceiveToToken(void* data, int len, const char token, int* bytesRead)
{

	char    c;
	char* ptr = (char*)data;
	int     bytesRecv = 0;
	if (len == 0) throw SocketException("Invalid data length while receiving token");

	while (bytesRecv >= 0)// && difftime(time(NULL),startTime) < TIMEOUT )
	{
		Receive((void*)&c, 1, &bytesRecv);

		if (bytesRecv > 0)
		{
			*ptr = c;
			ptr++;
			if (ptr - (char*)data == len) throw SocketException("Overflow while receiving token");
			if (c == token)
			{
				*bytesRead = (int)(ptr - (char*)data);
				return true;
			}
		}
	}

	return false;
}
