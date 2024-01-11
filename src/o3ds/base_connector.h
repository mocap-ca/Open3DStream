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

#ifndef O3DS_BASE_CONNECTOR
#define O3DS_BASE_CONNECTOR

#include <string>
#include <vector>
#include <mutex>
#include "nng/nng.h"

namespace O3DS
{
	//! Abstract base class for all connector
	/*! \class Connector base_server.h o3ds/base_server.h
	The base server defines common methods for all servers and allows different server types to
	be easily swapped out.  The base server holds a nng_socket object and string error.
	*/
	class Connector
	{
	public:
		enum eState { NOTSTARTED, STARTED, READING, STATE_ERROR, CLOSED };

		Connector();

		virtual ~Connector();

		//! Starts the connector, often using nng_dial or nng_listen
		virtual bool start(const char* url) = 0;

		//! Write bytes - len is the size of data 
		virtual bool write(const char* data, size_t len) = 0;

		//! Read a message, realloc and rezie *data if needed.
		//! If the buffer is reszied, len will be updated.
		//! Returns the size of the message, which may be less than the buffer size
		virtual size_t read(char** data, size_t *len) = 0;

		// Return the state (eState) of the connection
		enum eState getState() { return mState; }

		// Get the last error
		const std::string& getError();

		// Set the error message
		void setError(const char* msg);


	protected:
		enum eState mState;
		std::string mError;
	};

	typedef void(*InDataFunc)(void*, void*, size_t);

	class AsyncConnector : public Connector
	{
	public:
		void setFunc(void* ctx, InDataFunc f);        //!< User implemented callback to receive data (optional)

		virtual void stop() = 0;
		InDataFunc mInDataFunc;
		void*      mContext;
	};
}

#endif  // O3DS_BASE_CONNECTOR
