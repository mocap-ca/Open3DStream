
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sender.h"

Sender::Sender()
{}

bool Sender::connect(const char *url)
{
	int ret;

	if ((ret = nng_req0_open(&mSocket)) != 0)
	{
		fprintf(stderr, "nng_req0_open: %s\n", nng_strerror(ret));
		return false;
	}

	if ((ret = nng_dial(mSocket, url, NULL, 0)) != 0)
	{
		fprintf(stderr, "nng_dial: %s\n", nng_strerror(ret));
		return false;
	}
	return true;
}

bool Sender::send(void *data, size_t len)
{
	nng_msg *msg=nullptr;
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

