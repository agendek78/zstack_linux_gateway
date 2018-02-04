/*******************************************************************************
 Filename:       tcp_client.h
 Revised:        $Date$
 Revision:       $Revision$

 Description:   Client communication via TCP ports


 Copyright 2013 Texas Instruments Incorporated. All rights reserved.

 IMPORTANT: Your use of this Software is limited to those specific rights
 granted under the terms of a software license agreement between the user
 who downloaded the software, his/her employer (which must be your employer)
 and Texas Instruments Incorporated (the "License").  You may not use this
 Software unless you agree to abide by the terms of the License. The License
 limits your use, and you acknowledge, that the Software may not be modified,
 copied or distributed unless used solely and exclusively in conjunction with
 a Texas Instruments radio frequency device, which is integrated into
 your product.  Other than for the foregoing purpose, you may not use,
 reproduce, copy, prepare derivative works of, modify, distribute, perform,
 display or sell this Software and/or its documentation for any purpose.

 YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
 PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,l
 INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
 NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
 TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
 NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
 LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
 INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
 OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
 OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
 (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

 Should you have any questions regarding your right to use this Software,
 contact Texas Instruments Incorporated at www.TI.com.
*******************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

#include "tcp_client.h"
#include "tcp_sessions.h"
#include "polling.h"
#include "types.h"
#include "dbgLog.h"

/******************************************************************************
 * Constants
 *****************************************************************************/
#define SERVER_RECONNECTION_RETRY_TIME 2000
#define MAX_TCP_PACKET_SIZE 2048

/******************************************************************************
 * Function Prototypes
 *****************************************************************************/
void tcp_socket_event_handler(server_details_t * server_details);
void tcp_socket_reconnect_to_server(server_details_t * server_details);

/******************************************************************************
 * Functions
 *****************************************************************************/
int tcp_send_packet(server_details_t * server_details, uint8_t * buf, int len)
{
	if (write(polling_fds[server_details->fd_index].fd, buf, len) != len)
	{
		return -1;
	}

	return 0;
}

int tcp_new_server_connection(server_details_t * server_details, const char * hostname, u_short port, server_incoming_data_handler_t server_incoming_data_handler, char * name, server_connected_disconnected_handler_t server_connected_disconnected_handler)
{
    struct addrinfo hints;
    struct addrinfo *result;
    int status;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_CANONNAME;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    if ((status = getaddrinfo(hostname, 0, &hints, &result)) != 0)
    {
        fprintf(stderr,"ERROR, no such host as %s. Error %d - %s\n", hostname, status, gai_strerror(status));
        return -1;
    }

#if 0
    struct hostent *server;
	
    server = gethostbyname(hostname);
    if (server == NULL) 
	{
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        return -1;
    }
#endif

	bzero((char *) &server_details->serveraddr, sizeof(server_details->serveraddr));
	server_details->serveraddr.sin_family = AF_INET;

	if (result->ai_family != AF_INET)
	{
	  DBG_ERR("Resolved address isn't supported!");
	  return -1;
	}

	bcopy((char *) &result->ai_addr->sa_data[2], (char *) &server_details->serveraddr.sin_addr.s_addr, 4);
	server_details->serveraddr.sin_port = port;
	server_details->server_incoming_data_handler = server_incoming_data_handler;
	server_details->server_reconnection_timer.in_use = false;
	server_details->name = name;
	server_details->server_connected_disconnected_handler = server_connected_disconnected_handler;
	server_details->connected = false;

	freeaddrinfo(result);

	tcp_sessions_add_client(server_details);
	tcp_socket_reconnect_to_server(server_details);
	
	return 0;
}

int tcp_disconnect_from_server(server_details_t * server)
{
	//TBD
	return 0;
}

int tcp_connect_to_server(server_details_t * server_details)
{
	int fd;
	
	fd = socket(AF_INET, SOCK_STREAM, 0);
	
	DBG_LOG("IP addr %d.%d.%d.%d:%d", server_details->serveraddr.sin_addr.s_addr & 0xFF,
	    (server_details->serveraddr.sin_addr.s_addr >> 8) & 0xFF,
	    (server_details->serveraddr.sin_addr.s_addr >> 16) & 0xFF,
	    (server_details->serveraddr.sin_addr.s_addr >> 24) & 0xFF,
	    ntohs(server_details->serveraddr.sin_port));

	if (fd < 0)
	{
		DBG_ERR("ERROR opening socket");
	}
	else if (connect(fd, (const struct sockaddr *)&server_details->serveraddr, sizeof(server_details->serveraddr)) < 0)
	{
		close(fd);
	}
	else
	{
		if ((server_details->fd_index = polling_define_poll_fd(fd, POLLIN, (event_handler_cb_t)tcp_socket_event_handler, server_details)) != -1)
		{
			DBG_LOG("Successfully connected to %s server", server_details->name);

			server_details->connected = true;

			if (server_details->server_connected_disconnected_handler != NULL)
			{
				server_details->server_connected_disconnected_handler();
			}
			return 0;
		}

		DBG_LOG("ERROR adding poll fd");

		close(fd);
	}
	
	return -1;
}

void tcp_socket_reconnect_to_server(server_details_t * server_details)
{
	if (tcp_connect_to_server(server_details) == -1)
	{
		tu_set_timer(&server_details->server_reconnection_timer, SERVER_RECONNECTION_RETRY_TIME, false , (timer_handler_cb_t)tcp_socket_reconnect_to_server ,server_details);
	}
	else
	{
		//ui_redraw_server_state();
	}
}

void tcp_socket_event_handler(server_details_t * server_details)
{
  char buf[MAX_TCP_PACKET_SIZE];
	pkt_buf_t * pkt_ptr = (pkt_buf_t *)buf;
	int remaining_len;

	bzero(buf, MAX_TCP_PACKET_SIZE);
	remaining_len = recv(polling_fds[server_details->fd_index].fd, buf, MAX_TCP_PACKET_SIZE-1, MSG_DONTWAIT);
	
	if (remaining_len < 0)
	{
		DBG_LOG("ERROR reading from socket (server %s)", server_details->name);
	}
	else if (remaining_len == 0)
	{
		DBG_LOG("Server %s disconnected", server_details->name);
		close(polling_fds[server_details->fd_index].fd);
		polling_undefine_poll_fd(server_details->fd_index);
		server_details->connected = false;
		
		if (server_details->server_connected_disconnected_handler != NULL)
		{
			server_details->server_connected_disconnected_handler();
		}

		tcp_socket_reconnect_to_server(server_details);
	}
	else
	{
		while (remaining_len > 0)
		{
			if (remaining_len < sizeof(pkt_ptr->header))
			{
				DBG_LOG("ERROR: Packet header incomplete. expected_len=%d, actual_len=%d", (int) sizeof(pkt_ptr->header), (int) remaining_len);
			}
			else if (remaining_len < (pkt_ptr->header.len + 4))
			{
				DBG_LOG("ERROR: Packet truncated. expected_len=%d, actual_len=%d", (pkt_ptr->header.len + 4), remaining_len);
			}
			else
			{
				DBG_LOG("received from %s: ", server_details->name);
				
				server_details->server_incoming_data_handler(pkt_ptr, remaining_len);
				
				remaining_len -= (pkt_ptr->header.len + 4);
				pkt_ptr = ((pkt_buf_t *)(((uint8_t *)pkt_ptr) + (pkt_ptr->header.len + 4)));
				
				if (remaining_len > 0)
				{
					DBG_LOG("Additional API command in the same TCP packet");
				}
			}
		}
	}
}
