#include "Open3DServer.h"


O3DS_Server::O3DS_Server()
{}


bool O3DS_Server::Start(const char *url)
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

bool O3DS_Server::Recv()
{
	nng_msg *msg;
	int ret;
	if ((ret = nng_recvmsg(mSocket, &msg, 0)) != 0) {
		return false;
	}

	size_t len = nng_msg_len(msg);
	char *ptr = (char*) nng_msg_body(msg);
	mutex.Lock();
	buffer.resize(len);
	buffer.assign(ptr, ptr + len);
	mutex.Unlock();
	nng_msg_free(msg);
	return true;
}
