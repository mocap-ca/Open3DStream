#include "pair.h"

namespace O3DS
{
	Pair::Pair()
	{}

	bool Pair::Start(const char* url)
	{
		int ret;

		ret = nng_pair1_open(&mSocket);
		if (ret != 0) { return false; }

		ret = nng_dial(mSocket, url, NULL, 0);
		if (ret != 0) { return false; }

		return true;
	}

	bool Pair::Recv(char *data, size_t &len)
	{
		nng_msg *msg;
		int ret = nng_recvmsg(mSocket, &msg, 0);
		if (ret != 0) {
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

	bool Pair::Send(const char *data, size_t len)
	{

		int ret = nng_send(mSocket, (void*)data, len, 0);
		if (ret != 0)
			return false;

		return true;
	}


}