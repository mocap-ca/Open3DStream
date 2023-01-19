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

#ifndef O3DS_ASYNC_REQUEST
#define O3DS_ASYNC_REQUEST

#include <nng/nng.h>
#include <nng/protocol/reqrep0/req.h>
#include "nng_connector.h"


namespace O3DS
{
	/*! \class AsyncRequestReply async_request.h o3ds/async_request.h
	Wraps [nng req0](https://nng.nanomsg.org/man/tip/nng_req.7.html) 
	pattern. Provides a common Callback function to get messages. */
	//! Base class for Request/Reply connection pattern.
	class AsyncRequestReply : public AsyncNngConnector
	{
	public:
		static void Callback(void *ref) { ((AsyncRequestReply*)ref)->Callback_(); }
		void Callback_();
	};

	//! Wrapper for nng req0 dialer
	class AsyncRequest : public AsyncRequestReply
	{
	public:
		bool start(const char* url) override;
	};

	//! Wrapper for nng req0 listener
	class AsyncReply : public AsyncRequestReply
	{
	public:
		bool start(const char* url) override;
	};
}

#endif