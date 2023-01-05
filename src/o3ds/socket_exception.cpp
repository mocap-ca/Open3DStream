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

