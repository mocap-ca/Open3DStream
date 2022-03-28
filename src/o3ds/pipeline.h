#ifndef O3DS_PIPELINE_H
#define O3DS_PIPELINE_H

#include <nng/nng.h>
#include "nng_connector.h"

#include <string>

namespace O3DS
{
	class Push : public BlockingNngConnector
	{
		// Request connector (blocking)
	public:
		bool start(const char *url);
	};

	class Pull : public BlockingNngConnector
	{
		// Reply connector (blocking)
	public:
		bool start(const char *url);
	};
}

#endif