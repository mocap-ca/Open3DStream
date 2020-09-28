#ifndef O3DS_ASYNC_PIPELINE
#define O3DS_ASYNC_PIPELINE

#include <nng/nng.h>
#include <nng/protocol/pair1/pair.h>
#include "base_server.h"
#include <string>

namespace O3DS
{
	/*! \class AsyncPipeline async_pipeline.h o3ds/async_pipeline.h */
	//! Wrapper for nng pipeline pattern with asynchronous calls
	class AsyncPipeline : public AsyncConnector
	{
	public:
		static void Callback(void *ref) { ((AsyncPipeline*)ref)->Callback_(); }
		void Callback_();
	};

	/*! \class AsyncClient async_pipeline.h o3ds/async_pipeline.h */
	//! Wrapper for nng pipeline pattern with asynchronous calls
	class AsyncClient : public AsyncPipeline
	{
	public:
		bool start(const char* url) override;
	};

	/*! \class AsyncServer async_pipeline.h o3ds/async_pipeline.h */
	//! Wrapper for nng pipeline pattern with asynchronous calls
	class AsyncServer : public AsyncPipeline
	{
	public:
		bool start(const char* url) override;
	};
}

#endif