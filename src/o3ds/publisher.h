#ifndef O3DS_ASYNC_PUBLISHER_H
#define O3DS_ASYNC_PUBLISHER_H

#include <nng/nng.h>
#include <nng/protocol/pubsub0/pub.h>
#include <nng/protocol/pubsub0/sub.h>

#include <vector>

namespace O3DS
{

	class Publisher
	{
	public:
		Publisher();
		~Publisher() { nng_close(mSocket); }
		bool start(const char*url, int workers);

		bool send(void *data, size_t len);

		void setError(const char *, int);

		std::string mError;

		nng_socket mSocket;
	};
} // namespace O3DS

#endif

