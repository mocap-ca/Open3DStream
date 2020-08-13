
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "broadcaster.h"


O3DS::Broadcaster::Broadcaster()
{}

bool O3DS::Broadcaster::start(const char *url, int workers)
{
	int ret;

	if ((ret = nng_pub0_open(&mSocket)) != 0) {
		setError("Could not open socket", ret);
		return false;
	}
	if ((ret = nng_listen(mSocket, url, NULL, 0)) < 0) {
		setError("Could not listen", ret);
		return false;
	}
	return true;
}

void O3DS::Broadcaster::setError(const char *msg, int ret)
{
	mError = msg;
	mError += ": ";
	mError += nng_strerror(ret);
}

bool O3DS::Broadcaster::send(void *data, size_t len)
{
	int ret;
	if ((ret = nng_send(mSocket, data, len, 0)) != 0)
	{
		setError("Error sending", ret);
	}

	return true;
}

