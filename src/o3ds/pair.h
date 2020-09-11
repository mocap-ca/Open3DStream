#pragma once

#include <nng/nng.h>
#include <nng/protocol/pair1/pair.h>

#include <vector>
namespace O3DS
{

	class Pair
	{
	public:
		Pair();

		bool Start(const char *url);
		bool Recv(char *data, size_t &len);
		bool Send(const char *data, size_t len);

		nng_socket mSocket;

		nng_aio *aio;

	};
}