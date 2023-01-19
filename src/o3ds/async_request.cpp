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

#include "async_request.h"

namespace O3DS
{
	bool AsyncRequest::start(const char *url)
	{
		int ret;

		ret = nng_req0_open(&mSocket);
		NNG_ERROR("Creating request socket")

		ret = nng_aio_alloc(&aio, AsyncRequest::Callback, this);
		NNG_ERROR("Creating request socket aio")

		ret = nng_dial(mSocket, url, 0, 0);
		NNG_ERROR("Conecting request socket")

		nng_recv_aio(mSocket, aio);

		mState = Connector::STARTED;

		return true;
	}

	bool AsyncReply::start(const char *url)
	{
		int ret;

		ret = nng_req0_open(&mSocket);
		NNG_ERROR("Creating reply socket")

		ret = nng_aio_alloc(&aio, AsyncReply::Callback, this);
		NNG_ERROR("Creating reply socket aio")

		ret = nng_listen(mSocket, url, NULL, 0);
		NNG_ERROR("Listening on reply socket");

		nng_recv_aio(mSocket, aio);

		mState = Connector::STARTED;

		return true;
	}

	void AsyncRequestReply::Callback_()
	{
		int ret;

		ret = nng_aio_result(aio);
		if (ret != 0)
		{
			setError("Aio result on async request reply", ret);
			return;
		}

		char *buf = NULL;
		size_t sz;
		ret = nng_recv(mSocket, &buf, &sz, NNG_FLAG_ALLOC);
		if (ret != 0)
		{
			setError("Async receive request", ret);
			return;
		}

		if (mInDataFunc) mInDataFunc(mContext, (void*)buf, sz);

		nng_free(buf, sz);

		nng_recv_aio(mSocket, aio);
	}


}

