#ifndef AVAGO_FW_LOAD__H_
#define AVAGO_FW_LOAD_H_

#ifdef FW_DOWNLOAD_FROM_SERVER
const char *serdesFileName = "/usr/bin/serdes.0x104A_0045.rom";
const char *sbusMasterFileName = "/usr/bin/sbus_master.0x100F_8001.rom";
const char *serdesSwapFileName = NULL;
#else

#include "serdes.0x104A_0045.h"
#include "sbus_master.0x100F_8001.h"

const int *sbusMasterFwPtr = sbusMaster01x100F_8001Data;
const int *serdesFwPtr = serdes0x104A_0045Data;

#ifdef AVAGO_FW_SWAP_IMAGE_EXIST
GT_U32 *serdesFwDataSwapPtr;
#endif /*AVAGO_FW_SWAP_IMAGE_EXIST*/

#endif /*FW_DOWNLOAD_FROM_SERVER*/
#endif /*AVAGO_FW_Data_H_H_*/
