#include <stdio.h>
//#include "o3ds/async_subscriber.h"
//#include "o3ds/async_request.h"
//#include "o3ds/async_pipeline.h"
#include "o3ds/async_subscriber.h"
#include <nng/nng.h>
#include <chrono>
#include <thread>

#include "o3ds/model.h"
#include "o3ds/subscriber.h"


int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "%s protocol url\n", argv[0]);
		fprintf(stderr, "Protocols: client server\n");
		return 1;
	}

	O3DS::BlockingConnector* connector = nullptr;

	if (strcmp(argv[1], "sub") == 0)
	{
		printf("Connecting to: %s\n", argv[2]);
		connector = new O3DS::Subscriber();
	}

	if (!connector)
	{
		fprintf(stderr, "Invalid Protocol: %s\n", argv[1]);
		return 2;
	}

	if (!connector->start(argv[2]))
	{
		fprintf(stderr, "Could not start server: %s\n", connector->err().c_str());
		return 3;
	}

	using namespace std::chrono_literals;

	char buf[1024 * 12];

	while (1)
	{
		size_t ret = connector->read(buf, 1024 * 12);
		if (ret == 0)
		{
			printf(".");
			continue;
		}

		std::string key;
		O3DS::SubjectList subjects;
		O3DS::Parse(key, subjects, (const char*)buf, ret);
		printf("%s  ", key.c_str());
		printf("%zd\n", ret);
	}
}