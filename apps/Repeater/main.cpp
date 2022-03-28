#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "o3ds/websocket.h"

O3DS::WebsocketBroadcastServer server1;
O3DS::WebsocketBroadcastServer server2;


int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: %s listen-addr broadcast-addr\n", argv[0]);
        return 1;
    }

    printf("Publishing to %s\n", argv[2]);
    if (!server1.start(argv[2]))
    {
        printf("Could not start publisher\n");
        return 2;
    }

    printf("Listening on %s\n", argv[1]);
    if (!server2.start(argv[1]))
    {
        printf("Could not start listener on %s\n", argv[1]);
        return 3;
    }
    

    char* data = (char*)malloc(1024 * 80);

    size_t sz;
    
    /*
    while (1)
    {
        sz = listen.read(data, 1024 * 80);
        if (sz > 0)
        {
            publish.write(data, sz);
            
            printf("%d\n", sz);
        }
    }

    */

    return 0;
}
