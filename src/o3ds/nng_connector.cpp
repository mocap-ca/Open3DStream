/*
Open 3D Stream

Copyright 2022 Alastair Macleod

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "nng_connector.h"

#include <string>
#include <string.h>
#include "nng/nng.h"


namespace O3DS
{
	BlockingNngConnector::~BlockingNngConnector()
	{
		std::lock_guard<std::mutex> guard(mutex);
		nng_close(mSocket);
	}

	void BlockingNngConnector::setError(const char* msg, int ret)
	{
		mError = msg;
		mError += ": ";
		mError += nng_strerror(ret);
		mState = Connector::STATE_ERROR;
	}

	bool BlockingNngConnector::write(const char* data, size_t len)
	{
		int ret;
		nng_msg* msg;

		std::lock_guard<std::mutex> guard(mutex);

		ret = nng_msg_alloc(&msg, 0);
		NNG_ERROR("Creating message")

		ret = nng_msg_append(msg, data, len);
		NNG_ERROR("Appending message")

		ret = nng_sendmsg(mSocket, msg, 0);
		NNG_ERROR("Sending message")

		return true;
	}

	size_t BlockingNngConnector::read(char** data, size_t* len)
	{
		if (data == nullptr || len == nullptr)
		{
			Connector::setError("Invalid parameter");
			return 0;
		}

		int ret;

		nng_msg* msg = nullptr;

		ret = nng_recvmsg(mSocket, &msg, 0);
		NNG_ERROR("Receiving message");

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
			Connector::setError("Invalid Message");
			nng_msg_free(msg);
			return false;
		}

		memcpy(*data, msgBody, msglen);

		nng_msg_free(msg);

		return msglen;

	}

	/*  ASYNC */

	void AsyncNngConnector::stop()
	{
		if(mSocket.id == 0)
			return;
		
		nng_close(mSocket);
		mSocket = NNG_SOCKET_INITIALIZER;
		nng_dialer_close(mDialer);
		if (aio) nng_aio_stop(aio);
		aio = nullptr;
	}

	void AsyncNngConnector::setError(const char* msg, int ret)
	{
		mError = msg;
		mError += ": ";
		mError += nng_strerror(ret);
		mState = Connector::STATE_ERROR;
	}


	bool AsyncNngConnector::write(const char* data, size_t len)
	{
		int ret;
		nng_msg* msg;

		std::lock_guard<std::mutex> guard(mutex);

		ret = nng_msg_alloc(&msg, 0);
		NNG_ERROR("Message alloc");

		ret = nng_msg_append(msg, data, len);
		NNG_ERROR("Creating message")

			ret = nng_sendmsg(mSocket, msg, NNG_FLAG_NONBLOCK);
		NNG_ERROR("Sending message")

			return true;
	}

	size_t AsyncNngConnector::read(char** data, size_t* len)
	{
		if (data == nullptr || len == nullptr)
		{
			Connector::setError("Invalid parameter");
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
			Connector::setError("Invalid Message");
			nng_msg_free(msg);
			return false;
		}

		memcpy(*data, msgBody, msglen);

		nng_msg_free(msg);

		return msglen;
	}


	bool AsyncNngConnector::asyncReadMsg()
	{
		// Only calls nng_recv_aio if the message was okay.
		int ret;

		std::lock_guard<std::mutex> guard(mutex);

		ret = nng_aio_result(aio);
		if (ret != 0)
		{
			setError("Async read", ret);
			mState = Connector::STATE_ERROR;
			return false;
		}

		nng_msg* msg = nng_aio_get_msg(aio);
		if (msg == nullptr)
		{
			Connector::setError("No message wile doing an async read");
			mState = Connector::STATE_ERROR;
			return false;
		}

		if (mInDataFunc) mInDataFunc(mContext, nng_msg_body(msg), nng_msg_len(msg));

		nng_msg_free(msg);

		nng_recv_aio(mSocket, aio);

		mState = Connector::READING;

		return true;

	}
} // namespace
