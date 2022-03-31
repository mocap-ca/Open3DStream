#include <stdio.h>
//#include "o3ds/async_subscriber.h"
//#include "o3ds/async_request.h"
//#include "o3ds/async_pipeline.h"
#include "o3ds/async_subscriber.h"
#include "o3ds/async_pair.h"
#include "o3ds/websocket.h"
#include <nng/nng.h>
#include <chrono>
#include <thread>

#include "o3ds/model.h"

void ReadFunc(void *ptr, void *buf, size_t len)
{
	O3DS::SubjectList subjects;
	subjects.Parse((const char*)buf, len);
	printf("%zd\n", len);
	for (auto i : subjects)
	{
		printf("   %s\n", i->mName.c_str());
	}
}

int main(int argc, char *argv[])	
{
	if (argc != 3)
	{
		fprintf(stderr, "%s protocol url\n", argv[0]);
		fprintf(stderr, "Protocols: client server\n");
		return 1;
	}

	O3DS::AsyncConnector* connector = nullptr;

	if (strcmp(argv[1], "sub") == 0)
	{
		printf("Connecting to: %s\n", argv[2]);
		connector = new O3DS::AsyncSubscriber();
	}

	if (strcmp(argv[1], "client") == 0)
	{
		printf("Connecting to: %s\n", argv[2]);
		connector = new O3DS::AsyncPairClient();
	}

	if (strcmp(argv[1], "server") == 0)
	{
		printf("Connecting to: %s\n", argv[2]);
		connector = new O3DS::AsyncPairServer();
	}
	
	if (strcmp(argv[1], "ws") == 0)
	{
		printf("Connecting to: %s\n", argv[2]);
		connector = new O3DS::WebsocketClient();
	}

	if (!connector)
	{
		fprintf(stderr, "Invalid Protocol: %s\n", argv[1]);
		return 2;
	}

	connector->setFunc(nullptr, &ReadFunc);

	if (!connector->start(argv[2]))
	{
		fprintf(stderr, "Could not start server: %s\n", connector->getError().c_str());
		return 3;
	}

	using namespace std::literals::chrono_literals;

	while (1)
	{
		nng_msleep(1000);
		printf(".");
		continue;
	}
}
