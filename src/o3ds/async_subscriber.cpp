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

#include "async_subscriber.h"
namespace O3DS
{

bool AsyncSubscriber::start(const char *url)
{
	int ret;

	ret = nng_sub0_open(&mSocket);
	NNG_ERROR("Async Subscriber open socket");

	ret = nng_setopt(mSocket, NNG_OPT_SUB_SUBSCRIBE, "", 0);
	NNG_ERROR("Setting subscribe flag")

	ret = nng_dialer_create(&mDialer, mSocket, url);
	NNG_ERROR("Creating dialer")

	//ret = nng_pipe_notify(mSocket, nng_pipe_ev::NNG_PIPE_EV_ADD_POST,  AsyncSubscriber::pipeEvent, this);
	//NNG_ERROR("Setting pipe notify")

	// Async dial - pipe will be created on connection
	ret = nng_dialer_start(mDialer, NNG_FLAG_NONBLOCK);
	NNG_ERROR("Subscriber async dialier")

	ret = nng_aio_alloc(&aio, AsyncSubscriber::callback, this);
	NNG_ERROR("Subscriber async aio alloc")

	nng_recv_aio(mSocket, aio);

	mState = Connector::STARTED;

	return true;
}

void AsyncSubscriber::pipeEvent_(nng_pipe pipe, nng_pipe_ev pipe_ev)
{
//	int ret;
	if (pipe_ev == nng_pipe_ev::NNG_PIPE_EV_ADD_POST)
	{
		nng_socket s= nng_pipe_socket(pipe);

		//int ret = nng_ctx_open(&ctx, mSocket);
		//if (ret != 0) return;

		//nng_ctx_recv(ctx, aio);

		//in_pipe();
	}

	if (pipe_ev == nng_pipe_ev::NNG_PIPE_EV_REM_POST)
		return; // TODO
}


}
