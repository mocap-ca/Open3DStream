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

#include "pipeline.h"

#include <nng/protocol/pipeline0/pull.h>
#include <nng/protocol/pipeline0/push.h>

namespace O3DS
{
	bool Pull::start(const char* url)
	{
		int ret;

		ret = nng_push0_open(&mSocket);
		if (ret != 0)
		{
			setError("Could not create pair socket for dial", ret);
			return false;
		}

		ret = nng_dial(mSocket, url, NULL, 0);
		if (ret != 0)
		{
			setError("Could not dial using pair", ret);
			return false;
		}

		return true;
	}

	bool Push::start(const char *url)
	{
		int ret;

		ret = nng_pull0_open(&mSocket);
		if (ret != 0)
		{
			setError("Could not create pair socket for listen", ret);
			return false;
		}

		ret = nng_listen(mSocket, url, NULL, 0);
		if (ret != 0)
		{
			setError("Could not listen on pair connection", ret);
			return false;
		}

		return true;
	}
}