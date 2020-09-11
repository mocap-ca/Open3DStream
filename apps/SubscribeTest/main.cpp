#include <stdio.h>
//#include "o3ds/async_subscriber.h"
#include "o3ds/async_pair.h"
#include <nng/nng.h>
#include <chrono>
#include <thread>
#include "o3ds/model.h"

class Sub : public O3DS::AsyncPair
{
public:
	Sub();
	void InData(void* msg, size_t len);
	void in_pipe();
};


void func(void *ctx, void *msg, size_t ptr)
{
	static_cast<Sub*>(ctx)->InData(msg, ptr);
}

Sub::Sub()
{ 
	setFunc(this, &func);
}

void Sub::InData(void* msg, size_t len)
{
	std::string key;
	O3DS::SubjectList subjects;
	O3DS::Parse(key, subjects, (const char*)msg, len);
	printf("%s  ", key.c_str());
	printf("%zd\n", len);
}

void Sub::in_pipe()
{
	printf("PIPE\n");
}





int main(int argc, char *argv[])
{
	Sub s;

	//const char *url = "tcp://3.131.65.210:6000";
	const char *url = "tcp://127.0.0.1:6001";

	printf("Connecting to: %s\n", url);
	s.connect(url);

	using namespace std::chrono_literals;

	while (1)
	{
		std::this_thread::sleep_for(2s);
		printf(".");
	}
}