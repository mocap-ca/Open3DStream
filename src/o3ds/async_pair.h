#ifndef O3DS_ASYNC_PAIR
#define O3DS_ASYNC_PAIR

#include <nng/nng.h>
#include <nng/protocol/pair1/pair.h>
#include "base_server.h"
#include <string>

namespace O3DS
{
	class AsyncPair : public AsyncConnector
	{
	public:
		static void Callback(void *ref) { ((AsyncPair*)ref)->Callback_(); }
		void Callback_();

		nng_dialer mDialer;
		nng_aio *aio;
	};

	class AsyncPairClient : public AsyncPair
	{
	public:
		bool start(const char* url) override;
	};

	class AsyncPairServer : public AsyncPair
	{
	public:
		bool start(const char* url) override;
	};
}

#endif