#include "subscriber.h"


namespace O3DS
{

bool Subscriber::start(const char *url)
{
	int ret;

	ret = nng_sub0_open(&mSocket);
	if (ret != 0) { return false; }

	ret = nng_dial(mSocket, url, NULL, 0);
	if (ret != 0) { return false; }

	ret = nng_setopt(mSocket, NNG_OPT_SUB_SUBSCRIBE, "", 0);
	if (ret != 0) { return false; }

	return true;
}

}