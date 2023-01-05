#include "socket_address.h"

#include <string.h>

O3DS::SocketAddress::SocketAddress()
{
	memset(&m_address, 0, sizeof(m_address));
}

O3DS::SocketAddress::SocketAddress(SOCKET s)
{
	memset(&m_address, 0, sizeof(m_address));

	int l = sizeof(sockaddr_in);
#ifdef _WIN32
	getsockname(s, (sockaddr*)&m_address, &l);
#else
	getsockname(s, (sockaddr*)&m_address, (socklen_t*)&l);
#endif
}


O3DS::SocketAddress::SocketAddress(struct sockaddr_in val)
	: m_address(val)
{}

void O3DS::SocketAddress::Set(unsigned long addr, unsigned short port)
{
	m_address.sin_family = AF_INET;
	m_address.sin_addr.s_addr = addr;
	m_address.sin_port = htons(port);
}

void O3DS::SocketAddress::Clear()
{
	memset(&m_address, 0, sizeof(m_address));
}

std::string O3DS::SocketAddress::GetName()
{
	char buf[255];
	inet_ntop(AF_INET, (void*)&(m_address.sin_addr), buf, 255);
	return std::string(buf);
}