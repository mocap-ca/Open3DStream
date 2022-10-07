#ifndef O3DS_ASYNC_REQUEST
#define O3DS_ASYNC_REQUEST

#include <nng/nng.h>
#include <nng/protocol/reqrep0/req.h>
#include "nng_connector.h"


namespace O3DS
{
	/*! \class AsyncRequestReply async_request.h o3ds/async_request.h
	Wraps [nng req0](https://nng.nanomsg.org/man/tip/nng_req.7.html) 
	pattern. Provides a common Callback function to get messages. */
	//! Base class for Request/Reply connection pattern.
	class AsyncRequestReply : public AsyncNngConnector
	{
	public:
		static void Callback(void *ref) { ((AsyncRequestReply*)ref)->Callback_(); }
		void Callback_();
	};

	//! Wrapper for nng req0 dialer
	class AsyncRequest : public AsyncRequestReply
	{
	public:
		bool start(const char* url) override;
	};

	//! Wrapper for nng req0 listener
	class AsyncReply : public AsyncRequestReply
	{
	public:
		bool start(const char* url) override;
	};
}

#endif