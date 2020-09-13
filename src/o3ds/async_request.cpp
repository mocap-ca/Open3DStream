#include "async_request.h"

namespace O3DS
{
	bool AsyncRequest::start(const char *url)
	{
		int ret;

		ret = nng_req0_open(&mSocket);
		if (ret != 0) { return false; }

		ret = nng_aio_alloc(&aio, AsyncRequest::Callback, this);
		if (ret != 0) { return false; }

		ret = nng_dial(mSocket, url, 0, 0);
		if (ret != 0) { return false; }

		nng_recv_aio(mSocket, aio);

		return true;
	}

	bool AsyncReply::start(const char *url)
	{
		int ret;

		ret = nng_req0_open(&mSocket);
		if (ret != 0) { return false; }

		ret = nng_aio_alloc(&aio, AsyncReply::Callback, this);
		if (ret != 0) { return false; }

		ret = nng_listen(mSocket, url, NULL, 0);
		if (ret != 0) return false;

		nng_recv_aio(mSocket, aio);

		return true;
	}

	void AsyncRequestReply::Callback_()
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

