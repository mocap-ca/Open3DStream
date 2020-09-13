#include <stdio.h>
//#include "o3ds/async_subscriber.h"
#include "o3ds/async_subscriber.h"
#include <nng/nng.h>
#include <chrono>
#include <thread>
#include "o3ds/model.h"


void InData(void *ref, void* msg, size_t len)
{
	std::string key;
	O3DS::SubjectList subjects;
	O3DS::Parse(key, subjects, (const char*)msg, len);
	printf("%s  ", key.c_str());
	printf("%zd\n", len);
}


int main(int argc, char *argv[])
{
	
	O3DS::AsyncSubscriber sub;
	sub.setFunc(nullptr, InData);

	//const char *url = "tcp://3.131.65.210:6000";
	const char *url = "tcp://127.0.0.1:6001";

	printf("Connecting to: %s\n", url);
	sub.start(url);

	using namespace std::chrono_literals;

	while (1)
	{
		std::this_thread::sleep_for(2s);
		printf(".");
	}
}