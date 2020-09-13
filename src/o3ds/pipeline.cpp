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