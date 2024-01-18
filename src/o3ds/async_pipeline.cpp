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

#include "async_pipeline.h"

namespace O3DS
{
	bool AsyncPipelinePush::start(const char *url)
	{
		int ret;

		ret = nng_push0_open(&mSocket);
		if (ret != 0) { return false; }

		ret = nng_aio_alloc(&aio, AsyncPipeline::Callback, this);
		if (ret != 0) { return false; }

		ret = nng_dial(mSocket, url, 0, 0);
		if (ret != 0) { return false; }

		nng_recv_aio(mSocket, aio);

		return true;
	}

	// Pipeline 
	bool AsyncPipelinePull::start(const char *url)
	{
		int ret;

		ret = nng_pull0_open(&mSocket);
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

		if (mInDataFunc) mInDataFunc(mContext, (void*)buf, sz);

		nng_free(buf, sz);

		nng_recv_aio(mSocket, aio);
	}
}

