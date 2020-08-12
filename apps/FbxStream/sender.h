#ifndef FBX_STREAM_SENDER_H
#define FBX_STREAM_SENDER_H


#include <nng/nng.h>
#include <nng/protocol/reqrep0/req.h>
#include <nng/supplemental/util/platform.h>

class Sender
{
public:
	Sender();
	~Sender() { nng_close(mSocket); }
	bool connect(const char*url);

	bool send(void *data, size_t len);

private:

	nng_socket mSocket;
};

#endif