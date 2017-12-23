/*
 * tcp_server.c
 *
 *  Created on: 26.08.2017
 *      Author: Andrzej
 */
#include "tcp_server.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>

#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#ifndef TRUE
#define TRUE true
#endif

#ifndef FALSE
#define FALSE false
#endif

#define APIS_CONNECTION_QUEUE_SIZE  4

#define uiPrintfEx(dbg, ...)    printf(__VA_ARGS__)
#define uiPrintf(...)           printf(__VA_ARGS__)

typedef struct _connectioninfo_t
{
  struct _connectioninfo_t *next;
  bool garbage;
  int sock;
} ConnectionInfo_t;

typedef struct
{
    pthread_t                   threadHandle;
    tcpServerProcessingFn_t     procFn;
    void*                       procArg;
    bool                        canWork;
    int                         listenPort;
    struct addrinfo*            listenServerInfo;
    struct sockaddr_in          listenAddressInfo;
    uint32_t                    listenSocketHndl;
    fd_set                      activeConnectionsFDs;

    ConnectionInfo_t            *activeConnectionList;
    int                         activeConnectionCount;
} tcpServerInstanceData_t;

static int tcpServerSetupAddress(tcpServerInstanceData_t* ip_handle)
{
    int status;
    struct addrinfo hints;
    char            strBuffer[16];

    do
    {
        // Check initialization and parameters
        memset(&hints, 0, sizeof(hints));

        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        uiPrintfEx(trDEBUG, "Port: %d\n", ip_handle->listenPort);
        sprintf(strBuffer, "%d", ip_handle->listenPort);

        if ((status = getaddrinfo(NULL, strBuffer, &hints, &ip_handle->listenServerInfo)) != 0)
        {
            uiPrintfEx(trERROR, "getaddrinfo error: %s\n", gai_strerror(status));
            break;
        }

        uiPrintfEx(trDEBUG, "Following IP addresses are available:\n\n");
        {
            struct ifaddrs * ifAddrStruct = NULL;
            struct ifaddrs * ifa = NULL;
            void * tmpAddrPtr = NULL;

            getifaddrs(&ifAddrStruct);

            for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
            {
                if (ifa->ifa_addr->sa_family == AF_INET)
                { // check it is IP4
                    char addressBuffer[INET_ADDRSTRLEN];
                    // is a valid IP4 Address
                    tmpAddrPtr = &((struct sockaddr_in *) ifa->ifa_addr)->sin_addr;
                    inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
                    uiPrintfEx(trDEBUG, " IPv4: interface: %s\t IP Address %s\n", ifa->ifa_name, addressBuffer);
                }
                else if (ifa->ifa_addr->sa_family == AF_INET6)
                { // check it is IP6
                    char addressBuffer[INET6_ADDRSTRLEN];

                    // is a valid IP6 Address
                    tmpAddrPtr = &((struct sockaddr_in6 *) ifa->ifa_addr)->sin6_addr;
                    inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
                    uiPrintfEx(trDEBUG, " IPv6: interface: %s\t IP Address %s\n", ifa->ifa_name, addressBuffer);
                }
            }

            if (ifAddrStruct != NULL)
            {
                freeifaddrs(ifAddrStruct);
            }
        }

        uiPrintfEx(trDEBUG, "The socket will listen on the following IP addresses:\n\n" );
        {
            struct addrinfo *p;
            char ipstr[INET6_ADDRSTRLEN];

            for (p = ip_handle->listenServerInfo; p != NULL; p = p->ai_next)
            {
                void *addr;
                char *ipver;

                // get the pointer to the address itself,
                // different fields in IPv4 and IPv6:
                if ( p->ai_family == AF_INET )
                { // IPv4
                    struct sockaddr_in *ipv4 = (struct sockaddr_in *) p->ai_addr;
                    addr = &(ipv4->sin_addr);
                    ipver = "IPv4";
                }
                else
                { // IPv6
                    struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) p->ai_addr;
                    addr = &(ipv6->sin6_addr);
                    ipver = "IPv6";
                }

                // convert the IP to a string and print it:
                inet_ntop( p->ai_family, addr, ipstr, sizeof ipstr );
                uiPrintfEx(trDEBUG, "  %s: %s\n", ipver, ipstr );
            }
        }
        uiPrintfEx(trDEBUG, "0.0.0.0 means it will listen to all available IP address\n\n" );

        status = 0;
    } while (0);

    return status;
}


static int tcpServerCreateListeningSocket(tcpServerInstanceData_t* ip_handle)
{
  int yes = 1;
  int apisErrorCode = 0;


  do
  {
#if 0
      apisErrorCode = tcpServerSetupAddress(ip_handle);
      if (apisErrorCode != 0)
      {
          break;
      }
#endif

      ip_handle->listenAddressInfo.sin_family = AF_INET;
      ip_handle->listenAddressInfo.sin_port = htons(8011);
      ip_handle->listenAddressInfo.sin_addr.s_addr = INADDR_ANY;

      ip_handle->listenSocketHndl = socket(AF_INET, SOCK_STREAM, 0);

      // avoid "Address already in use" error message
      if (setsockopt(ip_handle->listenSocketHndl, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
      {
        perror( "setsockopt" );
        apisErrorCode = -1; //APIS_ERROR_IPC_SOCKET_SET_REUSE_ADDRESS;
        break;
      }

      // Bind socket
      if (bind(ip_handle->listenSocketHndl, (const struct sockaddr *)&ip_handle->listenAddressInfo, sizeof(struct sockaddr_in) ) == -1 )
      {
        perror( "bind" );
        apisErrorCode = -2; //APIS_ERROR_IPC_SOCKET_BIND;
        break;
      }

      // Listen, allow 4 connections in the queue
      if (listen(ip_handle->listenSocketHndl, APIS_CONNECTION_QUEUE_SIZE ) == -1 )
      {
        perror( "listen" );
        apisErrorCode = -3;
        break;
      }

      // Clear file descriptor sets
      FD_ZERO( &ip_handle->activeConnectionsFDs );

      // Add the listener to the set
      FD_SET(ip_handle->listenSocketHndl, &ip_handle->activeConnectionsFDs );


      apisErrorCode = 0;

  } while (0);

  return (apisErrorCode);
}

static int addToActiveList(tcpServerInstanceData_t* ip_handle, int c )
{
  if ( ip_handle->activeConnectionCount <= APIS_CONNECTION_QUEUE_SIZE )
  {
    // Entry at position activeConnections.size is always the last available entry
    ConnectionInfo_t *newinfo = malloc( sizeof(ConnectionInfo_t) );
    newinfo->next = ip_handle->activeConnectionList;
    newinfo->garbage = FALSE;
    //pthread_mutex_init( &newinfo->mutex, NULL );
    newinfo->sock = c;
    ip_handle->activeConnectionList = newinfo;

    // Increment size
    ip_handle->activeConnectionCount++;

    //pthread_mutex_unlock( &aclMutex );

    return 0;
  }

  return -1;
}

static void dropActiveConnection(tcpServerInstanceData_t* ip_handle, int connection )
{
  ConnectionInfo_t *entry;

  entry = ip_handle->activeConnectionList;
  while ( entry )
  {
    if (entry->sock == connection )
    {
      entry->garbage = TRUE;
      break;
    }
    entry = entry->next;
  }
}

static void* tcpServerFn(void* arg)
{
    tcpServerInstanceData_t*    handle = (tcpServerInstanceData_t*)arg;
    int         fdmax, retval, c, justConnected;
    fd_set      activeConnectionsFDsSafeCopy;
    struct timeval timeout;
    struct sockaddr_storage their_addr;

    //trace_init_thread("TCPS");

    if (tcpServerCreateListeningSocket(handle) == 0)
    {
        handle->canWork = true;
        fdmax = handle->listenSocketHndl;
    }
    else
    {
        handle->canWork = false;
    }

    while (handle->canWork == true)
    {
        activeConnectionsFDsSafeCopy = handle->activeConnectionsFDs;

        timeout.tv_usec = 1000 * 1000 * 10;
        timeout.tv_sec = 0;

        // First use select to find activity on the sockets
        retval = select(fdmax + 1, &activeConnectionsFDsSafeCopy, NULL, NULL, &timeout);

        if (retval == -1)
        {
            perror("select()");
        }
        else if (retval)
        {
            uiPrintf("Data is available now.\n");
            /* FD_ISSET(0, &rfds) will be true. */
        }
        else
        {
            //timeout
            continue;
        }

        // Then process this activity
        for (c = 0; c <= fdmax; c++)
        {
            if (FD_ISSET(c, &activeConnectionsFDsSafeCopy))
            {
                if (c == handle->listenSocketHndl)
                {
                    int addrLen = 0;

                    uiPrintf("Accepting new connection\n");
                    // Accept a connection from a client.
                    addrLen = sizeof(their_addr);
                    justConnected = accept(handle->listenSocketHndl,
                                           (struct sockaddr *) &their_addr,
                                           (socklen_t *) &addrLen);
                    if (justConnected == -1)
                    {
                        perror("accept");

                        break;
                    }
                    else
                    {
                        char ipstr[INET6_ADDRSTRLEN];
                        char ipstr2[INET6_ADDRSTRLEN];

                        retval = addToActiveList(handle, justConnected);
                        if (retval != 0)
                        {
                            // Adding to the active connection list failed.
                            // Close the accepted connection.
                            close(justConnected);
                        }
                        else
                        {
                            FD_SET(justConnected, &handle->activeConnectionsFDs);
                            if (justConnected > fdmax)
                            {
                                fdmax = justConnected;
                            }

                            //                                            debug_
                            inet_ntop(
                                    AF_INET,
                                    &((struct sockaddr_in *) &their_addr)->sin_addr,
                                    ipstr, sizeof ipstr);
                            inet_ntop(
                                    AF_INET6,
                                    &((struct sockaddr_in6 *) &their_addr)->sin6_addr,
                                    ipstr2, sizeof ipstr2);

                            uiPrintfEx(trDEBUG, "Connected to #%d.(%s / %s)\n",
                                       justConnected, ipstr, ipstr2);
                        }
                    }
                }
                else
                {
                    retval = handle->procFn(c, handle->procArg);
                    if (retval < 0)
                    {
                        // Everything is ok
                        close(c);
                        uiPrintfEx(trDEBUG, "%s(%d): Removing connection #%d\n",
                                 __FUNCTION__, __LINE__, c);

                        // Connection closed. Remove from set
                        FD_CLR(c, &handle->activeConnectionsFDs);

                        // We should now set ret to APIS_LNX_SUCCESS, but there is still one fatal error
                        // possibility so simply set ret = to return value from removeFromActiveList().
                        dropActiveConnection(handle, c);
                    }
                }
            }
        }
    }

    uiPrintf("TCP server exited!");

    return NULL;
}


tcpServerHandle_t tcpServerNew(tcpServerInitParams_t* ip_params)
{
    tcpServerInstanceData_t*  ret = NULL;
    pthread_attr_t      attr;

    do
    {
        // prepare thread creation
        if (pthread_attr_init( &attr ))
        {
          perror( "pthread_attr_init" );
          break;
        }

        if (pthread_attr_setstacksize( &attr, PTHREAD_STACK_MIN))
        {
          perror( "pthread_attr_setstacksize" );
          break;
        }

        ret = (tcpServerInstanceData_t*)malloc(sizeof(tcpServerInstanceData_t));
        if (ret != NULL)
        {
            ret->procFn = ip_params->processingFn;
            ret->procArg = ip_params->arg;

            // Create thread for listening
            if (pthread_create(&ret->threadHandle, &attr, tcpServerFn, (void*)ret))
            {
              // thread creation failed
              uiPrintf( "Failed to create an API Server Listening thread\n" );
              free(ret);
              ret = NULL;
              break;
            }

            uiPrintf("Started new TCP server!\n");
        }
    } while (0);

    return (tcpServerHandle_t)ret;
}

int tcpServerDel(tcpServerHandle_t i_handle)
{
    if (i_handle != NULL)
    {
        tcpServerInstanceData_t*    instance = (tcpServerInstanceData_t*)i_handle;
        void*   retVal;

        instance->canWork = false;
        pthread_join(instance->threadHandle, &retVal);
        free(i_handle);

        return 0;
    }

    return -1;
}
