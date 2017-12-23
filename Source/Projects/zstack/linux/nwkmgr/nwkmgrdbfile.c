/*
 * nwkmgrdbfile.c
 *
 *  Created on: Jul 9, 2017
 *      Author: andrzej
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "nwkmgrdatabase.h"

#include "hal_types.h"
#include "hal_defs.h"
#include "trace.h"

typedef struct __attribute__((__packed__))
{
	uint32_t	fileTypeId;
	uint16_t	deviceRecordsCnt;
} nwkMgrDbFileHeader_t;

#define MAKE_DB_ID(a, b, c, d)	((uint32_t)(a) | (uint32_t)(b << 8) | (uint32_t)(c << 16) | (uint32_t)(d << 24))
#define DB_FILE_TYPE_ID		MAKE_DB_ID('Z', 'B', 'D', 'B')

#define DB_INITIAL_DEV_CNT		10
#define DB_INITIAL_ENDP_CNT		(DB_INITIAL_DEV_CNT * 5)
#define DB_INITIAL_CLST_CNT		(DB_INITIAL_ENDP_CNT * 8)

static void dbFreeRecords(sNwkMgrDb_DeviceList_t* i_pDeviceDb)
{
	int i;

	for(i = 0; i < i_pDeviceDb->count; i++)
	{
		sNwkMgrDb_DeviceInfo_t*	dev = &i_pDeviceDb->pDevices[i];
		int k;

		for(k = 0; k < dev->endpointCount; k++)
		{
			sNwkMgrDb_Endpoint_t*	ep = &dev->aEndpoint[k];
			if (ep->inputClusters != NULL)
			{
				free(ep->inputClusters);
			}

			if (ep->outputClusters != NULL)
			{
				free(ep->outputClusters);
			}
		}
		if (dev->aEndpoint != NULL)
		{
			free(dev->aEndpoint);
		}
	}

	if (i_pDeviceDb->pDevices != NULL)
	{
		free(i_pDeviceDb->pDevices);
	}

	memset(i_pDeviceDb, 0, sizeof(sNwkMgrDb_DeviceList_t));
}

bool nwkMgrDbFile_Load(const char* i_fileName, sNwkMgrDb_DeviceList_t* o_pDeviceDb)
{
	FILE*	f = NULL;
	bool 	rc = FALSE;

	f = fopen(i_fileName, "rb");
	if (f != NULL)
	{
		nwkMgrDbFileHeader_t	header;
		int	num;

		dbFreeRecords(o_pDeviceDb);

		num = fread(&header, sizeof(nwkMgrDbFileHeader_t), 1, f);
		if (num == 1 && header.fileTypeId == DB_FILE_TYPE_ID)
		{
			o_pDeviceDb->pDevices = calloc(header.deviceRecordsCnt, sizeof(sNwkMgrDb_DeviceInfo_t));
			while(o_pDeviceDb->pDevices != NULL && o_pDeviceDb->count < header.deviceRecordsCnt)
			{
				sNwkMgrDb_DeviceInfo_t*	dev = &o_pDeviceDb->pDevices[o_pDeviceDb->count];

				if (fread(dev, sizeof(sNwkMgrDb_DeviceInfo_t) - sizeof(dev->aEndpoint), 1, f) > 0)
				{
					if (dev->endpointCount > 0)
					{
						dev->aEndpoint = malloc((sizeof(sNwkMgrDb_Endpoint_t)) * dev->endpointCount);

						if (dev->aEndpoint != NULL)
						{
							int epNum;

							for(epNum = 0; epNum < dev->endpointCount; epNum++)
							{
								sNwkMgrDb_Endpoint_t*	ep = &dev->aEndpoint[epNum];

								if (fread(ep, sizeof(sNwkMgrDb_Endpoint_t) - sizeof(uint16_t*) * 2, 1, f) > 0)
								{
									if (ep->inputClusterCount > 0)
									{
										ep->inputClusters = malloc(sizeof(uint16_t) * ep->inputClusterCount);
										if(fread(ep->inputClusters, sizeof(uint16_t), ep->inputClusterCount, f) != ep->inputClusterCount)
										{
											//error
											break;
										}
									}
									else
									{
										ep->inputClusters = NULL;
									}

									if (ep->outputClusterCount > 0)
									{
										ep->outputClusters = malloc(sizeof(uint16_t) * ep->outputClusterCount);
										if(fread(ep->outputClusters, sizeof(uint16_t), ep->outputClusterCount, f) != ep->outputClusterCount)
										{
											//error
											break;
										}
									}
									else
									{
										ep->outputClusters = NULL;
									}
								}
								else
								{
									//error
									break;
								}
							}
						}
						else
						{
							//error
							break;
						}
					}
					else
					{
						dev->aEndpoint = NULL;
					}
					o_pDeviceDb->count++;
				}
				else
				{
					//error
					break;
				}
			}

			rc = TRUE;
		}

		fclose(f);
	}

	return rc;
}

bool nwkMgrDbFile_Save(const char* i_fileName, const sNwkMgrDb_DeviceList_t* i_pDb)
{
	FILE*	f = NULL;
	bool 	rc = FALSE;

	f = fopen(i_fileName, "wb");
	if (f != NULL)
	{
		nwkMgrDbFileHeader_t	header = {DB_FILE_TYPE_ID, i_pDb->count};

		if (fwrite(&header, sizeof(nwkMgrDbFileHeader_t), 1, f) > 0)
		{
			int i;

			rc = TRUE;

			for(i = 0; i < i_pDb->count; i++)
			{
				sNwkMgrDb_DeviceInfo_t*	dev = &i_pDb->pDevices[i];

				if (fwrite(dev, sizeof(sNwkMgrDb_DeviceInfo_t) - sizeof(dev->aEndpoint), 1, f) > 0)
				{
					int k;
					for(k = 0; k < dev->endpointCount; k++)
					{
						sNwkMgrDb_Endpoint_t*	ep = &dev->aEndpoint[k];
						if (fwrite(ep, sizeof(sNwkMgrDb_Endpoint_t) - sizeof(uint16_t*) * 2, 1, f) > 0)
						{
							if (ep->inputClusterCount > 0)
							{
								if (fwrite(ep->inputClusters, sizeof(uint16_t), ep->inputClusterCount, f) != ep->inputClusterCount)
								{
									//error
									rc = FALSE;
									break;
								}
							}

							if (ep->outputClusterCount > 0)
							{
								if (fwrite(ep->outputClusters, sizeof(uint16_t), ep->outputClusterCount, f) != ep->outputClusterCount)
								{
									//error
									rc = FALSE;
									break;
								}
							}
						}
						else
						{
							//error
							rc = FALSE;
							break;
						}
					}
				}
				else
				{
					//error
					rc = FALSE;
					break;
				}
			}
		}
		else
		{
			//error
		}

		fclose(f);
	}

	return rc;
}

