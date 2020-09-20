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
		void callback_()
		{
			AsyncConnector::asyncReadMsg();
		}
		static void callback(void *ref)
		{
			((AsyncPair*)ref)->callback_();
		}
		void Callback_();
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