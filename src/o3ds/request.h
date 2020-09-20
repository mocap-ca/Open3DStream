#pragma once

#include <nng/nng.h>
#include "base_server.h"

#include <string>

namespace O3DS
{
	class Request : public BlockingConnector
	{
		// Request connector (blocking)
	public:
		bool start(const char *url);
	};

	class Reply : public BlockingConnector
	{
		// Reply connector (blocking)
	public:
		bool start(const char *url);
	};

}