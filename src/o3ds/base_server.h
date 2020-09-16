#ifndef O3DS_BASE_SERVER
#define O3DS_BASE_SERVER

#include <string>
#include "nng/nng.h"

namespace O3DS
{
	class Connector
	{
	public:
		virtual ~Connector()
		{
			nng_close(mSocket);
		}
		// Base class for all servers.  Has  a nng_socket and error handling.
	public:

		virtual bool start(const char* url) = 0;
		virtual bool write(const char *data, size_t len) = 0;
		virtual bool writeMsg(const char *data, size_t len) = 0;
		virtual size_t read(char *data, size_t len) = 0;
		virtual size_t readMsg(char *data, size_t len) = 0;

		const std::string& getError();

		void setError(const char *msg, int ret);
		std::string err() { return mError;  }

	protected:
		std::string mError;
		nng_socket mSocket;
	};

	class BlockingConnector : public Connector
	{
	public:
		// Base class for blocking connectors
		virtual bool write(const char *data, size_t len);
		virtual size_t read(char *data, size_t len);  // Read bytes - len is the size of data
		virtual bool writeMsg(const char *data, size_t len);
		virtual size_t readMsg(char *data, size_t len);  // Read bytes - len is the size of data
	};


	typedef void(*inDataFunc)(void *, void *, size_t);

	class AsyncConnector : public Connector
	{
		// Bass class for async connectors
		// If the messages needs to be recieved, pass a callback to setFunc - this is optional
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

		virtual bool start(const char* url) = 0;  // Starts the server - servers will listen, clients will dial

		bool write(const char *data, size_t len);
		size_t read(char *data, size_t len); // Read bytes - len is the size of data
		bool writeMsg(const char *data, size_t len);
		size_t readMsg(char *data, size_t len); // Read bytes - len is the size of data

		void setFunc(void* ctx, inDataFunc f); // User implemented callback to receive data (optional)

		void asyncReadMsg();


	protected:
		void *     fnContext;  // The context provided for the user recieve callback
		inDataFunc fnRef;      // The user receive callback

		nng_dialer mDialer;
		nng_aio *aio;
		//nng_ctx  ctx;

	};
}

#endif