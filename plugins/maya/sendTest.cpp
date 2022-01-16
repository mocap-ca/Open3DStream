//#include "udpSocket.h"
//#include "item.h"
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <vector>

// for sleep
#include <chrono>
#include <thread>

UdpClient *client = NULL;

bool run = true;

void intHandler( int x )
{
    run = false;
}


int main(int argc, char *argv[])
{
#ifdef _WIN32
	int iResult;

	WSADATA wsaData;
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}
#endif

    if( argc != 3 )
    {
        fprintf(stderr, "Usage: %s host port\n", argv[0]);
        return 1;
    }

    char* host = argv[1];
    int   port = atoi(argv[2]);

    if(port < 1024)
    {
        fprintf(stderr, "Invalid port\n");
        return 2;
    }

    signal(SIGINT, intHandler );

    client = new UdpClient();

    if(!client->create()) return 1;
    printf("\n");

    char buffer[1024];

	Segment *s1 = new Segment("test1", 0.0, 1.0, 2.0, 0.0, 0.0, 0.0, 1.0);
	Segment *s2 = new Segment("test2", 1.0, 2.0, 5.0, 0.0, 0.0, 0.0, 1.0);

    std::vector< Item* > items;
    items.push_back( s1 );
    items.push_back( s2 );


    float testval = 0.0f;


    int c = 0;

    while(run)
    {

        s1->ty = testval * 2;
        s2->tx = testval * 3;
        testval += 0.1f;
        if(testval > 10.0f) testval = 0.0f;

        size_t sz = serializeItems( items, buffer, 1024);


        if (c++ == 24)
        {
            printf("%f ", testval);
            fflush( stdout );
            c = 0;
        }
        client->sendDatagram( host, port, buffer, sz);

		std::this_thread::sleep_for(std::chrono::milliseconds(1000/24));
    }

	delete s1;
	delete s2;
    

    printf("Closing socket\n");
    delete client;

    
#ifdef _WIN32
	WSACleanup();
#endif

}
