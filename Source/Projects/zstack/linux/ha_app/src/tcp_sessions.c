/*
 * tcp_sessions.c
 *
 *  Created on: Feb 5, 2018
 *      Author: andy
 */

#include "tcp_sessions.h"
#include "tcp_client.h"
#include "idleCb.h"
#include "dbgLog.h"

#include <string.h>
#include <stdbool.h>

static bool waiting_for_confirmation = false;
static confirmation_processing_cb_t confirmation_processing_cb = NULL;
static void * confirmation_processing_arg = NULL;
static server_details_t *servers[MAX_TCP_CLIENT_SESSIONS];
static tu_timer_t confirmation_wait_timer = TIMER_RESET_VALUE;

/*******************************************************************************
 * Functions
 ******************************************************************************/
static void confirmation_timeout_handler(void * arg)
{
  DBG_ERR("TIMEOUT waiting for confirmation");
  waiting_for_confirmation = false;
  confirmation_processing_cb = NULL;

  idleCbCall(true);
}

void tcp_sessions_confirmation_receive_handler(pkt_buf_t * pkt)
{
  waiting_for_confirmation = false;
  tu_kill_timer(&confirmation_wait_timer);

  if (confirmation_processing_cb != NULL)
  {
    DBG_LOG("Calling confirmation callback");
    confirmation_processing_cb(pkt, confirmation_processing_arg);
  }

  idleCbCall(false);
}

int tcp_sessions_add_client(server_details_t *server)
{
  for(int i = 0; i < MAX_TCP_CLIENT_SESSIONS; i++)
  {
    if (servers[i] == NULL)
    {
      servers[i] = server;
      return 0;
    }
  }
  return -1;
}

server_details_t *tcp_sessions_get_server(uint8_t subsystem)
{
  for(int i = 0; i < MAX_TCP_CLIENT_SESSIONS; i++)
  {
    if ((servers[i] != NULL) && (servers[i]->subsystem == subsystem))
    {
      return servers[i];
    }
  }

  return NULL;
}

int client_send_packet(pkt_buf_t * pkt, confirmation_processing_cb_t _confirmation_processing_cb, void * _confirmation_processing_arg)
{
  server_details_t * server = tcp_sessions_get_server(pkt->header.subsystem);

//  if (pkt->header.subsystem == ZSTACK_NWK_MGR_SYS_ID_T__RPC_SYS_PB_NWK_MGR)
//  {
//    server = &network_manager_server;
//  }
//  else if (pkt->header.subsystem == ZSTACK_GW_SYS_ID_T__RPC_SYS_PB_GW)
//  {
//    server = &gateway_server;
//  }
//  else if (pkt->header.subsystem == ZSTACK_OTASYS_IDS__RPC_SYS_PB_OTA_MGR)
//  {
//    server = &ota_server;
//  }


  if (server == NULL)
  {
    DBG_ERR("unknown subsystem ID %d. Following packet discarded:", pkt->header.subsystem);
    return -1;
  }

  if (!server->connected)
  {
    DBG_ERR("Please wait while connecting to server");
    return -1;
  }

  if (waiting_for_confirmation)
  {
    DBG_ERR("BUSY - please wait for previous operation to complete");
    return -1;
  }

  if (tcp_send_packet(server, (uint8_t *)pkt, pkt->header.len + sizeof(pkt_buf_hdr_t)) == -1)
  {
    DBG_ERR("ERROR sending packet to %s\n", server->name);
    return -1;
  }

  DBG_LOG("sent to %s: ", server->name);

  confirmation_processing_cb = _confirmation_processing_cb;
  confirmation_processing_arg = _confirmation_processing_arg;
  waiting_for_confirmation = true;
  tu_set_timer(&confirmation_wait_timer, server->confirmation_timeout_interval, false, confirmation_timeout_handler, NULL);

  if (server->confirmation_timeout_interval != STANDARD_CONFIRMATION_TIMEOUT)
  {
    server->confirmation_timeout_interval = STANDARD_CONFIRMATION_TIMEOUT;
  }

  return 0;
}

bool client_is_waiting_for_confirmation(void)
{
  return waiting_for_confirmation;
}
