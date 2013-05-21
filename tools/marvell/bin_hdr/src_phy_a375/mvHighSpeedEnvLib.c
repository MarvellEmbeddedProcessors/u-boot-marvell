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
#include "config_marvell.h"     /* Required to identify SOC and Board */
#if defined(MV88F78X60)
#include "ddr3_axp.h"
#elif defined(MV88F6710)
#include "ddr3_a370.h"
#elif defined(MV88F66XX)
#include "ddr3_alp.h"
#elif defined(MV88F672X)
#include "ddr3_a375.h"
#else
#error "No SOC define for uart in binary header."
#endif
#include "mvHighSpeedEnvSpec.h"
#include "mvBHboardEnvSpec.h"

#include "bin_hdr_twsi.h"
#include "mvUart.h"
#include "util.h"


#define SERDES_VERION   "0.1.1"
#define ENDED_OK "High speed PHY - Ended Successfully\n"


extern MV_BIN_SERDES_UNIT_INDX boardLaneConfig[];

/***************************   defined ******************************/
#define BOARD_INFO(boardId) boardInfoTbl[boardId - BOARD_ID_BASE]

#define BOARD_DEV_TWSI_EEPROM               0x54
#define BOARD_DEV_TWSI_IO_EXPANDER          0x21
#define BOARD_DEV_TWSI_IO_EXPANDER_JUMPER1  0x24

/******************   Global parameters ******************************/
MV_U8 configVal[MV_IO_EXP_MAX_REGS];

/****************************  Local function *****************************************/
MV_U16 mvCtrlModelGet(MV_VOID);
MV_STATUS mvBoardTwsiGet(MV_U32 address, MV_U8 devNum, MV_U8 regNum, MV_U8 *pData);
MV_U32 mvPexConfigRead(MV_U32 pexIf, MV_U32 bus, MV_U32 dev, MV_U32 func, MV_U32 regOff);
MV_STATUS mvPexLocalBusNumSet(MV_U32 pexIf, MV_U32 busNum);
MV_STATUS mvPexLocalDevNumSet(MV_U32 pexIf, MV_U32 devNum);

/****************************  function implementation *****************************************/

/*******************************************************************************
* mvBoardIdGet - Get Board model
*
* DESCRIPTION:
*       This function returns board ID.
*       Board ID is 32bit word constructed of board model (16bit) and
*       board revision (16bit) in the following way: 0xMMMMRRRR.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit board ID number, '-1' if board is undefined.
*
*******************************************************************************/
MV_U32 mvBoardIdGet(MV_VOID)
{
    MV_U32 boardId, value;

    value = MV_REG_READ(MPP_SAMPLE_AT_RESET(1));
    boardId = ((value & (0xF0)) >> 4);
    if (boardId >= MV_MAX_BOARD_ID) {
     DEBUG_INIT_C("Error: read wrong board ID=0x", boardId,2);
    }

#if 0
    if (boardId == 0)
    {
      DEBUG_INIT_S(">>>>> boardid is zero. changing it temporarily to 3 . Giora <<<<<<<<<<<<<<<\n");
      boardId = 3;
    }
#endif
	DEBUG_INIT_FULL_C("Read board ID=0x", boardId,2);

    return boardId;
}

/*******************************************************************************
* mvCtrlIsEepromEnabled - read jumper and verify if EEPROM is enabled
*
* DESCRIPTION:
*       This function returns MV_TRUE if board configuration jumper is set to EEPROM.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BOOL :  MV_TRUE if EEPROM enabled, else return MV_FALSE.
*
*******************************************************************************/
MV_BOOL mvCtrlIsEepromEnabled()
{
    MV_U8 regVal;

     if (MV_OK == mvBoardTwsiGet(BOARD_DEV_TWSI_IO_EXPANDER_JUMPER1, 0, 0, &regVal))
		 return ((regVal & 0x80)? MV_FALSE : MV_TRUE);
	 return MV_FALSE;
}
/*******************************************************************************
* mvCtrlBoardConfigGet - read Board Configuration, from EEPROM / Dip Switch
*
* DESCRIPTION:
*       This function reads all board configuration from EEPROM / Dip Switch:
*           1. read the EEPROM enable jumper, and read from configured device
*           2. read first 2 registers for all boards
*           3. read specific registers for specific boards
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BOOL :  MV_TRUE if EEPROM enabled, else return MV_FALSE.
*
*******************************************************************************/
MV_STATUS mvCtrlBoardConfigGet(MV_U8 *tempVal)
{
    MV_STATUS rc1, rc2;
    MV_BOOL isEepromEnabled = mvCtrlIsEepromEnabled();
    MV_U32 address= (isEepromEnabled ? BOARD_DEV_TWSI_EEPROM : BOARD_DEV_TWSI_IO_EXPANDER);

    rc1 = mvBoardTwsiGet(address, 0, 0, &tempVal[0] ); /* EEPROM/Dip Switch Reg#0 */
    rc2 = mvBoardTwsiGet(address, 0, 1, &tempVal[1] );  /* EEPROM/Dip Switch Reg#1 */

    /* verify that all TWSI reads were successfully */
    if ((rc1 != MV_OK) || (rc2 != MV_OK))
        return MV_ERROR;

    return MV_OK;
}
/*******************************************************************************
* mvCtrlSatrInit
*
* DESCRIPTION: read relevant board configuration (using TWSI/EEPROM access)
*
* INPUT:  None
*
* OUTPUT: None
*
* RETURN: NONE
*
*******************************************************************************/
MV_VOID mvCtrlSatrInit(void)
{
    MV_U8 tmp;
    MV_U32  boardId = mvBoardIdGet();
	boardLaneConfig[0] = SERDES_UNIT_PEX;

    /* Verify that board support Auto detection from S@R & board configuration
     else write manually the lane configurations*/

    if (boardId == RD_88F6660_BP_ID){
		boardLaneConfig[1] = SERDES_UNIT_PEX;
		boardLaneConfig[2] = SERDES_UNIT_SATA;
		boardLaneConfig[3] = SERDES_UNIT_USB3;
        return;
    }

    /*Read rest of Board Configuration, EEPROM / Dip Switch access read : */
    if (mvCtrlBoardConfigGet(configVal) == MV_OK ) {
		tmp = ((configVal[1] & 0x0c) >> 2);
		switch (tmp) {
		case 0: boardLaneConfig[1] = SERDES_UNIT_PEX; 	break;
		case 1: boardLaneConfig[1] = SERDES_UNIT_SGMII;	break;
		case 2: boardLaneConfig[1] = SERDES_UNIT_SATA; 	break;
		case 3:
		default:
			DEBUG_INIT_S("Error: Read board configuration (SERDES LAN1) from EEPROM/Dip Switch failed \n");
			boardLaneConfig[1] = SERDES_UNIT_UNCONNECTED;
			break;
		}
		boardLaneConfig[2] = ((configVal[1] & 0x10) >> 4)? SERDES_UNIT_SATA:SERDES_UNIT_SGMII;
		boardLaneConfig[3] = ((configVal[1] & 0x20) >> 5)? SERDES_UNIT_SGMII:SERDES_UNIT_USB3;
	}
    else{
        DEBUG_INIT_S("Error: Read board configuration from EEPROM/Dip Switch failed \n");
        DEBUG_INIT_S(">>>>> temporarily setting boardLaneConfig to PEX1, SGMII, SATA2, USB3     for testing. Giora <<<<\n");
		boardLaneConfig[1] = SERDES_UNIT_SGMII;
		boardLaneConfig[2] = SERDES_UNIT_SATA;
		boardLaneConfig[3] = SERDES_UNIT_USB3;
    }
}
/*******************************************************************************
* mvBoardTclkGet -
*
* DESCRIPTION: this function read the TCLK frequency from S@R register and return
*              the TCLK frequency
*
* INPUT:
*        None
*
* OUTPUT:
*        None.
*
* RETURN:
*        TCLK frequency
*
*******************************************************************************/
MV_U32 mvBoardTclkGet(MV_VOID)
{
    MV_U32 value = MV_REG_READ(MPP_SAMPLE_AT_RESET(1));
    value= ((value & (MSAR_TCLK_MASK)) >> MSAR_TCLK_OFFS);

    switch (value) {
        case 0:
            return MV_BOARD_TCLK_166MHZ;
        case 1:
            return MV_BOARD_TCLK_200MHZ;
        default:
            DEBUG_INIT_S("Error : Board: Read from S@R fail\n");
            return MV_BOARD_TCLK_200MHZ;
    }
}
/*******************************************************************************
* mvBoardTwsiGet -
*
* DESCRIPTION:
*
* INPUT:
*           device num - one of three devices
*           reg num - 0 or 1
*
* OUTPUT:
*                         None.
*
* RETURN:
*                         reg value
*
*******************************************************************************/
MV_STATUS mvBoardTwsiGet(MV_U32 address, MV_U8 devNum, MV_U8 regNum, MV_U8 *pData)
{
     MV_TWSI_SLAVE twsiSlave;
     MV_TWSI_ADDR slave;

     /* TWSI init */
     slave.type = ADDR7_BIT;
     slave.address = 0;

     DEBUG_INIT_FULL_C(">>> in mvBoardTwsiGet 1, address=0x",address, 2);
     DEBUG_INIT_FULL_C(" devmum=0x", devNum, 2);
     DEBUG_INIT_FULL_C(" regnum=0x", regNum, 2);
     DEBUG_INIT_FULL_S("\n");

     mvTwsiInit(0, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);

     /* Read MPP module ID */
     DEBUG_INIT_FULL_S("Board: Read S@R device read\n");

     twsiSlave.slaveAddr.address = address;
     twsiSlave.slaveAddr.type = ADDR7_BIT;
     twsiSlave.validOffset = MV_TRUE;
     /* Use offset as command */
     twsiSlave.offset = regNum;
     twsiSlave.moreThen256 = MV_FALSE;

     if (MV_OK != mvTwsiRead(0, &twsiSlave, pData, 1)) {
            DEBUG_INIT_S("TWSI Read failed\n");
            return MV_ERROR;
     }
     DEBUG_INIT_FULL_C("Board: Read TWSI succeeded data=0x",*pData, 2);

     return MV_OK;
}
/*******************************************************************************
* mvCtrlModelGet - Get Marvell controller device model (Id)
*
* DESCRIPTION:
*       This function returns 16bit describing the device model (ID) as defined
*       in PCI Device and Vendor ID configuration register offset 0x0.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       16bit describing Marvell controller ID
*
*******************************************************************************/
MV_U16 mvCtrlModelGet(MV_VOID)
{
    MV_U32 ctrlId, satr0;

    ctrlId = MV_REG_READ(DEV_ID_REG);
    ctrlId = (ctrlId & (DEVICE_ID_MASK)) >> DEVICE_ID_OFFS;

    if (ctrlId == 0x6660)
	return MV_6660_DEV_ID;

    satr0 = MV_REG_READ(MPP_SAMPLE_AT_RESET(0));
    satr0 &= SATR_DEVICE_ID_2_0_MASK;
    if (satr0 == 0)
	return MV_6650_DEV_ID;
    return MV_6610_DEV_ID;
}

/*******************************************************************************
* mvCtrlSerdesMaxLinesGet - Get Marvell controller number of SERDES lanes.
*
* DESCRIPTION:
*       This function returns Marvell controller number of SERDES lanes.
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
MV_U32 mvCtrlSerdesMaxLanesGet(MV_VOID){
      switch (mvCtrlModelGet()) {
        case MV_6660_DEV_ID:
                return 4;
        case MV_6650_DEV_ID:
                return 1;
        case MV_6610_DEV_ID:
                if (mvBoardIdGet() == DB_88F6650_BP_ID)
                  return 1;
                else
                  return 0;
        default:
                return 0;
        }
}
/*******************************************************************************
* mvGetSerdesLaneCfg
*
* DESCRIPTION: This function returns the serdes unit index from the boardLaneConfig
*              array
*
* INPUT:
*       serdes lane number
*
* OUTPUT:
*       None.
*
* RETURN:
*       serdes unit index
*
*******************************************************************************/
#define mvGetSerdesLaneCfg(serdesLaneNum) boardLaneConfig[serdesLaneNum];
/*******************************************************************************
* GetLaneSelectorConfig
*
* DESCRIPTION: This function returns the value of the serdes configuration
*              to the comphy selector
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Register value
*
*******************************************************************************/
MV_U32 GetLaneSelectorConfig(void)
{
    MV_U32 tmp,uiReg;

    uiReg = 0x1; /* lane 0 is always PEX */

	switch (boardLaneConfig[1]) {
	case SERDES_UNIT_PEX:	tmp = 0;	break;
	case SERDES_UNIT_SGMII: tmp = 1;	break;
	case SERDES_UNIT_SATA:	tmp = 2;	break;
	default:				tmp = 3;	break;
	}
    uiReg |= (tmp<<1); /* lane 1  */

    uiReg |= (((boardLaneConfig[2]==SERDES_UNIT_SATA)?1:0)  << 3); /* lane 2  */
    uiReg |= (((boardLaneConfig[3]==SERDES_UNIT_SGMII)?1:0)  << 4); /* lane 3  */


    DEBUG_INIT_FULL_S(">>>>>>> after  GetLaneSelectorConfig, uiReg=0x");
    DEBUG_INIT_FULL_D(uiReg, 8);
    DEBUG_INIT_FULL_S(" <<<<<<<<<\n");
    return uiReg;
}
/*******************************************************************************
* getSerdesSpeedConfig
*
* DESCRIPTION:  This function return the ser-des speed configuration
*
* INPUT:
*       serdes lane configuration
*
* OUTPUT:
*       speed configuration
*
* RETURN:
*       speed configuration
*
*******************************************************************************/
MV_U32 getSerdesSpeedConfig(MV_BIN_SERDES_UNIT_INDX serdesLaneCfg)
{

    switch (serdesLaneCfg){
	case SERDES_UNIT_SGMII:
		{
            if ((configVal[0] & 0x40)) /* 0 \96 1G SGMII  1 \96 2.5G SGMII*/
                return 0x8;
			return 0x6;
		}
        case SERDES_UNIT_USB3:
        case SERDES_UNIT_SATA:
            return 0x1;
        default:
            return 0;
    }
}
/*******************************************************************************
* mvCtrlHighSpeedSerdesPhyConfig
*
* DESCRIPTION: This is the main function which configure the
*              PU sequence of the ser-des
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK   - success
*       MV_ERROR - failure
*******************************************************************************/
MV_STATUS mvCtrlHighSpeedSerdesPhyConfig(MV_VOID)
{
    MV_U32      serdesLaneNum, pexUnit;
    MV_U32      uiReg;
    MV_BIN_SERDES_UNIT_INDX     serdesLaneCfg;
    MV_U32	regAddr[16][11], regVal[16][11]; /* addr/value for each line @ every setup step */
    MV_U8	maxSerdesLanes;
    MV_U32	tmp;
    MV_U32	tempReg, tempPexReg;
    MV_U32	pexIf=0;
	MV_U32  first_busno, next_busno;
    MV_U32	addr;
	MV_TWSI_ADDR slave;

    maxSerdesLanes = mvCtrlSerdesMaxLanesGet();

    if (maxSerdesLanes == 0)
        return MV_OK;

	/*Set MPP1 for twsi access */
	uiReg = (MV_REG_READ(MPP_CONTROL_REG(1))  & 0x00FFFF00) | 0x22000022;
	MV_REG_WRITE(MPP_CONTROL_REG(1), uiReg);

	/* TWSI init */
	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(0, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);


	mvUartInit();

    /* initialize board configuration database */
    mvCtrlSatrInit();

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
	DEBUG_INIT_FULL_C("SERDES 0=",boardLaneConfig[0],2);
	DEBUG_INIT_FULL_C("SERDES 1=",boardLaneConfig[1],2);
	DEBUG_INIT_FULL_C("SERDES 2=",boardLaneConfig[2],2);
	DEBUG_INIT_FULL_C("SERDES 3=",boardLaneConfig[3],2);

    /*------------------------------------------*/
    /* STEP - 1 Power Down PLL, RX, TX all phys */
    /*------------------------------------------*/

    for (serdesLaneNum = 0; serdesLaneNum < maxSerdesLanes; serdesLaneNum++)
    {
      uiReg=MV_REG_READ(COMMON_PHY_CONFIGURATION1_REG(serdesLaneNum));
      uiReg &= ~(PHY_POWER_UP_PLL_MASK | PHY_POWER_UP_RX_MASK | PHY_POWER_UP_TX_MASK);
      MV_REG_WRITE(COMMON_PHY_CONFIGURATION1_REG(serdesLaneNum),uiReg);
    }

    mvOsUDelay(10000);

    for (serdesLaneNum = 0; serdesLaneNum < maxSerdesLanes; serdesLaneNum++)
    {
      uiReg=MV_REG_READ(COMMON_PHY_CONFIGURATION1_REG(serdesLaneNum));
      uiReg &= ~(PHY_SOFTWARE_RESET_MASK);    /* PHY Software Reset = Normal Mode */
      uiReg &= ~(PHY_RESET_CORE_MASK);    /* PHY Reset Core = 0x0 */
      uiReg |= PHY_CORE_RST_MASK;     /* PHY Core RSTn = Normal */
      uiReg |= PHY_POWER_ON_RESET_MASK;   /* PHY Power On Reset = Normal */
      MV_REG_WRITE(COMMON_PHY_CONFIGURATION1_REG(serdesLaneNum),uiReg);
    }


    /*--------------------------------*/
    /* STEP - 2 Common PHYs Selectors */
    /*--------------------------------*/

    MV_REG_WRITE(COMMON_PHY_SELECTOR_REG, GetLaneSelectorConfig());

    /*--------------------------------*/
    /* STEP - 3 Configuration 1       */
    /*--------------------------------*/

    for (serdesLaneNum = 0; serdesLaneNum < maxSerdesLanes; serdesLaneNum++) {
        serdesLaneCfg = mvGetSerdesLaneCfg(serdesLaneNum);
        if(serdesLaneCfg >= SERDES_LAST_UNIT){
            return MV_ERROR;
        }
        uiReg=MV_REG_READ(COMMON_PHY_CONFIGURATION1_REG(serdesLaneNum));
        switch(serdesLaneCfg){
            case SERDES_UNIT_USB3:
                uiReg |= PHY_MODE_MASK;  /* PHY Mode = USB */
                break;
            case SERDES_UNIT_PEX:
                uiReg |= PIPE_SELECT_MASK ; /* Select USB3_PEX */
                break;
            case SERDES_UNIT_SGMII:
            case SERDES_UNIT_SATA:
                uiReg &= ~(PIPE_SELECT_MASK); /* Select SATA_SGMII */
                uiReg |= POWER_UP_IVREF_MASK;  /* Power UP IVREF = Power Up */
                break;
		case SERDES_UNIT_UNCONNECTED:
		default:
			break;
        }

        /* Serdes speed config */
        tmp = getSerdesSpeedConfig(serdesLaneCfg);
        uiReg &= ~(GEN_RX_MASK); /* SERDES RX Speed config */
        uiReg |= tmp<<GEN_RX_OFFS;
        uiReg &= ~(GEN_TX_MASK); /* SERDES TX Speed config */
        uiReg |= tmp<<GEN_TX_OFFS;
        MV_REG_WRITE(COMMON_PHY_CONFIGURATION1_REG(serdesLaneNum),uiReg);
    }

    /*----------------------------------------*/
    /* STEP - 4 COMPHY register configuration */
    /*----------------------------------------*/
    for (serdesLaneNum = 0; serdesLaneNum < maxSerdesLanes; serdesLaneNum++) {
        serdesLaneCfg = mvGetSerdesLaneCfg(serdesLaneNum);
        if(serdesLaneCfg >= SERDES_LAST_UNIT){
            return MV_ERROR;
        }
        switch(serdesLaneCfg){
            case SERDES_UNIT_PEX:
                MV_REG_WRITE(RESET_AND_CLOCK_CONTROL_REG(serdesLaneNum),0x25); /* Enable soft_reset*/
                MV_REG_WRITE(POWER_AND_PLL_CONTROL_REG(serdesLaneNum),0xFC60); /* PHY Mode = PEX */
                MV_REG_WRITE(KVCO_CALOBRATION_CONTROL_REG(serdesLaneNum),0x40); /* use_max_pll_rate=0x0, ext_force_cal_done=0x0 */
                MV_REG_WRITE(RESET_AND_CLOCK_CONTROL_REG(serdesLaneNum),0x24); /* Release soft_reset */
            break;
            case SERDES_UNIT_USB3:
                MV_REG_WRITE(RESET_AND_CLOCK_CONTROL_REG(serdesLaneNum),0x21); /* Enable soft_reset*/
                MV_REG_WRITE(POWER_AND_PLL_CONTROL_REG(serdesLaneNum),0xFCA0); /* PHY Mode = USB3 */
                MV_REG_WRITE(KVCO_CALOBRATION_CONTROL_REG(serdesLaneNum),0x40); /* use_max_pll_rate=0x0, ext_force_cal_done=0x0 */
                MV_REG_WRITE(RESET_AND_CLOCK_CONTROL_REG(serdesLaneNum),0x20); /* Release soft_reset */
                break;
            case SERDES_UNIT_SATA:
                MV_REG_WRITE(RESERVED_46_REG(serdesLaneNum),0xFF00);
                MV_REG_WRITE(POWER_AND_PLL_CONTROL_REG(serdesLaneNum),0xFC01); /* PHY Mode = SATA */
                MV_REG_WRITE(MISCELLANEOUS_CONTROL0_REG(serdesLaneNum),0x6417); /* REFCLK SEL =0x0 (the ref_clk comes from the group 1 pins) */
                break;
            case SERDES_UNIT_SGMII:
                MV_REG_WRITE(RESERVED_46_REG(serdesLaneNum),0xFF00); /* Enable soft_reset*/
                MV_REG_WRITE(POWER_AND_PLL_CONTROL_REG(serdesLaneNum),0xFC81); /* PHY Mode = SGMII */ /*moti need to change offset*/
                MV_REG_WRITE(PHY_ISOLATION_MODE_CONTROL_REG(serdesLaneNum),0x166); /* Set PHY_GEN_TX/RX to 1.25Gbps */
                MV_REG_WRITE(DIGITAL_LOOPBACK_ENABLE_REG(serdesLaneNum),0x0); /* SEL_BITS = 0x0 (10-bits mode) */
                MV_REG_WRITE(MISCELLANEOUS_CONTROL0_REG(serdesLaneNum),0x6417); /* REFCLK SEL =0x1 (the ref_clk comes from the group 1 pins) */
                break;
	    case SERDES_UNIT_UNCONNECTED:
	    default:
			break;
        }
    }

    /*------------------------------------------*/
    /* STEP - 4.5 Power up PLL, RX, TX all phys */
    /*------------------------------------------*/

    for (serdesLaneNum = 0; serdesLaneNum < maxSerdesLanes; serdesLaneNum++)
    {
      uiReg=MV_REG_READ(COMMON_PHY_CONFIGURATION1_REG(serdesLaneNum));
      uiReg |= (PHY_POWER_UP_PLL_MASK | PHY_POWER_UP_RX_MASK | PHY_POWER_UP_TX_MASK);
      MV_REG_WRITE(COMMON_PHY_CONFIGURATION1_REG(serdesLaneNum),uiReg);
    }

    /*----------------------------------------*/
    /* STEP - 5 PEX Only                      */
    /*----------------------------------------*/
	for (pexUnit = 0; pexUnit < 4; pexUnit++) {
		if (boardLaneConfig[pexUnit] != SERDES_UNIT_PEX)
			continue;
		tmp = MV_REG_READ(PEX_CAPABILITIES_REG(pexUnit));
		DEBUG_RD_REG(PEX_CAPABILITIES_REG(pexUnit), tmp );
		tmp &= ~(0xf<<20);
		tmp |= (0x4<<20);
		MV_REG_WRITE(PEX_CAPABILITIES_REG(pexUnit),tmp);
		DEBUG_WR_REG(PEX_CAPABILITIES_REG(pexUnit),tmp);
	}

	tmp = MV_REG_READ(SOC_CTRL_REG);
	DEBUG_RD_REG(SOC_CTRL_REG, tmp);
	tmp &= ~(0x03);
    tmp |= 0x1<<PCIE0_ENABLE_OFFS;
	if (boardLaneConfig[1] == SERDES_UNIT_PEX)
		tmp |= 0x1<<PCIE1_ENABLE_OFFS;
	MV_REG_WRITE(SOC_CTRL_REG, tmp);
	DEBUG_WR_REG(SOC_CTRL_REG, tmp);

    /*----------------------------------------*/
    /* STEP - 6 PEX Only - support gen1/gen2  */
    /*----------------------------------------*/
	next_busno = 0;

	mvOsDelay(150);

    for (pexIf = 0; pexIf < 2; pexIf++)  // only pexIf 0 on avanta_lp
    {
		if (boardLaneConfig[pexIf] != SERDES_UNIT_PEX)
			continue;
      tmp = MV_REG_READ(PEX_DBG_STATUS_REG(pexIf));
      DEBUG_RD_REG(PEX_DBG_STATUS_REG(pexIf), tmp);
	  first_busno = next_busno;
      if ((tmp & 0x7f) == 0x7E) {
		  next_busno++;
		  tempPexReg = MV_REG_READ((PEX_CFG_DIRECT_ACCESS(pexIf, PEX_LINK_CAPABILITY_REG)));
		  DEBUG_RD_REG((PEX_CFG_DIRECT_ACCESS(pexIf, PEX_LINK_CAPABILITY_REG)),tempPexReg );
		  tempPexReg &= (0xF);
		  if (tempPexReg == 0x2) {
				tempReg = (MV_REG_READ(PEX_CFG_DIRECT_ACCESS(pexIf, PEX_LINK_CTRL_STAT_REG)) & 0xF0000) >> 16;
				DEBUG_RD_REG(PEX_CFG_DIRECT_ACCESS(pexIf, PEX_LINK_CTRL_STAT_REG),tempReg );

				/* check if the link established is GEN1 */
				if (tempReg == 0x1) {
					mvPexLocalBusNumSet(pexIf, first_busno);
					mvPexLocalDevNumSet(pexIf, 1);
					DEBUG_INIT_FULL_S("PEX: pexIf ");
					DEBUG_INIT_FULL_D(pexIf, 1);
					DEBUG_INIT_FULL_S(", link is Gen1, checking the EP capability \n");


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

						DEBUG_INIT_FULL_S("PEX: pexIf ");
						DEBUG_INIT_FULL_D(pexIf, 1);
						DEBUG_INIT_FULL_S(", Link upgraded to Gen2 based on client cpabilities \n");
					} else {
						DEBUG_INIT_FULL_S("PEX: pexIf ");
						DEBUG_INIT_FULL_D(pexIf, 1);
						DEBUG_INIT_FULL_S(", remains Gen1\n");
					}
				}
		  }
      }

      else
      {
	DEBUG_INIT_FULL_S("PEX: pexIf ");
	DEBUG_INIT_FULL_D(pexIf, 1);
	DEBUG_INIT_FULL_S(", detected no link\n");
      }
    }
	/* Step 7: update pex DEVICE ID*/
	{
		MV_U32 devId;
		MV_U32 ctrlMode = mvCtrlModelGet();
		for (pexIf = 0; pexIf < 2; pexIf++) {
			if (boardLaneConfig[pexIf] != SERDES_UNIT_PEX)
				continue;
			devId = MV_REG_READ(PEX_CFG_DIRECT_ACCESS(pexIf, PEX_DEVICE_AND_VENDOR_ID));
			devId &= 0xFFFF;
			devId |= ((ctrlMode << 16) & 0xffff0000);
			MV_REG_WRITE(PEX_CFG_DIRECT_ACCESS(pexIf, PEX_DEVICE_AND_VENDOR_ID), devId);
		}
		DEBUG_INIT_FULL_S("Update PEX Device ID 0x");
		DEBUG_INIT_FULL_D(ctrlMode,4);
		DEBUG_INIT_FULL_S("\n");
	}

	DEBUG_INIT_S(ENDED_OK);
	DEBUG_INIT_S("\n");

    return MV_OK;
}




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

  pexStatus = MV_REG_READ(PEX_STATUS_REG(pexIf));

  localDev = ((pexStatus & PXSR_PEX_DEV_NUM_MASK) >> PXSR_PEX_DEV_NUM_OFFS);
  localBus = ((pexStatus & PXSR_PEX_BUS_NUM_MASK) >> PXSR_PEX_BUS_NUM_OFFS);


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
  pexData |= (regOff & PXCAR_REG_NUM_MASK); /* lgacy register space */
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

	pexStatus = MV_REG_READ(PEX_STATUS_REG(pexIf));

	pexStatus &= ~PXSR_PEX_DEV_NUM_MASK;

	pexStatus |= (devNum << PXSR_PEX_DEV_NUM_OFFS) & PXSR_PEX_DEV_NUM_MASK;

	MV_REG_WRITE(PEX_STATUS_REG(pexIf), pexStatus);

	return MV_OK;
}
