#pragma once

#include <nng/nng.h>
#include <nng/protocol/pubsub0/pub.h>
#include <nng/protocol/pubsub0/sub.h>

#include <vector>
namespace O3DS
{

class Subscriber
{
public:
	Subscriber();

	bool Start(const char *url);
	bool Recv(char *data, size_t &len);

	nng_socket mSocket;

	nng_aio *aio;
	nng_ctx  ctx;

};

} // namespace

