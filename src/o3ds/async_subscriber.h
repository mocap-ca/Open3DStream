#ifndef O3DS_ASYNC_SUBSCRIBER_H
#define O3DS_ASYNC_SUBSCRIBER_H

#include <nng/nng.h>
#include <nng/protocol/pubsub0/sub.h>
#include <nng/supplemental/util/platform.h>
#include "nng_connector.h"
#include <string>

namespace O3DS
{
	// The client pulls data down from a listen server
	class AsyncSubscriber : public AsyncNngConnector
	{
	public:

		bool start(const char*url);

		void callback_()
		{
			AsyncNngConnector::asyncReadMsg();
		}
		static void callback(void *ref)
		{
			((AsyncSubscriber*)ref)->callback_();
		}
		static void pipeEvent(nng_pipe pipe, nng_pipe_ev pipe_ev, void* ref)
		{
			((AsyncSubscriber*)ref)->pipeEvent_(pipe, pipe_ev);
		}
		void pipeEvent_(nng_pipe pipe, nng_pipe_ev pipe_ev);

		bool listen(const char* url) { return false; }

		bool write(const char* data, size_t ptr) { return false; }

		//virtual void in_pipe() = 0;

	};


}


#endif