#include "async_pair.h"

namespace O3DS
{
	AsyncPair::AsyncPair()
	{}

	bool AsyncPair::connect(const char *url)
	{
		int ret;

		ret = nng_pair1_open(&mSocket);
		if (ret != 0) { return false; }

		ret = nng_aio_alloc(&aio, AsyncPair::Callback, this);
		if (ret != 0) { return false; }

		ret = nng_dial(mSocket, url, 0, 0);
		if (ret != 0) { return false; }

		nng_recv_aio(mSocket, aio);

		return true;
	}

	bool AsyncPair::listen(const char *url)
	{
		int ret;

		ret = nng_pair1_open(&mSocket);
		if (ret != 0) { return false; }

		ret = nng_aio_alloc(&aio, AsyncPair::Callback, this);
		if (ret != 0) { return false; }

		ret = nng_listen(mSocket, url, NULL, 0);
		if (ret != 0) return false;

		nng_recv_aio(mSocket, aio);

		return true;
	}

	void AsyncPair::Callback_()
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

	bool AsyncPair::write(const char *data, size_t len)
	{
		int ret;

		ret = nng_send(mSocket, (void*)data, len, NNG_FLAG_NONBLOCK);

		if (ret != 0) return false;

		return true;
	}

}

