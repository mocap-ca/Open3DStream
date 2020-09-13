#ifndef O3DS_ASYNC_PUBLISHER_H
#define O3DS_ASYNC_PUBLISHER_H

#include <nng/nng.h>
#include <nng/protocol/pubsub0/pub.h>
#include <nng/protocol/pubsub0/sub.h>
#include "base_server.h"
#include <vector>

namespace O3DS
{
	class Publisher : public BlockingConnector
	{
	public:
		Publisher();
		~Publisher() { nng_close(mSocket); }
		bool start(const char*url);

	};
} // namespace O3DS

#endif

