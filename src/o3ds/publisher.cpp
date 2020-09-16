
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <string>
#include "publisher.h"


bool O3DS::Publisher::start(const char *url)
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

