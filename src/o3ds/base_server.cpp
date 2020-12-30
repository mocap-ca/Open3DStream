#include "base_server.h"

#include <string>
#include <string.h>
#include "nng/nng.h"

#define NERR(msg) if(ret != 0) { setError(msg, ret); return false;  }

namespace O3DS
{
	const std::string& Connector::getError()
	{
		return mError;
	}

	void Connector::setError(const char *msg, int ret)
	{
		mError = msg;
		mError += ": ";
		mError += nng_strerror(ret);
	}

	bool BlockingConnector::write(const char *data, size_t len)
	{
		int ret;
		ret = nng_send(mSocket, (void*)data, len, 0);
		NERR("Sending data")

		return true;
	}

	bool BlockingConnector::writeMsg(const char *data, size_t len)
	{
		int ret;
		nng_msg *msg;

		ret = nng_msg_alloc(&msg, 0);
		NERR("Creating message")

		ret = nng_msg_append(msg, data, len);
		NERR("Appending message")

		ret = nng_sendmsg(mSocket, msg, 0);
		NERR("Sending message")

		return true;
	}

	size_t BlockingConnector::read(char *data, size_t len)  // Read bytes - len is the size of data
	{
		size_t sz = len;
		int ret = nng_recv(mSocket, data, &sz, 0);
		NERR("Reading data")

		return sz;
	}

	size_t BlockingConnector::readMsg(char *data, size_t len)  // Read bytes - len is the size of data
	{
		int ret;

		nng_msg *msg = nullptr;

		ret = nng_recvmsg(mSocket, &msg, 0);
		NERR("Receiving message");

		size_t msglen = nng_msg_len(msg);
		if (msglen > len)
		{
			nng_msg_free(msg);
			mError = "Message too large";
			return false;
		}

		void *msgBody = nng_msg_body(msg);
		if (!msgBody)
		{
			mError = "Invalid Message";
			return false;
		}

		memcpy(data, msgBody, msglen);

		nng_msg_free(msg);

		return msglen;
	}


	bool AsyncConnector::write(const char *data, size_t len)
	{
		int ret;
		ret = nng_send(mSocket, (void*)data, len, NNG_FLAG_NONBLOCK);
		NERR("Sending data")
		return true;
	}

	bool AsyncConnector::writeMsg(const char *data, size_t len)
	{
		int ret;
		nng_msg *msg;
		
		ret = nng_msg_alloc(&msg, 0);
		NERR("Message alloc");

		ret =nng_msg_append(msg, data, len);
		NERR("Creating message")

		ret = nng_sendmsg(mSocket, msg, NNG_FLAG_NONBLOCK);
		NERR("Sending message")

		return true;
	}


	size_t AsyncConnector::read(char *data, size_t len)  // Read bytes - len is the size of data
	{
		size_t sz = len;
		int ret = nng_recv(mSocket, data, &sz, NNG_FLAG_NONBLOCK);
		if (ret == NNG_EAGAIN) { return 0; }
		NERR("Receiving data")
		return sz;
	}

	size_t AsyncConnector::readMsg(char *data, size_t len)  // Read bytes - len is the size of data
	{
		int ret;

		nng_msg *msg;

		ret = nng_recvmsg(mSocket, &msg, NNG_FLAG_NONBLOCK);
		if (ret == NNG_EAGAIN) { return 0; }
		NERR("Receiving message");

		if (msg == nullptr)
			return 0;

		size_t msglen = nng_msg_len(msg);
		if (msglen > len)
		{
			mError = "Message too large";
			return false;
		}

		memcpy(data, nng_msg_body(msg), len);

		nng_msg_free(msg);

		return msglen;
	}

	void AsyncConnector::setFunc(void* ctx, inDataFunc f)  // User implemented callback to receive data (optional)
	{ 
		fnContext = ctx;
		fnRef = f;
	}



	void AsyncConnector::asyncReadMsg()
	{
		int ret;

		ret = nng_aio_result(aio);
		if (ret != 0)
		{
			//nng_ctx_recv(ctx, aio);
			nng_recv_aio(mSocket, aio);
			return;
		}

		nng_msg *msg = nng_aio_get_msg(aio);
		if (msg == nullptr)
		{
			//nng_ctx_recv(ctx, aio);
			nng_recv_aio(mSocket, aio);
			return;
		}

		if (fnRef) fnRef(fnContext, nng_msg_body(msg), nng_msg_len(msg));

		nng_msg_free(msg);

		//nng_ctx_recv(ctx, aio);
		nng_recv_aio(mSocket, aio);

	}
} // namespace
