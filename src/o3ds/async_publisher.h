#ifndef O3DS_ASYNC_PUBLISHER_H
#define O3DS_ASYNC_PUBLISHER_H

#include <nng/nng.h>
#include <nng/protocol/pubsub0/pub.h>
#include "nng_connector.h"
#include <vector>

namespace O3DS
{
	//! Wrapper for nng pub0 - Asynchronous publish connection
	/*! \class Starts a server and listens for connections, publishes data to clients. */
	class AsyncPublisher : public AsyncNngConnector
	{
	public:
		virtual bool start(const char* url);
	};
} // namespace O3DS

#endif

