#include <stdio.h>
#include "o3ds/subscriber.h"
#include "o3ds/pair.h"
#include <nng/nng.h>
#include <chrono>
#include <thread>
#include <sstream>

#include "o3ds/model.h"

void ReadFunc(void *ptr, void *buf, size_t len)
{
	printf("DATA: %zu\n", len);
	O3DS::SubjectList subjects;
	subjects.parse((const char*)buf, len);
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
		fprintf(stderr, "Protocols: sub client client server\n");
		return 1;
	}

	O3DS::Connector* connector = nullptr;

	if (strcmp(argv[1], "sub") == 0)
	{
		printf("Connecting to: %s\n", argv[2]);
		connector = new O3DS::Subscriber();
	}

	if (strcmp(argv[1], "client") == 0)
	{
		printf("Connecting to: %s\n", argv[2]);
		connector = new O3DS::ClientPair();
	}

	if (strcmp(argv[1], "server") == 0)
	{
		printf("Connecting to: %s\n", argv[2]);
		connector = new O3DS::ServerPair();
	}
	
	//if (strcmp(argv[1], "ws") == 0)
	//{
		//printf("Connecting to: %s\n", argv[2]);
		//connector = new O3DS::WebsocketClient();
	//}

	if (!connector)
	{
		fprintf(stderr, "Invalid Protocol: %s\n", argv[1]);
		return 2;
	}

	if (!connector->start(argv[2]))
	{
		fprintf(stderr, "Could not start server: %s\n", connector->getError().c_str());
		return 3;
	}

	using namespace std::literals::chrono_literals;

	size_t bufsz = 1024 * 80;
	char *data = (char*)malloc(bufsz);

        O3DS::SubjectList sl;

	int n = 0;

	while (1)	
	{
		size_t ret = connector->read(&data, &bufsz);

		if(ret > 0)
		{
			std::ostringstream oss;
			oss << "data." << n++ << ".dat";
			FILE *fp = fopen(oss.str().c_str(), "wb");
			if(fp)
			{
				fwrite(data, ret, 1, fp);
				fclose(fp);
			}
			printf("Data: %zu\n", ret);

			if(!sl.parse(data, ret)) {
				printf("Could not parse packet: %s\n", sl.mError.c_str());
			}
			else
			{
				for(auto &i : sl.mItems)
				{
					printf("%s has %zu transforms\n", i->mName.c_str(), i->size());
				}
			}
		}
	}
}
