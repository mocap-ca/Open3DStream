#pragma once

#include <nng/nng.h>
#include "nng_connector.h"

#include <string>

namespace O3DS
{
	class Request : public BlockingNngConnector
	{
		// Request connector (blocking)
	public:
		bool start(const char *url);
	};

	class Reply : public BlockingNngConnector
	{
		// Reply connector (blocking)
	public:
		bool start(const char *url);
	};

}