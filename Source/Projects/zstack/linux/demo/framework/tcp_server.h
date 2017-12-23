/*
 * tcp_server.h
 *
 *  Created on: 26.08.2017
 *      Author: Andrzej
 */

#ifndef SOURCE_PROJECTS_ZSTACK_LINUX_DEMO_FRAMEWORK_TCP_SERVER_H_
#define SOURCE_PROJECTS_ZSTACK_LINUX_DEMO_FRAMEWORK_TCP_SERVER_H_

#include <sys/socket.h>
#include <netinet/in.h>

typedef void*   tcpServerHandle_t;

typedef int (*tcpServerProcessingFn_t)(int i_socket, void* i_arg);

typedef struct
{
    tcpServerProcessingFn_t     processingFn;
    void*                       arg;
} tcpServerInitParams_t;


tcpServerHandle_t tcpServerNew(tcpServerInitParams_t* ip_params);
int               tcpServerDel(tcpServerHandle_t i_handle);

#endif /* SOURCE_PROJECTS_ZSTACK_LINUX_DEMO_FRAMEWORK_TCP_SERVER_H_ */
