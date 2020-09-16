#include "o3ds/async_publisher.h"

namespace O3DS
{

	bool AsyncPublisher::start(const char *url)
	{
		int ret;

		ret = nng_pub0_open(&mSocket);
		if (ret != 0) 
		{
			setError("Creating publish socket", ret);
			return false;
		}

		if ((ret = nng_listen(mSocket, url, NULL, 0)) < 0) {
			setError("Could not listen", ret);
			return false;
		}
		return true;
	}

}