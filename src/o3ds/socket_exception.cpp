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

#include "socket_exception.h"

#include <sstream>

#ifdef _WIN32
#include <Ws2tcpip.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#endif


O3DS::SocketException::SocketException(std::string s) : m_str(s), m_err(0)
{
};

O3DS::SocketException::SocketException(std::string s, int err) : m_str(s), m_err(err)
{
};

O3DS::SocketException::SocketException(std::string s, int err, std::string host, int port)
{
	std::ostringstream ss;
	ss << s << " host: " << host << ":" << port;
	m_str = ss.str();
	m_err = err;
};

std::string O3DS::SocketException::str()
{
	std::ostringstream ss;
	ss << m_str;

	if (m_err != 0)
	{
#ifdef _WIN32	
		char buf[1024];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, m_err, 0, buf, 1024, 0);
		ss << " - " << buf;
#else
		ss << " - " << strerror(m_err);
#endif
	}
	return ss.str();
}

