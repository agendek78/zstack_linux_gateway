/*
 * remoteMain.c
 *
 *  Created on: 05.09.2017
 *      Author: Andrzej
 */

#include "tcp_server.h"
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

static tcpServerHandle_t    remoteUiHandle;

static int remoteMainCb(int i_socket, void* i_arg)
{
    int n = 0;
    char buf[16];

    printf("New data received!\n");

    n = read(i_socket, buf, sizeof(buf));
    printf("read %d bytes!\n", n);

    return 0;
}


int remoteMainInit(void)
{
    tcpServerInitParams_t   initP;

    initP.arg = NULL;
    initP.processingFn = remoteMainCb;

    remoteUiHandle = tcpServerNew(&initP);
    if (remoteUiHandle != NULL)
    {
        return 0;
    }

    return -1;
}
