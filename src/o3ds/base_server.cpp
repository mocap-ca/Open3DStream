#include "base_server.h"

#include <string>
#include <string.h>
#include "nng/nng.h"


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
		mState = Connector::STATE_ERROR;
	}

	void Connector::setError(const char* msg)
	{
		mError = msg;
		mState = Connector::STATE_ERROR;
	}

	bool BlockingConnector::write(const char *data, size_t len)
	{
		int ret;
		ret = nng_send(mSocket, (void*)data, len, 0);
		NNG_ERROR("Sending data")

		return true;
	}

	bool BlockingConnector::writeMsg(const char *data, size_t len)
	{
		int ret;
		nng_msg *msg;

		ret = nng_msg_alloc(&msg, 0);
		NNG_ERROR("Creating message")

		ret = nng_msg_append(msg, data, len);
		NNG_ERROR("Appending message")

		ret = nng_sendmsg(mSocket, msg, 0);
		NNG_ERROR("Sending message")

		return true;
	}

	size_t BlockingConnector::read(char *data, size_t len)  // Read bytes - len is the size of data
	{
		size_t sz = len;
		int ret = nng_recv(mSocket, data, &sz, 0);
		NNG_ERROR("Reading data")

		return sz;
	}

	// Read bytes - len is the fixed size of data
	size_t BlockingConnector::readMsg(char *data, size_t len)  
	{
		int ret;

		nng_msg *msg = nullptr;

		ret = nng_recvmsg(mSocket, &msg, 0);
		NNG_ERROR("Receiving message");

		size_t msglen = nng_msg_len(msg);
		if (msglen > len)
		{
			nng_msg_free(msg);
			setError("Message too large");
			return false;
		}

		void *msgBody = nng_msg_body(msg);
		if (!msgBody)
		{
			setError("Invalid Message");
			nng_msg_free(msg);
			return false;
		}

		memcpy(data, msgBody, msglen);

		nng_msg_free(msg);

		return msglen;
	}

	size_t BlockingConnector::readMsg(char** data, size_t *len)
	{
		if (data == nullptr || len == nullptr)
		{
			setError("Invalid parameter");
			return 0;
		}

		int ret;

		nng_msg* msg = nullptr;

		ret = nng_recvmsg(mSocket, &msg, 0);
		NNG_ERROR("Receiving message");

		size_t msglen = nng_msg_len(msg);
		if (msglen > *len)
		{
			char *buf = (char*)realloc(*data, msglen);
			if (!buf) return 0;
			*len = msglen;
		}

		void* msgBody = nng_msg_body(msg);
		if (!msgBody)
		{
			setError("Invalid Message");
			nng_msg_free(msg);
			return false;
		}

		memcpy(*data, msgBody, msglen);

		nng_msg_free(msg);

		return msglen;

	}


	bool AsyncConnector::write(const char *data, size_t len)
	{
		int ret;
		ret = nng_send(mSocket, (void*)data, len, NNG_FLAG_NONBLOCK);
		NNG_ERROR("Sending data")
		return true;
	}

	bool AsyncConnector::writeMsg(const char *data, size_t len)
	{
		int ret;
		nng_msg *msg;
		
		ret = nng_msg_alloc(&msg, 0);
		NNG_ERROR("Message alloc");

		ret =nng_msg_append(msg, data, len);
		NNG_ERROR("Creating message")

		ret = nng_sendmsg(mSocket, msg, NNG_FLAG_NONBLOCK);
		NNG_ERROR("Sending message")

		return true;
	}


	size_t AsyncConnector::read(char *data, size_t len)  // Read bytes - len is the size of data
	{
		size_t sz = len;
		int ret = nng_recv(mSocket, data, &sz, NNG_FLAG_NONBLOCK);
		if (ret == NNG_EAGAIN) { return 0; }
		NNG_ERROR("Receiving data")
		return sz;
	}

	size_t AsyncConnector::readMsg(char *data, size_t len)  // Read bytes - len is the size of data
	{
		int ret;

		nng_msg *msg;

		ret = nng_recvmsg(mSocket, &msg, NNG_FLAG_NONBLOCK);
		if (ret == NNG_EAGAIN) { return 0; }
		NNG_ERROR("Receiving message");

		if (msg == nullptr)
		{
			return 0;
		}

		size_t msglen = nng_msg_len(msg);
		if (msglen > len)
		{
			setError("Message too large");
			return false;
		}

		void* msgBody = nng_msg_body(msg);
		if (!msgBody)
		{
			setError("Invalid Message");
			nng_msg_free(msg);
			return false;
		}

		memcpy(data, msgBody, len);

		nng_msg_free(msg);

		mState = Connector::READING;

		return msglen;
	}

	size_t AsyncConnector::readMsg(char** data, size_t* len)
	{
		if (data == nullptr || len == nullptr)
		{
			setError("Invalid parameter");
			return 0;
		}

		int ret;

		nng_msg* msg = nullptr;

		ret = nng_recvmsg(mSocket, &msg, NNG_FLAG_NONBLOCK);
		if (ret == NNG_EAGAIN) { return 0; }
		NNG_ERROR("Receiving message");

		if (msg == nullptr)
			return 0;

		size_t msglen = nng_msg_len(msg);
		if (msglen > *len)
		{
			char* buf = (char*)realloc(*data, msglen);
			if (!buf) return 0;
			*len = msglen;
		}

		void* msgBody = nng_msg_body(msg);
		if (!msgBody)
		{
			setError("Invalid Message");
			nng_msg_free(msg);
			return false;
		}

		memcpy(*data, msgBody, msglen);

		nng_msg_free(msg);

		return msglen;

	}

	void AsyncConnector::setFunc(void* ctx, inDataFunc f)  // User implemented callback to receive data (optional)
	{ 
		fnContext = ctx;
		fnRef = f;
	}



	bool AsyncConnector::asyncReadMsg()
	{
		// Only calls nng_recv_aio if the message was okay.
		int ret;

		ret = nng_aio_result(aio);
		if (ret != 0)
		{
			setError("Async read", ret);
			mState = Connector::STATE_ERROR;
			return false;
		}

		nng_msg *msg = nng_aio_get_msg(aio);
		if (msg == nullptr)
		{
			setError("No message wile doing an async read");
			mState = Connector::STATE_ERROR;
			return false;
		}

		if (fnRef) fnRef(fnContext, nng_msg_body(msg), nng_msg_len(msg));

		nng_msg_free(msg);

		nng_recv_aio(mSocket, aio);

		mState = Connector::READING;

		return true;

	}
} // namespace
