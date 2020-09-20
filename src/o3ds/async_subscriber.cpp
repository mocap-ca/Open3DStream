
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>

#include "async_subscriber.h"
namespace O3DS
{

bool AsyncSubscriber::start(const char *url)
{
	int ret;

	ret = nng_sub0_open(&mSocket);
	if (ret != 0)
	{	
		mError = std::string("nng_req0_open: %s\n") + nng_strerror(ret);
		return false;
	}

	ret = nng_setopt(mSocket, NNG_OPT_SUB_SUBSCRIBE, "", 0);
	if (ret != 0) { setError("Setting subscribe flag", ret);  return false; }

	ret = nng_dialer_create(&mDialer, mSocket, url);
	if (ret != 0) { setError("Creating dialer", ret);  return false;  }

	ret = nng_pipe_notify(mSocket, nng_pipe_ev::NNG_PIPE_EV_ADD_POST, 
		AsyncSubscriber::pipeEvent, this);
	if (ret != 0) { setError("Setting pipe notify", ret); return false; }

	// Async dial - pipe will be created on connection
	ret = nng_dialer_start(mDialer, NNG_FLAG_NONBLOCK);
	if (ret != 0) { setError("Connecting to publisher", ret); return false; }

	return true;
}

void AsyncSubscriber::pipeEvent_(nng_pipe pipe, nng_pipe_ev pipe_ev)
{
	int ret;
	if (pipe_ev == nng_pipe_ev::NNG_PIPE_EV_ADD_POST)
	{
		nng_socket s= nng_pipe_socket(pipe);

		ret = nng_aio_alloc(&aio, AsyncSubscriber::callback, this);
		if (ret != 0) return;

		nng_recv_aio(mSocket, aio);

		//ret = nng_ctx_open(&ctx, mSocket);
		//if (ret != 0) return;

		//nng_ctx_recv(ctx, aio);

		//in_pipe();
	}

	if (pipe_ev == nng_pipe_ev::NNG_PIPE_EV_REM_POST)
		return; // TODO
}


}
