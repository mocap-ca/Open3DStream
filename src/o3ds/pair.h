#pragma once

#include <nng/nng.h>
#include <nng/protocol/pair1/pair.h>
#include "nng_connector.h"

#include <string>

namespace O3DS
{
	class ClientPair : public BlockingNngConnector
	{
		// Pair connector (blocking)
	public:
		bool start(const char *url);
	};

	class ServerPair : public BlockingNngConnector
	{
		// Pair connector (blocking)
	public:
		bool start(const char *url);
	};

}