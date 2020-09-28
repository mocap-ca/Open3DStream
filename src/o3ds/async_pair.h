#ifndef O3DS_ASYNC_PAIR
#define O3DS_ASYNC_PAIR

#include <nng/nng.h>
#include <nng/protocol/pair1/pair.h>
#include "base_server.h"
#include <string>

namespace O3DS
{
	/*! \class AsyncPair async_pair.h o3ds/async_pair.h */
	//! Wrapper for nng pair pattern with asynchronous calls
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

	/*! \class AsyncPairClient async_pair.h o3ds/async_pair.h */
	//! Calls nng_dial to connect
	class AsyncPairClient : public AsyncPair
	{
	public:
		bool start(const char* url) override;
	};

	/*! \class AsyncPairServer async_pair.h o3ds/async_pair.h */
	//! Calls nng_listen to wait for a connection
	class AsyncPairServer : public AsyncPair
	{
	public:
		bool start(const char* url) override;
	};
}

#endif