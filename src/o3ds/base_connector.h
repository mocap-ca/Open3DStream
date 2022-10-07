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

		//! Read bytes - len is the fixed size of buffer, returns the number of bytes read
		virtual size_t read(char* data, size_t len) = 0;

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