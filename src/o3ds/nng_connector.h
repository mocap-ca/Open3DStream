#ifndef O3DS_NNG_CONNECTOR
#define O3DS_NNG_CONNECTOR

#include <string>
#include <vector>
#include <mutex>
#include "base_connector.h"
#include "nng/nng.h"

#define NNG_ERROR(msg) if(ret != 0) { setError(msg, ret); return false;  }

namespace O3DS
{
	class BlockingNngConnector : public Connector
	{
	public:

		virtual ~BlockingNngConnector();

		// Base class for blocking connectors
		virtual bool   write(const char* data, size_t len) override;  //!< Writes an nng message
		virtual size_t read(char* data, size_t len) override;         //!< Read bytes - len is the size of data
		virtual size_t read(char** data, size_t* len) override;       //!< Read bytes, resize data if needed

		// Set the error with nng return code
		void setError(const char* msg, int ret);
	protected:
		nng_socket mSocket;
		std::mutex  mutex;
	};


	

	//! Bass class for async nng connectors
	/*! \class AsyncConnector base_server.h o3ds/base_server.h
	If the messages needs to be recieved, pass a callback to setFunc - this is optional  */
	class AsyncNngConnector : public AsyncConnector
	{
	public:
		AsyncNngConnector()
			: AsyncConnector()
			, mDialer()
			, aio(nullptr)
			, mSocket(NNG_SOCKET_INITIALIZER)
		{
			nng_ctx_open(&ctx, mSocket);
		};

		virtual ~AsyncNngConnector()
		{
			stop();
			if (aio) nng_aio_free(aio);
		}

		// Set the error with nng return code
		void setError(const char* msg, int ret);

		virtual bool start(const char* url) = 0;              //!< Starts the server - servers will listen, clients will dial
		bool         write(const char* data, size_t len) override;  //!< Writes an nng message
		size_t       read(char* data, size_t len) override;            //!< Read bytes - len is the size of data
		size_t       read(char** data, size_t* len) override;       //!< Read bytes, resize data if needed
		bool         asyncReadMsg();                          //!< handles a nng_recv_aio call.  Calls nng_recv_aio again if message was okay and returns true
		void         stop();
	protected:
		nng_dialer mDialer;
		nng_aio* aio;

		nng_socket mSocket;
		std::mutex  mutex;
		nng_ctx  ctx;
	};
}

#endif  // O3DS_NNG_CONNECTOR