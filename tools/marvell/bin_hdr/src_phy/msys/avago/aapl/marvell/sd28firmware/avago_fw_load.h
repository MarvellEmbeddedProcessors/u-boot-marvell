#ifndef AVAGO_FW_LOAD__H_
#define AVAGO_FW_LOAD_H_

#ifdef FW_DOWNLOAD_FROM_SERVER
const char *serdesFileName = "/usr/bin/serdes.0x1055_0045.rom";
const char *sbusMasterFileName = "/usr/bin/sbus_master.0x1015_0001.rom";
const char *serdesSwapFileName = NULL;
#else

#include "serdes.0x1055_0045.h"
#include "sbus_master.0x1015_0001.h"

#define AVAGO_FW_SWAP_IMAGE_EXIST
#ifdef AVAGO_FW_SWAP_IMAGE_EXIST
#include "serdes.0x1055_0045swap.h"
#endif /*AVAGO_FW_SWAP_IMAGE_EXIST*/

#endif /*FW_DOWNLOAD_FROM_SERVER*/
#endif /*AVAGO_FW_Data_H_H_*/


