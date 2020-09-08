#include "Open3DServer.h"


USubscriber::USubscriber()
{}

void USubscriber::in_data(const char *msg, size_t len)
{
	mutex.Lock();
	buffer.resize(len);
	buffer.assign(msg, msg + len);
	mutex.Unlock();

	DataDelegate.ExecuteIfBound();
}

void USubscriber::in_pipe()
{}

/*
bool USubscriber::Recv()
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

*/
