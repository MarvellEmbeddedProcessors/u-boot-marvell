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
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "mv_os.h"
#include "mvHighSpeedEnvSpec.h"
#include "mvDeviceId.h"
#include "config_marvell.h"  	/* Required to identify SOC and Board */

#include "pex/mvPexRegs.h"
#include "sys/mvCpuIfRegs.h"
#include "bin_hdr_twsi.h"
#include "mvGppRegs.h"

static const MV_U8 serdesCfg[][SERDES_LAST_UNIT] = BIN_SERDES_CFG;
//extern  MV_U8 serdesCfg;
extern MV_BIN_SERDES_CFG *SerdesInfoTbl[];

#define BOARD_INFO(boardId)	boardInfoTbl[boardId - BOARD_ID_BASE]

#define MV_BOARD_TCLK_200MHZ	200000000
#define MV_BOARD_TCLK_250MHZ	250000000
MV_BOOL PexModule = 0;
MV_BOOL SwitchModule = 0;

MV_U32 mvBoardIdGet(MV_VOID)
{
#if defined(DB_88F78X60)
                return DB_88F78XX0_BP_ID;
#elif defined(RD_88F78460_SERVER)
                return RD_78460_SERVER_ID;
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
#elif defined(RD_78460_CUSTOMER)
                return RD_78460_CUSTOMER_ID;
#else
                while (1) {
                        continue;
                }
#endif
}
#if 1
MV_U32 mvBoardTclkGet(MV_VOID)
{
	if ((MV_REG_READ(MPP_SAMPLE_AT_RESET(0)) & MSAR_TCLK_MASK) != 0)
		return MV_BOARD_TCLK_200MHZ;
	else
		return MV_BOARD_TCLK_250MHZ;
}


MV_U8 mvBoardTwsiSatRGet(MV_U8 devNum, MV_U8 regNum)
{
        MV_TWSI_SLAVE twsiSlave;
        MV_TWSI_ADDR slave;
        MV_U8 data;

        /* TWSI init */
        slave.type = ADDR7_BIT;
        slave.address = 0;
        mvTwsiInit(0, TWSI_SPEED , mvBoardTclkGet(), &slave, 0);

        /* Read MPP module ID */
        twsiSlave.slaveAddr.address = 0x4D;
        twsiSlave.slaveAddr.type = ADDR7_BIT;

        twsiSlave.validOffset = MV_TRUE;
        /* Use offset as command */
        twsiSlave.offset = regNum;
        twsiSlave.moreThen256 = MV_FALSE;

        if (MV_OK != mvTwsiRead(0, &twsiSlave, &data, 1)) {
                return MV_ERROR;
        }

        return data;
}
#endif
#if 1

#define MV_BOARD_PEX_MODULE_ADDR		0x23
#define MV_BOARD_PEX_MODULE_ADDR_TYPE		ADDR7_BIT
#define MV_BOARD_PEX_MODULE_ID			0

MV_STATUS mvBoardModulesScan(void)
{
	MV_U8 regVal;
	MV_TWSI_SLAVE twsiSlave;
	MV_U32 boardId = mvBoardIdGet();

	/* Perform scan only for DB board */
	if ( (boardId == DB_88F78XX0_BP_ID) || (boardId == DB_88F78XX0_BP_REV2_ID) ) {
		/* reset modules flags */
		PexModule = 0;
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
#endif

MV_BOOL mvBoardIsPexModuleConnected(void)
{

        return PexModule;
}


MV_U16 mvCtrlModelGet(MV_VOID)
{
        MV_U32 devId;
        MV_U16 model = 0;
        MV_U32 reg, reg2;

        /* if PEX0 clocks are disabled - enabled it to read */
        reg = MV_REG_READ(POWER_MNG_CTRL_REG);
        if ((reg & PMC_PEXSTOPCLOCK_MASK(0)) == PMC_PEXSTOPCLOCK_STOP(0)) {
                reg2 = ((reg & ~PMC_PEXSTOPCLOCK_MASK(0)) | PMC_PEXSTOPCLOCK_EN(0));
                MV_REG_WRITE(POWER_MNG_CTRL_REG, reg2);
        }

        devId = MV_REG_READ(PEX_CFG_DIRECT_ACCESS(0, PEX_DEVICE_AND_VENDOR_ID));

        /* Reset the original value of the PEX0 clock */
        if ((reg & PMC_PEXSTOPCLOCK_MASK(0)) == PMC_PEXSTOPCLOCK_STOP(0))
                MV_REG_WRITE(POWER_MNG_CTRL_REG, reg);


        model = (MV_U16) ((devId >> 16) & 0xFFFF);
        return model;
}



/*******************************************************************************/

MV_U16 mvBoardPexModeGet(MV_VOID)
{
        MV_U8 sar;
 
         sar = mvBoardTwsiSatRGet(1, 1);
         if ((MV_8)MV_ERROR == (MV_8)sar)
                 return MV_ERROR;
 
         return (sar & 0x6) >> 1;
}

MV_U32 mvBoardSledCpuNumGet(MV_VOID)
{
	MV_U32 reg;

	reg = MV_REG_READ(GPP_DATA_IN_REG(0));

	return ((reg & 0xF0000) >> 16);
}

MV_BIN_SERDES_CFG *mvBoardSerdesCfgGet(void)
{
	MV_U32 boardId;
	MV_U32 serdesCfg_val = 0; /* default */
	int pex0 = 1;
	int pex1 = 1;
		
	mvBoardModulesScan();

	MV_BOOL moduleConnected = mvBoardIsPexModuleConnected();
	MV_U16 pexMode = mvBoardPexModeGet();
	
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
        case MV_6710_DEV_ID:
        case MV_78230_DEV_ID:
                return 7;
        case MV_78160_DEV_ID:
        case MV_78260_DEV_ID:
                return 12;
                break;
        case MV_78460_DEV_ID:
        case MV_78000_DEV_ID:
                return 16;
        default:
                return 0;
	}
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

MV_U32 get_serdesLineCfg(MV_U32 serdesLineNum,MV_BIN_SERDES_CFG *pSerdesInfo) 
{
	if (serdesLineNum < 8)
		return (pSerdesInfo->serdesLine0_7 >> (serdesLineNum << 2)) & 0xF;
	else
		return (pSerdesInfo->serdesLine8_15 >> ((serdesLineNum - 8) << 2)) & 0xF;
}

MV_U32 get_pexUnit(MV_U32 serdesLineNum,MV_BIN_SERDES_CFG *pSerdesInfo)
{
	MV_U32 pexUnit;
	MV_U32 pexLineNum;
	pexUnit    = serdesLineNum >> 2;
	pexLineNum = serdesLineNum % 4;

	if ((serdesLineNum > 7) && (pSerdesInfo->pex3Mod == PEX_BUS_MODE_X8))
		pexUnit = 3; /* lines 8 - 15 are belong to PEX3 in x8 mode */

	return pexUnit;
}
MV_STATUS mvCtrlHighSpeedSerdesPhyConfig(MV_VOID)
{

	MV_U32		serdesLineCfg;
	MV_U8		serdesLineNum;
	MV_U32		regAddr[16][11], regVal[16][11], regMask[16][11]; /* addr/value for each line @ every setup step */
	MV_U8		pexUnit, pexLineNum;
	MV_U8		step;
	MV_U8		maxSerdesLines = mvCtrlSerdesMaxLinesGet();
	MV_BIN_SERDES_CFG	*pSerdesInfo = mvBoardSerdesCfgGet();
	MV_STATUS	status = MV_OK;
	MV_U32		tmp;
	MV_U32 in_direct;
	MV_PEX_UNIT_CFG 	pexUnitCfg[MV_PEX_MAX_UNIT];

	/* Check if no SERDESs available - FPGA */
	if (maxSerdesLines == 0)
		return MV_OK;

	if (pSerdesInfo == NULL) {
		return MV_ERROR;
	}
	
	memset(regAddr, 0, sizeof(regAddr));
	memset(regVal,  0, sizeof(regVal));
	memset(regMask,  0, sizeof(regMask));


	for (pexUnit = 0; pexUnit < mvCtrlPexMaxUnitGet(); pexUnit++) {
		switch (pexUnit) {
		case 0:
			pexUnitCfg[pexUnit] = pSerdesInfo->pex0Mod;
			break;
		case 1:
			pexUnitCfg[pexUnit] = pSerdesInfo->pex1Mod;
			break;
		case 2:
			pexUnitCfg[pexUnit] = pSerdesInfo->pex2Mod;
			break;
		case 3:
			pexUnitCfg[pexUnit] = pSerdesInfo->pex3Mod;
			break;
		}
	}


	/* Prepare PHY parameters for each step according to  MUX selection */
	for (serdesLineNum = 0; serdesLineNum < maxSerdesLines; serdesLineNum++) {
		/* for each serdes lane*/
		MV_U32	*pRegVal = regVal[serdesLineNum];
		MV_U32	*pRegAddr = regAddr[serdesLineNum];
		MV_U32	*pRegMask = regMask[serdesLineNum];
		MV_U8	sgmiiPort = 0;
		
		serdesLineCfg = get_serdesLineCfg(serdesLineNum,pSerdesInfo);

		if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_UNCONNECTED]) {
				continue;

		} else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_PEX]) {
				continue;

		} else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SATA]) {

			MV_U8	sataPort;

			if ((serdesLineNum == 4) || (serdesLineNum == 6)) {
				sataPort = 0;
			} else if (serdesLineNum == 5) {
				sataPort = 1;
			} else
				goto err_cfg;

			pRegAddr[0] = SATA_LP_PHY_EXT_CTRL_REG(sataPort);
			pRegVal[0]  = (pSerdesInfo->busSpeed & (1 << serdesLineNum)) != 0 ? ((0x11 << 5) | (0x0<<0)) : 0x0;
			pRegMask[0]  = ~((0xff << 5) |(0x7 <<0));

			pRegAddr[1] = SATA_PWR_PLL_CTRL_REG(sataPort);
			pRegVal[1]  = (0x0<<5) | (0x1<<0);
			pRegMask[1]  = ~((0x7 << 5) |(0x1f <<0));

			pRegAddr[2] = SATA_DIG_LP_ENA_REG(sataPort);
			pRegVal[2]  = 0x1<<10;
			pRegMask[2]  = ~(0x3 << 10) ;

			pRegAddr[3] = SATA_REF_CLK_SEL_REG(sataPort);
			pRegVal[3]  = 0x1<<10;
			pRegMask[3]  = ~(0x1<<10);

			pRegAddr[4] = SATA_COMPHY_CTRL_REG(sataPort);
			pRegVal[4] = 0x0<<2;
			pRegMask[4] = ~(0x1<<2);

			pRegAddr[5] = SATA_LP_PHY_EXT_CTRL_REG(sataPort);
			pRegVal[5]  = 0x7<<0;
			pRegMask[5]  = ~(0x7 <<0);

			pRegAddr[6] = SATA_LP_PHY_EXT_STAT_REG(sataPort);
			pRegVal[6]  = 0x7<<0;
			pRegVal[6]  =  ~(0x7 <<0);



		} else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_QSGMII]) {
			pRegAddr[0] = 0x724a0; /* SGMII_COMPHY_CTRL_REG(sgmiiPort);	*/
			pRegVal[0]  = ((0x33 << 5) | (0x0<<0));
			pRegMask[0]  = ~((0xff << 5) |(0x7 <<0));

			pRegAddr[1] = 0x72e04; /* SGMII_PWR_PLL_CTRL_REG(sgmiiPort); */
			pRegVal[1]  = (0x4<<5)|(0x1<<0);
			pRegMask[1]  = ~((0x7 << 5) |(0x1f <<0));

			pRegAddr[2] = 0x72e8c; /* SGMII_DIG_LP_ENA_REG(sgmiiPort);	*/
			pRegVal[2]  = 0x1<<10;
			pRegMask[2]  = ~(0x3 << 10) ;

			pRegAddr[3] = 0x72f18; /* SGMII_REF_CLK_SEL_REG(sgmiiPort);	*/
			pRegVal[3]  = 0x1<<10;
			pRegMask[3]  = ~(0x1<<10);

			pRegAddr[4] = 0x72f20; /* SGMII_SERDES_CFG_REG(sgmiiPort);	*/
			pRegVal[4] = 0x0<<2;
			pRegMask[4] = ~(0x1<<2);

			pRegAddr[5] = 0x724a0; /* SGMII_COMPHY_CTRL_REG(sgmiiPort);	*/
			pRegVal[5]  = 0x7<<0;
			pRegMask[5]  = ~(0x7 <<0);

			pRegAddr[6] = 0x724a4; /* SGMII_SERDES_STAT_REG(sgmiiPort);	*/
			pRegVal[6]  = 0x7<<0;
			pRegVal[6]  =  ~(0x7 <<0);
	
		} else {

			if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII0]) {
				sgmiiPort = 0;
			} else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII1]) {
				sgmiiPort = 1;
			} else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII2]) {
				sgmiiPort = 2;
			} else if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_SGMII3]) {
				sgmiiPort = 3;
			} else	
				continue;

			pRegAddr[0] = SGMII_SERDES_CFG_REG(sgmiiPort);
			pRegVal[0]  = (pSerdesInfo->busSpeed & (1 << serdesLineNum)) != 0 ? ((0x88 << 5) | (0x0<<0)) : ((0x66 << 5) | (0x0<<0));
			pRegMask[0]  = ~((0xff << 5) |(0x7 <<0));

			pRegAddr[1] = SGMII_PWR_PLL_CTRL_REG(sgmiiPort);
			pRegVal[1]  = (0x4<<5)|(0x1<<0);
			pRegMask[1]  = ~((0x7 << 5) |(0x1f <<0));

			pRegAddr[2] = SGMII_DIG_LP_ENA_REG(sgmiiPort);
			pRegVal[2]  = 0x0<<10;
			pRegMask[2]  = ~(0x3 << 10) ;

			pRegAddr[3] = SGMII_REF_CLK_SEL_REG(sgmiiPort);
			pRegVal[3]  = 0x1<<10;
			pRegMask[3]  = ~(0x1<<10);

			pRegAddr[4] = SGMII_COMPHY_CTRL_REG(sgmiiPort);
			pRegVal[4] = 0x0<<2;
			pRegMask[4] = ~(0x1<<2);

			pRegAddr[5] = SGMII_SERDES_CFG_REG(sgmiiPort);
			pRegVal[5]  = 0x7<<0;
			pRegMask[5]  = ~(0x7 <<0);

			pRegAddr[6] = SGMII_SERDES_STAT_REG(sgmiiPort);
			pRegVal[6]  = 0x7<<0;
			pRegVal[6]  =  ~(0x7 <<0);
		}

	} /* for each serdes lane*/
	


		/*----------------------------------------------*/
		/*----------------------------------------------*/
		/*----------------------------------------------*/
		/*----------------------------------------------*/


		/* Step 0 - configure SERDES MUXes */
		MV_REG_WRITE(SERDES_LINE_MUX_REG_0_7,  pSerdesInfo->serdesLine0_7);
		MV_REG_WRITE(SERDES_LINE_MUX_REG_8_15, pSerdesInfo->serdesLine8_15);
		/* QSGMII enable */
		for (serdesLineNum = 0; serdesLineNum < maxSerdesLines; serdesLineNum++) {
			serdesLineCfg = get_serdesLineCfg(serdesLineNum,pSerdesInfo);

			if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_QSGMII]) {
				/* QSGMII Active bit set to true */
				tmp = MV_REG_READ(QSGMII_CONTROL_1_REG);
				tmp |= BIT30;
				MV_REG_WRITE(QSGMII_CONTROL_1_REG,  tmp);
			}
		}

		/* Step 1 [PEX-Only] PEX-Main configuration (X4 or X1): */
		/* First disable all PEXs in SoC Control Reg */
		/* SoC Control 0x18204 [7,8]
			0x0 X4-Link.
			0x1 n*X1-Link’s */
		tmp = MV_REG_READ(SOC_CTRL_REG) & 0xfffffe00;
		for (pexUnit = 0; pexUnit < mvCtrlPexMaxUnitGet(); pexUnit++) {
			if (pexUnitCfg[pexUnit] == PEX_BUS_DISABLED)
				continue;
			if (pexUnit < 2) {
				if (pexUnitCfg[pexUnit] == PEX_BUS_MODE_X1)
					tmp |= SCR_PEX_4BY1_MASK(pexUnit);
				else
					tmp &= ~(SCR_PEX_4BY1_MASK(pexUnit));

			}
		}
		MV_REG_WRITE(SOC_CTRL_REG, tmp);


		for (serdesLineNum = 0; serdesLineNum < maxSerdesLines; serdesLineNum++) {
			serdesLineCfg = get_serdesLineCfg(serdesLineNum,pSerdesInfo);

			if (serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_PEX]) {
				/* PCI Express Control
					0xX1A00 [0]:
					0x0 X4-Link.
					0x1 X1-Link */
				tmp = MV_REG_READ(PEX_LINK_CAPABILITIES_REG(MV_SERDES_NUM_TO_PEX_NUM(serdesLineNum)));
				tmp &= ~(0x3FF);
				if (pexUnitCfg[serdesLineNum >> 2] == PEX_BUS_MODE_X1)
					tmp |= (0x1 << 4);
				if (pexUnitCfg[serdesLineNum >> 2] == PEX_BUS_MODE_X4)
					tmp |= (0x4 << 4);

				MV_REG_WRITE(PEX_LINK_CAPABILITIES_REG(MV_SERDES_NUM_TO_PEX_NUM(serdesLineNum)), tmp);

		
				/* if pex is X4, no need to pass thru the other 3X1 serdes lines */
				if (pexUnitCfg[serdesLineNum >> 2] == PEX_BUS_MODE_X4)
					serdesLineNum += 3;
			}
		}

		/* Step 2 [PEX-X4 Only] To create PEX-Link that contain 4-lanes you need to config the
		 register SOC_Misc/General Purpose2 (Address= 182F8)*/
		tmp = MV_REG_READ(GEN_PURP_RES_2_REG);
		  tmp |= 0x0000000F;

		if (pSerdesInfo->pex0Mod == PEX_BUS_MODE_X4)
                       tmp |= 0x0000000F;

		if (pSerdesInfo->pex1Mod == PEX_BUS_MODE_X4)
			tmp |= 0x000000F0;

		if (pSerdesInfo->pex2Mod == PEX_BUS_MODE_X4)
			tmp |= 0x00000F00;

		if (pSerdesInfo->pex3Mod == PEX_BUS_MODE_X4)
			tmp |= 0x0000F000;

		MV_REG_WRITE(GEN_PURP_RES_2_REG, tmp);

		/* STEP 3 [PEX-Only] First phase of PEX-PIPE Configuration:*/
		for (pexUnit = 0; pexUnit < mvCtrlPexMaxUnitGet(); pexUnit++) {
			if (pexUnitCfg[pexUnit] == PEX_BUS_DISABLED)
				continue;
			
			MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0xC1 << 16) | 0x25);
				
			if (pexUnitCfg[pexUnit] == PEX_BUS_MODE_X4) {
				MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0xC2 << 16) | 0x200);
				MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0xC3 << 16) | 0x01);
			}
				
			if (pexUnitCfg[pexUnit] == PEX_BUS_MODE_X1) {
				MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0xC2 << 16) | 0x00);
				MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0xC3 << 16) | 0x0F);
			}
 				

			MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0xC8 << 16) | 0x05);
			MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0xC5 << 16) | 0x11F);
		}

		/* Steps 4, 5, 6, 7, 8 - use prepared REG addresses and values */
		for (step = 0; step < 5; step++) {
			for (serdesLineNum = 0; serdesLineNum < maxSerdesLines; serdesLineNum++) {
				if (regAddr[serdesLineNum][step] != 0) {					
					MV_REG_WRITE(regAddr[serdesLineNum][step],((MV_REG_READ(regAddr[serdesLineNum][step]) & regMask[serdesLineNum][step]) | regVal[serdesLineNum][step]));
				}
			}
		}
		
		/* PEX step 5*/	
		for (serdesLineNum = 0; serdesLineNum < maxSerdesLines; serdesLineNum++) {
			serdesLineCfg = get_serdesLineCfg(serdesLineNum,pSerdesInfo);
			if(serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_PEX]) {
				pexUnit    = serdesLineNum >> 2;
				pexLineNum = serdesLineNum % 4;
				/* Needed for PEX_PHY_ACCESS_REG macro */
				if ((serdesLineNum > 7) && (pSerdesInfo->pex3Mod == PEX_BUS_MODE_X8))
					pexUnit = 3; /* lines 8 - 15 are belong to PEX3 in x8 mode */

			/* regVal bits:
			bit[31] - 0:write, 1:read
			bit[23:16] - PHY REG offset
			bit[29:24] - PEX line
			bit[15:0] - value to be set in PHY REG
			*/

	 		in_direct =  (0x1<<31) | (pexLineNum << 24) | (0x01 << 16);
			MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit),in_direct);
			tmp = MV_REG_READ(PEX_PHY_ACCESS_REG(pexUnit)) & 0xffff;

	 		in_direct =  (0x0<<31) | (pexLineNum << 24) | (0x01 << 16) | (tmp & 0xff00) | 0x60;
			MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit),in_direct);
			}
		}
	
 		/* PEX step 8*/	
		for (serdesLineNum = 0; serdesLineNum < maxSerdesLines; serdesLineNum++) {
			serdesLineCfg = get_serdesLineCfg(serdesLineNum,pSerdesInfo);
			if(serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_PEX]) {
				pexUnit    = serdesLineNum >> 2;
				pexLineNum = serdesLineNum % 4;
				/* Needed for PEX_PHY_ACCESS_REG macro */
				if ((serdesLineNum > 7) && (pSerdesInfo->pex3Mod == PEX_BUS_MODE_X8))
					pexUnit = 3; /* lines 8 - 15 are belong to PEX3 in x8 mode */

			/* regVal bits:
			bit[31] - 0:write, 1:read
			bit[23:16] - PHY REG offset
			bit[29:24] - PEX line
			bit[15:0] - value to be set in PHY REG
			*/

                        in_direct  = (0x1<<31) | (0x48 << 16) | (pexLineNum << 24);
			MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), in_direct);
			tmp = MV_REG_READ(PEX_PHY_ACCESS_REG(pexUnit)) & 0xffff;
                        in_direct  = (0x48 << 16) | (pexLineNum << 24) | (tmp & 0xeffb) | 0x1000;
			MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit),in_direct);
			}
		}



		/* Step 9 [PEX-Only] Last phase of PEX-PIPE Configuration */
		for (pexUnit = 0; pexUnit < mvCtrlPexMaxUnitGet(); pexUnit++) {
			if (pexUnitCfg[pexUnit] != PEX_BUS_DISABLED)
				MV_REG_WRITE(PEX_PHY_ACCESS_REG(pexUnit), (0xC1 << 16) | 0x24);
		}

		/*Step 10 power up to the PU_PLL,PU_RX,PU_TX.  */
		for (; step < 6; step++) {
			for (serdesLineNum = 0; serdesLineNum < maxSerdesLines; serdesLineNum++) {
				if (regAddr[serdesLineNum][step] != 0) {
					MV_REG_WRITE(regAddr[serdesLineNum][step],((MV_REG_READ(regAddr[serdesLineNum][step]) & regMask[serdesLineNum][step]) | regVal[serdesLineNum][step]));
				}
			}
		}


		/* Step11 Wait 10ms , check that PLL RX/TX is ready and RX init done */
		mvOsDelay(10);
		for (serdesLineNum = 0; serdesLineNum < maxSerdesLines; serdesLineNum++) {
			MV_U32  tmp;
			if (regAddr[serdesLineNum][step] != 0) { /* SATA, QSGMII, SGMII */
				tmp = MV_REG_READ(regAddr[serdesLineNum][step]);
				if ((tmp & 0x7) != regVal[serdesLineNum][step]) { /* check bits[2:0] */
					status = MV_ERROR;
				}
			}
		}

#if 0
		/* step12 [PEX-Only]  In order to configure RC/EP mode please write  to register 0x0060 bits */
		for (serdesLineNum = 0; serdesLineNum < maxSerdesLines; serdesLineNum++) {
			serdesLineCfg = get_serdesLineCfg(serdesLineNum,pSerdesInfo);
			if(serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_PEX]) {
				pexUnit    = serdesLineNum >> 2;
				pexLineNum = serdesLineNum % 4;
 				if (pexUnitCfg[pexUnit] == PEX_BUS_DISABLED)
					continue;
				tmp = MV_REG_READ(PEX_CAPABILITIES_REG(MV_SERDES_NUM_TO_PEX_NUM(serdesLineNum)));
				if ( pSerdesInfo->pexType == MV_PEX_ROOT_COMPLEX)
					tmp = (tmp & ~(0xf<<20)) | (0x4<<20);
				else
					tmp = (tmp & ~(0xf<<20)) | (0x1<<20);
				MV_REG_WRITE(PEX_CAPABILITIES_REG(MV_SERDES_NUM_TO_PEX_NUM(serdesLineNum)),tmp);

  				if (pexUnitCfg[pexUnit] == PEX_BUS_MODE_X4)
                                        serdesLineNum += 3;
                        }
                }

		/* step 13 [PEX-Only] Only for EP mode set to Zero bits 19 and 16 of register 0x1a60 */
		for (serdesLineNum = 0; serdesLineNum < maxSerdesLines; serdesLineNum++) {
			serdesLineCfg = get_serdesLineCfg(serdesLineNum,pSerdesInfo);
			if(serdesLineCfg == serdesCfg[serdesLineNum][SERDES_UNIT_PEX]) {
				pexUnit    = serdesLineNum >> 2;
				pexLineNum = serdesLineNum % 4;

				if (pexUnitCfg[pexUnit] == PEX_BUS_DISABLED)
					continue;
				if ( pSerdesInfo->pexType == MV_PEX_END_POINT)
					MV_REG_WRITE(PEX_DBG_CTRL_REG(MV_SERDES_NUM_TO_PEX_NUM(serdesLineNum)),(MV_REG_READ(PEX_DBG_CTRL_REG(MV_SERDES_NUM_TO_PEX_NUM(serdesLineNum))) & 0xfff6ffff));
 				if (pexUnitCfg[pexUnit] == PEX_BUS_MODE_X4)
                                        serdesLineNum += 3;
			}
		}
#else
#define MV_PEX_UNIT_TO_IF(pexUnit)	((pexUnit < 3) ? (pexUnit*4) : 9)
		/* step12 [PEX-Only]  In order to configure RC/EP mode please write  to register 0x0060 bits */
		for (pexUnit = 0; pexUnit < mvCtrlPexMaxUnitGet(); pexUnit++) {
			if (pexUnitCfg[pexUnit] == PEX_BUS_DISABLED)
				continue;
			tmp = MV_REG_READ(PEX_CAPABILITIES_REG(MV_PEX_UNIT_TO_IF(pexUnit)));
			if ( pSerdesInfo->pexType == MV_PEX_ROOT_COMPLEX)
				tmp = (tmp & ~(0xf<<20)) | (0x4<<20);
			else
				tmp = (tmp & ~(0xf<<20)) | (0x1<<20);
			MV_REG_WRITE(PEX_CAPABILITIES_REG(MV_PEX_UNIT_TO_IF(pexUnit)),tmp);
		}
		
		/* step 13 [PEX-Only] Only for EP mode set to Zero bits 19 and 16 of register 0x1a60 */
		for (pexUnit = 0; pexUnit < mvCtrlPexMaxUnitGet(); pexUnit++) {
			if (pexUnitCfg[pexUnit] == PEX_BUS_DISABLED)
				continue;
			if ( pSerdesInfo->pexType == MV_PEX_END_POINT)
				MV_REG_WRITE(PEX_DBG_CTRL_REG(MV_PEX_UNIT_TO_IF(pexUnit)),(MV_REG_READ(PEX_DBG_CTRL_REG(MV_PEX_UNIT_TO_IF(pexUnit))) & 0xfff6ffff));
		}
#endif		
				
		/* step 14 [PEX-Only] Training Enable */
		tmp = MV_REG_READ(SOC_CTRL_REG);
		for (pexUnit = 0; pexUnit < mvCtrlPexMaxUnitGet(); pexUnit++) {
			if (pexUnitCfg[pexUnit] == PEX_BUS_DISABLED)
				tmp = tmp & ~(0x1<<pexUnit);
			else
				tmp = tmp | (0x1<<pexUnit);
		}
		MV_REG_WRITE(SOC_CTRL_REG, tmp);


	return status;
err_cfg:
	return MV_ERROR;


}
