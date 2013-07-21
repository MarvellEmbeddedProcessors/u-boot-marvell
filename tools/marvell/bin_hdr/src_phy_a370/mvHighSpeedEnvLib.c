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
#define mvBoardPexCapabilityGet(satr) 	(satr & 3)
#define MV_PEX_UNIT_TO_IF(pexUnit)	(pexUnit)
/****************************  Local function *****************************************/
MV_U32 mvBoardIdGet(MV_VOID)
{
#if defined(DB_88F6710)
		return DB_88F6710_BP_ID;
#elif defined(DB_88F6710_PCAC)
		return DB_88F6710_PCAC_ID;
#elif defined(RD_88F6710)
		return RD_88F6710_ID;
#else
		return INVALID_BAORD_ID;
#endif

}
/*******************************************************************************
* mvBoardTclkGet - Get the board Tclk (Controller clock)
*
* DESCRIPTION:
*       This routine extract the controller core clock.
*       This function uses the controller counters to make identification.
*		Note: In order to avoid interference, make sure task context switch
*		and interrupts will not occure during this function operation
*
* INPUT:
*       countNum - Counter number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit clock cycles in Hertz.
*
*******************************************************************************/
MV_U32 mvBoardTclkGet(MV_VOID)
{
	if ((MV_REG_READ(MPP_SAMPLE_AT_RESET(0)) & MSAR_TCLK_MASK) != 0)
		return MV_BOARD_TCLK_200MHZ;
	else
		return MV_BOARD_TCLK_166MHZ;
}
/*********************************************************************/
MV_U8 mvBoardTwsiSatRGet(MV_U8 devNum, MV_U8 regNum)
{
        MV_TWSI_SLAVE twsiSlave;
        MV_TWSI_ADDR slave;
        MV_U8 data;

        /* TWSI init */
        slave.type = ADDR7_BIT;
        slave.address = 0;
		mvTwsiInit(0, CONFIG_SYS_I2C_SPEED, CONFIG_SYS_TCLK, &slave, 0);

        /* Read MPP module ID */
        twsiSlave.slaveAddr.address = 0x4D;
        twsiSlave.slaveAddr.type = ADDR7_BIT;
        twsiSlave.validOffset = MV_TRUE;
        twsiSlave.offset = regNum;	        /* Use offset as command */ 
        twsiSlave.moreThen256 = MV_FALSE;
        if (MV_OK != mvTwsiRead(0, &twsiSlave, &data, 1)) {
                return MV_ERROR;
        }
        return data;
}
/*******************************************************************************
* mvBoardSerdesModulesScan
*
* DESCRIPTION:
*	Scan for modules connected through SERDES lines.
*
* INPUT:
*	None.
*
* OUTPUT:
*	None
*
* RETURN:
*       MV_STATUS - MV_OK, MV_ERROR.
*
*******************************************************************************/
MV_U32 mvBoardSerdesModulesScan(MV_U32 default_serdes)
{
	MV_U8 regVal;
	MV_U8 swCfg;
	MV_TWSI_SLAVE twsiSlave;
	MV_U32 srdsMode = 0;
	MV_U8 usedLanes = 0;

	/* Check if scan is enabled.	*/
	mvOsUDelay(1000);

	/* Read SW EEPROM */
	FILL_TWSI_SLAVE(twsiSlave, MV_BOARD_EEPROM_MODULE_ADDR);
	if (mvTwsiRead(0, &twsiSlave, &swCfg, 1) != MV_OK)
	{
		DEBUG_INIT_S("mvBoardSerdesModulesScan: mvTwsiRead erro serdes to default ****");
		return default_serdes;

	}

	/* SGMII module. */
	FILL_TWSI_SLAVE(twsiSlave, MV_BOARD_SERDES_CON_ADDR);
	if (mvTwsiRead(0, &twsiSlave, &regVal, 1) == MV_OK) {
		/* This sets Lanes 2 & 3 to SGMII. */
		srdsMode |= 0x2200;
		usedLanes = LANE2 | LANE3;
	}

	/* PCIE config. */
	switch (MV_BOARD_CFG_PCIE_MODE(swCfg)) {
	case (1):
		srdsMode |= 0x0001;
		usedLanes |= LANE0;
		break;
	case (2):
		srdsMode |= 0x0010;
		usedLanes |= LANE1;
		break;
	case (3):
		srdsMode |= 0x0011;
		usedLanes |= LANE0 | LANE1;
		break;
	default:
		break;
	}

	/* Sata0 config. */
	switch (MV_BOARD_CFG_SATA0_MODE(swCfg)) {
	case (1):
		if (!(usedLanes & LANE0)) {
			srdsMode |= 0x0002;
			usedLanes |= LANE0;
		}
		break;
	case (2):
		if (!(usedLanes & LANE2)) {
			srdsMode |= 0x0100;
			usedLanes |= LANE2;
		}
		break;
	default:
		break;
	}

	/* Sata1 config. */
	if (MV_BOARD_CFG_SATA1_MODE(swCfg) == 1) {
		if (!(usedLanes & LANE3)) {
			srdsMode |= 0x1000;
			usedLanes |= LANE3;
		}
	}

	return srdsMode;
}
/*********************************************************************/
MV_U16 mvCtrlModelGet(MV_VOID)
{
#if defined(MV88F6W11)
	return MV_6W11_DEV_ID;
#elif defined(MV88F6707)
	return MV_6707_DEV_ID;
#else /* defined(MV88F6710) */
	return MV_6710_DEV_ID;
#endif
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
	case MV_6W11_DEV_ID:
	case MV_6707_DEV_ID:
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
	case MV_6W11_DEV_ID:
	case MV_6707_DEV_ID:
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
	MV_U32 cpuFreq;
	cpuFreq = MV_REG_READ(REG_SAMPLE_RESET_LOW_ADDR);	/* 0x18230 [23:21] */
    cpuFreq = ((cpuFreq & REG_SAMPLE_RESET_CPU_FREQ_MASK) >> REG_SAMPLE_RESET_CPU_FREQ_OFFS);
	return cpuFreq;
}
MV_U32 mvCtrlPexMaxIfGet(MV_VOID)
{
	switch (mvCtrlModelGet()) {
	case MV_78130_DEV_ID:
	case MV_78230_DEV_ID:
		return 7;
	case MV_6W11_DEV_ID:
	case MV_6710_DEV_ID:
	case MV_6707_DEV_ID:
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
/*******************************************************************************/
MV_BIN_SERDES_CFG *mvBoardSerdesCfgGet(void)
{
	MV_U32 boardId;
	MV_BIN_SERDES_CFG *pSerdes;
	boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		DEBUG_INIT_S("mvBoardSerdesCfgGet: Board unknown.\n");
		return NULL;
	}
	pSerdes =  &SerdesInfoTbl[boardId-BOARD_ID_BASE][0];
	if (pSerdes->enableModuleScan == MV_FALSE)
		return pSerdes;
	pSerdes->serdesLine0_3 = mvBoardSerdesModulesScan(pSerdes->serdesLine0_3);
	pSerdes->pexMod[0] = ((pSerdes->serdesLine0_3 & 0x0f) == 1) ? PEX_BUS_MODE_X1: PEX_BUS_DISABLED;
	pSerdes->pexMod[1] = ((pSerdes->serdesLine0_3 & 0xf0) == 0x10) ? PEX_BUS_MODE_X1: PEX_BUS_DISABLED;
	return pSerdes;
}
/*********************************************************************/
MV_U32 get_serdesLineCfg(MV_U32 serdesLineNum,MV_BIN_SERDES_CFG *pSerdesInfo) 
{
	return (pSerdesInfo->serdesLine0_3 >> (serdesLineNum << 2)) & 0xF;
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
	MV_U32		rxHighImpedanceMode;
	maxSerdesLines = mvCtrlSerdesMaxLinesGet();
	MV_U16 ctrlMode = mvCtrlModelGet();

	MV_TWSI_ADDR slave;

	/* TWSI init */
	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(0, CONFIG_SYS_I2C_SPEED, CONFIG_SYS_TCLK, &slave, 0);

	if (maxSerdesLines == 0)
		return MV_OK;


	memset(regAddr, 0, sizeof(regAddr));
	memset(regVal,  0, sizeof(regVal));

	mvUartInit();
	satr11 = mvBoardTwsiSatRGet(1, 1);
	if (satr11 == MV_ERROR) {
		DEBUG_INIT_S("mvBoardTwsiSatRGet(1,1) read error ************\n");
		mvOsUDelay(1);
		mvTwsiInit(0, CONFIG_SYS_I2C_SPEED, CONFIG_SYS_TCLK, &slave, 0);
		  satr11 = 0;
	}
	
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
#if 0 /*   for B0 only */ 
	{
		MV_U8		freq;
		/*   AVS :  disable AVS for frequency less than 799*/
		freq = mvBoardCpuFreqGet();
		/* 	DEBUG_INIT_C("\n\n  **** Read SatR freq: ", freq,2); */
		tmp = MV_REG_READ(AVS_CONTROL2_REG);
		if (freq >= 3){
			MV_U32 tmp2;
			DEBUG_RD_REG(AVS_CONTROL2_REG, tmp);
			tmp2 = MV_REG_READ(AVS_LOW_VDD_LIMIT);
			DEBUG_RD_REG(AVS_LOW_VDD_LIMIT, tmp2);
			tmp2 |= 0x0f0;
			MV_REG_WRITE(AVS_LOW_VDD_LIMIT , tmp2);
			DEBUG_WR_REG(AVS_LOW_VDD_LIMIT , tmp2);
			tmp |= BIT9;
		}
		else
			tmp &= ~BIT9;
		MV_REG_WRITE(AVS_CONTROL2_REG , tmp);
		DEBUG_WR_REG(AVS_CONTROL2_REG , tmp);
	}
#endif
/**********************************************************************************/
	pSerdesInfo = mvBoardSerdesCfgGet();
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
		MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0xC3 << 16) | 0x0F);
		DEBUG_WR_REG(PEX_PHY_ACCESS_REG(pexUnit), (0xC3 << 16) | 0x0F);
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
			case 0:
			case 2:
				sataPort = 0;
				break;
			case 3:
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
        if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII0]) {
			if ((serdesLineNum == 1) || (serdesLineNum == 2))
					sgmiiPort = 0;
			else 
					sgmiiPort = 1;
			tmp = MV_REG_READ(SGMII_SERDES_CFG_REG(sgmiiPort));
			DEBUG_RD_REG(SGMII_SERDES_CFG_REG(sgmiiPort), tmp);
			tmp &= ~ ((0x1ff<<5) | 0x7);
			tmp  |= (((pSerdesInfo->busSpeed & (1 << serdesLineNum)) != 0) ? (0x88 << 5) : (0x66 << 5));
			MV_REG_WRITE(SGMII_SERDES_CFG_REG(sgmiiPort), tmp);
			DEBUG_WR_REG(SGMII_SERDES_CFG_REG(sgmiiPort), tmp);
				continue;
		}
	}
	/* Step 3 - QSGMII enable */
	DEBUG_INIT_FULL_S("Step 3 QSGMII enable \n");

	/* Step 4 - configure SERDES MUXes */
	/*----------------------------------------------*/
	DEBUG_INIT_FULL_S("Step 4: Configure SERDES MUXes \n");
	MV_REG_WRITE(SERDES_LINE_MUX_REG_0_3,  pSerdesInfo->serdesLine0_3);
	DEBUG_WR_REG(SERDES_LINE_MUX_REG_0_3,  pSerdesInfo->serdesLine0_3);

	/* Step 5: Activate the RX High Impedance Mode  */
	DEBUG_INIT_FULL_S("Step 5: Activate the RX High Impedance Mode  \n");
	rxHighImpedanceMode=0x8080;
	for (serdesLineNum = 0; serdesLineNum < maxSerdesLines; serdesLineNum++) {
		/* for each serdes lane*/
		serdesLineCfg = get_serdesLineCfg(serdesLineNum,pSerdesInfo);
		if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_UNCONNECTED]) 
			continue;
		if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_PEX]) {
			pexUnit    = serdesLineNum;
			pexLineNum = 0;

			if (pSerdesInfo->pexMod[pexUnit] == PEX_BUS_DISABLED)
				continue;
			/*  8)	Activate the RX High Impedance Mode field (bit [2]) in register /PCIe_USB Control (Each MAC contain different Access to reach its Serdes-Regfile). 
					[PEX-Only] Set bit[12]: The analog part latches idle if PU_TX = 1 and PU_PLL =1. */ 

			/* Termination enable */
			in_direct  = (0x48 << 16) | (pexLineNum << 24) | 0x1000 | rxHighImpedanceMode; /* x1 */
			MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), in_direct);
			DEBUG_WR_REG(PEX_PHY_ACCESS_REG(pexUnit), in_direct);
			continue;
		}
		if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SATA]) {

			sataPort =  serdesLineNum & 1; /* port 0 for serdes lines 4,6,  and port 1 for serdes lines 5*/
			MV_REG_WRITE(SATA_COMPHY_CTRL_REG(sataPort), rxHighImpedanceMode);
			DEBUG_WR_REG(SATA_COMPHY_CTRL_REG(sataPort), rxHighImpedanceMode);
			continue;
		}
        if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII0]) {
			if ((serdesLineNum == 1) || (serdesLineNum == 2))
					sgmiiPort = 0;
			else 
					sgmiiPort = 1;
			MV_REG_WRITE(SGMII_COMPHY_CTRL_REG(sgmiiPort), rxHighImpedanceMode);
			DEBUG_WR_REG(SGMII_COMPHY_CTRL_REG(sgmiiPort), rxHighImpedanceMode);
		}
	} /* for each serdes lane*/

	/* Step 6 [PEX-Only] PEX-Main configuration (X4 or X1): */
	/*----------------------------------------------*/
	DEBUG_INIT_FULL_S("Step 6: [PEX-Only] PEX-Main configuration (X4 or X1)\n");
	tmp = MV_REG_READ(SOC_CTRL_REG);
	DEBUG_RD_REG(SOC_CTRL_REG, tmp);
	tmp &= 0x200;

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

			pexUnit    = serdesLineNum;
			if (pSerdesInfo->pexMod[pexUnit] == PEX_BUS_DISABLED)
				continue;
			tmp = MV_REG_READ(PEX_LINK_CTRL_STATUS_REG(pexUnit));
			DEBUG_RD_REG(PEX_LINK_CTRL_STATUS_REG(pexUnit),tmp);
			tmp |= BIT6;
			MV_REG_WRITE(PEX_LINK_CTRL_STATUS_REG(pexUnit), tmp);
			DEBUG_WR_REG(PEX_LINK_CTRL_STATUS_REG(pexUnit), tmp);

			tmp = MV_REG_READ(PEX_LINK_CAPABILITIES_REG(MV_SERDES_NUM_TO_PEX_NUM(serdesLineNum)));
			DEBUG_RD_REG(PEX_LINK_CAPABILITIES_REG(MV_SERDES_NUM_TO_PEX_NUM(serdesLineNum)), tmp );
			tmp &= ~(0x3FF);
			tmp |= (0x1 << 4);		/* PEX_BUS_MODE_X1 */
			if (1 == mvBoardPexCapabilityGet(satr11))
				tmp |= 0x1;
			else
				tmp	|= 0x2;
			MV_REG_WRITE(PEX_LINK_CAPABILITIES_REG(MV_SERDES_NUM_TO_PEX_NUM(serdesLineNum)), tmp);
			DEBUG_WR_REG(PEX_LINK_CAPABILITIES_REG(MV_SERDES_NUM_TO_PEX_NUM(serdesLineNum)), tmp);
		}
	}

	/* Step 7 [PEX-X4 Only] To create PEX-Link that contain 4-lanes you need to config the
			register SOC_Misc/General Purpose2 (Address= 182F8) */
	/*----------------------------------------------*/
	DEBUG_INIT_FULL_S("Step 7: [PEX-X4 Only] To create PEX-Link \n");
	tmp = MV_REG_READ(GEN_PURP_RES_2_REG);
	DEBUG_RD_REG(GEN_PURP_RES_2_REG, tmp );

	tmp &= 0xFFFF0000;
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
			pexUnit    = serdesLineNum;
			pexLineNum = 0;

			if (pSerdesInfo->pexMod[pexUnit] == PEX_BUS_DISABLED)
				continue;
			/* 8)	Configure the desire PHY_MODE (bits [7:5]) and REF_FREF_SEL (bits[4:0]) in the register Power and PLL Control (Each MAC contain different Access to reach its Serdes-Regfile).   */

				MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit),(0x01 << 16) | (pexLineNum << 24) | 0xFC60);
				DEBUG_WR_REG(PEX_PHY_ACCESS_REG(pexUnit),(0x01 << 16) | (pexLineNum << 24) | 0xFC60);
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
		if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII0]) {
			if ((serdesLineNum == 1) || (serdesLineNum == 2))
				sgmiiPort = 0;
			else
				sgmiiPort = 1;
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
			continue;
		}

	} /* for each serdes lane*/
	/* Step 12 [PEX-Only] Last phase of PEX-PIPE Configuration */
	DEBUG_INIT_FULL_S("Steps 12: [PEX-Only] Last phase of PEX-PIPE Configuration\n");
	for (serdesLineNum = 0; serdesLineNum < maxSerdesLines; serdesLineNum++) {
		/* for each serdes lane*/

		serdesLineCfg = get_serdesLineCfg(serdesLineNum,pSerdesInfo);

		if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_UNCONNECTED])
			continue;

		if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_PEX]) {
			pexUnit    = serdesLineNum;
			pexLineNum = 0;
			MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0xC1 << 16) | 0x24);
			DEBUG_WR_REG(PEX_PHY_ACCESS_REG(pexUnit), (0xC1 << 16) | 0x24);
		}
	}

	/*--------------------------------------------------------------*/
	/* Step 13: Wait 15ms before checking results */
	DEBUG_INIT_FULL_S("Steps 13: Wait 15ms before checking results\n");
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
			if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII0]) {
				if ((serdesLineNum == 1) || (serdesLineNum == 2))
					sgmiiPort = 0;
				else
					sgmiiPort = 1;
				tmp = MV_REG_READ(SGMII_SERDES_STAT_REG(sgmiiPort));
				DEBUG_RD_REG(SGMII_SERDES_STAT_REG(sgmiiPort), tmp );
				if ((tmp & 0x7) !=  0x7)
					status = MV_ERROR;
				continue;
			}
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
					if (serdesLineCfg != serdesCfg[serdesLineNum][SERDES_UNIT_PEX]) 
						break;
					pexUnit    = serdesLineNum;
					pexLineNum = 0;
					if (pSerdesInfo->pexMod[pexUnit] == PEX_BUS_DISABLED)
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
					if (serdesLineCfg != serdesCfg[serdesLineNum][SERDES_UNIT_SATA])
						break;
					sataPort =  serdesLineNum & 1; /* port 0 for serdes lines 4,6,  and port 1 for serdes lines 5*/
					if (busSpeed) {
						MV_REG_WRITE(SATA_BASE_REG(sataPort) | pSserdesMphyChange->serdesRegHiSpeed , pSserdesMphyChange->serdesValueLowSpeed);
						DEBUG_WR_REG(SATA_BASE_REG(sataPort) | pSserdesMphyChange->serdesRegHiSpeed , pSserdesMphyChange->serdesValueLowSpeed);
					}
					else {
						MV_REG_WRITE(SATA_BASE_REG(sataPort) | pSserdesMphyChange->serdesRegLowSpeed, pSserdesMphyChange->serdesValueLowSpeed);
						DEBUG_WR_REG(SATA_BASE_REG(sataPort) | pSserdesMphyChange->serdesRegLowSpeed, pSserdesMphyChange->serdesValueLowSpeed);
					}
					break;
				case SERDES_UNIT_SGMII0:
				case SERDES_UNIT_SGMII1:
					if (serdesLineCfg != serdesCfg[serdesLineNum][SERDES_UNIT_SGMII0]) 
						break;
					if ((serdesLineNum == 1) || (serdesLineNum == 2))
							sgmiiPort = 0;
					else 
							sgmiiPort = 1;
					if (busSpeed) {
						MV_REG_WRITE(MV_ETH_REGS_BASE(sgmiiPort) | pSserdesMphyChange->serdesRegHiSpeed  ,pSserdesMphyChange->serdesValueLowSpeed);
						DEBUG_WR_REG(MV_ETH_REGS_BASE(sgmiiPort) | pSserdesMphyChange->serdesRegHiSpeed  ,pSserdesMphyChange->serdesValueLowSpeed);
					}                                                                                                                       
					else {                                                                                                                  
						MV_REG_WRITE(MV_ETH_REGS_BASE(sgmiiPort) | pSserdesMphyChange->serdesRegLowSpeed ,pSserdesMphyChange->serdesValueLowSpeed);
						DEBUG_WR_REG(MV_ETH_REGS_BASE(sgmiiPort) | pSserdesMphyChange->serdesRegLowSpeed ,pSserdesMphyChange->serdesValueLowSpeed);
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
	DEBUG_INIT_FULL_S("Steps 16: [PEX-Only] Training Enable\n");
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

#ifdef CONFIG_DB_88F6710_PCAC
	/* set Pex terminations for Pex Compliance */
	MV_REG_WRITE(SERDES_LINE_MUX_REG_0_7, 0x201);
	MV_REG_WRITE(0x41b00, (((0x48 & 0x3fff) << 16) | 0x8080));
#endif


	/* Step 17: Speed change to target speed and width*/
	{
		MV_U32 pexIfNum = mvCtrlPexMaxIfGet();
		MV_U32 tempReg, tempPexReg;
		MV_U32 addr;
		MV_U32 pexIf=0;
		MV_U32 first_busno, next_busno;
#if 0 /* only for X4 */
		MV_U32 maxLinkWidth = 0;
		MV_U32 negLinkWidth = 0;
#endif

		mvOsDelay(150);
		DEBUG_INIT_FULL_C("step 17: max_if= 0x", pexIfNum,1);
		next_busno = 0;
		for (pexIf = 0; pexIf < pexIfNum; pexIf++) {

			pexUnit    = pexIf;
			DEBUG_INIT_FULL_S("step 17:  PEX"); DEBUG_INIT_FULL_D(pexIf,1);
			DEBUG_INIT_FULL_C("  pexUnit= ", pexUnit,1);

			if (pSerdesInfo->pexMod[pexUnit] == PEX_BUS_DISABLED) {
				DEBUG_INIT_FULL_C("PEX disabled interface ", pexIf,1);
				continue;
			}
			first_busno = next_busno;
			if (pSerdesInfo->pexType == MV_PEX_END_POINT)
			{
				continue;
			}

			tmp = MV_REG_READ(PEX_DBG_STATUS_REG(pexIf));
			DEBUG_RD_REG(PEX_DBG_STATUS_REG(pexIf), tmp);
			if ((tmp & 0x7f) == 0x7E) {
				next_busno++;
#if 0 /* only for X4 */
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
#endif
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
						DEBUG_INIT_FULL_C("mvPexConfigRead: return addr=0x", addr,4);
						if (addr == 0xff) {
							DEBUG_INIT_FULL_C("mvPexConfigRead: return 0xff -->PEX : Detected No Link. pexif=", pexIf,1);
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
		for (pexIf = 0; pexIf < pexIfNum; pexIf++) {
			pexUnit    = pexIf;
			if (pSerdesInfo->pexMod[pexUnit] == PEX_BUS_DISABLED) {
				continue;
			}
			devId = MV_REG_READ(PEX_CFG_DIRECT_ACCESS(pexIf, PEX_DEVICE_AND_VENDOR_ID));
			devId &= 0xFFFF;
			devId |= ((ctrlMode << 16) & 0xffff0000);
			MV_REG_WRITE(PEX_CFG_DIRECT_ACCESS(pexIf, PEX_DEVICE_AND_VENDOR_ID), devId);
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
		DEBUG_INIT_C("mvPexConfigRead: ERR. device number illigal %d", dev,1);
		return 0xFFFFFFFF;
	}

	if (func >= MAX_PEX_FUNCS) {
		DEBUG_INIT_C("mvPexConfigRead: ERR. function num illigal %d", func,1);
		return 0xFFFFFFFF;
	}

	if (bus >= MAX_PEX_BUSSES) {
		DEBUG_INIT_C("mvPexConfigRead: ERR. bus number illigal %d", bus,1);
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

	DEBUG_INIT_FULL_C("mvPexConfigRead: got : %x ", pexData,4);

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
/*******************************************************************************
* mvCpuPclkGet - Get the CPU pClk (pipe clock)
*
* DESCRIPTION:
*       This routine extract the CPU core clock.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit clock cycles in Hertz.
*
*******************************************************************************/
MV_U32 mvCpuPclkGet(MV_VOID)
{
#if defined(PCLCK_AUTO_DETECT)
	MV_U32 idx;
	MV_U32 cpuClk[] = MV_CPU_CLK_TBL;

	idx = MSAR_CPU_CLK_IDX(MV_REG_READ(MPP_SAMPLE_AT_RESET));

	return cpuClk[idx] * 1000000;
#else
	return MV_DEFAULT_PCLK;
#endif
}
/*******************************************************************************
* mvCpuL2ClkGet - Get the CPU L2 (CPU bus clock)
*
* DESCRIPTION:
*       This routine extract the CPU L2 clock.
*
* RETURN:
*       32bit clock cycles in Hertz.
*
*******************************************************************************/
MV_U32 mvCpuL2ClkGet(MV_VOID)
{
	MV_U32 idx;
	MV_U32 freqMhz, l2FreqMhz;
	MV_CPU_ARM_CLK_RATIO clockRatioTbl[] = MV_DDR_L2_CLK_RATIO_TBL;

	idx = MSAR_DDR_L2_CLK_RATIO_IDX(MV_REG_READ(MPP_SAMPLE_AT_RESET(0)));

	if (clockRatioTbl[idx].vco2cpu != 0) {
		freqMhz = mvCpuPclkGet() / 1000000;	/* CPU freq */
		freqMhz *= clockRatioTbl[idx].vco2cpu;	/* VCO freq */
		l2FreqMhz = freqMhz / clockRatioTbl[idx].vco2l2c;
		/* round up to integer MHz */
		if (((freqMhz % clockRatioTbl[idx].vco2l2c) * 10 / clockRatioTbl[idx].vco2l2c) >= 5)
			l2FreqMhz++;

		return l2FreqMhz * 1000000;
	} else
		return (MV_U32)-1;
}
