#pragma once

#include <nng/nng.h>
#include <nng/protocol/pair1/pair.h>
#include "base_server.h"

#include <string>

namespace O3DS
{
	class ClientPair : public BlockingConnector
	{
		// Pair connector (blocking)
	public:
		bool start(const char *url);
	};

	class ServerPair : public BlockingConnector
	{
		// Pair connector (blocking)
	public:
		bool start(const char *url);
	};

}