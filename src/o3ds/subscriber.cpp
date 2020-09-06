#include "subscriber.h"


namespace O3DS
{

Subscriber::Subscriber()
{}


bool Subscriber::Start(const char *url)
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

bool Subscriber::Recv(char *data, size_t &len)
{
	nng_msg *msg;
	int ret;
	if ((ret = nng_recvmsg(mSocket, &msg, 0)) != 0) {
		return false;
	}

	size_t datalen = nng_msg_len(msg);
	if (datalen > len)
		return false;
	len = datalen;
	memcpy(data, nng_msg_body(msg), len);

	nng_msg_free(msg);
	return true;
}


}