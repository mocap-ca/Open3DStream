#ifndef O3DS_ASYNC_SUBSCRIBER_H
#define O3DS_ASYNC_SUBSCRIBER_H

#include <nng/nng.h>
#include <nng/protocol/pubsub0/sub.h>
#include <nng/supplemental/util/platform.h>
#include "base_server.h"
#include <string>

namespace O3DS
{
	// The client pulls data down from a listen server
	class AsyncSubscriber : public AsyncConnector
	{
	public:
		AsyncSubscriber();
		virtual ~AsyncSubscriber() { nng_close(mSocket); }
		bool start(const char*url);

		static void Callback(void *ref) { ((AsyncSubscriber*)ref)->Callback_(); }
		void Callback_();
		static void PipeEvent(nng_pipe pipe, nng_pipe_ev pipe_ev, void* ref)
		{
			((AsyncSubscriber*)ref)->PipeEvent_(pipe, pipe_ev);
		}
		void PipeEvent_(nng_pipe pipe, nng_pipe_ev pipe_ev);

		bool listen(const char* url) { return false; }

		bool write(const char* data, size_t ptr) { return false; }

		//virtual void in_pipe() = 0;

		std::string mError;

	};


}


#endif