#ifndef O3DS_ASYNC_REQUEST
#define O3DS_ASYNC_REQUEST

#include <nng/nng.h>
#include <nng/protocol/reqrep0/req.h>
#include "base_server.h"


namespace O3DS
{
	class AsyncRequestReply : public AsyncConnector
	{
	public:
		static void Callback(void *ref) { ((AsyncRequestReply*)ref)->Callback_(); }
		void Callback_();
	};

	class AsyncRequest : public AsyncRequestReply
	{
	public:
		bool start(const char* url) override;
	};

	class AsyncReply : public AsyncRequestReply
	{
	public:
		bool start(const char* url) override;
	};
}

#endif