#ifndef O3DS_PIPELINE_H
#define O3DS_PIPELINE_H

#include <nng/nng.h>
#include "base_server.h"

#include <string>

namespace O3DS
{
	class Push : public BlockingConnector
	{
		// Request connector (blocking)
	public:
		bool start(const char *url);
	};

	class Pull : public BlockingConnector
	{
		// Reply connector (blocking)
	public:
		bool start(const char *url);
	};
}

#endif