#ifndef O3DS_CLIENT_H
#define O3DS_CLIENT_H

#include <nng/nng.h>
#include <nng/protocol/reqrep0/req.h>
#include <nng/supplemental/util/platform.h>

namespace O3DS
{
	// The client pulls data down from a listen server
	class Subscriber
	{
	public:
		Subscriber();
		~Subscriber() { nng_close(mSocket); }
		bool connect(const char*url);

		static void Callback(void *ref) { ((Subscriber*)ref)->Callback_(); }
		void Callback_();

		virtual void in_data(const char *msg, size_t len) = 0; 
		bool send(void *data, size_t len);

		nng_socket mSocket;
		nng_aio *aio;
		nng_ctx  ctx;

		std::string mError;

	};


}


#endif