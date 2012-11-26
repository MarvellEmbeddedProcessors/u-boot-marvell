/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell 
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File in accordance with the terms and conditions of the General 
Public License Version 2, June 1991 (the "GPL License"), a copy of which is 
available along with the File in the license.txt file or by writing to the Free 
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or 
on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY 
DISCLAIMED.  The GPL License provides additional details about this warranty 
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File under the following licensing terms. 
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer. 

    *   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution. 

    *   Neither the name of Marvell nor the names of its contributors may be 
        used to endorse or promote products derived from this software without 
        specific prior written permission. 
    
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#ifndef __INCmvDram
#define __INCmvDram

#include "mv_os.h"

/* Registered DIMM Support - In case registered DIMM is attached, please supply the following values:
(see JEDEC - JESD82-29A "Definition of the SSTE32882 Registering Clock Driver with Parity and Quad Chip
Selects for DDR3/DDR3L/DDR3U RDIMM 1.5 V/1.35 V/1.25 V Applications") */
/* RC0: Global Features Control Word */
/* RC1: Clock Driver Enable Control Word */
/* RC2: Timing Control Word */
/* RC3-RC5 - taken from SPD */
/* RC8: Additional IBT Setting Control Word */
/* RC9: Power Saving Settings Control Word */
/* RC10: Encoding for RDIMM Operating Speed */
/* RC11: Operating Voltage VDD and VREFCA Control Word */
#define RDIMM_RC0								0
#define RDIMM_RC1								0
#define RDIMM_RC2								0
#define RDIMM_RC8								0
#define RDIMM_RC9								0
#define RDIMM_RC10								0
#define RDIMM_RC11								0


#define MAX_DIMM_NUM 									2
#define SPD_SIZE										128

/* Dimm spd offsets */
#define SPD_DEV_TYPE_BYTE								2

#define SPD_MODULE_TYPE_BYTE							3
#define SPD_MODULE_MASK									0xf
#define SPD_MODULE_TYPE_RDIMM							1
#define SPD_MODULE_TYPE_UDIMM							2

#define SPD_DEV_DENSITY_BYTE 							4
#define SPD_DEV_DENSITY_MASK 							0xf

#define SPD_ROW_NUM_BYTE 								5
#define SPD_ROW_NUM_MIN 								12
#define SPD_ROW_NUM_OFF		 							3
#define SPD_ROW_NUM_MASK 								(7 << SPD_ROW_NUM_OFF)

#define SPD_COL_NUM_BYTE				 				5
#define SPD_COL_NUM_MIN 								9
#define SPD_COL_NUM_OFF					 				0
#define SPD_COL_NUM_MASK 								(7 << SPD_COL_NUM_OFF)

#define SPD_MODULE_ORG_BYTE		 						7
#define SPD_MODULE_SDRAM_DEV_WIDTH_OFF 					0
#define SPD_MODULE_SDRAM_DEV_WIDTH_MASK					(7 << SPD_MODULE_SDRAM_DEV_WIDTH_OFF)
#define SPD_MODULE_BANK_NUM_MIN 						1
#define SPD_MODULE_BANK_NUM_OFF 						3
#define SPD_MODULE_BANK_NUM_MASK						(7 << SPD_MODULE_BANK_NUM_OFF)

#define SPD_BUS_WIDTH_BYTE 								8
#define SPD_BUS_WIDTH_OFF 								0
#define SPD_BUS_WIDTH_MASK								(7 << SPD_BUS_WIDTH_OFF)
#define SPD_BUS_ECC_OFF 								3
#define SPD_BUS_ECC_MASK								(3 << SPD_BUS_ECC_OFF)

#define SPD_MTB_DIVIDEND_BYTE				 			10
#define SPD_MTB_DIVISOR_BYTE					 		11
#define SPD_TCK_BYTE									12
#define SPD_SUP_CAS_LAT_LSB_BYTE						14
#define SPD_SUP_CAS_LAT_MSB_BYTE						15
#define SPD_TAA_BYTE					 				16
#define SPD_TWR_BYTE						       	 	17
#define SPD_TRCD_BYTE									18
#define SPD_TRRD_BYTE									19
#define SPD_TRP_BYTE									20

#define SPD_TRAS_MSB_BYTE								21
#define SPD_TRAS_MSB_MASK								0xf

#define SPD_TRC_MSB_BYTE								21
#define SPD_TRC_MSB_MASK								0xf0

#define SPD_TRAS_LSB_BYTE								22
#define SPD_TRC_LSB_BYTE								23
#define SPD_TRFC_LSB_BYTE								24
#define SPD_TRFC_MSB_BYTE								25
#define SPD_TWTR_BYTE									26
#define SPD_TRTP_BYTE									27

#define SPD_TFAW_MSB_BYTE								28
#define SPD_TFAW_MSB_MASK								0xf

#define SPD_TFAW_LSB_BYTE								29
#define SPD_OPT_FEATURES_BYTE							30
#define SPD_THERMAL_REFRESH_OPT_BYTE					31

#define SPD_ADDR_MAP_BYTE								63
#define SPD_ADDR_MAP_MIRROR_OFFS						0

#define SPD_RDIMM_RC_BYTE								69
#define SPD_RDIMM_RC_NIBBLE_MASK						0xF
#define SPD_RDIMM_RC_NUM								16


/* Dimm Memory Type values */
#define SPD_MEM_TYPE_SDRAM								0x4
#define SPD_MEM_TYPE_DDR1 								0x7
#define SPD_MEM_TYPE_DDR2 								0x8
#define SPD_MEM_TYPE_DDR3 								0xB
        
#define DIMM_MODULE_MANU_OFFS					 		64
#define DIMM_MODULE_MANU_SIZE 							8
#define DIMM_MODULE_VEN_OFFS 							73
#define DIMM_MODULE_VEN_SIZE 							25
#define DIMM_MODULE_ID_OFFS 							99
#define DIMM_MODULE_ID_SIZE 							18


/* enumeration for voltage levels. */
typedef enum {
    TTL_5V_TOLERANT, 
    LVTTL, 
    HSTL_1_5V, 
    SSTL_3_3V, 
    SSTL_2_5V, 
    VOLTAGE_UNKNOWN, 
} MV_DIMM_VOLTAGE_IF;


/* enumaration for SDRAM CAS Latencies. */
typedef enum _mvDimmSdramCas
{
    SD_CL_1 =1,  
    SD_CL_2,  
    SD_CL_3, 
    SD_CL_4, 
    SD_CL_5, 
    SD_CL_6, 
    SD_CL_7, 
    SD_FAULT
}MV_DIMM_SDRAM_CAS;

/* enumeration for memory types */
typedef enum _mvMemoryType
{
    MEM_TYPE_SDRAM,
    MEM_TYPE_DDR1,
    MEM_TYPE_DDR2,
    MEM_TYPE_DDR3
}MV_MEMORY_TYPE;


/* DIMM information structure */                                                    
typedef struct _mvDimmInfo
{
    MV_MEMORY_TYPE  memoryType; 	/* DDR or SDRAM */

    MV_U8   spdRawData[SPD_SIZE];  	/* Content of SPD-EEPROM copied 1:1  */

    /* DIMM dimensions */
    MV_U32  numOfModuleRanks;
    MV_U32  dataWidth;
	MV_U32 	dimmRankCapacity;
	MV_U32  dimmSize;               /* 16,64,128,256 or 512 MByte in MB units */
	MV_U32  numberOfDevices;
	
	MV_U32 	sdramWidth;
	MV_U32  numOfBanksOnEachDevice;
	MV_U32  sdramCapacity;
	
	MV_U32  numOfRowAddr;
	MV_U32  numOfColAddr;
	
	MV_U32 	addressMirroring;
	
    MV_U32  errorCheckType;             /* ECC , PARITY..*/
	MV_U32  dimmTypeInfo;           /* DDR2 only */
	MV_U32  dimmAttributes;
	
	
    /* DIMM timing parameters */
	MV_U32  suportedCasLatencies;
	MV_U32  refreshInterval;
	MV_U32  minCycleTime;
	MV_U32  minRowPrechargeTime;
	MV_U32  minRowActiveToRowActive;
	MV_U32  minRasToCasDelay;
	MV_U32  minWriteRecoveryTime;   /* DDR3/2 only */
    MV_U32  minWriteToReadCmdDelay; /* DDR3/2 only */
    MV_U32  minReadToPrechCmdDelay; /* DDR3/2 only */
	MV_U32 	minActiveToPrecharge;
	MV_U32  minRefreshRecovery;  /* DDR3/2 only */
	MV_U32  minCasLatTime;
	MV_U32  minFourActiveWinDelay;
	MV_U8	regDimmRC[SPD_RDIMM_RC_NUM];

} MV_DIMM_INFO;

MV_STATUS ddr3DunitSetup(MV_U32 uiEccEna, MV_U32 uiHClkTime, MV_BOOL *pBRegDimm, MV_U32 *pUiDdrWidth);

#endif /* __INCmvDram */
