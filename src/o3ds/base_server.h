#ifndef O3DS_BASE_SERVER
#define O3DS_BASE_SERVER

#include <string>
#include <vector>
#include "nng/nng.h"

#define NNG_ERROR(msg) if(ret != 0) { setError(msg, ret); return false;  }

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

		Connector() : mState(NOTSTARTED) {};

		virtual ~Connector()
		{
			nng_close(mSocket);
		}

		//! Starts the connector, often using nng_dial or nng_listen
		virtual bool start(const char* url) = 0;                  

		//! Write bytes - len is the size of data 
		virtual bool write(const char *data, size_t len) = 0;    

		//! Writes an nng message.  Len is the size of the data to write
		virtual bool writeMsg(const char *data, size_t len) = 0;  

		//! Read bytes - len is the fixed size of buffer, returns the number of bytes read
		virtual size_t read(char *data, size_t len) = 0;          

		//! Read an nng message - len is the size of buffer, returns the number of bytes read
		virtual size_t readMsg(char *data, size_t len) = 0;       

		//! Read an nng message, realloc and rezie *data if needed.
		//! If the buffer is reszied, len will be updated.
		//! Returns the size of the message, which may be less than the buffer size
		virtual size_t readMsg(char **data, size_t *len) = 0; 

		// Return the state (eState) of the connection
		enum eState getState() { return mState;  }

		// Get the last error
		const std::string& getError();

		// Set the error with nng return code
		void setError(const char *msg, int ret);

		// Set the error message
		void setError(const char* msg);


	protected:
		enum eState mState;
		std::string mError;
		nng_socket mSocket;
	};

	class BlockingConnector : public Connector
	{
	public:
		// Base class for blocking connectors
		virtual bool write(const char *data, size_t len);    //!< Write bytes - len is the size of teh data to write
		virtual size_t read(char *data, size_t len);         //!< Read bytes - len is the size of buffer, returns the number of bytes read
		virtual bool writeMsg(const char *data, size_t len); //!< Writes an nng message.  Len is the size of the data to write
		virtual size_t readMsg(char *data, size_t len);      //!< Read bytes - len is the size of buffer, returns the number of bytes read
		virtual size_t readMsg(char** data, size_t* len);
	};


	typedef void(*inDataFunc)(void *, void *, size_t);

	//! Bass class for async connectors
	/*! \class AsyncConnector base_server.h o3ds/base_server.h
	If the messages needs to be recieved, pass a callback to setFunc - this is optional  */
	class AsyncConnector : public Connector
	{
	public:
		AsyncConnector()
			: Connector()
			, fnContext(nullptr)
			, fnRef(nullptr)
			, aio(nullptr)
		{
			//nng_ctx_open(&ctx, mSocket);
		};

		virtual ~AsyncConnector()
		{
			//nng_dialer_close(mDialer);
			//if (aio)	nng_aio_free(aio);
		}

		virtual bool start(const char* url) = 0;              //!< Starts the server - servers will listen, clients will dial
		bool         write(const char *data, size_t len);     //!< Write bytes 
		size_t       read(char *data, size_t len);            //!< Read bytes - len is the size of data
		bool         writeMsg(const char *data, size_t len);  //!< Writes an nng message
		size_t       readMsg(char *data, size_t len);         //!< Reads an nng message - len is the size of data
		void         setFunc(void* ctx, inDataFunc f);        //!< User implemented callback to receive data (optional)
		bool         asyncReadMsg();  //!< handles a nng_recv_aio call.  Calls nng_recv_aio again if message was okay and returns true

	protected:
		void *     fnContext;  //!< The context provided for the user recieve callback
		inDataFunc fnRef;      //!< The user receive callback

		nng_dialer mDialer;
		nng_aio *aio;
		//nng_ctx  ctx;
	};
}

#endif