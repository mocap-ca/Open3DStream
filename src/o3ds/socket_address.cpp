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