#ifndef O3DS_ASYNC_PUBLISHER_H
#define O3DS_ASYNC_PUBLISHER_H

#include <nng/nng.h>
#include <nng/protocol/pubsub0/pub.h>
#include "base_server.h"
#include <vector>

namespace O3DS
{
	class AsyncPublisher : public AsyncConnector
	{
	public:
		virtual bool start(const char* url);
	};
} // namespace O3DS

#endif

