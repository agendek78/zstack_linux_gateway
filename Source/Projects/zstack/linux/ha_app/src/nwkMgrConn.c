/*
 * nwkMgrConn.c
 *
 *  Created on: Jan 31, 2018
 *      Author: andy
 */

#include "types.h"
#include "nwkmgr.pb-c.h"
#include "network_info_engine.h"
#include "device_list_engine.h"
#include "nwkMgrConn.h"
#include "tcp_client.h"
#include "tcp_sessions.h"
#include "dbgLog.h"
#include "idleCb.h"

/*******************************************************************************
 * Types
 ******************************************************************************/
typedef enum
{
  nwkMgrState_INIT,
  nwkMgrState_NWK_INFO,
  nwkMgrState_DEV_LOCAL_INFO,
  nwkMgrState_DEV_LIST,
  nwkMgrState_NWK_CONNECT
} nwkMgrState_t;

/*******************************************************************************
 * Constants
 ******************************************************************************/
#define INIT_STATE_MACHINE_STARTUP_DELAY 1000

/*******************************************************************************
 * Variables
 ******************************************************************************/
static server_details_t network_manager_server;
network_info_t ds_network_status;


/*******************************************************************************
 * Functions
 ******************************************************************************/
static void nwkMgrConnStateMachine(bool timed_out, void * arg)
{
  static nwkMgrState_t state = nwkMgrState_INIT;

  if (!network_manager_server.connected)
  {
    state = nwkMgrState_NWK_CONNECT;
  }
  else if ((!timed_out) || (state == nwkMgrState_INIT))
  {
    state++;
  }

  DBG_LOG("Init state %d", state);

  switch (state)
  {
    case nwkMgrState_NWK_INFO:
      IDLE_CB_REG(nwkMgrConnStateMachine, NULL, __func__);

      if(!client_is_waiting_for_confirmation())
      {
        nwk_send_info_request();
      }
      else
      {
        state = nwkMgrState_INIT;
      }
      break;
    case nwkMgrState_DEV_LOCAL_INFO:
      device_send_local_info_request();
      break;
    case nwkMgrState_DEV_LIST:
      device_send_list_request();
      break;
    case nwkMgrState_NWK_CONNECT:
      IDLE_CB_UNREG(__func__);
      state = nwkMgrState_INIT;
      break;
    default:
      break;
  }
}

static void init_state_machine_startup_handler(void * arg)
{
  nwkMgrConnStateMachine(false, NULL);
}

static void nwkMgrConnDataHandler(pkt_buf_t * pkt, int len)
{
  switch (pkt->header.cmd_id)
  {
    case NWK_MGR_CMD_ID_T__ZIGBEE_GENERIC_CNF:
    case NWK_MGR_CMD_ID_T__NWK_ZIGBEE_SYSTEM_RESET_CNF:
    case NWK_MGR_CMD_ID_T__NWK_SET_ZIGBEE_POWER_MODE_CNF:
    case NWK_MGR_CMD_ID_T__NWK_GET_LOCAL_DEVICE_INFO_CNF:
    case NWK_MGR_CMD_ID_T__NWK_ZIGBEE_NWK_INFO_CNF:
    case NWK_MGR_CMD_ID_T__NWK_GET_NWK_KEY_CNF:
    case NWK_MGR_CMD_ID_T__NWK_GET_DEVICE_LIST_CNF:
      DBG_LOG("Confirmation received - 0x%02x!", pkt->header.cmd_id);
      tcp_sessions_confirmation_receive_handler(pkt);
      break;
    case NWK_MGR_CMD_ID_T__NWK_ZIGBEE_DEVICE_IND:
      device_process_change_indication(pkt);
      DBG_LOG("Device indication received!");
      break;
    case NWK_MGR_CMD_ID_T__NWK_ZIGBEE_NWK_READY_IND:
      nwk_process_ready_ind(pkt);
      break;
    case NWK_MGR_CMD_ID_T__NWK_SET_BINDING_ENTRY_RSP_IND:
      //comm_device_binding_entry_request_rsp_ind(pkt);
      DBG_LOG("Binding set entry response received!");
      break;
    default:
      DBG_LOG("Unsupported incoming command id from nwk manager server (cmd_id %d)", (pkt->header.cmd_id));
      break;
  }
}

static void nwkMgrConnDiscHandler(void)
{
  static tu_timer_t init_state_machine_timer = {0,0,0,0,0};

  DBG_LOG("nwk_mgr_server_connected_disconnected_handler");
  network_manager_server.confirmation_timeout_interval = INITIAL_CONFIRMATION_TIMEOUT;
  if (!network_manager_server.connected)
  {
    tu_kill_timer(&init_state_machine_timer);
    nwkMgrConnStateMachine(false, NULL);
    ds_network_status.state = ZIGBEE_NETWORK_STATE_UNAVAILABLE;
    //ui_redraw_network_info();
  }
  else
  {
    tu_set_timer(&init_state_machine_timer, INIT_STATE_MACHINE_STARTUP_DELAY, false, init_state_machine_startup_handler, NULL);
  }
}

int nwkMgrConnInit(const char *address, int port)
{
  network_manager_server.subsystem = ZSTACK_NWK_MGR_SYS_ID_T__RPC_SYS_PB_NWK_MGR;
  if (tcp_new_server_connection(&network_manager_server, address, port, (server_incoming_data_handler_t)nwkMgrConnDataHandler, "NWK_MGR", nwkMgrConnDiscHandler) == -1)
  {
    DBG_ERR("ERROR, wrong network manager server\n");
    return -1;
  }

  return 0;
}
