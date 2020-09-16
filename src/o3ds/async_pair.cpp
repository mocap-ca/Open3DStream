#include "async_pair.h"

namespace O3DS
{
	bool AsyncPairClient::start(const char *url)
	{
		int ret;

		ret = nng_pair1_open(&mSocket);
		if (ret != 0) { return false; }

		ret = nng_aio_alloc(&aio, AsyncPairClient::callback, this);
		if (ret != 0) { return false; }

		ret = nng_dial(mSocket, url, 0, 0);
		if (ret != 0) { return false; }

		nng_recv_aio(mSocket, aio);

		return true;
	}

	bool AsyncPairServer::start(const char *url)
	{
		int ret;

		ret = nng_pair1_open(&mSocket);
		if (ret != 0) { return false; }

		ret = nng_aio_alloc(&aio, AsyncPair::callback, this);
		if (ret != 0) { return false; }

		ret = nng_listen(mSocket, url, NULL, 0);
		if (ret != 0) return false;

		nng_recv_aio(mSocket, aio);

		return true;
	}


}

