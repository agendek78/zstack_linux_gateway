/*
 * nwkMgrConn.h
 *
 *  Created on: Jan 31, 2018
 *      Author: andy
 */

#ifndef SOURCE_PROJECTS_ZSTACK_LINUX_HA_APP_SRC_NWKMGRCONN_H_
#define SOURCE_PROJECTS_ZSTACK_LINUX_HA_APP_SRC_NWKMGRCONN_H_

#include "types.h"

extern network_info_t ds_network_status;

int nwkMgrConnInit(const char *address, int port);

#endif /* SOURCE_PROJECTS_ZSTACK_LINUX_HA_APP_SRC_NWKMGRCONN_H_ */
