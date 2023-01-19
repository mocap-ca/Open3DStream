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