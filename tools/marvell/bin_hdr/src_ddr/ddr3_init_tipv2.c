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

#if defined(MV_MSYS_BC2)
#define MARVELL_BOARD	BC2_MARVELL_BOARD_ID_BASE
#elif defined(MV_MSYS_AC3)
#define MARVELL_BOARD	AC3_MARVELL_BOARD_ID_BASE
#endif

extern MV_STATUS ddr3TipInitSpecificRegConfig
(
    MV_U32              devNum,
	MV_DRAM_MC_INIT		*regConfigArr
);
extern MV_STATUS    ddr3TipSetTopologyMap
(
    MV_U32                  devNum,
    MV_HWS_TOPOLOGY_MAP     *topology
);
extern MV_U32 ddr3TipGetInitFreq();
MV_STATUS ddr3LoadTopologyMap(void);
extern MV_VOID ddr3HwsSetLogLevel(
	MV_DDR_LIB_DEBUG_BLOCK 	block,
	MV_U8					level
);

#include "bootstrap_os.h"

#ifdef SUPPORT_STATIC_DUNIT_CONFIG
static MV_U32 ddr3GetStaticDdrMode(void);
#endif

/*Set 1 to use dynamic DUNIT configuration,
	set 0(supported for A380 and AC3) to configure DUNIT in values set by ddr3TipInitSpecificRegConfig*/
MV_U8 genericInitController = 1;

MV_VOID		getTargetFreq(MV_U32 uiFreqMode, MV_U32 *ddrFreq, MV_U32 *hclkPs);
MV_VOID		ddr3FastPathDynamicCsSizeConfig(MV_U32 uiCsEna);
MV_VOID		ddr3FastPathStaticCsSizeConfig(MV_U32 uiCsEna);

MV_U32 	mvBoardIdIndexGet(MV_U32 boardId);
MV_U32 mvBoardIdGet(MV_VOID);

extern MV_VOID ddr3SetLogLevel(MV_U32 nLogLevel);
static MV_U32 gLogLevel = 0;

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

#if defined(MV88F68XX) || defined(MV88F69XX)
	ddr3FastPathDynamicCsSizeConfig(uiCsEna);
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

static MV_VOID ddr3SaveAndSetTrainingWindows(MV_U32 *auWinBackup)
{
	MV_U32 uiCsEna = ddr3GetCSEnaFromReg();
	MV_U32 uiReg, uiTempCount, uiCs, ui;
	MV_U32 winCtrlReg, winBaseReg, winRemapReg;
	MV_U32 numOfWinRegs, winJumpIndex;

	winCtrlReg  = REG_XBAR_WIN_4_CTRL_ADDR;
	winBaseReg  = REG_XBAR_WIN_4_BASE_ADDR;
	winRemapReg = REG_XBAR_WIN_4_REMAP_ADDR;
	winJumpIndex = 0x10;
	numOfWinRegs = 16;

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

	mvPrintf("%s Training Sequence - Ended Successfully\n", ddrType);

#if defined(MV88F68XX) || defined(MV88F69XX)
	if( MV_TRUE == ddr3IfEccEnabled()){
		ddr3NewTipEccScrub();
	}
#endif

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
	MV_HWS_TOPOLOGY_MAP* topologyMap;
	MV_U8 	devNum = 0;
	MV_U32 boardIdIndex = mvBoardIdIndexGet(mvBoardIdGet());


	/*Get topology data by board ID*/
	if (sizeof(TopologyMap)/sizeof(MV_HWS_TOPOLOGY_MAP*) > boardIdIndex)
		topologyMap = &TopologyMap[boardIdIndex];
	else {
		DEBUG_INIT_FULL_S("Failed loading DDR3 Topology map (invalid board ID)\n");
		return MV_FAIL;
	}

#if defined(MV_DDR_TOPOLOGY_UPDATE_FROM_TWSI)
	/*Update topology data*/
	if(MV_OK != ddr3UpdateTopologyMap(topologyMap))
		DEBUG_INIT_FULL_S("Failed update of DDR3 Topology map\n");
#endif

	/*Set topology data for internal DDR training usage*/
	if(MV_OK != ddr3TipSetTopologyMap(devNum, topologyMap))
		return MV_FAIL;

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

	/*Write the configuration*/
	while(ddr3DlbConfigTable[i].regAddr != 0)
	{
		MV_REG_WRITE(ddr3DlbConfigTable[i].regAddr, ddr3DlbConfigTable[i].regData);
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

MV_VOID ddr3FastPathDynamicCsSizeConfig(MV_U32 uiCsEna) {

	MV_U32 uiReg, uiCs;
    MV_U32 uiMemTotalSize = 0;
    MV_U32 uiCsMemSize = 0;
    /* Open fast path windows */
    for (uiCs = 0; uiCs < MAX_CS; uiCs++) {
        if (uiCsEna & (1 << uiCs)) {
            /* get CS size */

            if (ddr3CalcMemCsSize(uiCs, &uiCsMemSize) != MV_OK)
                return;

            /* set fast path window control for the cs */
            uiReg = 0xFFFFE1;
            uiReg |= (uiCs << 2);
            uiReg |= (uiCsMemSize - 1) & 0xFFFF0000;
            MV_REG_WRITE(REG_FASTPATH_WIN_CTRL_ADDR(uiCs), uiReg); /*Open fast path Window */
            /* set fast path window base address for the cs */
            uiReg = ((uiCsMemSize) * uiCs) & 0xFFFF0000;
            MV_REG_WRITE(REG_FASTPATH_WIN_BASE_ADDR(uiCs), uiReg); /*Set base address */
            uiMemTotalSize += uiCsMemSize;
#if defined(MV88F68XX) || defined(MV88F69XX)
			break;/*KW28 works with single CS*/
#endif
        }
    }
    /* Set L2 filtering to Max Memory size */
    MV_REG_WRITE(0x8c04, uiMemTotalSize);
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
	MV_U8	configVal;
	MV_TWSI_SLAVE twsiSlave;
	/*Fix the topology for A380 by SatR values*/
	twsiSlave.slaveAddr.address = 0x50;
	twsiSlave.slaveAddr.type = ADDR7_BIT;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 0;
	twsiSlave.moreThen256 = MV_TRUE;

	/* Reading board id */
	if (mvTwsiRead(0, &twsiSlave, &configVal, 1) != MV_OK) {
		DEBUG_INIT_S("mvHwsBoardIdGet: TWSI Read failed\n");
		return MV_FAIL;
	}

	/*Read 3 bits, 0 is 16/32 mode, 1 is ECC mode, 2 is special ECC mode*/
	switch( (configVal & DDR_SATR_CONFIG_MASK) >> DDR_SATR_CONFIG_OFFSET ){
		case DDR_SATR_16BIT_VALUE:
			topologyMap->activeBusMask = BUS_MASK_16BIT;
			break;
		case DDR_SATR_16BIT_NOT_VALID_ECC_VALUE:
			topologyMap->activeBusMask = BUS_MASK_16BIT;
			DEBUG_INIT_S("The ECC DRAM mode not valid, configured 16bit mode no ECC\n");
			break;
		case DDR_SATR_16BIT_ECC_PUP3_VALUE:
		case DDR_SATR_16BIT_ECC_PUP4_VALUE:
			topologyMap->activeBusMask = BUS_MASK_16BIT;
			DEBUG_INIT_S("The ECC DRAM mode not supported, ECC disabled for 16bit mode\n");
			break;

		case DDR_SATR_32BIT_VALUE:
			topologyMap->activeBusMask = BUS_MASK_32BIT;
			break;
		case DDR_SATR_32BIT_ECC_VALUE:
			topologyMap->activeBusMask = BUS_MASK_32BIT;
			DEBUG_INIT_S("The ECC DRAM mode not supported, ECC disabled for 32bit mode\n");
			break;
		case DDR_SATR_32BIT_NOT_VALID_NO_ECC_VALUE:
		case DDR_SATR_32BIT_NOT_VALID_ECC_VALUE:
			topologyMap->activeBusMask = BUS_MASK_32BIT;
			DEBUG_INIT_S("The ECC DRAM mode not valid, configured 32bit mode no ECC\n");
			break;
		default:
			break;
	}

	return MV_OK;
}
#endif

