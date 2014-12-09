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
#include "config_marvell.h"     /* Required to identify SOC and Board */

#include "ddr3_init.h"
#include "ddr3_spd.h"
#include "bin_hdr_twsi.h"
#include "mvUart.h"
#include "printf.h"
#include "mvSysEnvLib.h"
#include "ddr3_hws_hw_training.h"
#include "mvSiliconIf.h"

#if defined(MV88F69XX)
#include "ddr3_a39x.h"
#include "ddr3_a39x_vars.h"
#include "ddr3_a39x_topology.h"
#include "ddr_a38x_dlb_config.h"
#elif defined(MV88F68XX)
#include "ddr3_a38x.h"
#include "ddr3_a38x_vars.h"
#include "ddr3_a38x_topology.h"
#include "ddr_a38x_dlb_config.h"
#elif defined(MV_MSYS_BC2)
#include "ddr3_msys_bc2.h"
#include "ddr3_msys_bc2_config.h"
#include "ddr3_msys_bc2_vars.h"
#include "ddr3_msys_bc2_topology.h"
#elif defined(MV_MSYS_AC3)
#include "ddr3_msys_ac3.h"
#include "ddr3_msys_ac3_config.h"
#include "ddr3_msys_ac3_vars.h"
#include "ddr3_msys_ac3_topology.h"
#endif

#include "bootstrap_os.h"



#if defined(MV_MSYS_BC2)
#define MARVELL_BOARD	BC2_MARVELL_BOARD_ID_BASE
#elif defined(MV_MSYS_AC3)
#define MARVELL_BOARD	AC3_MARVELL_BOARD_ID_BASE
#endif
/* translates topology map definitions to real memory size in bits */
MV_U32 mv_memSize[] = { ADDR_SIZE_512Mb, ADDR_SIZE_1Gb, ADDR_SIZE_2Gb, ADDR_SIZE_4Gb ,ADDR_SIZE_8Gb };

MV_STATUS ddr3LoadTopologyMap(void);

extern MV_STATUS ddr3TipInitSpecificRegConfig
(
    MV_U32              devNum,
	MV_DRAM_MC_INIT		*regConfigArr
);

extern MV_VOID  ddr3TipSetTopologyMap
(
    MV_U32                  devNum,
    MV_HWS_TOPOLOGY_MAP     *topology
);

extern MV_U32 ddr3TipGetInitFreq();

extern MV_VOID ddr3HwsSetLogLevel(
	MV_DDR_LIB_DEBUG_BLOCK 	block,
	MV_U8					level
);

extern MV_STATUS ddr3TipTuneTrainingParams
(
    MV_U32                  devNum,
    MV_TUNE_TRAINING_PARAMS *params
);

#ifdef SUPPORT_STATIC_DUNIT_CONFIG
static MV_U32 ddr3GetStaticDdrMode(void);
#endif

/*Set 1 to use dynamic DUNIT configuration,
	set 0(supported for A380 and AC3) to configure DUNIT in values set by ddr3TipInitSpecificRegConfig*/
MV_U8 genericInitController = 1;

MV_VOID		getTargetFreq(MV_U32 uiFreqMode, MV_U32 *ddrFreq, MV_U32 *hclkPs);
MV_STATUS		ddr3FastPathDynamicCsSizeConfig(MV_U32 uiCsEna);
MV_VOID		ddr3FastPathStaticCsSizeConfig(MV_U32 uiCsEna);
MV_U32 ddr3GetDeviceWidth(MV_U32 uiCs);
MV_U32 	mvBoardIdIndexGet(MV_U32 boardId);
MV_U32 mvBoardIdGet(MV_VOID);
MV_U32 ddr3GetBusWidth(void);

extern MV_VOID ddr3SetLogLevel(MV_U32 nLogLevel);
static MV_U32 gLogLevel = 0;
static MV_STATUS ddr3HwsTuneTrainingParams(MV_U8 devNum);

MV_STATUS ddr3CalcMemCsSize(MV_U32 uiCs, MV_U32* puiCsSize);

#if defined(MV_DDR_TOPOLOGY_UPDATE_FROM_TWSI)
static MV_STATUS ddr3UpdateTopologyMap(MV_HWS_TOPOLOGY_MAP* topologyMap);
#endif

#ifdef CONFIG_DDR4
char* ddrType = "DDR4";
#else
char* ddrType = "DDR3";
#endif

/************************************************************************************
 * Name:     levelLogPrintD
 * Desc:     This routine printing data in hex-decimal if gLogLevel>=eLogLevel
 * Args:     char *str,MV_LOG_LEVEL eLogLevel
 * Notes:
 * Returns:  None.
 */
MV_VOID levelLogPrintD(MV_U32 dec_num, MV_U32 length, MV_LOG_LEVEL eLogLevel)
{
	if (gLogLevel >= eLogLevel)
		putdata(dec_num, length);
}

/************************************************************************************
 * Name:     levelLogPrintDD
 * Desc:     This routine printing data in decimal if gLogLevel>=eLogLevel
 * Args:     char *str,MV_LOG_LEVEL eLogLevel
 * Notes:
 * Returns:  None.
 */
MV_VOID levelLogPrintDD(MV_U32 dec_num, MV_U32 length, MV_LOG_LEVEL eLogLevel)
{
	if (gLogLevel >= eLogLevel)
		putdataDec(dec_num, length);
}

/******************************************************************************
 * Name:     ddr3GetTopologyMap
 * Desc:     gets board Topology map
 * Args:     pointer to topology map structure
 * Notes:
 * Returns:  MV_STATUS
 */
MV_STATUS ddr3GetTopologyMap(MV_HWS_TOPOLOGY_MAP** tMap)
{
	MV_U32 boardIdIndex = mvBoardIdIndexGet(mvBoardIdGet());


	/*Get topology data by board ID*/
	if (sizeof(TopologyMap)/sizeof(MV_HWS_TOPOLOGY_MAP*) > boardIdIndex)
		*tMap = &(TopologyMap[boardIdIndex]);
	else {
		mvPrintf("Failed get DDR3 Topology map (invalid board ID #d)\n",boardIdIndex);
		return MV_NOT_SUPPORTED;
	}

	return MV_OK;
}


static MV_VOID ddr3RestoreAndSetFinalWindows(MV_U32 *auWinBackup)
{
	MV_U32 winCtrlReg, numOfWinRegs;
	MV_U32 uiCsEna = ddr3GetCSEnaFromReg();

	winCtrlReg  = REG_XBAR_WIN_4_CTRL_ADDR;
	numOfWinRegs = 16;

	MV_U32 ui;
	/* Return XBAR windows 4-7 or 16-19 init configuration */
	for (ui = 0; ui < numOfWinRegs; ui++)
		MV_REG_WRITE((winCtrlReg + 0x4 * ui), auWinBackup[ui]);

	mvPrintf("%s Training Sequence - Switching XBAR Window to FastPath Window \n", ddrType);

#if defined DYNAMIC_CS_SIZE_CONFIG
	if (ddr3FastPathDynamicCsSizeConfig(uiCsEna)!= MV_OK)
		mvPrintf("ddr3FastPathDynamicCsSizeConfig FAILED\n");
#else
	MV_U32 uiReg, uiCs;
	uiReg = 0x1FFFFFE1;
	for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
		if (uiCsEna & (1 << uiCs)) {
			uiReg |= (uiCs << 2);
			break;
		}
	}
	MV_REG_WRITE(REG_FASTPATH_WIN_0_CTRL_ADDR, uiReg); /*Open fast path Window to - 0.5G */
#endif
}
static MV_STATUS ddr3SaveAndSetTrainingWindows(MV_U32 *auWinBackup)
{
	MV_U32 uiCsEna;
	MV_U32 uiReg, uiTempCount, uiCs, ui;
	MV_U32 winCtrlReg, winBaseReg, winRemapReg;
	MV_U32 numOfWinRegs, winJumpIndex;
	winCtrlReg  = REG_XBAR_WIN_4_CTRL_ADDR;
	winBaseReg  = REG_XBAR_WIN_4_BASE_ADDR;
	winRemapReg = REG_XBAR_WIN_4_REMAP_ADDR;
	winJumpIndex = 0x10;
	numOfWinRegs = 16;
	MV_HWS_TOPOLOGY_MAP* toplogyMap = NULL;

#ifdef DISABLE_L2_FILTERING_DURING_DDR_TRAINING
	/* Disable L2 filtering  during DDR training (when Cross Bar window is open)*/
	MV_REG_WRITE(ADDRESS_FILTERING_END_REGISTER, 0);
#endif

	CHECK_STATUS(ddr3GetTopologyMap(&toplogyMap));

	uiCsEna = toplogyMap->interfaceParams[0].asBusParams[0].csBitmask;

	/*  Close XBAR Window 19 - Not needed */
	/* {0x000200e8}  -   Open Mbus Window - 2G */
	MV_REG_WRITE(REG_XBAR_WIN_19_CTRL_ADDR, 0);

	/* Save XBAR Windows 4-19 init configurations */
	for (ui = 0; ui < numOfWinRegs; ui++)
		auWinBackup[ui] = MV_REG_READ(winCtrlReg + 0x4 * ui);

	/*  Open XBAR Windows 4-7 or 16-19 for other CS */
	uiReg = 0;
	uiTempCount = 0;
	for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
		if (uiCsEna & (1 << uiCs)) {
			switch (uiCs) {
			case 0:
				uiReg = 0x0E00;
				break;
			case 1:
				uiReg = 0x0D00;
				break;
			case 2:
				uiReg = 0x0B00;
				break;
			case 3:
				uiReg = 0x0700;
				break;
			}
			uiReg |= (1 << 0);
			uiReg |= (SDRAM_CS_SIZE & 0xFFFF0000);

			MV_REG_WRITE(winCtrlReg + winJumpIndex * uiTempCount, uiReg);
			uiReg = (((SDRAM_CS_SIZE + 1) * (uiTempCount)) & 0xFFFF0000);
			MV_REG_WRITE(winBaseReg + winJumpIndex * uiTempCount, uiReg);

			if(winRemapReg <= REG_XBAR_WIN_7_REMAP_ADDR)
				MV_REG_WRITE(winRemapReg + winJumpIndex * uiTempCount, 0);

			uiTempCount++;
		}
	}
return MV_OK;
}

/************************************************************************************
 * Name:     ddr3Init - Main DDR3 Init function
 * Desc:     This routine initialize the DDR3 MC and runs HW training.
 * Args:     None.
 * Notes:
 * Returns:  None.
 */
MV_U32 ddr3Init(void)
{
	MV_U32 uiReg = 0;
	MV_U32 socNum;
	MV_STATUS status;
	MV_U32 auWinBackup[16];

	/* SoC/Board special Initializtions */
	/*Get version from internal library*/
	ddr3PrintVersion();

	/*Add sub_version string*/
	DEBUG_INIT_C("",SUB_VERSION,1);

	/* Switching CPU to MRVL ID */
	socNum = (MV_REG_READ(REG_SAMPLE_RESET_HIGH_ADDR) & SAR1_CPU_CORE_MASK) >> SAR1_CPU_CORE_OFFSET;
	switch (socNum) {
	case 0x3:
		MV_REG_BIT_SET(CPU_CONFIGURATION_REG(3), CPU_MRVL_ID_OFFSET);
		MV_REG_BIT_SET(CPU_CONFIGURATION_REG(2), CPU_MRVL_ID_OFFSET);
	case 0x1:
		MV_REG_BIT_SET(CPU_CONFIGURATION_REG(1), CPU_MRVL_ID_OFFSET);
	case 0x0:
		MV_REG_BIT_SET(CPU_CONFIGURATION_REG(0), CPU_MRVL_ID_OFFSET);
	default:
		break;
	}

	/* Set DRAM Reset Mask in case of wakeup from suspend
	 * i.e the DRAM values will not be overwritten / reset when waking from suspend*/
	if (mvSysEnvSuspendWakeupCheck())
		MV_REG_BIT_SET(REG_SDRAM_INIT_CTRL_ADDR, 1 << REG_SDRAM_INIT_RESET_MASK_OFFS);

	/************************************************************************************/
	/* Stage 0 - Set board configuration                                                */
	/************************************************************************************/

	/* Check if DRAM is already initialized  */
	if (MV_REG_READ(REG_BOOTROM_ROUTINE_ADDR) & (1 << REG_BOOTROM_ROUTINE_DRAM_INIT_OFFS)) {
		mvPrintf("%s Training Sequence - 2nd boot - Skip \n", ddrType);
		return MV_OK;
	}

	/************************************************************************************/
	/* Stage 1 - Dunit Setup                                                            */
	/************************************************************************************/

	/* Fix read ready phases for all SOC in reg 0x15C8*/
	uiReg = MV_REG_READ(REG_TRAINING_DEBUG_3_ADDR);
	uiReg &= ~(REG_TRAINING_DEBUG_3_MASK);
	uiReg |= 0x4;                                           /* Phase 0 */
	uiReg &= ~(REG_TRAINING_DEBUG_3_MASK << REG_TRAINING_DEBUG_3_OFFS);
	uiReg |= (0x4 << (1 * REG_TRAINING_DEBUG_3_OFFS));      /* Phase 1 */
	uiReg &= ~(REG_TRAINING_DEBUG_3_MASK << (3 * REG_TRAINING_DEBUG_3_OFFS));
	uiReg |= (0x6 << (3 * REG_TRAINING_DEBUG_3_OFFS));      /* Phase 3 */
	uiReg &= ~(REG_TRAINING_DEBUG_3_MASK << (4 * REG_TRAINING_DEBUG_3_OFFS));
	uiReg |= (0x6 << (4 * REG_TRAINING_DEBUG_3_OFFS));
	uiReg &= ~(REG_TRAINING_DEBUG_3_MASK << (5 * REG_TRAINING_DEBUG_3_OFFS));
	uiReg |= (0x6 << (5 * REG_TRAINING_DEBUG_3_OFFS));
	MV_REG_WRITE(REG_TRAINING_DEBUG_3_ADDR, uiReg);

	/* AxiBrespMode[8] = Compliant, AxiAddrDecodeCntrl[11] = Internal, AxiDataBusWidth[0] = 128bit */
	MV_REG_WRITE(REG_DRAM_AXI_CTRL_ADDR, 0);                /* 0x14A8 - AXI Control Register */

	/************************************************************************************/
	/* Stage 2 - Training Values Setup                                                  */
	/************************************************************************************/
	/* Set X-BAR windows for the training sequence */
	ddr3SaveAndSetTrainingWindows(auWinBackup);

#ifdef SUPPORT_STATIC_DUNIT_CONFIG
	/* load static controller configuration (in case dynamic/geenric init is not enabled */
	if( genericInitController == 0){
		ddr3TipInitSpecificRegConfig(0, ddr_modes[ddr3GetStaticDdrMode()].regs);
	}
#endif

	/*Load topology for New Training IP*/
	status = ddr3LoadTopologyMap();
	if (MV_OK != status) {
		mvPrintf("%s Training Sequence topology load - FAILED\n", ddrType);
		return status;
	}

	/*Tune training algo paramteres*/
	status = ddr3HwsTuneTrainingParams(0);
	if (MV_OK != status) {
		return status;
	}

	/*Set log level for training lib*/
	ddr3HwsSetLogLevel(MV_DEBUG_BLOCK_ALL, DEBUG_LEVEL_ERROR);

	/*Start New Training IP*/
	status = ddr3HwsHwTraining();
	if (MV_OK != status) {
		mvPrintf("%s Training Sequence - FAILED\n", ddrType);
		return status;
	}

	/************************************************************************************/
	/* Stage 3 - Finish                                                                 */
	/************************************************************************************/
	/* Restore and set windows */
	ddr3RestoreAndSetFinalWindows(auWinBackup);

	/* Update DRAM init indication in bootROM register */
	uiReg = MV_REG_READ(REG_BOOTROM_ROUTINE_ADDR);
	MV_REG_WRITE(REG_BOOTROM_ROUTINE_ADDR, uiReg | (1 << REG_BOOTROM_ROUTINE_DRAM_INIT_OFFS));

	/* DLB config */
	ddr3NewTipDlbConfig();

#if defined(ECC_SUPPORT)
	if( MV_TRUE == ddr3IfEccEnabled()){
		ddr3NewTipEccScrub();
	}
#endif

	mvPrintf("%s Training Sequence - Ended Successfully\n", ddrType);

	return MV_OK;
}

/******************************************************************************
 * Name:     ddr3GetCpuFreq
 * Desc:     read S@R and return CPU frequency
 * Args:
 * Notes:
 * Returns:  required value
 */

MV_U32 ddr3GetCpuFreq(void)
{
	return  ddr3TipGetInitFreq();
}

/******************************************************************************
 * Name:     ddr3GetFabOpt
 * Desc:     read S@R and return CPU frequency
 * Args:
 * Notes:
 * Returns:  required value
 */

MV_U32 ddr3GetFabOpt(void)
{
	return 0; /* No fabric */
}

/************************************************************************************
 * Name:     ddr3GetStaticMCValue - Init Memory controller with static parameters
 * Desc:     Use this routine to init the controller without the HW training procedure
 *           User must provide compatible header file with registers data.
 * Args:     None.
 * Notes:
 * Returns:  None.
 */
MV_U32 ddr3GetStaticMCValue(MV_U32 regAddr, MV_U32 offset1, MV_U32 mask1, MV_U32 offset2, MV_U32 mask2)
{
	MV_U32 uiReg, uiTemp;

	uiReg = MV_REG_READ(regAddr);

	uiTemp = (uiReg >> offset1) & mask1;
	if (mask2)
		uiTemp |= (uiReg >> offset2) & mask2;

	return uiTemp;
}

/************************************************************************************
 * Name:     ddr3GetStaticDdrMode - Init Memory controller with static parameters
 * Desc:     Use this routine to init the controller without the HW training procedure
 *           User must provide compatible header file with registers data.
 * Args:     None.
 * Notes:
 * Returns:  None.
 */
MV_U32 ddr3GetStaticDdrMode(void)
{

	MV_U32 chipBoardRev, i;
	MV_U32 size;

/*Valid only for A380 only, MSYS using dynamic controller config*/
#ifdef CONFIG_CUSTOMER_BOARD_SUPPORT
	chipBoardRev = mvBoardIdGet();	/* Customer boards select DDR mode according to board ID & Sample@Reset*/
#else
	chipBoardRev = MARVELL_BOARD;	/* Marvell boards select DDR mode according to Sample@Reset only */
#endif

	size = sizeof(ddr_modes) / sizeof(MV_DRAM_MODES);
	for (i = 0; i < size; i++) {
		if ((ddr3GetCpuFreq() == ddr_modes[i].cpuFreq) && (ddr3GetFabOpt() == ddr_modes[i].fabFreq) &&
		    (chipBoardRev == ddr_modes[i].chipBoardRev))
			return i;
	}

	DEBUG_INIT_S("\n*** Error: ddr3GetStaticDdrMode: No match for requested DDR mode. ***\n\n");
	return 0;
}

/******************************************************************************
 * Name:     ddr3GetCSNumFromReg
 * Desc:
 * Args:
 * Notes:
 * Returns:
 */
MV_U32 ddr3GetCSNumFromReg(void)
{
	MV_U32 uiCsEna = ddr3GetCSEnaFromReg();
	MV_U32 uiCsCount = 0;
	MV_U32 uiCs;

	for (uiCs = 0; uiCs < MAX_CS; uiCs++)
		if (uiCsEna & (1 << uiCs))
			uiCsCount++;

	return uiCsCount;
}

/******************************************************************************
 * Name:     ddr3GetCSEnaFromReg
 * Desc:
 * Args:
 * Notes:
 * Returns:
 */
MV_U32 ddr3GetCSEnaFromReg(void)
{
	return MV_REG_READ(REG_DDR3_RANK_CTRL_ADDR) & REG_DDR3_RANK_CTRL_CS_ENA_MASK;
}

/******************************************************************************
 * Name:     ddr3LoadTopologyMap
 * Desc:
 * Args:
 * Notes:
 * Returns:
 */
MV_STATUS ddr3LoadTopologyMap(void)
{
	MV_HWS_TOPOLOGY_MAP* toplogyMap = NULL;
	MV_U8 	devNum = 0;

	CHECK_STATUS(ddr3GetTopologyMap(&toplogyMap));

#if defined(MV_DDR_TOPOLOGY_UPDATE_FROM_TWSI)
	/*Update topology data*/
	if(MV_OK != ddr3UpdateTopologyMap(toplogyMap))
		DEBUG_INIT_FULL_S("Failed update of DDR3 Topology map\n");
#endif

	/*Set topology data for internal DDR training usage*/
	ddr3TipSetTopologyMap(devNum, toplogyMap);

	return MV_OK;
}

MV_VOID getTargetFreq(MV_U32 uiFreqMode, MV_U32 *ddrFreq, MV_U32 *hclkPs)
{
	MV_U32 tmp, hclk = 200;

	switch(uiFreqMode)
	{
		case 4:
			tmp = 1; //DDR_400;
			hclk = 200;
			break;
		case 0x8:
			tmp = 1; //DDR_666;
			hclk = 333;
			break;
		case 0xc:
			tmp = 1; //DDR_800;
			hclk = 400;
			break;
		default:
			*ddrFreq = 0;
			*hclkPs = 0;
			break;
	}
	*ddrFreq = tmp; /* DDR freq define */
	*hclkPs = 1000000/hclk; /* values are 1/HCLK in ps */
		return;
}


MV_VOID ddr3NewTipDlbConfig()
{
	MV_U32 uiReg, i = 0;
	MV_DRAM_DLB_CONFIG  *ConfigTablePtr= mvSysEnvDlbConfigPtrGet();


	/*Write the configuration*/
	while(ConfigTablePtr[i].regAddr != 0)
	{
		MV_REG_WRITE(ConfigTablePtr[i].regAddr, ConfigTablePtr[i].regData);
		i++;
	}

#ifdef CONFIG_DDR4
	uiReg = MV_REG_READ(REG_DDR_CONT_HIGH_ADDR);
	uiReg |= DLB_INTERJECTION_ENABLE;
	MV_REG_WRITE(REG_DDR_CONT_HIGH_ADDR, uiReg);
#endif

	/*Enable DLB*/
	uiReg = MV_REG_READ(REG_STATIC_DRAM_DLB_CONTROL);
	uiReg |= (DLB_ENABLE | DLB_WRITE_COALESING | DLB_AXI_PREFETCH_EN | DLB_MBUS_PREFETCH_EN | PreFetchNLnSzTr);
	MV_REG_WRITE(REG_STATIC_DRAM_DLB_CONTROL, uiReg);
}

MV_STATUS ddr3FastPathDynamicCsSizeConfig(MV_U32 uiCsEna) {

	MV_U32 uiReg, uiCs;
    MV_U32 uiMemTotalSize = 0;
    MV_U32 uiCsMemSize = 0;
	MV_U32 uiMemTotalSize_c, uiCsMemSize_c;

#ifdef MV_DEVICE_MAX_DRAM_ADDRESS_SIZE
	MV_U32 physicalMemSize;
	MV_U32 maxMemSize = MV_DEVICE_MAX_DRAM_ADDRESS_SIZE;
	MV_HWS_TOPOLOGY_MAP* toplogyMap = NULL;
#endif

	/* Open fast path windows */
    for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
        if (uiCsEna & (1 << uiCs)) {
		    /* get CS size */
			if (ddr3CalcMemCsSize(uiCs, &uiCsMemSize) != MV_OK)
                return MV_FAIL;

#ifdef MV_DEVICE_MAX_DRAM_ADDRESS_SIZE
			/* if number of address pins doesn't allow to use max mem size that is defined in topology
			 mem size is defined by MV_DEVICE_MAX_DRAM_ADDRESS_SIZE*/
			CHECK_STATUS(ddr3GetTopologyMap(&toplogyMap));
			physicalMemSize = mv_memSize [toplogyMap->interfaceParams[0].memorySize];

			if (ddr3GetDeviceWidth(uiCs) == 16)
				maxMemSize = MV_DEVICE_MAX_DRAM_ADDRESS_SIZE * 2; /* 16bit mem device can be twice more - no need in less significant pin*/

			if (physicalMemSize > maxMemSize ){
				uiCsMemSize = maxMemSize * (ddr3GetBusWidth() / ddr3GetDeviceWidth(uiCs)) ;
				mvPrintf ("Updated Physical Mem size is from 0x%x to %x\n", physicalMemSize, MV_DEVICE_MAX_DRAM_ADDRESS_SIZE);
			}
#endif
           /* set fast path window control for the cs */
            uiReg = 0xFFFFE1;
            uiReg |= (uiCs << 2);
            uiReg |= (uiCsMemSize - 1) & 0xFFFF0000;
            MV_REG_WRITE(REG_FASTPATH_WIN_CTRL_ADDR(uiCs), uiReg); /*Open fast path Window */
            /* set fast path window base address for the cs */
            uiReg = ((uiCsMemSize) * uiCs) & 0xFFFF0000;
            MV_REG_WRITE(REG_FASTPATH_WIN_BASE_ADDR(uiCs), uiReg); /*Set base address */
			/* since memory size may be bigger than 4G the summ may be more than 32 bit word,
			so to estimate the result divide uiMemTotalSize and uiCsMemSize by 0x10000 (it is equal to >>16) */
			uiMemTotalSize_c = uiMemTotalSize >> 16;
			uiCsMemSize_c = uiCsMemSize >>16;
			/*if the sum less than 2 G - calculate the value*/
			if (uiMemTotalSize_c + uiCsMemSize_c < 0x10000)
				uiMemTotalSize += uiCsMemSize;
			else /* put max possible size */
				uiMemTotalSize = L2_FILTER_FOR_MAX_MEMORY_SIZE;
       }
    }
	/* Set L2 filtering to Max Memory size */
	MV_REG_WRITE( ADDRESS_FILTERING_END_REGISTER, uiMemTotalSize);
	return MV_OK;
}

MV_U32 ddr3GetBusWidth(void) {

    MV_U32 uiBusWidth;

    uiBusWidth = (MV_REG_READ(REG_SDRAM_CONFIG_ADDR) & 0x8000) >> REG_SDRAM_CONFIG_WIDTH_OFFS;

    return (uiBusWidth == 0) ? 16 : 32;
}

MV_U32 ddr3GetDeviceWidth(MV_U32 uiCs) {

    MV_U32 uiDeviceWidth;
    uiDeviceWidth = (MV_REG_READ(REG_SDRAM_ADDRESS_CTRL_ADDR) & (0x3 << (REG_SDRAM_ADDRESS_CTRL_STRUCT_OFFS * uiCs))) >> (REG_SDRAM_ADDRESS_CTRL_STRUCT_OFFS * uiCs);

    return (uiDeviceWidth == 0) ? 8 : 16;
}

MV_FLOAT ddr3GetDeviceSize(MV_U32 uiCs) {

    MV_U32 uiDeviceSizeLow, uiDeviceSizeHigh, uiDeviceSize;
    MV_U32 uiRegData, uiCsLowOffset, uiCsHighOffset;

    uiCsLowOffset = REG_SDRAM_ADDRESS_SIZE_OFFS + uiCs * 4;
    uiCsHighOffset = REG_SDRAM_ADDRESS_SIZE_OFFS + REG_SDRAM_ADDRESS_SIZE_HIGH_OFFS + uiCs;

    uiRegData = MV_REG_READ(REG_SDRAM_ADDRESS_CTRL_ADDR);

    uiDeviceSizeLow = (uiRegData >> uiCsLowOffset) & 0x3;
    uiDeviceSizeHigh = (uiRegData >> uiCsHighOffset) & 0x1;

    uiDeviceSize = uiDeviceSizeLow | (uiDeviceSizeHigh << 2);

    switch(uiDeviceSize)
    {
        case 0:
            return 2;
        case 2:
            return 0.5;
        case 3:
            return 1;
        case 4:
            return 4;
        case 5:
            return 8;
        case 1:
        default:
            DEBUG_INIT_C("Error: Wrong device size of Cs: ", uiCs, 1);
            return 0.01; /* small value will give wrong emem size in ddr3CalcMemCsSize */
    }
}

MV_STATUS ddr3CalcMemCsSize(MV_U32 uiCs, MV_U32* puiCsSize){

    MV_FLOAT uiCsMemSize;

    uiCsMemSize = ((ddr3GetBusWidth() / ddr3GetDeviceWidth(uiCs)) * ddr3GetDeviceSize(uiCs)) / 8;/*calculate in Gbyte*/;

	if (uiCsMemSize == 0.125) {
        *puiCsSize = _128M;
    } else if (uiCsMemSize == 0.25) {
        *puiCsSize = _256M;
    } else if (uiCsMemSize == 0.5) {
        *puiCsSize = _512M;
    } else if (uiCsMemSize == 1) {
        *puiCsSize = _1G;
    } else if (uiCsMemSize == 2) {
        *puiCsSize = _2G;
    } else {
        DEBUG_INIT_C("Error: Wrong Memory size of Cs: ", uiCs, 1);
        return MV_BAD_VALUE;
    }
    return MV_OK;
}

#if defined(MV_DDR_TOPOLOGY_UPDATE_FROM_TWSI)
/******************************************************************************
 * Name:     ddr3UpdateTopologyMap
 * Desc:
 * Args:
 * Notes: Update topology map by SatR values
 * Returns:
 */
static MV_STATUS ddr3UpdateTopologyMap(MV_HWS_TOPOLOGY_MAP* topologyMap)
{
	MV_TOPOLOGY_UPDATE_INFO topologyUpdateInfo;

	topologyUpdateInfo.mvUpdateWidth = MV_FALSE;
	topologyUpdateInfo.mvUpdateWidth = MV_FALSE;
	mvSysEnvGetTopologyUpdateInfo(&topologyUpdateInfo);

	if(topologyUpdateInfo.mvUpdateECC == MV_TRUE)
	{
		topologyMap->activeBusMask &= ~(1<<MV_TOPOLOGY_UPDATE_ECC_OFFSET);
		topologyMap->activeBusMask |= topologyUpdateInfo.mvECC << MV_TOPOLOGY_UPDATE_ECC_OFFSET;
	}

	if(topologyUpdateInfo.mvUpdateWidth == MV_TRUE)
	{
		topologyMap->activeBusMask &= ~(MV_TOPOLOGY_UPDATE_WIDTH_ALL_BIT_MASK);
		if(topologyUpdateInfo.mvWidth == MV_TOPOLOGY_UPDATE_WIDTH_16BIT)
			topologyMap->activeBusMask |= MV_TOPOLOGY_UPDATE_WIDTH_16BIT_MASK;
		else
			topologyMap->activeBusMask |= MV_TOPOLOGY_UPDATE_WIDTH_32BIT_MASK;
	}

	return MV_OK;
}
#endif

/******************************************************************************
 * Name:     ddr3HwsTuneTrainingParams
 * Desc:
 * Args:
 * Notes: Tune internal training params
 * Returns:
 */
static MV_STATUS ddr3HwsTuneTrainingParams(MV_U8 devNum)
{
	MV_TUNE_TRAINING_PARAMS params;
	MV_STATUS status;

	/*NOTE: do not remove any field initilization*/
	params.ckDelay = MV_TUNE_TRAINING_PARAMS_CK_DELAY;
	params.ckDelay_16 = MV_TUNE_TRAINING_PARAMS_CK_DELAY_16;
	params.Pfinger = MV_TUNE_TRAINING_PARAMS_PFINGER;
	params.Nfinger = MV_TUNE_TRAINING_PARAMS_NFINGER;
	params.PhyReg3Val = MV_TUNE_TRAINING_PARAMS_PHYREG3VAL;

	status = ddr3TipTuneTrainingParams(devNum, &params);
	if (MV_OK != status) {
		mvPrintf("%s Training Sequence - FAILED\n", ddrType);
		return status;
	}

	return MV_OK;
}
