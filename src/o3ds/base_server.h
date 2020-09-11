#ifndef O3DS_BASE_SERVER
#define O3DS_BASE_SERVER

#include <string>
#include "nng/nng.h"

namespace O3DS
{
	typedef void(*inDataFunc)(void *, void *, size_t);

	class BaseServer
	{
	public:
		BaseServer() 
			: fnContext(nullptr)
			, fnRef(nullptr) {};

		virtual ~BaseServer() {}

		virtual bool connect(const char* url) = 0;
		virtual bool listen(const char *url) = 0;
		virtual bool write(const char *data, size_t ptr) = 0;

		void setFunc(void* ctx, inDataFunc f)
		{ 
			fnContext = ctx;
			fnRef = f;
		}

		const std::string& getError()
		{ return mError;  }

		void setError(const char *msg, int ret)
		{
			mError = msg;
			mError += ": ";
			mError += nng_strerror(ret);
		}

	protected:
		void *     fnContext;
		inDataFunc fnRef;

	private:
		std::string mError;


	};
}

#endif