#ifndef O3DS_CLIENT_H
#define O3DS_CLIENT_H

#include <nng/nng.h>
#include <nng/protocol/pubsub0/sub.h>
#include <nng/supplemental/util/platform.h>

namespace O3DS
{
	// The client pulls data down from a listen server
	class AsyncSubscriber
	{
	public:
		AsyncSubscriber();
		~AsyncSubscriber() { nng_close(mSocket); }
		bool connect(const char*url);

		static void Callback(void *ref) { ((AsyncSubscriber*)ref)->Callback_(); }
		void Callback_();

		virtual void in_data(const char *msg, size_t len) = 0; 

		nng_socket mSocket;
		nng_aio *aio;
		nng_ctx  ctx;

		std::string mError;

	};


}


#endif