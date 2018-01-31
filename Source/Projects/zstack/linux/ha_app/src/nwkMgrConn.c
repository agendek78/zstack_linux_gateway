/*
 * nwkMgrConn.c
 *
 *  Created on: Jan 31, 2018
 *      Author: andy
 */

#include "types.h"
#include "nwkmgr.pb-c.h"
#include "nwkMgrConn.h"
#include "tcp_client.h"
#include "dbgLog.h"

/*******************************************************************************
 * Constants
 ******************************************************************************/
#define INITIAL_CONFIRMATION_TIMEOUT 5000
#define STANDARD_CONFIRMATION_TIMEOUT 1000

#define INIT_STATE_MACHINE_STARTUP_DELAY 1000

/*******************************************************************************
 * Variables
 ******************************************************************************/
static server_details_t network_manager_server;
network_info_t ds_network_status;
bool waiting_for_confirmation = false;

/*******************************************************************************
 * Functions
 ******************************************************************************/
static void nwkMgrConnStateMachine(bool timed_out, void * arg)
{
  static int state = 0;

  if (!network_manager_server.connected)
  {
    state = 4;
  }
  else if ((!timed_out) || (state == 0))
  {
    state++;
  }

  DBG_LOG("Init state %d", state);

  switch (state)
  {
    case 1:
      //si_register_idle_callback(si_init_state_machine, NULL);

      if(!waiting_for_confirmation)
      {
        //nwk_send_info_request();
      }
      else
      {
        state = 0;
      }
      break;
    case 2:
      //device_send_local_info_request();
      break;
    case 3:
      //device_send_list_request();
      break;
    case 4:
      //si_unregister_idle_callback();
      state = 0;
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
      //confirmation_receive_handler(pkt);
      DBG_LOG("Confirmation received!");
      break;
    case NWK_MGR_CMD_ID_T__NWK_ZIGBEE_DEVICE_IND:
      //device_process_change_indication(pkt);
      DBG_LOG("Device indication received!");
      break;
    case NWK_MGR_CMD_ID_T__NWK_ZIGBEE_NWK_READY_IND:
      //nwk_process_ready_ind(pkt);
      DBG_LOG("Network ready received!");
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
    //si_init_state_machine(false, NULL);
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
  if (tcp_new_server_connection(&network_manager_server, address, port, (server_incoming_data_handler_t)nwkMgrConnDataHandler, "NWK_MGR", nwkMgrConnDiscHandler) == -1)
  {
    DBG_ERR("ERROR, wrong network manager server\n");
    return -1;
  }

  return 0;
}
