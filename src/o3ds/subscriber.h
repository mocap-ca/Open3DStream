#pragma once

#include <nng/nng.h>
#include <nng/protocol/pubsub0/pub.h>
#include <nng/protocol/pubsub0/sub.h>
#include "base_server.h"

#include <vector>
namespace O3DS
{

class Subscriber : public BlockingConnector
{
public:
	bool start(const char *url);
};

} // namespace

