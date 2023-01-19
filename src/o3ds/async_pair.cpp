/*
Open 3D Stream

Copyright 2022 Alastair Macleod

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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

