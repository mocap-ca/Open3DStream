#include "async_pipeline.h"

namespace O3DS
{
	bool AsyncPull::start(const char *url)
	{
		int ret;

		ret = nng_pair1_open(&mSocket);
		if (ret != 0) { return false; }

		ret = nng_aio_alloc(&aio, AsyncPipeline::Callback, this);
		if (ret != 0) { return false; }

		ret = nng_dial(mSocket, url, 0, 0);
		if (ret != 0) { return false; }

		nng_recv_aio(mSocket, aio);

		return true;
	}

	bool AsyncPush::start(const char *url)
	{
		int ret;

		ret = nng_pair1_open(&mSocket);
		if (ret != 0) { return false; }

		ret = nng_aio_alloc(&aio, AsyncPipeline::Callback, this);
		if (ret != 0) { return false; }

		ret = nng_listen(mSocket, url, NULL, 0);
		if (ret != 0) return false;

		nng_recv_aio(mSocket, aio);

		return true;
	}

	void AsyncPipeline::Callback_()
	{
		int ret;

		ret = nng_aio_result(aio);
		if (ret != 0) return;

		char *buf = NULL;
		size_t sz;
		ret = nng_recv(mSocket, &buf, &sz, NNG_FLAG_ALLOC);
		if (ret != 0) return;

		if (fnRef) fnRef(fnContext, (void*)buf, sz);

		nng_free(buf, sz);

		nng_recv_aio(mSocket, aio);
	}
}

