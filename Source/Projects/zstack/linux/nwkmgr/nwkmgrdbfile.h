/*
 * nwkmgrdbfile.h
 *
 *  Created on: Jul 9, 2017
 *      Author: andrzej
 */

#ifndef PROJECTS_ZSTACK_LINUX_NWKMGR_NWKMGRDBFILE_H_
#define PROJECTS_ZSTACK_LINUX_NWKMGR_NWKMGRDBFILE_H_

#include "nwkmgrdatabase.h"

bool nwkMgrDbFile_Load(const char* i_fileName, sNwkMgrDb_DeviceList_t* o_pDeviceDb);
bool nwkMgrDbFile_Save(const char* i_fileName, const sNwkMgrDb_DeviceList_t* i_pDb);


#endif /* PROJECTS_ZSTACK_LINUX_NWKMGR_NWKMGRDBFILE_H_ */
