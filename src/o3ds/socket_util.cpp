#include "socket_util.h"

WSADATA gWS;

#include "socket_exception.h"

O3DS::Socket::Socket()
	: m_socket(SOCKET_NULL)
	, deleteOnClose(true)
{}

O3DS::Socket::Socket(SOCKET s)
	: m_socket(s)
	, deleteOnClose(false)
{
}

O3DS::Socket::~Socket()
{
	if (!deleteOnClose) return;
	try
	{
		if (m_socket != SOCKET_NULL && m_socket != INVALID_SOCKET)
			DestroySocket();
	}
	catch (SocketException e)
	{
	}
}

void O3DS::Socket::CreateSocket()
{
#ifdef _WIN32
	if (!wsok)
	{
		WSAStartup(MAKEWORD(2, 2), &gWS);
		wsok = true;
	}
#endif

	if (m_socket != SOCKET_NULL && m_socket != INVALID_SOCKET)
		DestroySocket();

}


void O3DS::Socket::Bind(unsigned short port)
{
	struct sockaddr_in service;
	memset(&service, 0, sizeof(sockaddr_in));
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = INADDR_ANY;
	service.sin_port = htons(port);

	// bind the socket to the port
	if (bind(m_socket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
	{
		throw SocketException("Bind to port failed.", GetError(), "inaddr_any", port);
	}
}

void O3DS::Socket::SetBlocking(bool block)
{
#ifdef _WIN32
	u_long arg;
	if (block)
		arg = 0;
	else
		arg = 1;
	ioctlsocket(m_socket, FIONBIO, &arg);
#else
	int x;
	x = fcntl(m_socket, F_GETFL, 0);
	if (block)
		fcntl(m_socket, F_SETFL, x &= ~O_NONBLOCK);
	else
		fcntl(m_socket, F_SETFL, x | O_NONBLOCK);
#endif
	/*
		int nonblocking = block ? 0 : 1;
		if (ioctl(s, FIONBIO, &on) < 0) {
			throw SocketException("Could not set socket blocking mode");
	*/

}

int O3DS::Socket::GetError()
{
#ifdef _WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}

void O3DS::Socket::DestroySocket()
{
	if (m_socket == SOCKET_NULL)     throw SocketException("Attempting to close null socket");
	if (m_socket == INVALID_SOCKET)  throw SocketException("Attempting to close invalid socket");
	if (CLOSESOCKET(m_socket) != 0)  throw SocketException("Could not close socket", GetError());
	m_socket = SOCKET_NULL;
}




