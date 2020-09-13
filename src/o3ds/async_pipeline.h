#ifndef O3DS_ASYNC_PIPELINE
#define O3DS_ASYNC_PIPELINE

#include <nng/nng.h>
#include <nng/protocol/pair1/pair.h>
#include "base_server.h"
#include <string>

namespace O3DS
{
	class AsyncPipeline : public AsyncConnector
	{
	public:
		static void Callback(void *ref) { ((AsyncPipeline*)ref)->Callback_(); }
		void Callback_();
	};

	class AsyncPull : public AsyncPipeline
	{
	public:
		bool start(const char* url) override;
	};

	class AsyncPush : public AsyncPipeline
	{
	public:
		bool start(const char* url) override;
	};
}

#endif