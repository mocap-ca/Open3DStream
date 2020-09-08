#include <stdio.h>
#include "o3ds/async_subscriber.h"

class Sub : public O3DS::AsyncSubscriber
{
	void in_data(const char *msg, size_t len) override
	{
		printf("%ld\n", len);
	}

	void in_pipe()
	{
		printf("PIPE\n");
	}

};



int main(int argc, char *argv[])
{
	Sub s;

	s.connect("tcp://3.131.65.210:6000");

	while (1)
	{
		nng_msleep(1000);
		printf(".");
	}
}