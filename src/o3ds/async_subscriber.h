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

#ifndef O3DS_ASYNC_SUBSCRIBER_H
#define O3DS_ASYNC_SUBSCRIBER_H

#include <nng/nng.h>
#include <nng/protocol/pubsub0/sub.h>
#include <nng/supplemental/util/platform.h>
#include "nng_connector.h"
#include <string>

namespace O3DS
{
	// The client pulls data down from a listen server
	class AsyncSubscriber : public AsyncNngConnector
	{
	public:

		bool start(const char*url);

		void callback_()
		{
			AsyncNngConnector::asyncReadMsg();
		}
		static void callback(void *ref)
		{
			((AsyncSubscriber*)ref)->callback_();
		}
		static void pipeEvent(nng_pipe pipe, nng_pipe_ev pipe_ev, void* ref)
		{
			((AsyncSubscriber*)ref)->pipeEvent_(pipe, pipe_ev);
		}
		void pipeEvent_(nng_pipe pipe, nng_pipe_ev pipe_ev);

		bool listen(const char* url) { return false; }

		bool write(const char* data, size_t ptr) { return false; }

		//virtual void in_pipe() = 0;

	};


}


#endif