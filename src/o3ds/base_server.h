#ifndef O3DS_BASE_SERVER
#define O3DS_BASE_SERVER

#include <string>
#include "nng/nng.h"

namespace O3DS
{
	class Connector
	{
	public:
		virtual ~Connector() { nng_close(mSocket); }
		// Base class for all servers.  Has  a nng_socket and error handling.
	public:

		virtual bool start(const char* url) = 0;
		virtual bool write(const char *data, size_t len) = 0;
		virtual size_t read(char *data, size_t len) = 0;


		const std::string& getError()
		{
			return mError;
		}

		void setError(const char *msg, int ret)
		{
			mError = msg;
			mError += ": ";
			mError += nng_strerror(ret);
		}

		std::string err() { return mError;  }

	protected:
		std::string mError;
		nng_socket mSocket;
	};

	class BlockingConnector : public Connector
	{
	public:
		// Base class for blocking connectors
		virtual bool write(const char *data, size_t len)
		{
			int ret;
			ret = nng_send(mSocket, (void*)data, len, 0);
			if (ret != 0)
			{
				setError("Sending data", ret);
				return false;
			}

			return true;

		}
		virtual size_t read(char *data, size_t len)  // Read bytes - len is the size of data
		{
			size_t sz = len;
			int ret = nng_recv(mSocket, data, &sz, 0);
			if (ret != 0) {
				setError("Error reading", ret);
				return false;
			}

			return sz;
		}

	};


	typedef void(*inDataFunc)(void *, void *, size_t);

	class AsyncConnector : public Connector
	{
		// Bass class for async connectors
		// If the messages needs to be recieved, pass a callback to setFunc - this is optional
	public:
		AsyncConnector()
			: fnContext(nullptr)
			, fnRef(nullptr) {};

		virtual bool start(const char* url) = 0;  // Starts the server - servers will listen, clients will dial

		bool write(const char *data, size_t len)
		{
			int ret;
			ret = nng_send(mSocket, (void*)data, len, NNG_FLAG_NONBLOCK);
			if (ret != 0) 
			{
				setError("Sending data", ret);
				return false;
			}
			return true;
		}

		virtual size_t read(char *data, size_t len)  // Read bytes - len is the size of data
		{
			size_t sz = len;
			int ret = nng_recv(mSocket, data, &sz, NNG_FLAG_NONBLOCK);
			if (ret == NNG_EAGAIN)
			{
				return 0;
			}
			if (ret != 0) 
			{
				setError("Error reading", ret);
				return 0;
			}

			return sz;
		}

		void setFunc(void* ctx, inDataFunc f)  // User implemented callback to receive data (optional)
		{ 
			fnContext = ctx;
			fnRef = f;
		}

	protected:
		void *     fnContext;  // The context provided for the user recieve callback
		inDataFunc fnRef;      // The user receive callback

		nng_dialer mDialer;
		nng_aio *aio;

	};
}

#endif