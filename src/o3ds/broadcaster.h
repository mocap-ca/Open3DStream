#ifndef OPEN3D_STREAM_BROADCASTER_H
#define OPEN3D_STREAM_BROADCASTER_H


#include <nng/nng.h>
#include <nng/protocol/pubsub0/pub.h>
#include <nng/protocol/pubsub0/sub.h>

#include <vector>

namespace O3DS
{

	class Broadcaster
	{
	public:
		Broadcaster();
		~Broadcaster() { nng_close(mSocket); }
		bool start(const char*url, int workers);

		bool send(void *data, size_t len);

		void setError(const char *, int);

		std::string mError;

		nng_socket mSocket;
	};
} // namespace O3DS

#endif