/*
 * nwkmgrmemdb.c
 *
 *  Created on: Jul 5, 2017
 *      Author: andrzej
 */
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "nwkmgrdatabase.h"
#include "nwkmgrdbfile.h"

#include "hal_types.h"
#include "hal_defs.h"
#include "trace.h"

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))


static sNwkMgrDb_DeviceList_t	devDB =
{
	.count = 0,
	.pDevices = NULL
};

// called once upon init for each app (only the Network Server should consolidate)
bool nwkMgrDb_Init( bool consolidate )
{
	bool rc = TRUE;

	if (nwkMgrDbFile_Load(gszNwkMgrDb_DeviceInfo_c, &devDB) == TRUE)
	{
		uiPrintfEx(trDEBUG, "Loaded device database. Device count %d\n", devDB.count);
	}
	else
	{
		uiPrintfEx(trERROR, "Unable to load device database file!\n");
		memset(&devDB, 0, sizeof(sNwkMgrDb_DeviceList_t));
	}

	return rc;
}

// cleanly shut down the database
void nwkMgrDatabaseShutDown( void )
{
	uiPrintf("%s\n", __FUNCTION__);
}

// clears the database... now empty
void nwkMgrDatabaseReset( void )
{
	uiPrintf("%s\n", __FUNCTION__);
}

int nwkMgrDb_CheckDevicePresence(uint64_t i_ieeeAddr)
{
	int i;

	for(i = 0; i < devDB.count; i++)
	{
		if (devDB.pDevices[i].ieeeAddr == i_ieeeAddr)
		{
			return i;
		}
	}

	return -1;
}


bool nwkMgrDb_UpdateDeviceRecord( sNwkMgrDb_DeviceInfo_t *pDeviceInfo )
{
	bool rc = FALSE;
	int idx;

	idx = nwkMgrDb_CheckDevicePresence(pDeviceInfo->ieeeAddr);
	if (idx >= 0)
	{
		//don't update endpoint list - it will be implemented in the future
		memcpy(&devDB.pDevices[idx], pDeviceInfo, sizeof(sNwkMgrDb_DeviceInfo_t) - sizeof(sNwkMgrDb_Endpoint_t*));
		nwkMgrDbFile_Save(gszNwkMgrDb_DeviceInfo_c, &devDB);
		rc = TRUE;
	}

	return rc;
}

// add a device to the database. Must be filled out. Returns TRUE if added
bool nwkMgrDb_AddDevice( sNwkMgrDb_DeviceInfo_t *pDeviceInfo )
{
	bool rc = FALSE;
	sNwkMgrDb_DeviceInfo_t* newTab;
	int idx;

	uiPrintf("%s\n", __FUNCTION__);

	idx = nwkMgrDb_CheckDevicePresence(pDeviceInfo->ieeeAddr);

	if (idx < 0)
	{
		newTab = malloc(sizeof(sNwkMgrDb_DeviceInfo_t) * (devDB.count + 1));
		if (newTab != NULL)
		{
			if (devDB.count > 0)
			{
				memcpy(newTab, devDB.pDevices, sizeof(sNwkMgrDb_DeviceInfo_t) * devDB.count);
			}
			memcpy(&newTab[devDB.count], pDeviceInfo, sizeof(sNwkMgrDb_DeviceInfo_t));

			rc = TRUE;

			if (pDeviceInfo->aEndpoint != NULL)
			{
				sNwkMgrDb_Endpoint_t* ep = malloc(sizeof(sNwkMgrDb_Endpoint_t) * pDeviceInfo->endpointCount);

				if (ep != NULL)
				{
					int epCnt;

					memcpy(ep, pDeviceInfo->aEndpoint, sizeof(sNwkMgrDb_Endpoint_t) * pDeviceInfo->endpointCount);

					for(epCnt = 0; epCnt < pDeviceInfo->endpointCount; epCnt++)
					{
						if (ep[epCnt].inputClusterCount > 0)
						{
							ep[epCnt].inputClusters = malloc(sizeof(uint16_t) * ep[epCnt].inputClusterCount);
							memcpy(ep[epCnt].inputClusters, pDeviceInfo->aEndpoint[epCnt].inputClusters, sizeof(uint16_t) * ep[epCnt].inputClusterCount);
						}

						if (ep[epCnt].outputClusterCount > 0)
						{
							ep[epCnt].outputClusters = malloc(sizeof(uint16_t) * ep[epCnt].outputClusterCount);
							memcpy(ep[epCnt].outputClusters, pDeviceInfo->aEndpoint[epCnt].outputClusters, sizeof(uint16_t) * ep[epCnt].outputClusterCount);
						}
					}

					newTab[devDB.count].aEndpoint = ep;
				}
				else
				{
					uiPrintfEx(trERROR, "Unable allocate memory for endpoint!\n");
					rc = FALSE;
				}
			}

			if (rc == TRUE)
			{
				devDB.pDevices = newTab;
				devDB.count++;
				nwkMgrDbFile_Save(gszNwkMgrDb_DeviceInfo_c, &devDB);
			}
			else
			{
				free(newTab);
			}
		}
		else
		{
			uiPrintfEx(trERROR, "Unable to allocate memory for new device!\n");
		}
	}
	else
	{
		rc = nwkMgrDb_UpdateDeviceRecord(pDeviceInfo);
	}

	return rc;
}

//  Remove a device from the database. Removes all of its endpoints, attributes and deviceinfo. Returns TRUE if removed.
bool nwkMgrDb_RemoveDevice( uint64_t ieeeAddr )
{
	bool rc = TRUE;

	uiPrintf("%s\n", __FUNCTION__);

	return rc;
}

//  Allocates and returns DeviceInfo structure if found in the database. Returns NULL if not found.
sNwkMgrDb_DeviceInfo_t * nwkMgrDb_GetDeviceInfo( uint64_t ieeeAddr )
{
	int idx;

	idx = nwkMgrDb_CheckDevicePresence(ieeeAddr);
	if (idx < 0)
	{
		return NULL;
	}
	else
	{
		return &devDB.pDevices[idx];
	}
}

void nwkMgrDb_FreeDeviceInfo( sNwkMgrDb_DeviceInfo_t *pDeviceInfo )
{
}

// returns a pointer to the list of all devices in the database. Returns NULL if can't allocate memory.
sNwkMgrDb_DeviceList_t * nwkMgrDb_GetAllDevices( void )
{
	return &devDB;
}

void nwkMgrDb_FreeAllDevices( sNwkMgrDb_DeviceList_t *pDeviceList )
{
}

// Get the short address of a device from the database given its ieeeAddr. Returns TRUE if found.
bool nwkMgrDb_GetShortAddr( uint64_t ieeeAddr, uint16 *pShortAddr )
{
	int idx;

	idx = nwkMgrDb_CheckDevicePresence(ieeeAddr);
	if (idx < 0)
	{
		return FALSE;
	}
	else
	{
		*pShortAddr = devDB.pDevices[idx].nwkAddr;
		return TRUE;
	}
}

// Get the parent address of a device from the database given its ieeeAddr. Returns TRUE if found.
bool nwkMgrDb_GetParentAddr( uint64_t ieeeAddr, uint64_t *pParentIeeeAddr )
{
	int idx;

	idx = nwkMgrDb_CheckDevicePresence(ieeeAddr);
	if (idx < 0)
	{
		return FALSE;
	}
	else
	{
		*pParentIeeeAddr = devDB.pDevices[idx].parentAddr;
		return TRUE;
	}
}

// returns the ieeeAddr of a device from the database given its short address. Returns TRUE if found.
bool nwkMgrDb_GetIeeeAddr( uint16 shortAddr, uint64_t *pIeeeAddr )
{
	int i;

	for(i = 0; i < devDB.count; i++)
	{
		if (devDB.pDevices[i].nwkAddr == shortAddr)
		{
			*pIeeeAddr = devDB.pDevices[i].ieeeAddr;
			return TRUE;
		}
	}

	return FALSE;
}

// get capability info of an existing database record
bool nwkMgrDb_GetCapInfo( uint64_t ieeeAddr, uint8 *pCapInfo)
{
	int idx;

	idx = nwkMgrDb_CheckDevicePresence(ieeeAddr);
	if (idx < 0)
	{
		return FALSE;
	}
	else
	{
		*pCapInfo = devDB.pDevices[idx].capInfo;
		return TRUE;
	}
}

// set capability info for an existing database record
bool nwkMgrDb_SetCapInfo( uint64_t ieeeAddr, uint8 capInfo)
{
	int idx;

	idx = nwkMgrDb_CheckDevicePresence(ieeeAddr);
	if (idx < 0)
	{
		return FALSE;
	}
	else
	{
		devDB.pDevices[idx].capInfo = capInfo;
		nwkMgrDbFile_Save(gszNwkMgrDb_DeviceInfo_c, &devDB);
		return TRUE;
	}
}

// gets the status of a device in the database
bool nwkMgrDb_GetDeviceStatus( uint64_t ieeeAddr, uint8 *pStatus )
{
	int idx;

	idx = nwkMgrDb_CheckDevicePresence(ieeeAddr);
	if (idx < 0)
	{
		return FALSE;
	}
	else
	{
		*pStatus = devDB.pDevices[idx].status;
		return TRUE;
	}
}

// sets the status of a device in the database
bool nwkMgrDb_SetDeviceStatus( uint64_t ieeeAddr, uint8 status )
{
	int idx;

	idx = nwkMgrDb_CheckDevicePresence(ieeeAddr);
	if (idx < 0)
	{
		return FALSE;
	}
	else
	{
		devDB.pDevices[idx].status = status;
		nwkMgrDbFile_Save(gszNwkMgrDb_DeviceInfo_c, &devDB);
		return TRUE;
	}
}
