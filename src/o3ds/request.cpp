#include "request.h"

#include <nng/protocol/reqrep0/rep.h>
#include <nng/protocol/reqrep0/req.h>

namespace O3DS
{
	bool Request::start(const char* url)
	{
		int ret;

		ret = nng_req0_open(&mSocket);
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

	bool Reply::start(const char *url)
	{
		int ret;

		ret = nng_rep0_open(&mSocket);
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