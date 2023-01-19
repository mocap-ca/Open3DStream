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

#ifndef O3DS_ASYNC_PAIR
#define O3DS_ASYNC_PAIR

#include <nng/nng.h>
#include <nng/protocol/pair1/pair.h>
#include "nng_connector.h"
#include <string>

namespace O3DS
{
	/*! \class AsyncPair async_pair.h o3ds/async_pair.h */
	//! Wrapper for nng pair pattern with asynchronous calls
	class AsyncPair : public AsyncNngConnector
	{
	public:
		void callback_()
		{
			AsyncNngConnector::asyncReadMsg();
		}
		static void callback(void *ref)
		{
			((AsyncPair*)ref)->callback_();
		}
		void Callback_();
	};

	/*! \class AsyncPairClient async_pair.h o3ds/async_pair.h */
	//! Calls nng_dial to connect
	class AsyncPairClient : public AsyncPair
	{
	public:
		bool start(const char* url) override;
	};

	/*! \class AsyncPairServer async_pair.h o3ds/async_pair.h */
	//! Calls nng_listen to wait for a connection
	class AsyncPairServer : public AsyncPair
	{
	public:
		bool start(const char* url) override;
	};
}

#endif