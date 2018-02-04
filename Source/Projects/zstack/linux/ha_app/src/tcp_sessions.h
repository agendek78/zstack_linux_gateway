/*
 * tcp_sessions.h
 *
 *  Created on: Feb 5, 2018
 *      Author: andy
 */

#ifndef SOURCE_PROJECTS_ZSTACK_LINUX_HA_APP_SRC_TCP_SESSIONS_H_
#define SOURCE_PROJECTS_ZSTACK_LINUX_HA_APP_SRC_TCP_SESSIONS_H_

#include "types.h"
#include <stdbool.h>

/*******************************************************************************
 * Constants
 ******************************************************************************/
#define INITIAL_CONFIRMATION_TIMEOUT 5000
#define STANDARD_CONFIRMATION_TIMEOUT 1000
#define MAX_TCP_CLIENT_SESSIONS 3

typedef void (* confirmation_processing_cb_t)(pkt_buf_t * pkt, void * arg);

struct server_details_s;

int client_send_packet(pkt_buf_t * pkt, confirmation_processing_cb_t _confirmation_processing_cb, void * _confirmation_processing_arg);
bool client_is_waiting_for_confirmation(void);
int tcp_sessions_add_client(struct server_details_s *server);
void tcp_sessions_confirmation_receive_handler(pkt_buf_t * pkt);

#endif /* SOURCE_PROJECTS_ZSTACK_LINUX_HA_APP_SRC_TCP_SESSIONS_H_ */
