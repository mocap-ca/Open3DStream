#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "o3ds/pair.h"
#include "o3ds/publisher.h"
#include <nng/nng.h>
#include <nng/supplemental/util/platform.h>

O3DS::Publisher publish;



int main(int argc, char *argv[])
{
    O3DS::ServerPair listen;

    if (argc < 3)
    {
        printf("Usage: %s listen-addr broadcast-addr\n", argv[0]);
        return 1;
    }

    printf("Publishing to %s\n", argv[2]);
    if (!publish.start(argv[2]))
    {
        printf("Could not start publisher\n");
        return 2;
    }

    printf("Listening on %s\n", argv[1]);
    if (!listen.start(argv[1]))
    {
        printf("Could not start listener\n");
        return 3;
    }
    

    char* data = (char*)malloc(1024 * 12);

    size_t sz;
    

    while (1)
    {
        sz = listen.readMsg(data, 1024 * 12);
        if (sz > 0)
        {
            publish.writeMsg(data, sz);
            
            //printf("%d\n", sz);
        }
    }

    return 0;
}