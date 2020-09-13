#include "pair.h"

namespace O3DS
{
	bool ClientPair::start(const char* url)
	{
		int ret;

		ret = nng_pair1_open(&mSocket);
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

	bool ServerPair::start(const char *url)
	{
		int ret;

		ret = nng_pair1_open(&mSocket);
		if (ret != 0)
		{
			setError("Could not create pair socket for listen", ret);
			return false;
		}

		ret = nng_listen(mSocket, url, NULL, 0);
		{
			setError("Could not listen on pair connection", ret);
			return false;
		}

		return true;
	}
}