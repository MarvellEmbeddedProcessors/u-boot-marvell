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

#include "mv_os.h"
#include "config_marvell.h"  	/* Required to identify SOC and Board */
#if defined(MV88F78X60)
#include "ddr3_axp.h"
#include "ddr3_axp_config.h"
#elif defined(MV88F6710)
#include "ddr3_a370.h"
#else
#error "No SOC define for uart in binary header."
#endif
#include "mvHighSpeedEnvSpec.h"
#include "mvBHboardEnvSpec.h"

#include "bin_hdr_twsi.h"
#include "mvUart.h"
#include "util.h"


#define	SERDES_VERION	"2.1.3"
#define ENDED_OK "High speed PHY - Ended Successfully\n"
static const MV_U8 serdesCfg[][SERDES_LAST_UNIT] = BIN_SERDES_CFG;
			   
extern MV_BIN_SERDES_CFG *SerdesInfoTbl[];
MV_U32 mvPexConfigRead(MV_U32 pexIf, MV_U32 bus, MV_U32 dev, MV_U32 func, MV_U32 regOff);
MV_STATUS mvPexLocalBusNumSet(MV_U32 pexIf, MV_U32 busNum);
MV_STATUS mvPexLocalDevNumSet(MV_U32 pexIf, MV_U32 devNum);

/***************************   defined ******************************/
#define MV_BOARD_PEX_MODULE_ADDR		0x23
#define MV_BOARD_PEX_MODULE_ADDR_TYPE	ADDR7_BIT
#define MV_BOARD_PEX_MODULE_ID			0
/*******************************************************/
#define mvBoardPexModeGet(satr) 		((satr & 0x6) >> 1)
#define mvBoardPexCapabilityGet(satr) 	(satr & 1)
#define MV_PEX_UNIT_TO_IF(pexUnit)	((pexUnit < 3) ? (pexUnit*4) : 9)
/******************   Static parametes ******************************/
MV_BOOL PexModule = 0;
MV_BOOL SwitchModule = 0;

/****************************  Local function *****************************************/
MV_U32 mvBoardIdGet(MV_VOID)
{
#if defined(DB_88F78X60)
	return DB_88F78XX0_BP_ID;
#elif defined(RD_88F78460_SERVER)
	return RD_78460_SERVER_ID;
#elif defined(RD_78460_SERVER_REV2)
	return RD_78460_SERVER_REV2_ID;
#elif defined(DB_78X60_PCAC)
	return DB_78X60_PCAC_ID;
#elif defined(DB_88F78X60_REV2)
	return DB_88F78XX0_BP_REV2_ID;
#elif defined(RD_78460_NAS)
	return RD_78460_NAS_ID;
#elif defined(DB_78X60_AMC)
	return DB_78X60_AMC_ID;
#elif defined(DB_78X60_PCAC_REV2)
	return DB_78X60_PCAC_REV2_ID;
#elif defined(DB_784MP_GP)
	return DB_784MP_GP_ID;
#elif defined(RD_78460_CUSTOMER)
	return RD_78460_CUSTOMER_ID;
#else
	while (1) {
		continue;
	}
#endif
}
/*******************************************************************************
* mvCtrlPexMaxIfGet - Get Marvell controller number of PEX interfaces.
*
* DESCRIPTION:
*       This function returns Marvell controller number of PEX interfaces.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Marvell controller number of PEX interfaces. If controller
*		ID is undefined the function returns '0'.
*
*******************************************************************************/
MV_U32 mvBoardTclkGet(MV_VOID)
{
	if ((MV_REG_READ(MPP_SAMPLE_AT_RESET(0)) & MSAR_TCLK_MASK) != 0)
		return MV_BOARD_TCLK_200MHZ;
	else
		return MV_BOARD_TCLK_250MHZ;
}
/*********************************************************************/
extern MV_U8  rd78460gpInfoBoardTwsiDev[] ;
extern MV_U8 db88f78XX0rev2InfoBoardTwsiDev[];
MV_U8 mvBoardTwsiSatRGet(MV_U8 devNum, MV_U8 regNum)
{
        MV_TWSI_SLAVE twsiSlave;
        MV_TWSI_ADDR slave;
        MV_U8 data;
		MV_U8  *pDev;
		MV_U32 boardID = mvBoardIdGet();

        /* TWSI init */
        slave.type = ADDR7_BIT;
        slave.address = 0;
		mvTwsiInit(0, CONFIG_SYS_I2C_SPEED, CONFIG_SYS_TCLK, &slave, 0);
		switch (boardID) {
		case DB_784MP_GP_ID:
			pDev = rd78460gpInfoBoardTwsiDev;
			
			break;
		case DB_88F78XX0_BP_ID:
		case DB_88F78XX0_BP_REV2_ID:
			pDev = db88f78XX0rev2InfoBoardTwsiDev;
			break;

		case DB_78X60_PCAC_ID:
		case FPGA_88F78XX0_ID:
		case DB_78X60_PCAC_REV2_ID:
		case RD_78460_SERVER_REV2_ID:
		default:
			return 0;
		}
		
        /* Read MPP module ID */

        twsiSlave.slaveAddr.address = pDev[devNum];
        twsiSlave.slaveAddr.type = ADDR7_BIT;
        twsiSlave.validOffset = MV_TRUE;
        twsiSlave.offset = regNum;	        /* Use offset as command */ 
        twsiSlave.moreThen256 = MV_FALSE;
        if (MV_OK != mvTwsiRead(0, &twsiSlave, &data, 1)) {
                return MV_ERROR;
        }
        return data;
}
/*********************************************************************/
MV_STATUS mvBoardModulesScan(void)
{
	MV_U8 regVal;
	MV_TWSI_SLAVE twsiSlave;
	MV_TWSI_ADDR slave;
	MV_U32 boardId = mvBoardIdGet();

	/* Perform scan only for DB board */
	if ( (boardId == DB_88F78XX0_BP_ID) || (boardId == DB_88F78XX0_BP_REV2_ID) ) {
		/* reset modules flags */
		PexModule = 0;
        /* TWSI init */
        slave.type = ADDR7_BIT;
        slave.address = 0;
		mvTwsiInit(0, CONFIG_SYS_I2C_SPEED, CONFIG_SYS_TCLK, &slave, 0);

		/* SERDES module (only PEX moduel is supported now) */
		twsiSlave.slaveAddr.address = MV_BOARD_PEX_MODULE_ADDR;
		twsiSlave.slaveAddr.type = MV_BOARD_PEX_MODULE_ADDR_TYPE;
		twsiSlave.validOffset = MV_TRUE;
		twsiSlave.offset = 0;
		twsiSlave.moreThen256 = MV_FALSE;
		if (mvTwsiRead(0, &twsiSlave, &regVal, 1) == MV_OK) {
			if (regVal == MV_BOARD_PEX_MODULE_ID)
				PexModule = 1;			
		}
		
	} else if (boardId == RD_78460_NAS_ID) {
		SwitchModule = 0;
		if ((MV_REG_READ(GPP_DATA_IN_REG(2)) & MV_GPP66) == 0x0)
			SwitchModule = 1;
	}
	return MV_OK;
}
/*********************************************************************/
MV_BOOL mvBoardIsPexModuleConnected(void)
{
        return PexModule;
}
/*******************************************************************************/
MV_U16 mvBoardDramBusWidthGet(MV_VOID)
{
	MV_U8 sar;
	MV_U8 devNum;
	MV_U32 boardID = mvBoardIdGet();

	switch (boardID) {
	case DB_784MP_GP_ID:
		devNum = 2;
		break;
	case DB_88F78XX0_BP_ID:
	case DB_88F78XX0_BP_REV2_ID:
		devNum = 3;
		break;

	case DB_78X60_PCAC_ID:
	case FPGA_88F78XX0_ID:
	case DB_78X60_PCAC_REV2_ID:
	case RD_78460_SERVER_REV2_ID:
	default:
		return BUS_WIDTH;
	}
	sar = mvBoardTwsiSatRGet(devNum, 1);
	return (sar & 0x1);
}
/*******************************************************************************/
MV_U8 mvBoardCpuCoresNumGet(MV_VOID)
{
	MV_U32 socNum;

	socNum = (MV_REG_READ(REG_SAMPLE_RESET_HIGH_ADDR) & SAR1_CPU_CORE_MASK) >> SAR1_CPU_CORE_OFFSET;
	return (MV_U8)socNum;
}
/*********************************************************************/
static MV_16 ctrl_mode1=-1;

MV_U16 mvCtrlModelGet(MV_VOID)
{
        MV_U16 dramBusWidth;
        MV_U8 cpunum = 0;
		if (ctrl_mode1 == -1) {
		cpunum= mvBoardCpuCoresNumGet();
			if (3 == cpunum){
				ctrl_mode1 = MV_78460_DEV_ID;
			}
			else {
				dramBusWidth = mvBoardDramBusWidthGet();
				if (1 == dramBusWidth)
					ctrl_mode1 =  MV_78230_DEV_ID;
				else
					ctrl_mode1 = MV_78260_DEV_ID;
			}
		}
		return ctrl_mode1;
}
/*********************************************************************/
MV_U32 mvBoardSledCpuNumGet(MV_VOID)
{
	MV_U32 reg;
	reg = MV_REG_READ(GPP_DATA_IN_REG(0));
	return ((reg & 0xF0000) >> 16);
}
/*********************************************************************/
MV_BIN_SERDES_CFG *mvBoardSerdesCfgGet(MV_U8 pexMode)
{
	MV_U32 boardId;
	MV_U32 serdesCfg_val = 0; /* default */
	int pex0 = 1;
	int pex1 = 1;
	MV_BOOL moduleConnected = mvBoardIsPexModuleConnected();
	
	switch (pexMode) {
	case 0:
		pex0 = 1;
		pex1 = 1;
		break;
	case 1:
		pex0 = 4;
		pex1 = 1;
		break;
	case 2:
		pex0 = 1;
		pex1 = 4;
		break;
	case 3:
		pex0 = 4;
		pex1 = 4;
		break;
	}
	boardId = mvBoardIdGet();

	switch (boardId) {
	case DB_88F78XX0_BP_ID:
		if (moduleConnected)
			serdesCfg_val = 1;
		break;
	case RD_78460_SERVER_ID:
	case RD_78460_SERVER_REV2_ID:
		if (mvBoardSledCpuNumGet() > 0)
			serdesCfg_val = 1;
		break;
	case DB_88F78XX0_BP_REV2_ID:
		if ( (!moduleConnected) && (pex0 == 1)) /*if the module is not connected the PEX1 mode is not relevant*/
			serdesCfg_val = 0;
		if ( (moduleConnected) && (pex0 == 1) && (pex1 == 1))
			serdesCfg_val = 1;
		if ( (!moduleConnected) && (pex0 == 4))  /*if the module is not connected the PEX1 mode is not relevant*/
			serdesCfg_val = 2;
		if ( (moduleConnected) && (pex0 == 4) && (pex1 == 1))
			serdesCfg_val = 3;
		if ( (moduleConnected) && (pex0 == 1) && (pex1 == 4))
			serdesCfg_val = 4;
		if ( (moduleConnected) && (pex0 == 4) && (pex1 == 4))
			serdesCfg_val = 5;
		break;
	case DB_784MP_GP_ID:
		serdesCfg_val = 0;
		break;
	case RD_78460_NAS_ID:
		if (SwitchModule)
			serdesCfg_val = 1;
		break;
	}
	return &SerdesInfoTbl[boardId-BOARD_ID_BASE][serdesCfg_val];
}
/*******************************************************************************
* mvCtrlSerdesMaxLinesGet - Get Marvell controller number of SERDES lines.
*
* DESCRIPTION:
*       This function returns Marvell controller number of SERDES lines.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Marvell controller number of PEX units. If controller
*               ID is undefined the function returns '0'.
*
*******************************************************************************/
MV_U32 mvCtrlSerdesMaxLinesGet(MV_VOID)
{        
      switch (mvCtrlModelGet()) {
        case MV_78130_DEV_ID:
        case MV_78230_DEV_ID:
                return 7;
        case MV_78160_DEV_ID:
        case MV_78260_DEV_ID:
                return 12;
        case MV_78460_DEV_ID:
        case MV_78000_DEV_ID:
                return 16;
	  case MV_6710_DEV_ID:
		  return 4;

        default:
  			break;
	}
                return 0;
	}
/*******************************************************************************/
MV_U32 mvCtrlPexMaxUnitGet(MV_VOID)
{
      switch (mvCtrlModelGet()) {
        case MV_78130_DEV_ID:
        case MV_6710_DEV_ID:
        case MV_78230_DEV_ID:
                return 2;

        case MV_78160_DEV_ID:
        case MV_78260_DEV_ID:
                return 3;

        case MV_78460_DEV_ID:
        case MV_78000_DEV_ID:
                return MV_PEX_MAX_UNIT;

        default:
                return 0;
        }
}
MV_U8 mvBoardCpuFreqGet(MV_VOID)
{
	MV_U32 sar;
	MV_U32 sarMsb;

	sar = MV_REG_READ(MPP_SAMPLE_AT_RESET(0));
	sarMsb = MV_REG_READ(MPP_SAMPLE_AT_RESET(1));
	return (((sarMsb & 0x100000) >> 17) | ((sar & 0xe00000) >> 21));
}
MV_U32 mvCtrlPexMaxIfGet(MV_VOID)
{
	switch (mvCtrlModelGet()) {
	case MV_78130_DEV_ID:
	case MV_78230_DEV_ID:
		return 7;
	case MV_6710_DEV_ID:
		return MV_PEX_MAX_IF;

	case MV_78160_DEV_ID:
	case MV_78260_DEV_ID:
	case MV_78460_DEV_ID:
	case MV_78000_DEV_ID:
		return MV_PEX_MAX_IF;

	default:
		return 0;
	}
}
/*******************************************************************************
* mvCtrlRevGet - Get Marvell controller device revision number
*
* DESCRIPTION:
*       This function returns 8bit describing the device revision as defined
*       in PCI Express Class Code and Revision ID Register.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       8bit desscribing Marvell controller revision number
*
*******************************************************************************/
MV_U8 mvCtrlRevGet(MV_VOID);
/*********************************************************************/
MV_U32 get_serdesLineCfg(MV_U32 serdesLineNum,MV_BIN_SERDES_CFG *pSerdesInfo) 
{
	if (serdesLineNum < 8)
		return (pSerdesInfo->serdesLine0_7 >> (serdesLineNum << 2)) & 0xF;
	else
		return (pSerdesInfo->serdesLine8_15 >> ((serdesLineNum - 8) << 2)) & 0xF;
}
/*********************************************************************/
MV_STATUS mvCtrlHighSpeedSerdesPhyConfig(MV_VOID)
{
	MV_STATUS	status = MV_OK;
	MV_U32		serdesLineCfg;
	MV_U8		serdesLineNum;
	MV_U32		regAddr[16][11], regVal[16][11]; /* addr/value for each line @ every setup step */
	MV_U8		pexUnit, pexLineNum;
	MV_U8   	sgmiiPort = 0;
	MV_U32		tmp;
	MV_U32 		in_direct;
	MV_U8		maxSerdesLines;
	MV_BIN_SERDES_CFG	*pSerdesInfo;
	MV_U8 		satr11;
	MV_U8   	sataPort;
	MV_U8		freq;
	MV_U8		device_rev;
	MV_U32		rxHighImpedanceMode;
	MV_U16 ctrlMode;
	MV_TWSI_ADDR slave;
	MV_U32 boardId = mvBoardIdGet();

	/* TWSI init */
	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(0, CONFIG_SYS_I2C_SPEED, CONFIG_SYS_TCLK, &slave, 0);
	mvUartInit();


	maxSerdesLines = mvCtrlSerdesMaxLinesGet();

	if (maxSerdesLines == 0)
		return MV_OK;

	switch (boardId) {
        case DB_78X60_AMC_ID:
        case DB_78X60_PCAC_REV2_ID:
        case RD_78460_CUSTOMER_ID:
	case RD_78460_SERVER_ID:
	case RD_78460_SERVER_REV2_ID:
	case DB_78X60_PCAC_ID:
		satr11 = (0x1 << 1) | 1;
		break;
	case FPGA_88F78XX0_ID:
	case RD_78460_NAS_ID:
		satr11 = (0x0 << 1) | 1;
		break;	
	case DB_88F78XX0_BP_REV2_ID:
	case DB_784MP_GP_ID:
	case DB_88F78XX0_BP_ID:
		satr11 = mvBoardTwsiSatRGet(1, 1);
		if ((MV_8)MV_ERROR == (MV_8)satr11)
			return MV_ERROR;
		break;
	}

	mvBoardModulesScan();
	memset(regAddr, 0, sizeof(regAddr));
	memset(regVal,  0, sizeof(regVal));


	/* Check if DRAM is already initialized  */
	if (MV_REG_READ(REG_BOOTROM_ROUTINE_ADDR) & (1 << REG_BOOTROM_ROUTINE_DRAM_INIT_OFFS)) {
		DEBUG_INIT_S("High speed PHY - Version: ");
		DEBUG_INIT_S(SERDES_VERION);
		DEBUG_INIT_S(" - 2nd boot - Skip \n");
		return MV_OK;
	}
	DEBUG_INIT_S("High speed PHY - Version: ");
	DEBUG_INIT_S(SERDES_VERION);
	DEBUG_INIT_S(" (COM-PHY-V20) \n");

/**********************************************************************************/
	/*   AVS :  disable AVS for frequency less than 1333*/
	freq = mvBoardCpuFreqGet();
	device_rev = mvCtrlRevGet();

/* 	DEBUG_INIT_C("\n\n  **** Read SatR freq: ", freq,2); */
	if (device_rev == 2) {/*   for B0 only */
		MV_U32 cpuAvs;
		MV_U8 fabricFreq;
		cpuAvs  = MV_REG_READ(CPU_AVS_CONTROL2_REG);
		DEBUG_RD_REG(CPU_AVS_CONTROL2_REG, cpuAvs);
		cpuAvs &= ~BIT9;

		if ((0x4 == freq) || (0xB == freq)){
			MV_U32 tmp2;
			tmp2 = MV_REG_READ(CPU_AVS_CONTROL0_REG);
			DEBUG_RD_REG(CPU_AVS_CONTROL0_REG, tmp2);
			tmp2 |= 0x0FF; /* cpu upper limit = 1.1V  cpu lower limit = 0.9125V  */
			MV_REG_WRITE(CPU_AVS_CONTROL0_REG , tmp2);
			DEBUG_WR_REG(CPU_AVS_CONTROL0_REG , tmp2);
			cpuAvs  |= BIT9; /* cpu avs enable */
			cpuAvs  |= BIT18; /* AvsAvddDetEn enable  */
			fabricFreq = (MV_REG_READ(MPP_SAMPLE_AT_RESET(0)) & SAR0_FABRIC_FREQ_MASK) >> SAR0_FABRIC_FREQ_OFFSET;
			if ((0xB == freq) && (5 == fabricFreq)){
				MV_U32 coreAvs;
				coreAvs = MV_REG_READ(CORE_AVS_CONTROL_0REG);
				DEBUG_RD_REG(CORE_AVS_CONTROL_0REG, coreAvs);
				coreAvs &= ~(0xff);
				coreAvs |= 0x0E;	/*    Set core lower limit = 0.9V & core upper limit = 0.9125V */
/*				coreAvs |= 0x7F;    test1:   Set core lower limit = 0.925V & core High limit = 1V */
/*				coreAvs |= 0x3F;    test2  Set core lower limit = 0.925V & core High limit = 0.95V */
/*				coreAvs |= 0x0A;    test3    Set core lower limit = 0.85V & core High limit = 0.9125V */
				MV_REG_WRITE(CORE_AVS_CONTROL_0REG, coreAvs);
				DEBUG_WR_REG(CORE_AVS_CONTROL_0REG, coreAvs);

				coreAvs = MV_REG_READ(CORE_AVS_CONTROL_2REG);
				DEBUG_RD_REG(CORE_AVS_CONTROL_2REG, coreAvs);
				coreAvs |= BIT9; /*  core AVS enable  */
				MV_REG_WRITE(CORE_AVS_CONTROL_2REG, coreAvs);
				DEBUG_WR_REG(CORE_AVS_CONTROL_2REG, coreAvs);

				tmp2 = MV_REG_READ(GENERAL_PURPOSE_RESERVED0_REG );
				DEBUG_RD_REG(GENERAL_PURPOSE_RESERVED0_REG , tmp2);
				tmp2 |= BIT0; /*  AvsCoreAvddDetEn enable   */
				MV_REG_WRITE(GENERAL_PURPOSE_RESERVED0_REG , tmp2);
				DEBUG_WR_REG(GENERAL_PURPOSE_RESERVED0_REG , tmp2);
			}
		}
		MV_REG_WRITE(CPU_AVS_CONTROL2_REG,  cpuAvs);
		DEBUG_WR_REG(CPU_AVS_CONTROL2_REG,  cpuAvs);
	}
/**********************************************************************************/
	pSerdesInfo = mvBoardSerdesCfgGet(mvBoardPexModeGet(satr11));
	if (pSerdesInfo == NULL){
		DEBUG_INIT_S("Hight speed PHY Error #1\n");
		return MV_ERROR;
	}
	/* STEP -1 [PEX-Only] First phase of PEX-PIPE Configuration:*/
	/*----------------------------------------------*/
	DEBUG_INIT_FULL_S("Step 1: First phase of PEX-PIPE Configuration\n");
	for (pexUnit = 0; pexUnit < mvCtrlPexMaxUnitGet(); pexUnit++) {
		if (pSerdesInfo->pexMod[pexUnit] == PEX_BUS_DISABLED)
			continue;

		MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0xC1 << 16) | 0x25);		/* 1.	GLOB_CLK_CTRL Reset and Clock Control */
		DEBUG_WR_REG(PEX_PHY_ACCESS_REG(pexUnit), (0xC1 << 16) | 0x25);		/* 1.	GLOB_CLK_CTRL Reset and Clock Control */
		if (pSerdesInfo->pexMod[pexUnit] == PEX_BUS_MODE_X4) {						/* 2.	GLOB_TEST_CTRL Test Mode Control */
			MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0xC2 << 16) | 0x200);
			DEBUG_WR_REG(PEX_PHY_ACCESS_REG(pexUnit), (0xC2 << 16) | 0x200);
		}

		if (pSerdesInfo->pexMod[pexUnit] == PEX_BUS_MODE_X1) {						/* 3.	GLOB_CLK_SRC_LO Clock Source Low */
			MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0xC3 << 16) | 0x0F);
			DEBUG_WR_REG(PEX_PHY_ACCESS_REG(pexUnit), (0xC3 << 16) | 0x0F);
		}

		MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0xC5 << 16) | 0x11F);
		DEBUG_WR_REG(PEX_PHY_ACCESS_REG(pexUnit), (0xC5 << 16) | 0x11F);
	}

	/*  2	Configure the desire PIN_PHY_GEN and do power down to the PU_PLL,PU_RX,PU_TX. (bits[12:5]) */
	DEBUG_INIT_FULL_S("Step 2: Configure the desire PIN_PHY_GEN\n");
	for (serdesLineNum = 0; serdesLineNum < maxSerdesLines; serdesLineNum++) {
		serdesLineCfg = get_serdesLineCfg(serdesLineNum,pSerdesInfo);
		if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_UNCONNECTED]) 
			continue;
		if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_PEX])
			continue;
		if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SATA]) {
			switch (serdesLineNum) {
			case 4:
			case 6:
				sataPort = 0;
				break;
			case 5:
				sataPort = 1;
				break;
			default:
				DEBUG_INIT_C ("SATA port error for serdes line: ", serdesLineNum,2);
				return MV_ERROR;
			}
			tmp = MV_REG_READ(SATA_LP_PHY_EXT_CTRL_REG(sataPort));
			DEBUG_RD_REG(SATA_LP_PHY_EXT_CTRL_REG(sataPort), tmp);
			tmp &= ~ ((0x1ff<<5) | 0x7);
			tmp |= ((pSerdesInfo->busSpeed & (1 << serdesLineNum)) != 0) ? (0x11 << 5) : 0x0; 

			MV_REG_WRITE(SATA_LP_PHY_EXT_CTRL_REG(sataPort) , tmp);
			DEBUG_WR_REG(SATA_LP_PHY_EXT_CTRL_REG(sataPort) , tmp);
		}
        if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_QSGMII]) {
				/* 4)	Configure the desire PIN_PHY_GEN and do power down to the PU_PLL,PU_RX,PU_TX. (bits[12:5]) */
				tmp = MV_REG_READ(SGMII_SERDES_CFG_REG(0));
				DEBUG_RD_REG(SGMII_SERDES_CFG_REG(0), tmp);
				tmp &= ~ ((0x1ff<<5) | 0x7);
				tmp |= 0x660;
				MV_REG_WRITE(SGMII_SERDES_CFG_REG(0), tmp);
				DEBUG_WR_REG(SGMII_SERDES_CFG_REG(0), tmp);
				continue;
		}
		if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII0]) 
				sgmiiPort = 0;
		else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII1])
				sgmiiPort = 1;
		else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII2])
				sgmiiPort = 2;
		else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII3])
				sgmiiPort = 3;
		else
				continue;
			tmp = MV_REG_READ(SGMII_SERDES_CFG_REG(sgmiiPort));
			DEBUG_RD_REG(SGMII_SERDES_CFG_REG(sgmiiPort), tmp);
			tmp &= ~ ((0x1ff<<5) | 0x7);
			tmp  |= (((pSerdesInfo->busSpeed & (1 << serdesLineNum)) != 0) ? (0x88 << 5) : (0x66 << 5));
			MV_REG_WRITE(SGMII_SERDES_CFG_REG(sgmiiPort), tmp);
			DEBUG_WR_REG(SGMII_SERDES_CFG_REG(sgmiiPort), tmp);
	}
	/* Step 3 - QSGMII enable */
	DEBUG_INIT_FULL_S("Step 3 QSGMII enable \n");
	for (serdesLineNum = 0; serdesLineNum < maxSerdesLines; serdesLineNum++) {
		serdesLineCfg = get_serdesLineCfg(serdesLineNum,pSerdesInfo);
		if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_QSGMII]) {
			/* QSGMII Active bit set to true */
			tmp = MV_REG_READ(QSGMII_CONTROL_1_REG);
			DEBUG_RD_REG(QSGMII_CONTROL_1_REG, tmp );
			tmp |= BIT30;
			MV_REG_WRITE(QSGMII_CONTROL_1_REG,  tmp);
			DEBUG_WR_REG(QSGMII_CONTROL_1_REG,  tmp);
		}
	}

	/* Step 4 - configure SERDES MUXes */
	/*----------------------------------------------*/
	DEBUG_INIT_FULL_S("Step 4: Configure SERDES MUXes \n");
	MV_REG_WRITE(SERDES_LINE_MUX_REG_0_7,  pSerdesInfo->serdesLine0_7);
	DEBUG_WR_REG(SERDES_LINE_MUX_REG_0_7,  pSerdesInfo->serdesLine0_7);
	MV_REG_WRITE(SERDES_LINE_MUX_REG_8_15, pSerdesInfo->serdesLine8_15);
	DEBUG_WR_REG(SERDES_LINE_MUX_REG_8_15, pSerdesInfo->serdesLine8_15);

	/* Step 5: Activate the RX High Impedance Mode  */
	DEBUG_INIT_FULL_S("Step 5: Activate the RX High Impedance Mode  \n");
	rxHighImpedanceMode=0x8080;
	if (device_rev == 2) /*   for B0 only */
		rxHighImpedanceMode |= 4;

	for (serdesLineNum = 0; serdesLineNum < maxSerdesLines; serdesLineNum++) {
		/* for each serdes lane*/
		serdesLineCfg = get_serdesLineCfg(serdesLineNum,pSerdesInfo);
		if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_UNCONNECTED]) 
			continue;
		if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_PEX]) {
			pexUnit    = serdesLineNum >> 2;
			pexLineNum = serdesLineNum % 4;

			/* Needed for PEX_PHY_ACCESS_REG macro */
			if ((serdesLineNum > 7) && (pSerdesInfo->pexMod[3] == PEX_BUS_MODE_X8))
					pexUnit = 3; /* lines 8 - 15 are belong to PEX3 in x8 mode */

			if (pSerdesInfo->pexMod[pexUnit] == PEX_BUS_DISABLED)
				continue;
			/*  8)	Activate the RX High Impedance Mode field (bit [2]) in register /PCIe_USB Control (Each MAC contain different Access to reach its Serdes-Regfile). 
					[PEX-Only] Set bit[12]: The analog part latches idle if PU_TX = 1 and PU_PLL =1. */ 

			/* Termination enable */
			if (pSerdesInfo->pexMod[pexUnit] == PEX_BUS_MODE_X1){
					in_direct  = (0x48 << 16) | (pexLineNum << 24) | 0x1000 | rxHighImpedanceMode; /* x1 */
			}
			else if ((pSerdesInfo->pexMod[pexUnit] == PEX_BUS_MODE_X4) && (pexLineNum == 0))
				in_direct  = (0x48 << 16) | (pexLineNum << 24) | 0x1000 | ( rxHighImpedanceMode & 0xff)   ; /* x4 */
			else
				in_direct  = 0;

			if (in_direct)
			{
				MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), in_direct);
				DEBUG_WR_REG(PEX_PHY_ACCESS_REG(pexUnit), in_direct);
			}

			continue;
		}
		if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SATA]) {

			sataPort =  serdesLineNum & 1; /* port 0 for serdes lines 4,6,  and port 1 for serdes lines 5*/
			MV_REG_WRITE(SATA_COMPHY_CTRL_REG(sataPort), rxHighImpedanceMode);
			DEBUG_WR_REG(SATA_COMPHY_CTRL_REG(sataPort), rxHighImpedanceMode);
			continue;
		}
		if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_QSGMII]) {
			MV_REG_WRITE(SGMII_COMPHY_CTRL_REG(0), rxHighImpedanceMode);
			DEBUG_WR_REG(SGMII_COMPHY_CTRL_REG(0), rxHighImpedanceMode);
			continue;
		}
        if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII0]) 
			sgmiiPort = 0;
		else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII1])
			sgmiiPort = 1;
		else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII2])
			sgmiiPort = 2;
		else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII3])
			sgmiiPort = 3;
		else
			continue;
			MV_REG_WRITE(SGMII_COMPHY_CTRL_REG(sgmiiPort), rxHighImpedanceMode);
			DEBUG_WR_REG(SGMII_COMPHY_CTRL_REG(sgmiiPort), rxHighImpedanceMode);
	} /* for each serdes lane*/

	/* Step 6 [PEX-Only] PEX-Main configuration (X4 or X1): */
	/*----------------------------------------------*/
	DEBUG_INIT_FULL_S("Step 6: [PEX-Only] PEX-Main configuration (X4 or X1)\n");
	tmp = MV_REG_READ(SOC_CTRL_REG);
	DEBUG_RD_REG(SOC_CTRL_REG, tmp);
	tmp &= 0x200;
	if (pSerdesInfo->pexMod[0] == PEX_BUS_MODE_X1)
		tmp |= PCIE0_QUADX1_EN;
	if (pSerdesInfo->pexMod[1] == PEX_BUS_MODE_X1)
		tmp |= PCIE1_QUADX1_EN;
	if ( ((MV_REG_READ(MPP_SAMPLE_AT_RESET(0)) & PEX_CLK_100MHZ_MASK) >> PEX_CLK_100MHZ_OFFSET) == 0x1) 
	        tmp |= (PCIE0_CLK_OUT_EN_MASK | PCIE1_CLK_OUT_EN_MASK);

	MV_REG_WRITE(SOC_CTRL_REG, tmp);
	DEBUG_WR_REG(SOC_CTRL_REG, tmp);

	/* 6.2 PCI Express Link Capabilities*/
	/*----------------------------------------------*/
	DEBUG_INIT_FULL_S("Step 6.2: [PEX-Only] PCI Express Link Capabilities\n");

	for (serdesLineNum = 0; serdesLineNum < maxSerdesLines; serdesLineNum++) {
		serdesLineCfg = get_serdesLineCfg(serdesLineNum,pSerdesInfo);

		if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_PEX]) {
			/* PCI Express Control
				0xX1A00 [0]:
				0x0 X4-Link.
				0x1 X1-Link */

			pexUnit    = serdesLineNum >> 2;
			if (pSerdesInfo->pexMod[pexUnit] == PEX_BUS_DISABLED)
				continue;

			tmp = MV_REG_READ(PEX_LINK_CAPABILITIES_REG(MV_SERDES_NUM_TO_PEX_NUM(serdesLineNum)));
			DEBUG_RD_REG(PEX_LINK_CAPABILITIES_REG(MV_SERDES_NUM_TO_PEX_NUM(serdesLineNum)), tmp );
			tmp &= ~(0x3FF);
			if (pSerdesInfo->pexMod[pexUnit] == PEX_BUS_MODE_X1)
				tmp |= (0x1 << 4);
			if (pSerdesInfo->pexMod[pexUnit] == PEX_BUS_MODE_X4)
				tmp |= (0x4 << 4);
			if (0 == mvBoardPexCapabilityGet(satr11))
				tmp |= 0x1;
			else
				tmp	|= 0x2;
			MV_REG_WRITE(PEX_LINK_CAPABILITIES_REG(MV_SERDES_NUM_TO_PEX_NUM(serdesLineNum)), tmp);
			DEBUG_WR_REG(PEX_LINK_CAPABILITIES_REG(MV_SERDES_NUM_TO_PEX_NUM(serdesLineNum)), tmp);

			/* if pex is X4, no need to pass thru the other 3X1 serdes lines */
			if (pSerdesInfo->pexMod[pexUnit] == PEX_BUS_MODE_X4)
				serdesLineNum += 3;
		}
	}

	/* Step 7 [PEX-X4 Only] To create PEX-Link that contain 4-lanes you need to config the
			register SOC_Misc/General Purpose2 (Address= 182F8) */
	/*----------------------------------------------*/
	DEBUG_INIT_FULL_S("Step 7: [PEX-X4 Only] To create PEX-Link \n");
	tmp = MV_REG_READ(GEN_PURP_RES_2_REG);
	DEBUG_RD_REG(GEN_PURP_RES_2_REG, tmp );

	tmp &= 0xFFFF0000;
	if (pSerdesInfo->pexMod[0] == PEX_BUS_MODE_X4)
		tmp |= 0x0000000F;

	if (pSerdesInfo->pexMod[1] == PEX_BUS_MODE_X4)
		tmp |= 0x000000F0;

	if (pSerdesInfo->pexMod[2] == PEX_BUS_MODE_X4)
		tmp |= 0x00000F00;

	if (pSerdesInfo->pexMod[3] == PEX_BUS_MODE_X4)
		tmp |= 0x0000F000;

	MV_REG_WRITE(GEN_PURP_RES_2_REG, tmp);
	DEBUG_WR_REG(GEN_PURP_RES_2_REG, tmp);

	/* Steps  8 , 9 ,10 - use prepared REG addresses and values */
	/*----------------------------------------------*/
	DEBUG_INIT_FULL_S("Steps 7,8,9,10 and 11\n");

	/* Prepare PHY parameters for each step according to  MUX selection */
	for (serdesLineNum = 0; serdesLineNum < maxSerdesLines; serdesLineNum++) {
		/* for each serdes lane*/

		serdesLineCfg = get_serdesLineCfg(serdesLineNum,pSerdesInfo);

		if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_UNCONNECTED])
			continue;

		if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_PEX]) {
			pexUnit    = serdesLineNum >> 2;
			pexLineNum = serdesLineNum % 4;

			if (pSerdesInfo->pexMod[pexUnit] == PEX_BUS_DISABLED)
				continue;
			/* 8)	Configure the desire PHY_MODE (bits [7:5]) and REF_FREF_SEL (bits[4:0]) in the register Power and PLL Control (Each MAC contain different Access to reach its Serdes-Regfile).   */

			if (((pSerdesInfo->pexMod[pexUnit] == PEX_BUS_MODE_X4) && (0 == pexLineNum)) ||
				((pSerdesInfo->pexMod[pexUnit] == PEX_BUS_MODE_X1))){
				MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit),(0x01 << 16) | (pexLineNum << 24) | 0xFC60);
				DEBUG_WR_REG(PEX_PHY_ACCESS_REG(pexUnit),(0x01 << 16) | (pexLineNum << 24) | 0xFC60);
			}
			continue;
		}
		if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SATA]) {

			sataPort =  serdesLineNum & 1; /* port 0 for serdes lines 4,6,  and port 1 for serdes lines 5*/

			/* 8) Configure the desire PHY_MODE (bits [7:5]) and REF_FREF_SEL (bits[4:0]) in the register 
				  Power and PLL Control (Each MAC contain different Access to reach its Serdes-Regfile). */
			MV_REG_WRITE(SATA_PWR_PLL_CTRL_REG(sataPort), 0xF801);
			DEBUG_WR_REG(SATA_PWR_PLL_CTRL_REG(sataPort), 0xF801);

			/*  9)	Configure the desire SEL_BITS  */
			MV_REG_WRITE(SATA_DIG_LP_ENA_REG(sataPort) , 0x400);
			DEBUG_WR_REG(SATA_DIG_LP_ENA_REG(sataPort) , 0x400);


			/* 10)	Configure the desire REFCLK_SEL */

			MV_REG_WRITE(SATA_REF_CLK_SEL_REG(sataPort), 0x400);
			DEBUG_WR_REG(SATA_REF_CLK_SEL_REG(sataPort), 0x400);
			/* 11)	Power up to the PU_PLL,PU_RX,PU_TX.   */

			tmp = MV_REG_READ(SATA_LP_PHY_EXT_CTRL_REG(sataPort));
			DEBUG_RD_REG(SATA_LP_PHY_EXT_CTRL_REG(sataPort), tmp );
			tmp |= 7; 
			MV_REG_WRITE(SATA_LP_PHY_EXT_CTRL_REG(sataPort) , tmp);
			DEBUG_WR_REG(SATA_LP_PHY_EXT_CTRL_REG(sataPort) , tmp);

			continue;
		}
		if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_QSGMII]) {
			/* 8)	Configure the desire PHY_MODE (bits [7:5]) and REF_FREF_SEL (bits[4:0]) in the register  */
			MV_REG_WRITE(SGMII_PWR_PLL_CTRL_REG(0), 0xF881);
			DEBUG_WR_REG(SGMII_PWR_PLL_CTRL_REG(0), 0xF881);

			/* 9)	Configure the desire SEL_BITS (bits [11:0] in register */
			MV_REG_WRITE(SGMII_DIG_LP_ENA_REG(0), 0x400);
			DEBUG_WR_REG(SGMII_DIG_LP_ENA_REG(0), 0x400);

			/* 10)	Configure the desire REFCLK_SEL (bit [10]) in register  */
			MV_REG_WRITE(SGMII_REF_CLK_SEL_REG(0), 0x400);
			DEBUG_WR_REG(SGMII_REF_CLK_SEL_REG(0), 0x400);
	 
			/* 11)	Power up to the PU_PLL,PU_RX,PU_TX.  */
			tmp = MV_REG_READ(SGMII_SERDES_CFG_REG(0));
			DEBUG_RD_REG(SGMII_SERDES_CFG_REG(0), tmp );
			tmp |= 7;
			MV_REG_WRITE(SGMII_SERDES_CFG_REG(0), tmp);
			DEBUG_WR_REG(SGMII_SERDES_CFG_REG(0), tmp);
			continue;
		}
		if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII0])
			sgmiiPort = 0;
		else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII1])
			sgmiiPort = 1;
		else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII2])
			sgmiiPort = 2;
		else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII3])
			sgmiiPort = 3;
		else
			continue;
		/* 8)	Configure the desire PHY_MODE (bits [7:5]) and REF_FREF_SEL (bits[4:0]) in the register  */
		MV_REG_WRITE(SGMII_PWR_PLL_CTRL_REG(sgmiiPort), 0xF881);
		DEBUG_WR_REG(SGMII_PWR_PLL_CTRL_REG(sgmiiPort), 0xF881);

		/* 9)	Configure the desire SEL_BITS (bits [11:0] in register */
		MV_REG_WRITE(SGMII_DIG_LP_ENA_REG(sgmiiPort), 0);
		DEBUG_WR_REG(SGMII_DIG_LP_ENA_REG(sgmiiPort), 0);

		/* 10)	Configure the desire REFCLK_SEL (bit [10]) in register  */
		MV_REG_WRITE(SGMII_REF_CLK_SEL_REG(sgmiiPort), 0x400);
		DEBUG_WR_REG(SGMII_REF_CLK_SEL_REG(sgmiiPort), 0x400);

		/* 11)	Power up to the PU_PLL,PU_RX,PU_TX.  */

		tmp = MV_REG_READ(SGMII_SERDES_CFG_REG(sgmiiPort));
		DEBUG_RD_REG(SGMII_SERDES_CFG_REG(sgmiiPort), tmp );
		tmp |= 7;
		MV_REG_WRITE(SGMII_SERDES_CFG_REG(sgmiiPort), tmp);
		DEBUG_WR_REG(SGMII_SERDES_CFG_REG(sgmiiPort), tmp);

	} /* for each serdes lane*/
	/* Step 12 [PEX-Only] Last phase of PEX-PIPE Configuration */
	DEBUG_INIT_FULL_S("Steps 12: [PEX-Only] Last phase of PEX-PIPE Configuration\n");
	for (serdesLineNum = 0; serdesLineNum < maxSerdesLines; serdesLineNum++) {
		/* for each serdes lane*/

		serdesLineCfg = get_serdesLineCfg(serdesLineNum,pSerdesInfo);

		if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_UNCONNECTED])
			continue;

		if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_PEX]) {
			pexUnit    = serdesLineNum >> 2;
			pexLineNum = serdesLineNum % 4;
			if (0 == pexLineNum){
				MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0xC1 << 16) | 0x24);
				DEBUG_WR_REG(PEX_PHY_ACCESS_REG(pexUnit), (0xC1 << 16) | 0x24);
			}
		}
	}

	/*--------------------------------------------------------------*/
	/* Step 13: Wait 15ms before checking results */
	DEBUG_INIT_FULL_S("Steps 13: Wait 15ms before checking results");
	mvOsDelay(15);
	tmp = 20;
	while(tmp)
	{
		status=MV_OK;
		for (serdesLineNum = 0; serdesLineNum < maxSerdesLines; serdesLineNum++) {
			MV_U32  tmp;
			serdesLineCfg = get_serdesLineCfg(serdesLineNum,pSerdesInfo);
			if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_UNCONNECTED])
				continue;

			if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_PEX])
					continue;

			if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SATA]) {
				sataPort =  serdesLineNum & 1; /* port 0 for serdes lines 4,6,  and port 1 for serdes lines 5*/

				tmp = MV_REG_READ(SATA_LP_PHY_EXT_STAT_REG(sataPort));
				DEBUG_RD_REG(SATA_LP_PHY_EXT_STAT_REG(sataPort), tmp );
				if ((tmp & 0x7) !=  0x7)
					status = MV_ERROR;
				continue;
			}
			if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_QSGMII]) {
				tmp = MV_REG_READ(SGMII_SERDES_STAT_REG(0)); 
				DEBUG_RD_REG(SGMII_SERDES_STAT_REG(0), tmp) ; 
				if ((tmp & 0x7) !=  0x7)
					status = MV_ERROR;
				continue;
			}
			if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII0])
				sgmiiPort = 0;
			else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII1])
				sgmiiPort = 1;
			else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII2])
				sgmiiPort = 2;
			else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII3])
				sgmiiPort = 3;
			else
				continue;
			tmp = MV_REG_READ(SGMII_SERDES_STAT_REG(sgmiiPort));
			DEBUG_RD_REG(SGMII_SERDES_STAT_REG(sgmiiPort), tmp );
			if ((tmp & 0x7) !=  0x7)
				status = MV_ERROR;
		}
		if (status == MV_OK) 
			break;
		mvOsDelay(5);
		tmp--;
	}

	/* step14 [PEX-Only]  In order to configure RC/EP mode please write  to register 0x0060 bits */
	/*----------------------------------------------*/
	DEBUG_INIT_FULL_S("Steps 14: [PEX-Only]  In order to configure\n");
	for (pexUnit = 0; pexUnit < mvCtrlPexMaxUnitGet(); pexUnit++) {
		if (pSerdesInfo->pexMod[pexUnit] == PEX_BUS_DISABLED)
			continue;
		tmp = MV_REG_READ(PEX_CAPABILITIES_REG(MV_PEX_UNIT_TO_IF(pexUnit)));
		DEBUG_RD_REG(PEX_CAPABILITIES_REG(MV_PEX_UNIT_TO_IF(pexUnit)), tmp );
		tmp &= ~(0xf<<20);
		if ( pSerdesInfo->pexType == MV_PEX_ROOT_COMPLEX)
			tmp |= (0x4<<20);
		else
			tmp	|= (0x1<<20);
		MV_REG_WRITE(PEX_CAPABILITIES_REG(MV_PEX_UNIT_TO_IF(pexUnit)),tmp);
		DEBUG_WR_REG(PEX_CAPABILITIES_REG(MV_PEX_UNIT_TO_IF(pexUnit)),tmp);
	}
		
	/* step 15 [PEX-Only] Only for EP mode set to Zero bits 19 and 16 of register 0x1a60 */
	/*----------------------------------------------*/
	DEBUG_INIT_FULL_S("Steps 15: [PEX-Only]  In order to configure\n");
	for (pexUnit = 0; pexUnit < mvCtrlPexMaxUnitGet(); pexUnit++) {
		if (pSerdesInfo->pexMod[pexUnit] == PEX_BUS_DISABLED)
			continue;
		if ( pSerdesInfo->pexType == MV_PEX_END_POINT){
			tmp = MV_REG_READ(PEX_DBG_CTRL_REG(MV_PEX_UNIT_TO_IF(pexUnit)));
			DEBUG_RD_REG(PEX_DBG_CTRL_REG(MV_PEX_UNIT_TO_IF(pexUnit)), tmp );
			tmp &= 0xfff6ffff;
			MV_REG_WRITE(PEX_DBG_CTRL_REG(MV_PEX_UNIT_TO_IF(pexUnit)),tmp);
			DEBUG_WR_REG(PEX_DBG_CTRL_REG(MV_PEX_UNIT_TO_IF(pexUnit)),tmp);
		}
	}

	/************************** */
	if (pSerdesInfo->serdesMphyChange)
	{
		MV_SERDES_CHANGE_M_PHY *pSserdesMphyChange;
		MV_U32 busSpeed;
		for (serdesLineNum = 0; serdesLineNum < maxSerdesLines; serdesLineNum++) {
			serdesLineCfg = get_serdesLineCfg(serdesLineNum,pSerdesInfo);
			if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_UNCONNECTED])
				continue;
			pSserdesMphyChange = pSerdesInfo->serdesMphyChange;
			busSpeed = pSerdesInfo->busSpeed & (1 << serdesLineNum);
			while (pSserdesMphyChange->serdesType != SERDES_UNIT_UNCONNECTED) {
				switch (pSserdesMphyChange->serdesType) {
				case SERDES_UNIT_PEX:
					if (serdesLineCfg != SERDES_UNIT_PEX) 
						break;
					pexUnit    = serdesLineNum >> 2;
					pexLineNum = serdesLineNum % 4;
					if (pSerdesInfo->pexMod[pexUnit] == PEX_BUS_DISABLED)
						break;
					if ((pSerdesInfo->pexMod[pexUnit] == PEX_BUS_MODE_X4) && pexLineNum)
						break;

					if (busSpeed) {
						MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit) , (pexLineNum << 24) | pSserdesMphyChange->serdesValueHiSpeed);
						DEBUG_WR_REG(PEX_PHY_ACCESS_REG(pexUnit) , (pexLineNum << 24) | pSserdesMphyChange->serdesValueHiSpeed);
					}
					else {
						MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit) , (pexLineNum << 24) | pSserdesMphyChange->serdesValueLowSpeed);
						DEBUG_WR_REG(PEX_PHY_ACCESS_REG(pexUnit) , (pexLineNum << 24) | pSserdesMphyChange->serdesValueLowSpeed);
					}
					break;
				case SERDES_UNIT_SATA:
					if (serdesLineCfg != SERDES_UNIT_SATA) 
						break;
					sataPort =  serdesLineNum & 1; /* port 0 for serdes lines 4,6,  and port 1 for serdes lines 5*/
					if (busSpeed) {
						MV_REG_WRITE(SATA_BASE_REG(sataPort) | pSserdesMphyChange->serdesRegHiSpeed , pSserdesMphyChange->serdesValueHiSpeed);
						DEBUG_WR_REG(SATA_BASE_REG(sataPort) | pSserdesMphyChange->serdesRegHiSpeed , pSserdesMphyChange->serdesValueHiSpeed);
					}
					else {
						MV_REG_WRITE(SATA_BASE_REG(sataPort) | pSserdesMphyChange->serdesRegLowSpeed, pSserdesMphyChange->serdesValueLowSpeed);
						DEBUG_WR_REG(SATA_BASE_REG(sataPort) | pSserdesMphyChange->serdesRegLowSpeed, pSserdesMphyChange->serdesValueLowSpeed);
					}
					break;
				case SERDES_UNIT_SGMII0:
				case SERDES_UNIT_SGMII1:
				case SERDES_UNIT_SGMII2:
				case SERDES_UNIT_SGMII3:
					if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII0])
						sgmiiPort = 0;
					else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII1])
						sgmiiPort = 1;
					else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII2])
						sgmiiPort = 2;
					else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII3])
						sgmiiPort = 3;
					else
						break;
					if (busSpeed) {
						MV_REG_WRITE(MV_ETH_REGS_BASE(sgmiiPort) | pSserdesMphyChange->serdesRegHiSpeed  ,pSserdesMphyChange->serdesValueHiSpeed);
						DEBUG_WR_REG(MV_ETH_REGS_BASE(sgmiiPort) | pSserdesMphyChange->serdesRegHiSpeed  ,pSserdesMphyChange->serdesValueHiSpeed);
					}                                                                                                                       
					else {                                                                                                                  
						MV_REG_WRITE(MV_ETH_REGS_BASE(sgmiiPort) | pSserdesMphyChange->serdesRegLowSpeed ,pSserdesMphyChange->serdesValueLowSpeed);
						DEBUG_WR_REG(MV_ETH_REGS_BASE(sgmiiPort) | pSserdesMphyChange->serdesRegLowSpeed ,pSserdesMphyChange->serdesValueLowSpeed);
					}
					break;
				case SERDES_UNIT_QSGMII:
					if (serdesLineCfg != SERDES_UNIT_QSGMII) 
						break;
					if (busSpeed) {
						MV_REG_WRITE(pSserdesMphyChange->serdesRegHiSpeed  ,pSserdesMphyChange->serdesValueHiSpeed);
						DEBUG_WR_REG(pSserdesMphyChange->serdesRegHiSpeed  ,pSserdesMphyChange->serdesValueHiSpeed);
					}                                                                                              
					else {                                                                                         
						MV_REG_WRITE(pSserdesMphyChange->serdesRegLowSpeed ,pSserdesMphyChange->serdesValueLowSpeed);
						DEBUG_WR_REG(pSserdesMphyChange->serdesRegLowSpeed ,pSserdesMphyChange->serdesValueLowSpeed);
					}
					break;
				default:
					break;
				}
				pSserdesMphyChange++;
			}
		}
	}


	/* step 16 [PEX-Only] Training Enable */
	/*----------------------------------------------*/
	DEBUG_INIT_FULL_S("Steps 16: [PEX-Only] Training Enable");
	tmp = MV_REG_READ(SOC_CTRL_REG);
	DEBUG_RD_REG(SOC_CTRL_REG, tmp );
	tmp &= ~(0x0F);
	for (pexUnit = 0; pexUnit < mvCtrlPexMaxUnitGet(); pexUnit++) {
		MV_REG_WRITE(PEX_CAUSE_REG(pexUnit),0);
		DEBUG_WR_REG(PEX_CAUSE_REG(pexUnit),0);
		if (pSerdesInfo->pexMod[pexUnit] != PEX_BUS_DISABLED)
			tmp |= (0x1<<pexUnit);
	}
	MV_REG_WRITE(SOC_CTRL_REG, tmp);
	DEBUG_WR_REG(SOC_CTRL_REG, tmp);
	/* Step 17: Speed change to target speed and width*/
	{
		MV_U32 pexIfNum = mvCtrlPexMaxIfGet();
		MV_U32 tempReg, tempPexReg;
		MV_U32 addr;
		MV_U32 pexIf=0;
		MV_U32 first_busno, next_busno;
		MV_U32 maxLinkWidth = 0;
		MV_U32 negLinkWidth = 0;

		mvOsDelay(150);
		DEBUG_INIT_FULL_C("step 17: max_if= 0x", pexIfNum,1);
		next_busno = 0;
		for (pexIf = 0; pexIf < pexIfNum; pexIf++) {
			serdesLineNum = (pexIf <= 8)? pexIf: 12;
			serdesLineCfg = get_serdesLineCfg(serdesLineNum,pSerdesInfo);
			if (serdesLineCfg != serdesCfg[serdesLineNum][SERDES_UNIT_PEX])
				continue;
			pexUnit    = (pexIf<9)? (pexIf >> 2) : 3;
			DEBUG_INIT_FULL_S("step 17:  PEX"); DEBUG_INIT_FULL_D(pexIf,1);
			DEBUG_INIT_FULL_C("  pexUnit= ", pexUnit,1);

			if (pSerdesInfo->pexMod[pexUnit] == PEX_BUS_DISABLED) {
				DEBUG_INIT_FULL_C("PEX disabled interface ", pexIf,1);
				if (pexIf < 8)
					pexIf += 3;
				continue;
			}
			first_busno = next_busno;
			if ((pSerdesInfo->pexType == MV_PEX_END_POINT) && (0 == pexIf))
			{
				if ((pexIf<8) && (pSerdesInfo->pexMod[pexUnit] == PEX_BUS_MODE_X4))
				   pexIf += 3;
				continue;
			}

			tmp = MV_REG_READ(PEX_DBG_STATUS_REG(pexIf));
			DEBUG_RD_REG(PEX_DBG_STATUS_REG(pexIf), tmp);
			if ((tmp & 0x7f) == 0x7E) {
				next_busno++;
				tmp = maxLinkWidth = MV_REG_READ(PEX_LINK_CAPABILITIES_REG(pexIf));
				DEBUG_RD_REG((PEX_LINK_CAPABILITIES_REG(pexIf)),tmp);
				maxLinkWidth = ((maxLinkWidth >> 4) & 0x3F);
				negLinkWidth = MV_REG_READ(PEX_LINK_CTRL_STATUS_REG(pexIf));
				DEBUG_RD_REG((PEX_LINK_CTRL_STATUS_REG(pexIf)),negLinkWidth);
				negLinkWidth =  ((negLinkWidth >> 20) & 0x3F);
				if (maxLinkWidth >  negLinkWidth) {
					tmp &= ~(0x3F << 4);
					tmp |= (negLinkWidth << 4);
					MV_REG_WRITE(PEX_LINK_CAPABILITIES_REG(pexIf),tmp);
					DEBUG_WR_REG((PEX_LINK_CAPABILITIES_REG(pexIf)), tmp);
					mvOsUDelay(1000);/* wait 1ms before reading  capability for speed */
					DEBUG_INIT_S("PEX"); DEBUG_INIT_D(pexIf,1);
					DEBUG_INIT_C(": change width to X", negLinkWidth,1);
				}
				tempPexReg = MV_REG_READ((PEX_CFG_DIRECT_ACCESS(pexIf, PEX_LINK_CAPABILITY_REG)));
				DEBUG_RD_REG((PEX_CFG_DIRECT_ACCESS(pexIf, PEX_LINK_CAPABILITY_REG)),tempPexReg );
				tempPexReg &= (0xF);
				if (tempPexReg == 0x2) {
					tempReg = (MV_REG_READ(PEX_CFG_DIRECT_ACCESS(pexIf, PEX_LINK_CTRL_STAT_REG)) & 0xF0000) >> 16;
					DEBUG_RD_REG(PEX_CFG_DIRECT_ACCESS(pexIf, PEX_LINK_CTRL_STAT_REG),tempPexReg );
					/* check if the link established is GEN1 */
					if (tempReg == 0x1) {
						mvPexLocalBusNumSet(pexIf, first_busno);
						mvPexLocalDevNumSet(pexIf, 1);

						DEBUG_INIT_FULL_S("** Link is Gen1, check the EP capability \n");
						/* link is Gen1, check the EP capability */
						addr = mvPexConfigRead(pexIf, first_busno, 0, 0, 0x34) & 0xFF;
						DEBUG_INIT_FULL_C("mvPexConfigRead: return addr=0x%x", addr,4);
						if (addr == 0xff) {
							DEBUG_INIT_FULL_C("mvPexConfigRead: return 0xff -->PEX (%d): Detected No Link.", pexIf,1);
							continue;
						}
						while ((mvPexConfigRead(pexIf, first_busno, 0, 0, addr) & 0xFF) != 0x10) {
							addr = (mvPexConfigRead(pexIf, first_busno, 0, 0, addr) & 0xFF00) >> 8;
						}
						if ((mvPexConfigRead(pexIf, first_busno, 0, 0, addr + 0xC) & 0xF) == 0x2) {
							tmp = MV_REG_READ(PEX_LINK_CTRL_STATUS2_REG(pexIf));
							DEBUG_RD_REG(PEX_LINK_CTRL_STATUS2_REG(pexIf),tmp );
							tmp &=~(BIT0 | BIT1);
							tmp |= BIT1;
							MV_REG_WRITE(PEX_LINK_CTRL_STATUS2_REG(pexIf),tmp);
							DEBUG_WR_REG(PEX_LINK_CTRL_STATUS2_REG(pexIf),tmp);

							tmp = MV_REG_READ(PEX_CTRL_REG(pexIf));
							DEBUG_RD_REG(PEX_CTRL_REG(pexIf), tmp );
							tmp |= BIT10;
							MV_REG_WRITE(PEX_CTRL_REG(pexIf),tmp);
							DEBUG_WR_REG(PEX_CTRL_REG(pexIf),tmp);
							mvOsUDelay(10000);/* We need to wait 10ms before reading the PEX_DBG_STATUS_REG in order not to read the status of the former state*/
							DEBUG_INIT_FULL_S("Gen2 client!\n");
							}else {
								DEBUG_INIT_FULL_S("GEN1 client!\n");
							}
					}
				}
			}else{
				DEBUG_INIT_FULL_S("PEX"); DEBUG_INIT_FULL_D(pexIf,1);
				DEBUG_INIT_FULL_S(" : Detected No Link. Status Reg(0x"); DEBUG_INIT_FULL_D(PEX_DBG_STATUS_REG(pexIf),8);
				DEBUG_INIT_FULL_C(") = 0x", tmp,8);
			}
			if ((pexIf<8) && (pSerdesInfo->pexMod[pexUnit] == PEX_BUS_MODE_X4))
			   pexIf += 3;
		}
	}
	/* Step 18: update pex DEVICE ID*/
	{
		MV_U32 devId;
		MV_U32 pexIf;
		MV_U32 pexIfNum = mvCtrlPexMaxIfGet();
		ctrlMode = mvCtrlModelGet();
		for (pexIf = 0; pexIf < pexIfNum; pexIf++) {
			pexUnit    = (pexIf<9)? (pexIf >> 2) : 3;
			if (pSerdesInfo->pexMod[pexUnit] == PEX_BUS_DISABLED) {
				if ((pexIf<8) && (pSerdesInfo->pexMod[pexUnit] == PEX_BUS_MODE_X4))
					pexIf += 3;
				continue;
			}
			devId = MV_REG_READ(PEX_CFG_DIRECT_ACCESS(pexIf, PEX_DEVICE_AND_VENDOR_ID));
			devId &= 0xFFFF;
			devId |= ((ctrlMode << 16) & 0xffff0000);
			MV_REG_WRITE(PEX_CFG_DIRECT_ACCESS(pexIf, PEX_DEVICE_AND_VENDOR_ID), devId);
			if ((pexIf<8) && (pSerdesInfo->pexMod[pexUnit] == PEX_BUS_MODE_X4))
			   pexIf += 3;
		}
		DEBUG_INIT_S("Update PEX Device ID 0x");
		DEBUG_INIT_D(ctrlMode,4); 
		DEBUG_INIT_S("0\n");
	}

	DEBUG_INIT_S(ENDED_OK);
	return MV_OK;
}

/* PEX configuration space read write */

/*******************************************************************************
* mvPexConfigRead - Read from configuration space
*
* DESCRIPTION:
*       This function performs a 32 bit read from PEX configuration space.
*       It supports both type 0 and type 1 of Configuration Transactions
*       (local and over bridge). In order to read from local bus segment, use
*       bus number retrieved from mvPexLocalBusNumGet(). Other bus numbers
*       will result configuration transaction of type 1 (over bridge).
*
* INPUT:
*       pexIf   - PEX interface number.
*       bus     - PEX segment bus number.
*       dev     - PEX device number.
*       func    - Function number.
*       regOffs - Register offset.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit register data, 0xffffffff on error
*
*******************************************************************************/
MV_U32 mvPexConfigRead(MV_U32 pexIf, MV_U32 bus, MV_U32 dev, MV_U32 func, MV_U32 regOff)
{
	MV_U32 pexData = 0;
	MV_U32 localDev, localBus;
	MV_U32 pexStatus;

	if (pexIf >= MV_PEX_MAX_IF)
		return 0xFFFFFFFF;

	if (dev >= MAX_PEX_DEVICES) {
		DEBUG_INIT_C("mvPexConfigRead: ERR. device number illigal ", dev,1);
		return 0xFFFFFFFF;
	}

	if (func >= MAX_PEX_FUNCS) {
		DEBUG_INIT_C("mvPexConfigRead: ERR. function num illigal ", func,1);
		return 0xFFFFFFFF;
	}

	if (bus >= MAX_PEX_BUSSES) {
		DEBUG_INIT_C("mvPexConfigRead: ERR. bus number illigal ", bus,1);
		return MV_ERROR;
	}
	pexStatus = MV_REG_READ(PEX_STATUS_REG(pexIf));

	localDev = ((pexStatus & PXSR_PEX_DEV_NUM_MASK) >> PXSR_PEX_DEV_NUM_OFFS);
	localBus = ((pexStatus & PXSR_PEX_BUS_NUM_MASK) >> PXSR_PEX_BUS_NUM_OFFS);

	/* Speed up the process. In case on no link, return MV_ERROR */
	if ((dev != localDev) || (bus != localBus)) {
		pexData = MV_REG_READ(PEX_STATUS_REG(pexIf));

		if ((pexData & PXSR_DL_DOWN))
			return MV_ERROR;
	}

	/* in PCI Express we have only one device number */
	/* and this number is the first number we encounter
	   else that the localDev */
	/* spec pex define return on config read/write on any device */
	if (bus == localBus) {
		if (localDev == 0) {
			/* if local dev is 0 then the first number we encounter
			   after 0 is 1 */
			if ((dev != 1) && (dev != localDev))
				return MV_ERROR;
		} else {
			/* if local dev is not 0 then the first number we encounter
			   is 0 */

			if ((dev != 0) && (dev != localDev))
				return MV_ERROR;
		}
	}
	/* Creating PEX address to be passed */
	pexData = (bus << PXCAR_BUS_NUM_OFFS);
	pexData |= (dev << PXCAR_DEVICE_NUM_OFFS);
	pexData |= (func << PXCAR_FUNC_NUM_OFFS);
	pexData |= (regOff & PXCAR_REG_NUM_MASK);	/* lgacy register space */
	/* extended register space */
	pexData |= (((regOff & PXCAR_REAL_EXT_REG_NUM_MASK) >>
		     PXCAR_REAL_EXT_REG_NUM_OFFS) << PXCAR_EXT_REG_NUM_OFFS);

	pexData |= PXCAR_CONFIG_EN;

	/* Write the address to the PEX configuration address register */
	MV_REG_WRITE(PEX_CFG_ADDR_REG(pexIf), pexData);

	/* In order to let the PEX controller absorbed the address of the read  */
	/* transaction we perform a validity check that the address was written */
	if (pexData != MV_REG_READ(PEX_CFG_ADDR_REG(pexIf)))
		return MV_ERROR;

	/* cleaning Master Abort */
	MV_REG_BIT_SET(PEX_CFG_DIRECT_ACCESS(pexIf, PEX_STATUS_AND_COMMAND), PXSAC_MABORT);
		/* Read the Data returned in the PEX Data register */
	pexData = MV_REG_READ(PEX_CFG_DATA_REG(pexIf));


	DEBUG_INIT_FULL_C(" --> ", pexData,4);

	return pexData;

}
/*******************************************************************************
* mvPexLocalBusNumSet - Set PEX interface local bus number.
*
* DESCRIPTION:
*       This function sets given PEX interface its local bus number.
*       Note: In case the PEX interface is PEX-X, the information is read-only.
*
* INPUT:
*       pexIf  - PEX interface number.
*       busNum - Bus number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_NOT_ALLOWED in case PEX interface is PEX-X.
*		MV_BAD_PARAM on bad parameters ,
*       otherwise MV_OK
*
*******************************************************************************/
MV_STATUS mvPexLocalBusNumSet(MV_U32 pexIf, MV_U32 busNum)
{
	MV_U32 pexStatus;

	if (busNum >= MAX_PEX_BUSSES) {
		DEBUG_INIT_C("mvPexLocalBusNumSet: ERR. bus number illigal %d\n", busNum,4);
		return MV_ERROR;
	}

	pexStatus = MV_REG_READ(PEX_STATUS_REG(pexIf));

	pexStatus &= ~PXSR_PEX_BUS_NUM_MASK;

	pexStatus |= (busNum << PXSR_PEX_BUS_NUM_OFFS) & PXSR_PEX_BUS_NUM_MASK;

	MV_REG_WRITE(PEX_STATUS_REG(pexIf), pexStatus);

	return MV_OK;
}

/*******************************************************************************
* mvPexLocalDevNumSet - Set PEX interface local device number.
*
* DESCRIPTION:
*       This function sets given PEX interface its local device number.
*       Note: In case the PEX interface is PEX-X, the information is read-only.
*
* INPUT:
*       pexIf  - PEX interface number.
*       devNum - Device number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_NOT_ALLOWED in case PEX interface is PEX-X.
*		MV_BAD_PARAM on bad parameters ,
*       otherwise MV_OK
*
*******************************************************************************/
MV_STATUS mvPexLocalDevNumSet(MV_U32 pexIf, MV_U32 devNum)
{
	MV_U32 pexStatus;

	if (pexIf >= MV_PEX_MAX_IF)
		return MV_BAD_PARAM;

	pexStatus = MV_REG_READ(PEX_STATUS_REG(pexIf));

	pexStatus &= ~PXSR_PEX_DEV_NUM_MASK;

	pexStatus |= (devNum << PXSR_PEX_DEV_NUM_OFFS) & PXSR_PEX_DEV_NUM_MASK;

	MV_REG_WRITE(PEX_STATUS_REG(pexIf), pexStatus);

	return MV_OK;
}


