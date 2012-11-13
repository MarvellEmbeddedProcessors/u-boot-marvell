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


#include "config_marvell.h"  	/* Required to identify SOC and Board */
#include "ddr3_init.h"
#include "ddr3_spd.h"
#include "bin_hdr_twsi.h"
#include "bootstrap_os.h"
#include "mvUart.h"

#if defined(MV88F78X60)
#include "ddr3_axp_config.h"
#elif defined(MV88F67XX)
#include "ddr3_a370_config.h"
#endif

#ifdef DUNIT_SPD

static MV_STATUS ddr3SpdSumInit(MV_DIMM_INFO *pDimmInfo, MV_DIMM_INFO *pDimmSumInfo, MV_U32 uiDimm);
static MV_U32 ddr3GetMaxValue(MV_U32 spdVal, MV_U32 uiDimmNum, MV_U32 staticVal);
static MV_U32 ddr3GetMinValue(MV_U32 spdVal, MV_U32 uiDimmNum, MV_U32 staticVal);
static MV_U32 ddr3getDimmNum(MV_U32 *auiDimmAddr);
static MV_STATUS ddr3SpdInit(MV_DIMM_INFO *pDimmInfo, MV_U32 uiDimmAddr);
static MV_U32 ddr3DivFunc(MV_U32 uiValue, MV_U32 uiDivider, MV_U32 uiSub);

extern MV_U8 ucData[SPD_SIZE];
extern MV_U32 auiODTConfig[ODT_OPT];
extern MV_U16 auiODTStatic[ODT_OPT][MAX_CS];
extern MV_U16 auiODTDynamic[ODT_OPT][MAX_CS];

/************************************************************************************
* Name:		ddr3getDimmNum - Find number of dimms and their addresses
* Desc:
* Args:	 	auiDimmAddr - array of dimm addresses
* Notes:
* Returns:	None.
*/
MV_U32 ddr3getDimmNum(MV_U32 *auiDimmAddr)
{
#if defined(DB_88F6710) || defined(RD_88F6710)
	/* Armada 370 - SPD is not available on DIMM */
	/* Set MC registers according to Static SPD values Values */
	/* We only have one optional DIMM for the DB and we already got the SPD matching values */
	return 1;
#else	
	MV_U32 uiDimmCurAddr;
	MV_U8 ucData[3];
	MV_U32 uiDimmNum = 0;
	MV_TWSI_SLAVE twsiSlave;
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 0;
	twsiSlave.moreThen256 = MV_FALSE;

	/* read the dimm eeprom */
	for (uiDimmCurAddr = MAX_DIMM_ADDR; uiDimmCurAddr > MIN_DIMM_ADDR; uiDimmCurAddr--) {
		ucData[SPD_DEV_TYPE_BYTE] = 0;
		twsiSlave.slaveAddr.address = uiDimmCurAddr;

		/* Far-End DIMM must be connected */
		if ((uiDimmNum == 0) && (uiDimmCurAddr < FAR_END_DIMM_ADDR))
			return 0;
		 
		if (MV_OK == mvTwsiRead(0, &twsiSlave, ucData, 3)) {
			if (ucData[SPD_DEV_TYPE_BYTE] == SPD_MEM_TYPE_DDR3) {
				auiDimmAddr[uiDimmNum] = uiDimmCurAddr;
				uiDimmNum++;
			}
		}
	}
	return uiDimmNum;
#endif
}

/******************************************************************************
* Name:		dimmSpdInit - Get the SPD parameters.
* Desc:	 	Read the DIMM SPD parameters into given struct parameter.
* Args:	 	dimmNum - DIMM number. See MV_BOARD_DIMM_NUM enumerator.
*  			pDimmInfo - DIMM information structure.
* Notes:
* Returns:	MV_OK if function could read DIMM parameters, MV_FALSE otherwise.
*/
MV_STATUS ddr3SpdInit(MV_DIMM_INFO *pDimmInfo, MV_U32 uiDimmAddr)
{
	MV_U32 uiTemp, uiRC;
	MV_U32 uiTimeBase;
	MV_TWSI_SLAVE twsiSlave;

	if (uiDimmAddr != 0) {
		memset(ucData, 0, SPD_SIZE*sizeof(MV_U8));
	
		twsiSlave.slaveAddr.type = ADDR7_BIT;
		twsiSlave.slaveAddr.address = uiDimmAddr;
	
		twsiSlave.validOffset = MV_TRUE;
		twsiSlave.offset = 0;
		twsiSlave.moreThen256 = MV_FALSE;
	
		if (MV_OK != mvTwsiRead(0, &twsiSlave, ucData, SPD_SIZE))
			return MV_FAIL;
	}
	
	/* Check if DDR3 */
	if (ucData[SPD_DEV_TYPE_BYTE] != SPD_MEM_TYPE_DDR3)
		return MV_FAIL;

	/* Error Check Type */
	/* No byte for error check in DDR3 SPD, use DDR2 convention */
	pDimmInfo->errorCheckType = 0;
	/* Check if ECC */
	if ((ucData[SPD_BUS_WIDTH_BYTE] & 0x18) >> 3)
		pDimmInfo->errorCheckType = 1;
	DEBUG_INIT_FULL_C("DRAM errorCheckType ", pDimmInfo->errorCheckType,1);

	pDimmInfo->dimmTypeInfo = (ucData[SPD_MODULE_TYPE_BYTE]);

/* Size Calculations: */

	/* Number Of Row Addresses - 12/13/14/15/16 */
	pDimmInfo->numOfRowAddr = ((ucData[SPD_ROW_NUM_BYTE] & SPD_ROW_NUM_MASK) >> SPD_ROW_NUM_OFF);
	pDimmInfo->numOfRowAddr += SPD_ROW_NUM_MIN;
	DEBUG_INIT_FULL_C("DRAM numOfRowAddr ",pDimmInfo->numOfRowAddr,2);

	/* Number Of Column Addresses - 9/10/11/12 */
	pDimmInfo->numOfColAddr = ((ucData[SPD_COL_NUM_BYTE] & SPD_COL_NUM_MASK) >> SPD_COL_NUM_OFF);
	pDimmInfo->numOfColAddr += SPD_COL_NUM_MIN;
	DEBUG_INIT_FULL_C("DRAM numOfColAddr ",pDimmInfo->numOfColAddr,1);

	/* Number Of Ranks = number of CS on Dimm - 1/2/3/4 Ranks */
	pDimmInfo->numOfModuleRanks = ((ucData[SPD_MODULE_ORG_BYTE] & SPD_MODULE_BANK_NUM_MASK) >> SPD_MODULE_BANK_NUM_OFF);
	pDimmInfo->numOfModuleRanks += SPD_MODULE_BANK_NUM_MIN;
	DEBUG_INIT_FULL_C("DRAM numOfModuleBanks ",pDimmInfo->numOfModuleRanks,1);

	/* Data Width - 8/16/32/64 bits */
	pDimmInfo->dataWidth = (1 << (3 + (ucData[SPD_BUS_WIDTH_BYTE] & SPD_BUS_WIDTH_MASK)));
	DEBUG_INIT_FULL_C("DRAM dataWidth ", pDimmInfo->dataWidth,1);

	/* Number Of Banks On Each Device - 8/16/32/64 banks */
	pDimmInfo->numOfBanksOnEachDevice = (1 << (3 + ((ucData[SPD_DEV_DENSITY_BYTE]>>4) & 0x7)));
	DEBUG_INIT_FULL_C("DRAM numOfBanksOnEachDevice ",pDimmInfo->numOfBanksOnEachDevice,1);

	/* Total SDRAM capacity - 256Mb/512Mb/1Gb/2Gb/4Gb/8Gb/16Gb - MegaBits*/
	pDimmInfo->sdramCapacity = (ucData[SPD_DEV_DENSITY_BYTE] & SPD_DEV_DENSITY_MASK);

	/* Sdram Width - 4/8/16/32 bits */
	pDimmInfo->sdramWidth = (1 << (2 + (ucData[SPD_MODULE_ORG_BYTE] & SPD_MODULE_SDRAM_DEV_WIDTH_MASK)));
	DEBUG_INIT_FULL_C("DRAM sdramWidth ",pDimmInfo->sdramWidth,1);

	/* CS (Rank) Capacity - MB */
	/* DDR3 device uiDensity val are: (device capacity/8) * (Module_width/Device_width) */
	/* Jedec SPD DDR3 - page 7, Save ucData in Mb  - 2048=2GB*/
	pDimmInfo->dimmRankCapacity =
			(((1 << pDimmInfo->sdramCapacity) * 256 * (pDimmInfo->dataWidth / pDimmInfo->sdramWidth) *  0x2) << 16);
	/* 0x2 =>  0x100000-1Mbit / 8-bit->byte / 0x10000  */
	 DEBUG_INIT_FULL_C("DRAM dimmRankCapacity[31] ",pDimmInfo->dimmRankCapacity,1);

	pDimmInfo->dimmSize = pDimmInfo->dimmRankCapacity * pDimmInfo->numOfModuleRanks;
	DEBUG_INIT_FULL_C("Dram: dimm size in MB ",pDimmInfo->dimmSize,1);

	/* Number of devices includeing Error correction */
	pDimmInfo->numberOfDevices = ((pDimmInfo->dataWidth/pDimmInfo->sdramWidth) *
			 pDimmInfo->numOfModuleRanks) + pDimmInfo->errorCheckType;
	DEBUG_INIT_FULL_C("DRAM numberOfDevices  ",pDimmInfo->numberOfDevices,1);

	/* Address Mapping from Edge connector to DRAM - mirroring option */
	pDimmInfo->addressMirroring = ucData[SPD_ADDR_MAP_BYTE] & (1 << SPD_ADDR_MAP_MIRROR_OFFS);

/* Timings - All in ps */

	uiTimeBase = (1000 * ucData[SPD_MTB_DIVIDEND_BYTE])/ucData[SPD_MTB_DIVISOR_BYTE];

	/* Minimum Cycle Time At Max CasLatancy */
	pDimmInfo->minCycleTime = ucData[SPD_TCK_BYTE] * uiTimeBase;
	DEBUG_INIT_FULL_C("DRAM tCKmin ", pDimmInfo->minCycleTime,1);

	/* Refresh Interval */
	/* No byte for refresh interval in DDR3 SPD, use DDR2 convention */
	/* JEDEC param are 0 <= Tcase <= 85: 7.8uSec, 85 <= Tcase <= 95: 3.9uSec */
	pDimmInfo->refreshInterval = 7800000; /* Set to 7.8uSec */
	DEBUG_INIT_FULL_C("DRAM refreshInterval ", pDimmInfo->refreshInterval,1);

	/* Suported Cas Latencies -  DDR 3: */

	/******-******-******-******-******-******-******-*******-*******
	* 		  bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 *
	*******-******-******-******-******-******-******-*******-*******
	CAS =      11  |  10  |  9   |  8   |  7   |  6   |  5   |  4   *
	*********************************************************-*******
	*******-******-******-******-******-******-******-*******-*******
	* 		 bit15 |bit14 |bit13 |bit12 |bit11 |bit10 | bit9 | bit8 *
	*******-******-******-******-******-******-******-*******-*******
	CAS =     TBD  |  18  |  17  |  16  |  15  |  14  |  13  |  12  *
	*****************************************************************/

	/* DDR3 include 2 byte of CAS support */
	pDimmInfo->suportedCasLatencies = ((ucData[SPD_SUP_CAS_LAT_MSB_BYTE] << 8) | ucData[SPD_SUP_CAS_LAT_LSB_BYTE]);
	DEBUG_INIT_FULL_C("DRAM suportedCasLatencies ",pDimmInfo->suportedCasLatencies,1);

	/* Minimum Cycle Time At Max CasLatancy */
	pDimmInfo->minCasLatTime = (ucData[SPD_TAA_BYTE] * uiTimeBase);
	/* This field divided by the cycleTime will give us the CAS latency to config */

	/* For DDR3 and DDR2 includes Write Recovery Time field. Other SDRAM ignore     */
	pDimmInfo->minWriteRecoveryTime = ucData[SPD_TWR_BYTE] * uiTimeBase;
	DEBUG_INIT_FULL_C("DRAM minWriteRecoveryTime ",pDimmInfo->minWriteRecoveryTime,1);

	/* Mininmum Ras to Cas Delay */
	pDimmInfo->minRasToCasDelay = ucData[SPD_TRCD_BYTE] * uiTimeBase;
	DEBUG_INIT_FULL_C("DRAM minRasToCasDelay ", pDimmInfo->minRasToCasDelay,1);

	/* Minimum Row Active to Row Active Time */
	pDimmInfo->minRowActiveToRowActive = ucData[SPD_TRRD_BYTE] * uiTimeBase;
	DEBUG_INIT_FULL_C("DRAM minRowActiveToRowActive ",pDimmInfo->minRowActiveToRowActive,1);

	/* Minimum Row Precharge Delay Time */
	pDimmInfo->minRowPrechargeTime = ucData[SPD_TRP_BYTE] * uiTimeBase;
	DEBUG_INIT_FULL_C("DRAM minRowPrechargeTime ",pDimmInfo->minRowPrechargeTime,1);

	/* Minimum Active to Precharge Delay Time - tRAS   ps */
	pDimmInfo->minActiveToPrecharge = ((ucData[SPD_TRAS_MSB_BYTE] & SPD_TRAS_MSB_MASK) << 8);
	pDimmInfo->minActiveToPrecharge |= ucData[SPD_TRAS_LSB_BYTE];
	pDimmInfo->minActiveToPrecharge *= uiTimeBase;
	DEBUG_INIT_FULL_C("DRAM minActiveToPrecharge ",pDimmInfo->minActiveToPrecharge,1);

	/* Minimum Active to Active/Refresh Delay Time - tRC  ps*/
	/*	pDimmInfo->minActiveToPrecharge = ((ucData[SPD_TRC_MSB_BYTE] & SPD_TRC_MSB_MASK) << 4); */
	/*	pDimmInfo->minActiveToPrecharge |= ucData[SPD_TRC_LSB_BYTE];*/
	/*	pDimmInfo->minActiveToPrecharge *= uiTimeBase;*/
	/* DEBUG_INIT_FULL_C("DRAM minActiveToPrecharge ",pDimmInfo->minActiveToPrecharge,1); */

	/* Minimum Refresh Recovery Delay Time - tRFC  ps*/
	pDimmInfo->minRefreshRecovery = (ucData[SPD_TRFC_MSB_BYTE] << 8);
	pDimmInfo->minRefreshRecovery |= ucData[SPD_TRFC_LSB_BYTE];
	pDimmInfo->minRefreshRecovery *= uiTimeBase;
	DEBUG_INIT_FULL_C("DRAM minRefreshRecovery ",pDimmInfo->minRefreshRecovery,1);

	/* For DDR3 and DDR2 includes Internal Write To Read Command Delay field.       */
	pDimmInfo->minWriteToReadCmdDelay = ucData[SPD_TWTR_BYTE] * uiTimeBase;
	DEBUG_INIT_FULL_C("DRAM minWriteToReadCmdDelay ",pDimmInfo->minWriteToReadCmdDelay,1);

	/* For DDR3 and DDR2 includes Internal Read To Precharge Command Delay field.   */
	pDimmInfo->minReadToPrechCmdDelay = ucData[SPD_TRTP_BYTE] * uiTimeBase;
	DEBUG_INIT_FULL_C("DRAM minReadToPrechCmdDelay ",pDimmInfo->minReadToPrechCmdDelay,1);

	/* For DDR3 includes Minimum Activate to Activate/Refresh Command field */
	uiTemp = (((ucData[SPD_TFAW_MSB_BYTE] & SPD_TFAW_MSB_MASK) << 8) | ucData[SPD_TFAW_LSB_BYTE]);
	pDimmInfo->minFourActiveWinDelay = uiTemp * uiTimeBase;
	DEBUG_INIT_FULL_C("DRAM minFourActiveWinDelay ",pDimmInfo->minFourActiveWinDelay,1);
#if defined(MV88F78X60)
	/* Registered DIMM support */
	if (pDimmInfo->dimmTypeInfo == SPD_MODULE_TYPE_RDIMM) {
		for (uiRC=3; uiRC<6; uiRC+=2) {
			uiTemp = ucData[SPD_RDIMM_RC_BYTE + uiRC/2];
			pDimmInfo->regDimmRC[uiRC] = (ucData[SPD_RDIMM_RC_BYTE + uiRC/2] & SPD_RDIMM_RC_NIBBLE_MASK);
			pDimmInfo->regDimmRC[uiRC + 1] = ((ucData[SPD_RDIMM_RC_BYTE + uiRC/2] >> 4) & SPD_RDIMM_RC_NIBBLE_MASK);
		}

		pDimmInfo->regDimmRC[0] = RDIMM_RC0;
		pDimmInfo->regDimmRC[1] = RDIMM_RC1;
		pDimmInfo->regDimmRC[2] = RDIMM_RC2;
		pDimmInfo->regDimmRC[8] = RDIMM_RC8;
		pDimmInfo->regDimmRC[9] = RDIMM_RC9;
		pDimmInfo->regDimmRC[10] = RDIMM_RC10;
		pDimmInfo->regDimmRC[11] = RDIMM_RC11;
	}
#endif
	return MV_OK;
}

/******************************************************************************
* Name:		ddr3SpdSumInit - Get the SPD parameters.
* Desc:	 	Read the DIMM SPD parameters into given struct parameter.
* Args:	 	dimmNum - DIMM number. See MV_BOARD_DIMM_NUM enumerator.
*  			pDimmInfo - DIMM information structure.
* Notes:
* Returns:	MV_OK if function could read DIMM parameters, MV_FALSE otherwise.
*/

MV_STATUS ddr3SpdSumInit(MV_DIMM_INFO *pDimmInfo, MV_DIMM_INFO *pDimmSumInfo, MV_U32 uiDimm) {

	if (uiDimm == 0) {
		memcpy(pDimmSumInfo, pDimmInfo, sizeof(MV_DIMM_INFO));
		return MV_OK;
	}
	if (pDimmSumInfo->dimmTypeInfo != pDimmInfo->dimmTypeInfo) {
		DEBUG_INIT_S("DDR3 Dimm Compare - DIMM type does not match - FAIL \n");
		return MV_FAIL;
	}
	if (pDimmSumInfo->errorCheckType > pDimmInfo->errorCheckType) {
		pDimmSumInfo->errorCheckType = pDimmInfo->errorCheckType;
		DEBUG_INIT_S("DDR3 Dimm Compare - ECC does not match. ECC is disabled \n");
	}
	if (pDimmSumInfo->dataWidth != pDimmInfo->dataWidth) {
		DEBUG_INIT_S("DDR3 Dimm Compare - DRAM bus width does not match - FAIL \n");
		return MV_FAIL;
	}
	if (pDimmSumInfo->minCycleTime < pDimmInfo->minCycleTime)
		pDimmSumInfo->minCycleTime = pDimmInfo->minCycleTime;
	if (pDimmSumInfo->refreshInterval < pDimmInfo->refreshInterval)
		pDimmSumInfo->refreshInterval = pDimmInfo->refreshInterval;
	pDimmSumInfo->suportedCasLatencies &= pDimmInfo->suportedCasLatencies;
	if (pDimmSumInfo->minCasLatTime < pDimmInfo->minCasLatTime)
		pDimmSumInfo->minCasLatTime = pDimmInfo->minCasLatTime;
	if (pDimmSumInfo->minWriteRecoveryTime < pDimmInfo->minWriteRecoveryTime)
		pDimmSumInfo->minWriteRecoveryTime = pDimmInfo->minWriteRecoveryTime;
	if (pDimmSumInfo->minRasToCasDelay < pDimmInfo->minRasToCasDelay)
		pDimmSumInfo->minRasToCasDelay = pDimmInfo->minRasToCasDelay;
	if (pDimmSumInfo->minRowActiveToRowActive < pDimmInfo->minRowActiveToRowActive)
		pDimmSumInfo->minRowActiveToRowActive = pDimmInfo->minRowActiveToRowActive;
	if (pDimmSumInfo->minRowPrechargeTime < pDimmInfo->minRowPrechargeTime)
		pDimmSumInfo->minRowPrechargeTime = pDimmInfo->minRowPrechargeTime;
	if (pDimmSumInfo->minActiveToPrecharge < pDimmInfo->minActiveToPrecharge)
		pDimmSumInfo->minActiveToPrecharge = pDimmInfo->minActiveToPrecharge;
	if (pDimmSumInfo->minRefreshRecovery < pDimmInfo->minRefreshRecovery)
		pDimmSumInfo->minRefreshRecovery = pDimmInfo->minRefreshRecovery;
	if (pDimmSumInfo->minWriteToReadCmdDelay < pDimmInfo->minWriteToReadCmdDelay)
		pDimmSumInfo->minWriteToReadCmdDelay = pDimmInfo->minWriteToReadCmdDelay;
	if (pDimmSumInfo->minReadToPrechCmdDelay < pDimmInfo->minReadToPrechCmdDelay)
		pDimmSumInfo->minReadToPrechCmdDelay = pDimmInfo->minReadToPrechCmdDelay;
	if (pDimmSumInfo->minFourActiveWinDelay < pDimmInfo->minFourActiveWinDelay)
		pDimmSumInfo->minFourActiveWinDelay = pDimmInfo->minFourActiveWinDelay;
	if (pDimmSumInfo->minWriteToReadCmdDelay < pDimmInfo->minWriteToReadCmdDelay)
		pDimmSumInfo->minWriteToReadCmdDelay = pDimmInfo->minWriteToReadCmdDelay;

	return MV_OK;
}

/******************************************************************************
* Name:		ddr3DunitSetup
* Desc:		Set the controller with the timing values.
* Args:	 	uiEccEna - User ECC setup
* Notes:
* Returns:
*/
MV_STATUS ddr3DunitSetup(MV_U32 uiEccEna, MV_U32 uiHClkTime, MV_U32 *pUiDdrWidth)
{
	MV_U32 uiReg, uiTemp, uiCWL;
	MV_U32 uiDDRClkTime;
	MV_DIMM_INFO dimmInfo[2];
	MV_DIMM_INFO dimmSumInfo;
	MV_U32 uiStaticVal, uiSpdVal;
	MV_U32 uiCs, uiCL, uiCsNum, uiCsEna;
	MV_U32 uiDimmNum = 0;
#ifdef DUNIT_SPD
	MV_U32 uiDimmCount, uiCsCount, uiDimm;
	MV_U32 auiDimmAddr[2] = {0, 0};
#endif

#if defined(DB_88F6710) || defined(DB_88F6710_PCAC) || defined(RD_88F6710)
	/* Armada 370 - SPD is not available on DIMM */
	/* Set MC registers according to Static SPD values Values - must be set manually */
	/* We only have one optional DIMM for the DB and we already got the SPD matching values */
	ddr3SpdInit(&dimmInfo[0], 0);
	uiDimmNum = 1;
	/* Use JP8 to enable multiCS support for Armada 370 DB */
	if(!ddr3CheckConfig(EEPROM_MODULE_ADDR, CONFIG_MULTI_CS))
		dimmInfo[0].numOfModuleRanks = 1;
	ddr3SpdSumInit(&dimmInfo[0], &dimmSumInfo, 0);
#else	
	/* Dynamic D-Unit Setup - Read SPD values */
#ifdef DUNIT_SPD
	uiDimmNum = ddr3getDimmNum(auiDimmAddr);
	if (uiDimmNum == 0) {
#ifdef MIXED_DIMM_STATIC
		DEBUG_INIT_S("DDR3 Training Sequence - No DIMMs detected \n");
#else	
		DEBUG_INIT_S("DDR3 Training Sequence - FAILED (Wrong DIMMs Setup) \n");
		return MV_FAIL;
#endif
	} else {
		DEBUG_INIT_C("DDR3 Training Sequence - Number of DIMMs detected: ", uiDimmNum, 1);	
	}

	for (uiDimm = 0; uiDimm < uiDimmNum; uiDimm++) {
		ddr3SpdInit(&dimmInfo[uiDimm], auiDimmAddr[uiDimm]);
		ddr3SpdSumInit(&dimmInfo[uiDimm], &dimmSumInfo, uiDimm);
	}
#endif
#endif

	/* Set number of enabled CS */
	uiCsNum = 0;
#ifdef DUNIT_STATIC
	uiCsNum = ddr3GetCSNumFromReg();
#endif
#ifdef DUNIT_SPD
	for (uiDimm = 0; uiDimm < uiDimmNum; uiDimm++)
		uiCsNum += dimmInfo[uiDimm].numOfModuleRanks;
#endif
	if (uiCsNum > MAX_CS) {
		DEBUG_INIT_C("DDR3 Training Sequence - Number of CS exceed limit -  ", MAX_CS, 1);
		return MV_FAIL;
	}

	/* Set bitmap of enabled CS */
	uiCsEna = 0;
#ifdef DUNIT_STATIC
	uiCsEna = ddr3GetCSEnaFromReg();
#endif
#ifdef DUNIT_SPD
	uiDimm = 0;

	if (uiDimmNum) {
		for (uiCs = 0; uiCs < MAX_CS; uiCs+=2) {
			if (((1 << uiCs) & DIMM_CS_BITMAP) && !(uiCsEna & (1 << uiCs))) {
				if (dimmInfo[uiDimm].numOfModuleRanks == 1)
					uiCsEna |= (0x1 << uiCs);
				else if (dimmInfo[uiDimm].numOfModuleRanks == 2)
					uiCsEna |= (0x3 << uiCs);
				else if (dimmInfo[uiDimm].numOfModuleRanks == 3)
					uiCsEna |= (0x7 << uiCs);
				else if (dimmInfo[uiDimm].numOfModuleRanks == 4)
					uiCsEna |= (0xF << uiCs);

				uiDimm++;
				if (uiDimm == uiDimmNum)
				break;
			}
		}
	}
#endif
	if (uiCsEna > 0xF) {
		DEBUG_INIT_C("DDR3 Training Sequence - Number of enabled CS exceed limit -  ", MAX_CS, 1);
		return MV_FAIL;
	}
	
	DEBUG_INIT_FULL_C("DDR3 - DUNIT-SET - Number of CS = ", uiCsNum, 1);

/* Check Ratio - '1' - 2:1, '0' - 1:1 */
	if (MV_REG_READ(REG_DDR_IO_ADDR) & (1 << REG_DDR_IO_CLK_RATIO_OFFS))
		uiDDRClkTime = uiHClkTime / 2;
	else
		uiDDRClkTime = uiHClkTime;

#ifdef DUNIT_STATIC
	/* Get target CL value from set register */
	uiReg = (MV_REG_READ(REG_DDR3_MR0_ADDR) >> 2);
	uiReg = ((((uiReg >> 1) & 0xE)) | (uiReg & 0x1)) & 0xF;

	uiCL = ddr3GetMaxValue(ddr3DivFunc(dimmSumInfo.minCasLatTime, uiDDRClkTime, 0), uiDimmNum, ddr3ValidCLtoCL(uiReg));
#else
	uiCL = ddr3DivFunc(dimmSumInfo.minCasLatTime, uiDDRClkTime, 0);
#endif
	if (uiCL < 5)
		uiCL = 5;
	
	DEBUG_INIT_FULL_C("DDR3 - DUNIT-SET - Cas Latency = ", uiCL, 1);

/* {0x00001400} -	DDR SDRAM Configuration Register */
	uiReg = 0x73004000;
	uiStaticVal = ddr3GetStaticMCValue(REG_SDRAM_CONFIG_ADDR, REG_SDRAM_CONFIG_ECC_OFFS, 0x1, 0, 0);
	if (uiEccEna && ddr3GetMinValue(dimmSumInfo.errorCheckType, uiDimmNum, uiStaticVal)) {
		uiReg |= (1 << REG_SDRAM_CONFIG_ECC_OFFS);
		uiReg |= (1 << REG_SDRAM_CONFIG_IERR_OFFS);
		DEBUG_INIT_FULL_S("DDR3 - DUNIT-SET - ECC Enabled \n");
	} else
		DEBUG_INIT_FULL_S("DDR3 - DUNIT-SET - ECC Disabled \n");
	
	if (dimmSumInfo.dimmTypeInfo == SPD_MODULE_TYPE_RDIMM) {
#ifdef DUNIT_STATIC
		DEBUG_INIT_S("DDR3 Training Sequence - FAIL - Illegal R-DIMM setup \n");
		return MV_FAIL;
#endif
		uiReg |= (1 << REG_SDRAM_CONFIG_REGDIMM_OFFS);
		DEBUG_INIT_FULL_S("DDR3 - DUNIT-SET - R-DIMM \n");
	} else
		DEBUG_INIT_FULL_S("DDR3 - DUNIT-SET - U-DIMM \n");

#ifndef MV88F67XX
#ifdef DUNIT_STATIC
	if (ddr3GetMinValue(dimmSumInfo.dataWidth, uiDimmNum, BUS_WIDTH) == 64)  {
#else
	if (*pUiDdrWidth == 64) {
#endif
		uiReg |= (1 << REG_SDRAM_CONFIG_WIDTH_OFFS);
		DEBUG_INIT_FULL_S("DDR3 - DUNIT-SET - Datawidth - 64Bits \n");
	} else {
		DEBUG_INIT_FULL_S("DDR3 - DUNIT-SET - Datawidth - 32Bits \n");
	}
#else
	DEBUG_INIT_FULL_S("DDR3 - DUNIT-SET - Datawidth - 16Bits \n");
#endif
	
	uiStaticVal = ddr3GetStaticMCValue(REG_SDRAM_CONFIG_ADDR, 0, REG_SDRAM_CONFIG_RFRS_MASK, 0, 0);
	uiTemp = ddr3GetMaxValue(dimmSumInfo.refreshInterval/uiHClkTime, uiDimmNum, uiStaticVal);
	DEBUG_INIT_FULL_C("DDR3 - DUNIT-SET - RefreshInterval/Hclk = ", uiTemp, 4);
	uiReg |= uiTemp;

	if (uiCL != 3)
		uiReg |= (1<<16);	/*  If 2:1 need to set P2DWr */

	MV_REG_WRITE(REG_SDRAM_CONFIG_ADDR, uiReg);

/*{0x00001404}	-	DDR SDRAM Configuration Register */
	uiReg = 0x3630B800;
#ifdef DUNIT_SPD
	uiReg |= (DRAM_2T << REG_DUNIT_CTRL_LOW_2T_OFFS);
#endif
	MV_REG_WRITE(REG_DUNIT_CTRL_LOW_ADDR, uiReg);

/*{0x00001408}	- 	DDR SDRAM Timing (Low) Register */
	uiReg = 0x0;

	/*tRAS - (0:3,20) */
	uiSpdVal = ddr3DivFunc(dimmSumInfo.minActiveToPrecharge, uiDDRClkTime, 1);
	uiStaticVal = ddr3GetStaticMCValue(REG_SDRAM_TIMING_LOW_ADDR, 0, 0xF, 16, 0x10);
	uiTemp = ddr3GetMaxValue(uiSpdVal, uiDimmNum, uiStaticVal);
	DEBUG_INIT_FULL_C("DDR3 - DUNIT-SET - tRAS-1 = ", uiTemp, 1);
	uiReg |= (uiTemp & 0xF);
	uiReg |= ((uiTemp & 0x10) << 16); /* to bit 20 */

	/*tRCD - (4:7) */
	uiSpdVal = ddr3DivFunc(dimmSumInfo.minRasToCasDelay, uiDDRClkTime, 1);
	uiStaticVal = ddr3GetStaticMCValue(REG_SDRAM_TIMING_LOW_ADDR, 4, 0xF, 0, 0);
	uiTemp = ddr3GetMaxValue(uiSpdVal, uiDimmNum, uiStaticVal);
	DEBUG_INIT_FULL_C("DDR3 - DUNIT-SET - tRCD-1 = ", uiTemp, 1);
	uiReg |= ((uiTemp & 0xF) << 4);

	/*tRP - (8:11) */
	uiSpdVal = ddr3DivFunc(dimmSumInfo.minRowPrechargeTime, uiDDRClkTime, 1);
	uiStaticVal = ddr3GetStaticMCValue(REG_SDRAM_TIMING_LOW_ADDR, 8, 0xF, 0, 0);
	uiTemp = ddr3GetMaxValue(uiSpdVal, uiDimmNum, uiStaticVal);
	DEBUG_INIT_FULL_C("DDR3 - DUNIT-SET - tRP-1 = ", uiTemp, 1);
	uiReg |= ((uiTemp & 0xF) << 8);

	/*tWR - (12:15) */
	uiSpdVal = ddr3DivFunc(dimmSumInfo.minWriteRecoveryTime, uiDDRClkTime, 1);
	uiStaticVal = ddr3GetStaticMCValue(REG_SDRAM_TIMING_LOW_ADDR, 12, 0xF, 0, 0);
	uiTemp = ddr3GetMaxValue(uiSpdVal, uiDimmNum, uiStaticVal);
	DEBUG_INIT_FULL_C("DDR3 - DUNIT-SET - tWR-1 = ", uiTemp, 1);
	uiReg |= ((uiTemp & 0xF) << 12);

	/*tWTR - (16:19) */
	uiSpdVal = ddr3DivFunc(dimmSumInfo.minWriteToReadCmdDelay, uiDDRClkTime, 1);
	uiStaticVal = ddr3GetStaticMCValue(REG_SDRAM_TIMING_LOW_ADDR, 16, 0xF, 0, 0);
	uiTemp = ddr3GetMaxValue(uiSpdVal, uiDimmNum, uiStaticVal);
	DEBUG_INIT_FULL_C("DDR3 - DUNIT-SET - tWTR-1 = ", uiTemp, 1);
	uiReg |= ((uiTemp & 0xF) << 16);

	/*tRRD - (24:27) */
	uiSpdVal = ddr3DivFunc(dimmSumInfo.minRowActiveToRowActive, uiDDRClkTime, 1);
	uiStaticVal = ddr3GetStaticMCValue(REG_SDRAM_TIMING_LOW_ADDR, 24, 0xF, 0, 0);
	uiTemp = ddr3GetMaxValue(uiSpdVal, uiDimmNum, uiStaticVal);
	DEBUG_INIT_FULL_C("DDR3 - DUNIT-SET - tRRD-1 = ", uiTemp, 1);
	uiReg |= ((uiTemp & 0xF) << 24);

	/*tRTP - (28:31) */
	uiSpdVal = ddr3DivFunc(dimmSumInfo.minReadToPrechCmdDelay, uiDDRClkTime, 1);
	uiStaticVal = ddr3GetStaticMCValue(REG_SDRAM_TIMING_LOW_ADDR, 28, 0xF, 0, 0);
	uiTemp = ddr3GetMaxValue(uiSpdVal, uiDimmNum, uiStaticVal);
	DEBUG_INIT_FULL_C("DDR3 - DUNIT-SET - tRTP-1 = ", uiTemp, 1);
	uiReg |= ((uiTemp & 0xF) << 28);

	if (uiCL < 7)
		uiReg = 0x33137663;
		
	MV_REG_WRITE(REG_SDRAM_TIMING_LOW_ADDR, uiReg);

/*{0x0000140C}	-	DDR SDRAM Timing (High) Register */
/*	uiReg = 0x38000C00; */
	/* Add cycles to R2R W2W */
	uiReg = 0x39F8FF80;

	/* tRFC - (0:6,16:18) */
	uiSpdVal = ddr3DivFunc(dimmSumInfo.minRefreshRecovery, uiDDRClkTime, 1);
	uiStaticVal = ddr3GetStaticMCValue(REG_SDRAM_TIMING_HIGH_ADDR, 0, 0x7F, 9, 0x380);
	uiTemp = ddr3GetMaxValue(uiSpdVal, uiDimmNum, uiStaticVal);
	DEBUG_INIT_FULL_C("DDR3 - DUNIT-SET - tRFC-1 = ", uiTemp, 1);
	uiReg |= (uiTemp & 0x7F);
	uiReg |= ((uiTemp & 0x380) << 9); /* to bit 16 */

	MV_REG_WRITE(REG_SDRAM_TIMING_HIGH_ADDR, uiReg);

/*{0x00001410}	-	DDR SDRAM Address Control Register */
	uiReg = 0x000F0000;

	/*tFAW - (24:28)  */
#if defined(MV88F78X60) && !defined(MV88F78X60_Z1)
	uiTemp = dimmSumInfo.minFourActiveWinDelay;
	uiSpdVal = ddr3DivFunc(uiTemp, uiDDRClkTime, 0);
	uiStaticVal = ddr3GetStaticMCValue(REG_SDRAM_ADDRESS_CTRL_ADDR, 24, 0x3F, 0, 0);
	uiTemp = ddr3GetMaxValue(uiSpdVal, uiDimmNum, uiStaticVal);
	DEBUG_INIT_FULL_C("DDR3 - DUNIT-SET - tFAW = ", uiTemp, 1);
	uiReg |= ((uiTemp & 0x3F) << 24);
#else
	uiTemp = dimmSumInfo.minFourActiveWinDelay - 4*(dimmSumInfo.minRowActiveToRowActive);
	uiSpdVal = ddr3DivFunc(uiTemp, uiDDRClkTime, 0);
	uiStaticVal = ddr3GetStaticMCValue(REG_SDRAM_ADDRESS_CTRL_ADDR, 24, 0x1F, 0, 0);
	uiTemp = ddr3GetMaxValue(uiSpdVal, uiDimmNum, uiStaticVal);
	DEBUG_INIT_FULL_C("DDR3 - DUNIT-SET - tFAW-4*tRRD = ", uiTemp, 1);
	uiReg |= ((uiTemp & 0x1F) << 24);
#endif
	
	/* SDRAM device capacity */
#ifdef DUNIT_STATIC
	uiReg |= (MV_REG_READ(REG_SDRAM_ADDRESS_CTRL_ADDR) & 0xF0FFFF);
#endif
#ifdef DUNIT_SPD
	uiCsCount = 0;
	uiDimmCount = 0;
	for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
		if (uiCsEna & (1<<uiCs) & DIMM_CS_BITMAP) {
			if (dimmInfo[uiDimmCount].numOfModuleRanks == uiCsCount) {
				uiDimmCount++;
				uiCsCount = 0;
			}
			uiCsCount++;
			if (dimmInfo[uiDimmCount].sdramCapacity < 0x3) {
				uiReg |= ((dimmInfo[uiDimmCount].sdramCapacity + 1) << 
					(REG_SDRAM_ADDRESS_SIZE_OFFS + (REG_SDRAM_ADDRESS_CTRL_STRUCT_OFFS * uiCs)));
			} else if (dimmInfo[uiDimmCount].sdramCapacity > 0x3) {
				uiReg |= ((dimmInfo[uiDimmCount].sdramCapacity & 0x3) << 
					(REG_SDRAM_ADDRESS_SIZE_OFFS + (REG_SDRAM_ADDRESS_CTRL_STRUCT_OFFS * uiCs)));
				uiReg |= ((dimmInfo[uiDimmCount].sdramCapacity & 0x4) << (REG_SDRAM_ADDRESS_SIZE_HIGH_OFFS + uiCs));
			}
		}
	}

	/* SDRAM device structure */
	uiCsCount = 0;
	uiDimmCount = 0;
	for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
		if (uiCsEna & (1<<uiCs) & DIMM_CS_BITMAP) {
			if (dimmInfo[uiDimmCount].numOfModuleRanks == uiCsCount) {
				uiDimmCount++;
				uiCsCount = 0;
			}
			uiCsCount++;
			if (dimmInfo[uiDimmCount].sdramWidth == 16)
			uiReg |= (1 << (REG_SDRAM_ADDRESS_CTRL_STRUCT_OFFS * uiCs));
		}
	}
#endif
	MV_REG_WRITE(REG_SDRAM_ADDRESS_CTRL_ADDR, uiReg);

/*{0x00001418}	-	DDR SDRAM Operation Register */
	uiReg = 0xF00;
	for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
		if (uiCsEna & (1<<uiCs))
			uiReg &= ~(1<<(uiCs+REG_SDRAM_OPERATION_CS_OFFS));
	}
	MV_REG_WRITE(REG_SDRAM_OPERATION_ADDR, uiReg);

/*{0x00001420}	-	DDR SDRAM Extended Mode Register */
	uiReg = 0x00000004;
	MV_REG_WRITE(REG_SDRAM_EXT_MODE_ADDR, uiReg);

/*{0x00001424}	-	DDR Controller Control (High) Register */
#if defined(MV88F78X60) && !defined(MV88F78X60_Z1)
	uiReg = 0x0000D3FF;
#else
	uiReg = 0x0100D1FF;
#endif
	MV_REG_WRITE(REG_DDR_CONT_HIGH_ADDR	, uiReg);

/*{0x0000142C}	-	DDR3 Timing Register */
#if defined(MV88F78X60) && !defined(MV88F78X60_Z1)
	uiReg = 0x014C2F38;
#else
	if (MV_REG_READ(REG_DDR_IO_ADDR) & (1<<REG_DDR_IO_CLK_RATIO_OFFS))	
		uiReg = 0x214C2F38;
	else
		uiReg = 0x014C2F38;
#endif
	MV_REG_WRITE(0x142C, uiReg);

/*{0x0000147C}	-	DDR ODT Timing (High) Register */
	if (MV_REG_READ(REG_DDR_IO_ADDR) & (1<<REG_DDR_IO_CLK_RATIO_OFFS))
		uiReg = 0x0000c671;
	else
		uiReg = 0x00006571;
	MV_REG_WRITE(REG_ODT_TIME_HIGH_ADDR, uiReg);

/*{0x00001484}	- MBus CPU Block Register */
#ifdef MV88F67XX
	if (MV_REG_READ(REG_DDR_IO_ADDR) & (1<<REG_DDR_IO_CLK_RATIO_OFFS))
		MV_REG_WRITE(REG_MBUS_CPU_BLOCK_ADDR, 0x0000E907);
#endif

/* In case of mixed dimm and on-board devices setup paramters will be taken statically */
/*{0x00001494}	-	DDR SDRAM ODT Control (Low) Register */
	uiReg = auiODTConfig[uiCsEna];
	MV_REG_WRITE(REG_SDRAM_ODT_CTRL_LOW_ADDR, uiReg);

/*{0x00001498}	-	DDR SDRAM ODT Control (High) Register */
	uiReg = 0x00000000;
	MV_REG_WRITE(REG_SDRAM_ODT_CTRL_HIGH_ADDR, uiReg);

/*{0x0000149C}	-	DDR Dunit ODT Control Register */
	uiReg = uiCsEna;
	MV_REG_WRITE(REG_DUNIT_ODT_CTRL_ADDR, uiReg);

/*{0x000014A0}	-	DDR Dunit ODT Control Register */
#if defined(MV88F78X60) && !defined(MV88F78X60_Z1)	
	if (mvCtrlRevGet() == MV_78XX0_A0_REV) {
	uiReg = 0x000006A9;
	MV_REG_WRITE(REG_DRAM_FIFO_CTRL_ADDR, uiReg);
	}
#endif

/*{0x000014C0}	-	DRAM address and Control Driving Strenght */
	MV_REG_WRITE(REG_DRAM_ADDR_CTRL_DRIVE_STRENGTH_ADDR, 0x192435e9);

/*{0x000014C4}	-	DRAM Data and DQS Driving Strenght */
	MV_REG_WRITE(REG_DRAM_DATA_DQS_DRIVE_STRENGTH_ADDR, 0xB2C35E9);

#ifdef DUNIT_SPD
		uiCsCount = 0;
		uiDimmCount = 0;
		for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
			if ((1<<uiCs) & DIMM_CS_BITMAP) {
				if ((1<<uiCs) & uiCsEna) {
					if (dimmInfo[uiDimmCount].numOfModuleRanks == uiCsCount) {
						uiDimmCount++;
						uiCsCount = 0;
					}
					uiCsCount++;
					if (*pUiDdrWidth == 32)
						MV_REG_WRITE(REG_CS_SIZE_SCRATCH_ADDR+(uiCs*0x8),
							((dimmInfo[uiDimmCount].dimmRankCapacity >> 1)-1));
					else
						MV_REG_WRITE(REG_CS_SIZE_SCRATCH_ADDR+(uiCs*0x8),
							(dimmInfo[uiDimmCount].dimmRankCapacity-1));
				} else
					MV_REG_WRITE(REG_CS_SIZE_SCRATCH_ADDR+(uiCs*0x8), 0);
			}
		}
#endif

/*{0x00020184}	-	Close FastPath - 2G */
	MV_REG_WRITE(REG_FASTPATH_WIN_0_CTRL_ADDR, 0);
	
/*{0x00001538}	-	 Read Data Sample Delays Register */
	uiReg = 0;
	for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
		if (uiCsEna & (1<<uiCs))
			uiReg |= (uiCL << (REG_READ_DATA_SAMPLE_DELAYS_OFFS*uiCs));
	}
	MV_REG_WRITE(REG_READ_DATA_SAMPLE_DELAYS_ADDR, uiReg);
	DEBUG_INIT_FULL_C("DDR3 - SPD-SET - Read Data Sample Delays = ", uiReg, 1);

/*{0x0000153C}	-	Read Data Ready Delay Register */
	uiReg = 0;
	for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
		if (uiCsEna & (1<<uiCs))
			uiReg |= ((uiCL+2) << (REG_READ_DATA_READY_DELAYS_OFFS*uiCs));
	}
	MV_REG_WRITE(REG_READ_DATA_READY_DELAYS_ADDR, uiReg);
	DEBUG_INIT_FULL_C("DDR3 - SPD-SET - Read Data Ready Delays = ", uiReg, 1);

/* Set MR registers */
	/* MR0 */
	uiReg = 0x00000600;
	uiTemp = ddr3CLtoValidCL(uiCL);
	uiReg |= ((uiTemp & 0x1) << 2);
	uiReg |= ((uiTemp & 0xE) << 3); /* to bit 4 */
#ifdef MULTI_CS_MRS_SUPPORT
		for (uiCs = 0; uiCs < MAX_CS; uiCs++)
			if (uiCsEna & (1<<uiCs))
				MV_REG_WRITE(REG_DDR3_MR0_CS_ADDR + (uiCs << MR_CS_ADDR_OFFS), uiReg);
#else
	MV_REG_WRITE(REG_DDR3_MR0_ADDR, uiReg);
#endif

	/* MR1 */
		uiReg = 0x00000046 & REG_DDR3_MR1_ODT_MASK;
#ifdef MULTI_CS_MRS_SUPPORT
		for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
			if (uiCsEna & (1<<uiCs)) {
				uiReg |= auiODTStatic[uiCsEna][uiCs];
				MV_REG_WRITE(REG_DDR3_MR1_CS_ADDR + (uiCs << MR_CS_ADDR_OFFS), uiReg);
			}
		}
#else
	MV_REG_WRITE(REG_DDR3_MR1_ADDR, uiReg);
#endif

	/* MR2 */
	if (MV_REG_READ(REG_DDR_IO_ADDR) & (1<<REG_DDR_IO_CLK_RATIO_OFFS))
		uiTemp = uiHClkTime / 2;
	else
		uiTemp = uiHClkTime;
		
		if (uiTemp >= 2500)
			uiCWL = 5; /* CWL = 5 */
		else if (uiTemp >= 1875 && uiTemp < 2500)
			uiCWL = 6; /* CWL = 6 */
		else if (uiTemp >= 1500 && uiTemp < 1875)
			uiCWL = 7; /* CWL = 7 */
		else if (uiTemp >= 1250 && uiTemp < 1500)
			uiCWL = 8; /* CWL = 8 */
		else if (uiTemp >= 1070 && uiTemp < 1250)
			uiCWL = 9; /* CWL = 9 */
		else if (uiTemp >= 935 && uiTemp < 1070)
			uiCWL = 10; /* CWL = 10 */
		else if (uiTemp >= 833 && uiTemp < 935)
			uiCWL = 11; /* CWL = 11 */
		else if (uiTemp >= 750 && uiTemp < 833)
			uiCWL = 12; /* CWL = 12 */
		
		uiReg = ((uiCWL - 5) << REG_DDR3_MR2_CWL_OFFS);

#ifdef MULTI_CS_MRS_SUPPORT
		for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
			if (uiCsEna & (1<<uiCs)) {
				uiReg &= REG_DDR3_MR2_ODT_MASK;
				uiReg |= auiODTDynamic[uiCsEna][uiCs];
				MV_REG_WRITE(REG_DDR3_MR2_CS_ADDR + (uiCs << MR_CS_ADDR_OFFS), uiReg);
			}
		}
#else
	MV_REG_WRITE(REG_DDR3_MR2_ADDR, uiReg);
#endif

	/* MR3 */
	uiReg = 0x00000000;
#ifdef MULTI_CS_MRS_SUPPORT
		for (uiCs = 0; uiCs < MAX_CS; uiCs++)
			if (uiCsEna & (1<<uiCs))
				MV_REG_WRITE(REG_DDR3_MR3_CS_ADDR + (uiCs << MR_CS_ADDR_OFFS), uiReg);
#else
	MV_REG_WRITE(REG_DDR3_MR3_ADDR, uiReg);
#endif

/*{0x00001428}	-	DDR ODT Timing (Low) Register */
	uiReg = 0;
	uiReg |= (((uiCL - uiCWL + 1) & 0xF) << 4);
	uiReg |= (((uiCL - uiCWL + 6) & 0xF) << 8);
	uiReg |= ((((uiCL - uiCWL + 6) >> 4) & 0x1) << 21);
	uiReg |= (((uiCL - 1) & 0xF) << 12);
	uiReg |= (((uiCL + 6) & 0x1F) << 16);
	MV_REG_WRITE(REG_ODT_TIME_LOW_ADDR, uiReg);

#ifdef DUNIT_SPD
/*{0x000015E0} - DDR3 Rank Control Register */
	uiReg = uiCsEna;
	uiCsCount = 0;
	uiDimmCount = 0;
	for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
		if (uiCsEna & (1<<uiCs) & DIMM_CS_BITMAP) {
			if (dimmInfo[uiDimmCount].numOfModuleRanks == uiCsCount) {
				uiDimmCount++;
				uiCsCount = 0;
			}
			uiCsCount++;
			if (dimmInfo[uiDimmCount].addressMirroring && (uiCs == 1 || uiCs == 3) &&
							(dimmSumInfo.dimmTypeInfo != SPD_MODULE_TYPE_RDIMM)) {
				uiReg |= (1<<(REG_DDR3_RANK_CTRL_MIRROR_OFFS+uiCs));
				DEBUG_INIT_FULL_C("DDR3 - SPD-SET - Setting Address Mirroring for CS = ", uiCs, 1);
			}
		}
	}
	MV_REG_WRITE(REG_DDR3_RANK_CTRL_ADDR, uiReg);
#endif

/*{0xD00015E4}	-	ZQDS Configuration Register */
	uiReg = 0x00203c18;
	MV_REG_WRITE(REG_ZQC_CONF_ADDR, uiReg);

/* {0x00015EC}	-	DDR PHY */
#if defined(MV88F78X60) && !defined(MV88F78X60_Z1)
	uiReg = 0xF800AAA5;
	if (mvCtrlRevGet() == MV_78XX0_B0_REV) {
		uiReg = 0xF800A225;
	}
#else	
	uiReg = 0xDE000025;
#endif
	MV_REG_WRITE(REG_DRAM_PHY_CONFIG_ADDR, uiReg);


#if defined(MV88F78X60) && !defined(MV88F78X60_Z1)
	/* Registered DIMM support - supported only in AXP A0 devices */
	/* Currently supported for SPD detection only */
	/* Flow is according to the Registered DIMM chapter in the Functional Spec */

	if (dimmSumInfo.dimmTypeInfo == SPD_MODULE_TYPE_RDIMM) {
		DEBUG_INIT_S("DDR3 Training Sequence - Registered DIMM detected \n");
		
		/* Set commands parity completion */
		uiReg = MV_REG_READ(REG_REGISTERED_DRAM_CTRL_ADDR);
		uiReg &= ~REG_REGISTERED_DRAM_CTRL_PARITY_MASK;
		uiReg |= 0x8;
		MV_REG_WRITE(REG_REGISTERED_DRAM_CTRL_ADDR, uiReg);

		MV_REG_WRITE(REG_SDRAM_INIT_CTRL_ADDR, 1 << REG_SDRAM_INIT_CKE_ASSERT_OFFS);	/* De-assert M_RESETn and assert M_CKE */
		do {
			uiReg = ((MV_REG_READ(REG_SDRAM_INIT_CTRL_ADDR)) & (1 << REG_SDRAM_INIT_CKE_ASSERT_OFFS));
		} while (uiReg);

		{
			MV_U32 uiRC;
			for (uiRC=0; uiRC<SPD_RDIMM_RC_NUM; uiRC++) {
				if (uiRC != 6 && uiRC != 7) {
#if 0
					uiReg = (REG_SDRAM_OPERATION_CMD_CWA & ~(uiCsEna << REG_SDRAM_OPERATION_CS_OFFS)); /* Set CWA Command */
#endif 
					uiReg = (REG_SDRAM_OPERATION_CMD_CWA & ~(0xF << REG_SDRAM_OPERATION_CS_OFFS)); /* Set CWA Command */
					uiReg |= ((dimmInfo[0].regDimmRC[uiRC] & REG_SDRAM_OPERATION_CWA_DATA_MASK) << REG_SDRAM_OPERATION_CWA_DATA_OFFS);
					uiReg |= uiRC << REG_SDRAM_OPERATION_CWA_RC_OFFS;
					/* Configure - Set Delay - tSTAB/tMRD */
					if (uiRC == 2 || uiRC == 10)
						uiReg |= (0x1 << REG_SDRAM_OPERATION_CWA_DELAY_SEL_OFFS);
	
					MV_REG_WRITE(REG_SDRAM_OPERATION_ADDR, uiReg);  	/* 0x1418 - SDRAM Operation Register */
						/* Poll the "cmd" field in the SDRAM OP register for 0x0 */
					do {
						uiReg = (MV_REG_READ(REG_SDRAM_OPERATION_ADDR) & (REG_SDRAM_OPERATION_CMD_MASK));
					} while (uiReg);
				}
			}
		}
	}
#endif

#if defined(MV88F78X60)
	/* DLB Enable */
#if defined(MV88F78X60_Z1)
	MV_REG_WRITE(DLB_BUS_OPTIMIZATION_WEIGHTS_REG, 0x18C01E);
#else 
	if (mvCtrlRevGet() == MV_78XX0_B0_REV)
		MV_REG_WRITE(DLB_BUS_OPTIMIZATION_WEIGHTS_REG, 0xc19e);
	else
		MV_REG_WRITE(DLB_BUS_OPTIMIZATION_WEIGHTS_REG, 0x18C01E);

#endif
	MV_REG_WRITE(DLB_AGING_REGISTER , 0x0f7f007f);
	MV_REG_WRITE(DLB_EVICTION_CONTROL_REG, 0x0);
	MV_REG_WRITE(DLB_EVICTION_TIMERS_REGISTER_REG, 0x00FF3C1F);

	MV_REG_WRITE(MBUS_UNITS_PRIORITY_CONTROL_REG, 0x55555555);
	MV_REG_WRITE(FABRIC_UNITS_PRIORITY_CONTROL_REG , 0xAA);
	MV_REG_WRITE(MBUS_UNITS_PREFETCH_CONTROL_REG, 0xffff);
	MV_REG_WRITE(FABRIC_UNITS_PREFETCH_CONTROL_REG, 0xf0f);

	if (mvCtrlRevGet() == MV_78XX0_B0_REV) {
		uiReg = MV_REG_READ(REG_STATIC_DRAM_DLB_CONTROL);
		uiReg |= DLB_ENABLE;
		MV_REG_WRITE(REG_STATIC_DRAM_DLB_CONTROL, uiReg);
	}
#endif

	return MV_OK;
}

/******************************************************************************
* Name:		ddr3DivFunc - this function divides integers
* Desc:
* Args:		uiValue - the value
*			uiDivider - the divider
*			uiSub - substruction value
* Notes:
* Returns:	required value
*/
MV_U32 ddr3DivFunc(MV_U32 uiValue, MV_U32 uiDivider, MV_U32 uiSub)
{
	return (uiValue/uiDivider + (uiValue % uiDivider > 0 ? 1 : 0) - uiSub);
}

/******************************************************************************
* Name:		ddr3GetMaxValue 
* Desc:
* Args:		
* Notes:
* Returns:
*/
MV_U32 ddr3GetMaxValue(MV_U32 spdVal, MV_U32 uiDimmNum, MV_U32 staticVal)
{
#ifdef DUNIT_STATIC
	if (uiDimmNum > 0) {
		if (spdVal >= staticVal)
			return spdVal;
		else
			return staticVal;
	} else
		return staticVal;
#else
	return spdVal;
#endif
}

/******************************************************************************
* Name:		ddr3GetMinValue 
* Desc:
* Args:		
* Notes:
* Returns:
*/
MV_U32 ddr3GetMinValue(MV_U32 spdVal, MV_U32 uiDimmNum, MV_U32 staticVal)
{
#ifdef DUNIT_STATIC
	if (uiDimmNum > 0) {
		if (spdVal <= staticVal)
			return spdVal;
		else
			return staticVal;
	} else
		return staticVal;
#else
	return spdVal;
#endif
}

#endif