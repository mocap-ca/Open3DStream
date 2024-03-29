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

#include "udp.h"

O3DS::UdpSocket::UdpSocket()
	: Socket()
{}


O3DS::UdpSocket::UdpSocket(SOCKET s)
	: Socket(s)
{}

O3DS::UdpSocket::~UdpSocket()
{}

void O3DS::UdpSocket::CreateSocket()
{
	Socket::CreateSocket();

	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (m_socket == SOCKET_NULL || m_socket == INVALID_SOCKET)
		throw SocketException("Error creating socket: ", GetError());

}

void O3DS::UdpSocket::SetDestination(const char* ip, unsigned short port)
{
	unsigned long addrBuf;
	inet_pton(AF_INET, ip, &addrBuf);
	m_sendDest.Set(addrBuf, port);
}

void O3DS::UdpSocket::SetDestination(struct sockaddr_in& in, unsigned short port)
{
	m_sendDest.Set(in.sin_addr.s_addr, port);
}


void O3DS::UdpSocket::SendTo(const void* data, size_t len)
{
	size_t ret = sendto(m_socket, (const char*)data, (int)len, 0,
		(struct sockaddr*)&m_sendDest.m_address,
		(int)sizeof(m_sendDest.m_address));

	if (ret == (size_t)-1) throw SocketException("Error sending broadcast message", GetError());
	else if (ret != len)   throw SocketException("Not all bytes were sent during broadcast message", GetError());
}

size_t O3DS::UdpSocket::GetDatagram(char* buf, int len)
{
	socklen_t l = sizeof(struct sockaddr_in);
	size_t ret = recvfrom(m_socket, buf, len, 0, m_from, &l);
	if (ret == (size_t)-1) throw SocketException("Error getting datagram", errno);
	buf[ret] = '\0';

	return ret;

}