
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>

#include "async_subscriber.h"
namespace O3DS
{

AsyncSubscriber::AsyncSubscriber()
{}

bool AsyncSubscriber::connect(const char *url)
{
	int ret;

	ret = nng_sub0_open(&mSocket);
	if (ret != 0)
	{	
		mError = std::string("nng_req0_open: %s\n") + nng_strerror(ret);
		return false;
	}

	ret = nng_setopt(mSocket, NNG_OPT_SUB_SUBSCRIBE, "", 0);
	if (ret != 0) { return false; }

	ret = nng_dialer_create(&mDialer, mSocket, url);
	if (ret != 0) { return false;  }

	ret = nng_pipe_notify(mSocket, nng_pipe_ev::NNG_PIPE_EV_ADD_POST, 
		AsyncSubscriber::PipeEvent, this);
	if (ret != 0) { return false; }

	ret = nng_dialer_start(mDialer, NNG_FLAG_NONBLOCK);
	if (ret != 0) { return false; }

	return true;
}

void AsyncSubscriber::PipeEvent_(nng_pipe pipe, nng_pipe_ev pipe_ev)
{
	if (pipe_ev == nng_pipe_ev::NNG_PIPE_EV_ADD_POST)
	{
		nng_socket s= nng_pipe_socket(pipe);

		nng_aio_alloc(&aio, AsyncSubscriber::Callback, this);

		nng_recv_aio(s, aio);
		in_pipe();
	}

	if (pipe_ev == nng_pipe_ev::NNG_PIPE_EV_REM_POST)
		return; // TODO
}


void AsyncSubscriber::Callback_()
{
	nng_msg *msg;

	int ret;

	ret = nng_aio_result(aio);
	if (ret != 0) return;

	msg = nng_aio_get_msg(aio);

	size_t len = nng_msg_len(msg);
	if (len == 0) return;

	void *data = nng_msg_body(msg);

	if (fnRef) fnRef(fnContext, data, len);

	nng_msg_free(msg);

	nng_recv_aio(mSocket, aio);

}

}
