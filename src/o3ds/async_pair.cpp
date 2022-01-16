#include "async_pair.h"

namespace O3DS
{
	bool AsyncPairClient::start(const char *url)
	{
		int ret;

		ret = nng_pair1_open(&mSocket);
		NNG_ERROR("Async pair client connection")

		ret = nng_aio_alloc(&aio, AsyncPairClient::callback, this);
		NNG_ERROR("Async pair client connecting aio alloc")

		ret = nng_dial(mSocket, url, 0, 0);
		NNG_ERROR("Async pair client dial connecting")

		nng_recv_aio(mSocket, aio);

		mState = Connector::STARTED;

		return true;
	}

	bool AsyncPairServer::start(const char *url)
	{
		int ret;

		ret = nng_pair1_open(&mSocket);
		NNG_ERROR("Async pair server connecting");

		ret = nng_aio_alloc(&aio, AsyncPair::callback, this);
		NNG_ERROR("Async pair Server aio alloc")

		ret = nng_listen(mSocket, url, NULL, 0);
		NNG_ERROR("Async pair server listening")

		nng_recv_aio(mSocket, aio);

		mState = Connector::STARTED;

		return true;
	}


}

