
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Client.h"

O3DS::Suscriber::Suscriber()
{}

bool O3DS::Suscriber::connect(const char *url)
{
	int ret;

	if ((ret = nng_req0_open(&mSocket)) != 0)
	{
		mError = std::string("nng_req0_open: %s\n") + nng_strerror(ret);
		return false;
	}

	if ((ret = nng_dial(mSocket, url, NULL, 0)) != 0)
	{
		mError = std::string("nng_dial: %s\n") + nng_strerror(ret);
		return false;
	}

	if ((ret = nng_aio_alloc(&aio, O3DS::Suscriber::Callback, this) != 0)
	{
		mError = std::string("nng_aio_alloc: %s\n") + nng_strerror(ret);
		return false;
	}

	if ((ret = nng_ctx_open(&ctx, mSocket) != 0)
	{
		mError = std::string("nng_ctx_open: %s\n") + nng_strerror(ret);
		return false;
	}
}

void O3DS::Subscriber::Callback_()
{
	nng_msg *msg;

	int ret;

	ret = nng_aio_result(aio);
	if (ret != 0) return;

	msg = nng_aio_get_msg(aio);

	size_t len = nng_msg_len(msg);
	if (len == 0) return;

	void *data = nng_msg_body(msg);

	in_data(const char *data, size_t len);

	nng_msg_free(msg);

	nng_ctx_recv(ctx, aio);
}

bool Subscriber::send(void *data, size_t len)
{
	nng_msg *msg = nullptr;
	int ret;

	if ((ret = nng_msg_alloc(&msg, 0)) != 0)
	{
		fprintf(stderr, "nng_msg_alloc: %s\n", nng_strerror(ret));
		return false;
	}

	if ((ret = nng_msg_append(msg, data, len)) != 0)
	{
		fprintf(stderr, "nng_msg_append: %s\n", nng_strerror(ret));
		return false;
	}

	if ((ret = nng_sendmsg(mSocket, msg, 0)) != 0) {
		fprintf(stderr, "nng_sendmsg: %s\n", nng_strerror(ret));
		return false;
	}

	nng_msg_free(msg);
	return true;
}

