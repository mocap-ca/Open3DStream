#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//#include "o3ds/websocket.h"
//O3DS::WebsocketBroadcastServer server1;
//O3DS::WebsocketBroadcastServer server2;

#include "o3ds/async_publisher.h"
#include "o3ds/pair.h"

#define QUOTE_(str) #str
#define QUOTE(str) QUOTE_(str)

namespace O3DS
{
    ServerPair listener;
    AsyncPublisher broadcast;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("O3DS Repeater");
#ifdef VERSION_TAG
        printf(" - %s\n", QUOTE(VERSION_TAG));
#endif
        printf("Usage: %s listen-addr broadcast-addr\n", argv[0]);
        return 1;
    }


    printf("Listening on %s\n", argv[1]);
    if (!O3DS::listener.start(argv[1]))
    {
        printf("Could not start listener on %s: %s\n", argv[1], O3DS::listener.getError().c_str());
        return 2;
    }

    printf("Publishing to %s\n", argv[2]);
    if (!O3DS::broadcast.start(argv[2]))
    {
        printf("Could not start publisher on %s: %s\n", argv[2], O3DS::broadcast.getError().c_str());
        return 3;
    }
    
    size_t bufsz = 1024 * 80;
    char* data = (char*)malloc(bufsz);

    size_t sz;
    
    while (1)
    {
        sz = O3DS::listener.read(&data, &bufsz);
        if (sz > 0)
        {
            printf("%ld\n", sz);
            O3DS::broadcast.write(data, sz);
        }
    }

    free(data);
    return 0;
}
