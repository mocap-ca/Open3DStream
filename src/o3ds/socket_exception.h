#ifndef O3DS_SOCKET_EXCEPTION_H
#define O3DS_SOCKET_EXCEPTION_H

#include <string>

namespace O3DS
{

	//! Thrown by Sockets
	class SocketException
	{
	public:
		//! Create a socket error without an error code from std::string message
		SocketException(std::string s);

		//! Create a socket error object with error code
		SocketException(std::string s, int err);

		//! Create a socket error object including host and port data
		SocketException(std::string s, int err, std::string host, int port);

		//! Destory
		inline ~SocketException() {};

		//! Return the error as a string
		std::string str();

		//! Return m_str
		std::string msg() { return m_str; }

		//! return m_err
		int error() { return m_err; }


	private:
		std::string m_str; //!< The error string
		int         m_err; //!< The error code
	};

}


#endif // O3DS_SOCKET_EXCEPTION_H