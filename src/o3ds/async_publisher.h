#ifndef O3DS_ASYNC_PUBLISHER_H
#define O3DS_ASYNC_PUBLISHER_H

#include <nng/nng.h>
#include <nng/protocol/pubsub0/pub.h>
#include <nng/protocol/pubsub0/sub.h>
#include "base_server.h"
#include <vector>

namespace O3DS
{
	class AsyncPublisher : public BaseServer
	{
	public:
		AsyncPublisher();
		~AsyncPublisher() { nng_close(mSocket); }

		virtual bool connect(const char* url);
		virtual bool listen(const char *url);
		virtual bool write(const char *data, size_t ptr);

		bool listen(const char*url, int workers);

		bool write(void *data, size_t len);


		std::string mError;

		nng_socket mSocket;
	};
} // namespace O3DS

#endif


#endif