#pragma once

#include <nng/nng.h>
#include <nng/protocol/pubsub0/pub.h>
#include <nng/protocol/pubsub0/sub.h>
#include "nng_connector.h"

#include <vector>
namespace O3DS
{

class Subscriber : public BlockingNngConnector
{
public:
	bool start(const char *url);
};

} // namespace

