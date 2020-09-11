#ifndef O3DS_ASYNC_PAIR
#define O3DS_ASYNC_PAIR

#include <nng/nng.h>
#include <nng/protocol/pair1/pair.h>
#include "base_server.h"
#include <string>

namespace O3DS
{
	class AsyncPair : public BaseServer
	{
	public:
		AsyncPair();
		virtual ~AsyncPair() { nng_close(mSocket); }

		bool connect(const char* url) override;
		bool listen(const char *url) override;
		bool write(const char *data, size_t ptr) override;

		static void Callback(void *ref) { ((AsyncPair*)ref)->Callback_(); }
		void Callback_();

		nng_socket mSocket;
		nng_dialer mDialer;
		nng_aio *aio;

	};
}

#endif