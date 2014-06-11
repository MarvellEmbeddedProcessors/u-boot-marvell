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
#include "mvHighSpeedEnvSpec.h"

/***************************   defined ******************************/
#define SERDES_VERION   "0.1.1"
#define ENDED_OK "High speed PHY - Ended Successfully\n"

/******************   Macros ******************************/
#define A375_A0_COMMON_PHY_CONFIG(regData) \
	regData |= POWER_UP_IVREF_MASK;	/* Power UP IVREF = Power Up */		\
	regData &= ~(REF_CLK_DIS_MASK);	/* Request to disable the PHY digital reference clock */		\
	regData |= PIN_TX_IDLE_MASK;	/* to keep phyTxP/N output to idle during Phy init */

#define A375_A0_RESET_DFE_SEQUENCE(serdesLaneNum) \
	MV_REG_WRITE(POWER_REG1_REG(serdesLaneNum),0xE409); /* Reset DFE sequence */ \
	MV_REG_WRITE(POWER_REG1_REG(serdesLaneNum),0xE008); /* Unreset DFE sequence  */

/****************************  Local function *****************************************/

MV_U32 mvPexConfigRead(MV_U32 pexIf, MV_U32 bus, MV_U32 dev, MV_U32 func, MV_U32 regOff);
MV_STATUS mvPexLocalBusNumSet(MV_U32 pexIf, MV_U32 busNum);
MV_STATUS mvPexLocalDevNumSet(MV_U32 pexIf, MV_U32 devNum);
#ifndef CONFIG_ALP_A375_ZX_REV
MV_STATUS mvUsb2PhyInit(MV_U32 dev);
#endif
MV_STATUS mvPONPhyInit();

/**************************** globals *****************************/

#ifdef CONFIG_CUSTOMER_BOARD_SUPPORT
/************************* Topology - Customer Boards ****************************/
MV_BOARD_TOPOLOGY_CONFIG boardTopologyConfig[] =
{
/*	board name			Lane 1			Lane 2			Lane3			Sgmii Speed*/
	{"ARMADA_375_CUSTOMER_0",	{SERDES_UNIT_SATA,	SERDES_UNIT_SATA,	SERDES_UNIT_USB3},	MV_SGMII_GEN1},
	{"ARMADA_375_CUSTOMER_1",	{SERDES_UNIT_PEX,	SERDES_UNIT_SATA,	SERDES_UNIT_USB3},	MV_SGMII_GEN1},
};
#else
/************************* Topology - Marvell Boards ****************************/
MV_BOARD_TOPOLOGY_CONFIG boardTopologyConfig[] =
{
/*	board name			Lane 1			Lane 2			Lane3			Sgmii Speed*/
	{"DB_88F6720_BP_ID",		{SERDES_UNIT_SATA,	SERDES_UNIT_SATA,	SERDES_UNIT_USB3},	MV_SGMII_GEN1},
};
#endif

MV_BIN_SERDES_UNIT_INDX boardLaneConfig[MAX_LANE_NUM] = {SERDES_UNIT_UNCONNECTED,
							 SERDES_UNIT_UNCONNECTED,
							 SERDES_UNIT_UNCONNECTED,
							 SERDES_UNIT_UNCONNECTED};

/****************************  function implementation *****************************************/

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
#define mvGetSerdesLaneCfg(serdesLaneNum) boardLaneConfig[serdesLaneNum]

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
	return 4;
}

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
#ifdef CONFIG_ALP_A375_ZX_REV
#define	SERDES_LAN2_OFFS	2
#define	SERDES_LAN3_OFFS	3
#else
#define	SERDES_LAN2_OFFS	3
#define	SERDES_LAN3_OFFS	4
#endif
MV_U32 GetLaneSelectorConfig(void)
{
    MV_U32 tmp,uiReg;

    uiReg = 0x1; /* lane 0 is always PEX */

	switch (mvGetSerdesLaneCfg(1)) {
		default:
		case SERDES_UNIT_PEX:	tmp = 0;	break;
		case SERDES_UNIT_SGMII: tmp = 1;	break;
		case SERDES_UNIT_SATA:	tmp = 2;	break;
	}
	uiReg |= (tmp << 1); /* lane 1  */

	switch (mvGetSerdesLaneCfg(2)) {
		default:
		case SERDES_UNIT_PEX:	tmp = 0;	break;
		case SERDES_UNIT_SGMII: tmp = 1;	break;
		case SERDES_UNIT_SATA:	tmp = 2;	break;
	}
	uiReg |= (tmp << 2); /* lane 2  */

	uiReg |= (((mvGetSerdesLaneCfg(3) == SERDES_UNIT_SGMII) ? 1 : 0)  << SERDES_LAN3_OFFS); /* lane 3  */

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
MV_U32 getSerdesSpeedConfig(MV_U32  boardId, MV_BIN_SERDES_UNIT_INDX serdesLaneCfg)
{

    switch (serdesLaneCfg){
	case SERDES_UNIT_SGMII:
		if(boardTopologyConfig[boardId].sgmiiSpeed == MV_SGMII_NA) {
			DEBUG_INIT_S("Error: SGMII speed was not initialized for board ");
			DEBUG_INIT_S(boardTopologyConfig[boardId].boardName);
			DEBUG_INIT_S("\n");
			return MV_BAD_VALUE;
		}
		return (boardTopologyConfig[boardId].sgmiiSpeed == MV_SGMII_GEN1) ? 0x6 : 0x8 ;/* 0x6-> GEN1, 0x8->GEN2*/
        case SERDES_UNIT_USB3:
        case SERDES_UNIT_SATA:
            return 0x1;
        default:
            return 0;
    }
}

/*******************************************************************************
* resetPhyAndPipe
*
* DESCRIPTION:  This function reset or un-reset the phy and pipe as a result of
*               device revision ID
*
* INPUT:
*       serdesLaneNum
*
* OUTPUT:
*
*
* RETURN:
*
*
*******************************************************************************/
MV_VOID resetPhyAndPipe(MV_U32	serdesLaneNum, MV_BOOL bReset)
{
	MV_U32 uiReg;
	uiReg = MV_REG_READ(COMMON_PHY_CONFIGURATION1_REG(serdesLaneNum));
	if(bReset) {
		uiReg |= PHY_SOFTWARE_RESET_MASK;    /* PHY Software Reset = Reset Mode */
		uiReg |= PHY_RESET_CORE_MASK;    /* PHY Reset Core = 0x1 */
		uiReg &= ~(PHY_CORE_RST_MASK);     /* PHY Core RSTn = Reset */
		uiReg &= ~(PHY_POWER_ON_RESET_MASK);   /* PHY Power On Reset = Reset */
	} else {
		uiReg &= ~(PHY_SOFTWARE_RESET_MASK);    /* PHY Software Reset = Normal Mode */
		uiReg &= ~(PHY_RESET_CORE_MASK);    /* PHY Reset Core = 0x0 */
		uiReg |= PHY_CORE_RST_MASK;     /* PHY Core RSTn = Normal */
		uiReg |= PHY_POWER_ON_RESET_MASK;   /* PHY Power On Reset = Normal */
	}
	MV_REG_WRITE(COMMON_PHY_CONFIGURATION1_REG(serdesLaneNum),uiReg);
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
	MV_U32  boardId = mvBoardIdIndexGet(mvBoardIdGet());
    maxSerdesLanes = mvCtrlSerdesMaxLanesGet();

    if (maxSerdesLanes == 0)
        return MV_OK;

	/*Set MPP1 for twsi access */
	uiReg = (MV_REG_READ(MPP_CONTROL_REG(1))  & 0x00FFFFFF) | 0x22000000;
	MV_REG_WRITE(MPP_CONTROL_REG(1), uiReg);

	/* TWSI init */
	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(0, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);


	mvUartInit();

	/* update board configuration (serdes lane topology and speed), if needed */
	mvBoardUpdateBoardTopologyConfig(boardId);

	/* Initialize board configuration database */
	boardLaneConfig[0] = SERDES_UNIT_PEX;		/* SerDes 0 is alwyas PCIe0*/
	boardLaneConfig[1] = boardTopologyConfig[boardId].serdesTopology.lane1;
	boardLaneConfig[2] = boardTopologyConfig[boardId].serdesTopology.lane2;
	boardLaneConfig[3] = boardTopologyConfig[boardId].serdesTopology.lane3;

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
    /* STEP - 1.5 Power Down PLL, RX, TX all phys */
    /*------------------------------------------*/

    for (serdesLaneNum = 0; serdesLaneNum < maxSerdesLanes; serdesLaneNum++)
    {
      uiReg=MV_REG_READ(COMMON_PHY_CONFIGURATION1_REG(serdesLaneNum));
	  uiReg &= ~PIN_TX_IDLE_MASK;
      uiReg &= ~(PHY_POWER_UP_PLL_MASK | PHY_POWER_UP_RX_MASK | PHY_POWER_UP_TX_MASK);
      MV_REG_WRITE(COMMON_PHY_CONFIGURATION1_REG(serdesLaneNum),uiReg);
    }

    mvOsUDelay(10000);

    /*--------------------------------------------------------*/
    /* STEP - 2 Reset PHY and PIPE (Zx: Un-reset, Ax: Reset)*/
    /*--------------------------------------------------------*/
	for (serdesLaneNum = 0; serdesLaneNum < maxSerdesLanes; serdesLaneNum++)
	{
#ifndef CONFIG_ALP_A375_ZX_REV
			resetPhyAndPipe(serdesLaneNum, MV_TRUE);
#else
			resetPhyAndPipe(serdesLaneNum, MV_FALSE);
#endif
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
		uiReg = MV_REG_READ(COMMON_PHY_CONFIGURATION1_REG(serdesLaneNum));
		switch(serdesLaneCfg){
			case SERDES_UNIT_USB3:
#ifndef CONFIG_ALP_A375_ZX_REV
				A375_A0_COMMON_PHY_CONFIG(uiReg);
#endif
				uiReg |= PHY_MODE_MASK;	/* PHY Mode = USB */
				uiReg |= PIPE_SELECT_MASK ;	/* Select USB3_PEX */
				break;
			case SERDES_UNIT_PEX:
				uiReg |= PIPE_SELECT_MASK ;	/* Select USB3_PEX */
#ifndef CONFIG_ALP_A375_ZX_REV
					uiReg &= ~(PHY_MODE_MASK);	/* PHY Mode = PEX */
					A375_A0_COMMON_PHY_CONFIG(uiReg);
#endif
				break;
			case SERDES_UNIT_SGMII:
			case SERDES_UNIT_SATA:
#ifndef CONFIG_ALP_A375_ZX_REV
					A375_A0_COMMON_PHY_CONFIG(uiReg);
#endif
				uiReg &= ~(PIPE_SELECT_MASK);	/* Select SATA_SGMII */
				uiReg |= POWER_UP_IVREF_MASK;  /* Power UP IVREF = Power Up */
				break;
			case SERDES_UNIT_UNCONNECTED:
			default:
			break;
		}

		/* Serdes speed config */
		tmp = getSerdesSpeedConfig(boardId, serdesLaneCfg);
		uiReg &= ~(GEN_RX_MASK); /* SERDES RX Speed config */
		uiReg |= tmp<<GEN_RX_OFFS;
		uiReg &= ~(GEN_TX_MASK); /* SERDES TX Speed config */
		uiReg |= tmp<<GEN_TX_OFFS;
		MV_REG_WRITE(COMMON_PHY_CONFIGURATION1_REG(serdesLaneNum),uiReg);
	}

#ifndef CONFIG_ALP_A375_ZX_REV
	/*------------------------------------------*/
	/*	STEP - 3.5 Unreset PHY and PIPE(only Ax)*/
	/*------------------------------------------*/
		for (serdesLaneNum = 0; serdesLaneNum < maxSerdesLanes; serdesLaneNum++) {
			resetPhyAndPipe(serdesLaneNum, MV_FALSE);
		}
#endif

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
#ifndef CONFIG_ALP_A375_ZX_REV
					MV_REG_WRITE(MISCELLANEOUS_CONTROL0_REG(serdesLaneNum),0x6017); /* REFCLK SEL =0x0 (100Mhz) */
					MV_REG_WRITE(INTERFACE_REG1_REG(serdesLaneNum),0x1400); /* PHY_Gen_Max = 5G */
					MV_REG_WRITE(DIGITAL_LOOPBACK_ENABLE_REG(serdesLaneNum),0x400); /* SEL_Bits = 20-Bit */
					A375_A0_RESET_DFE_SEQUENCE(serdesLaneNum);
#else
					MV_REG_WRITE(KVCO_CALOBRATION_CONTROL_REG(serdesLaneNum),0x40); /* use_max_pll_rate=0x0, ext_force_cal_done=0x0 */
#endif
                MV_REG_WRITE(RESET_AND_CLOCK_CONTROL_REG(serdesLaneNum),0x24); /* Release soft_reset */
            break;
            case SERDES_UNIT_USB3:
                MV_REG_WRITE(RESET_AND_CLOCK_CONTROL_REG(serdesLaneNum),0x21); /* Enable soft_reset*/
				MV_REG_WRITE(POWER_AND_PLL_CONTROL_REG(serdesLaneNum),0xFCA0); /* PHY Mode = USB3 */
#ifndef CONFIG_ALP_A375_ZX_REV
					MV_REG_WRITE(LANE_CONFIGURATION_4_REG(serdesLaneNum),0x13); /* Ref_Clk =100Mhz */
					MV_REG_WRITE(MISCELLANEOUS_CONTROL0_REG(serdesLaneNum),0x6017); /* REFCLK SEL =0x0 (100Mhz) */
					MV_REG_WRITE(INTERFACE_REG1_REG(serdesLaneNum),0x1400); /* PHY_Gen_Max = 5G */
					MV_REG_WRITE(DIGITAL_LOOPBACK_ENABLE_REG(serdesLaneNum),0x400); /* SEL_Bits = 20-Bit */
					A375_A0_RESET_DFE_SEQUENCE(serdesLaneNum);
#else
					MV_REG_WRITE(KVCO_CALOBRATION_CONTROL_REG(serdesLaneNum),0x40); /* use_max_pll_rate=0x0, ext_force_cal_done=0x0 */
					MV_REG_WRITE(GENERETION_2_SETTINGS_1_REG(serdesLaneNum),0x149); /* Mulitiple frequency setup */
#endif
                MV_REG_WRITE(RESET_AND_CLOCK_CONTROL_REG(serdesLaneNum),0x20); /* Release soft_reset */
                break;
            case SERDES_UNIT_SATA:
                MV_REG_WRITE(POWER_AND_PLL_CONTROL_REG(serdesLaneNum),0xFC01); /* PHY Mode = SATA */
				MV_REG_WRITE(MISCELLANEOUS_CONTROL0_REG(serdesLaneNum),0x6417); /* REFCLK SEL =0x1 (25Mhz) */
#ifndef CONFIG_ALP_A375_ZX_REV
					MV_REG_WRITE(INTERFACE_REG1_REG(serdesLaneNum),0x1400); /* PHY_Gen_Max = 5G */
					MV_REG_WRITE(DIGITAL_LOOPBACK_ENABLE_REG(serdesLaneNum),0x400); /* SEL_Bits = 20-Bit */
					MV_REG_WRITE(DIGITAL_RESERVED0_REG(serdesLaneNum),0xE); /* Reg_sq_de_glitch_en */
					A375_A0_RESET_DFE_SEQUENCE(serdesLaneNum);
#else
					MV_REG_WRITE(RESERVED_46_REG(serdesLaneNum),0xFF00);
#endif
                break;
            case SERDES_UNIT_SGMII:
                MV_REG_WRITE(POWER_AND_PLL_CONTROL_REG(serdesLaneNum),0xFC81); /* PHY Mode = SGMII */ /*moti need to change offset*/
                MV_REG_WRITE(DIGITAL_LOOPBACK_ENABLE_REG(serdesLaneNum),0x0); /* SEL_BITS = 0x0 (10-bits mode) */
				MV_REG_WRITE(MISCELLANEOUS_CONTROL0_REG(serdesLaneNum),0x6417); /* REFCLK SEL =0x1 (25Mhz) */
#ifndef CONFIG_ALP_A375_ZX_REV
					MV_REG_WRITE(DIGITAL_RESERVED0_REG(serdesLaneNum),0xE); /* Reg_sq_de_glitch_en */
					A375_A0_RESET_DFE_SEQUENCE(serdesLaneNum);
#else
                MV_REG_WRITE(RESERVED_46_REG(serdesLaneNum),0xFF00); /* Enable soft_reset*/
#endif
                MV_REG_WRITE(PHY_ISOLATION_MODE_CONTROL_REG(serdesLaneNum),0x166); /* Set PHY_GEN_TX/RX to 1.25Gbps */
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

#ifndef CONFIG_ALP_A375_ZX_REV
	mvOsUDelay(5000);

    /*--------------------------------------------------------------------*/
    /* STEP - 4.6 (Only SGMII/SATA): WAIT for PHY Power up sequence to finish */
    /*--------------------------------------------------------------------*/
	for (serdesLaneNum = 0; serdesLaneNum < maxSerdesLanes; serdesLaneNum++) {
        serdesLaneCfg = mvGetSerdesLaneCfg(serdesLaneNum);
        if(serdesLaneCfg >= SERDES_LAST_UNIT){
            return MV_ERROR;
        }
        switch(serdesLaneCfg){
            case SERDES_UNIT_SATA:
			case SERDES_UNIT_SGMII:
                uiReg = MV_REG_READ(COMMON_PHY_STATUS1_REG(serdesLaneNum));
				if ((uiReg & 0x6) != 0x6) {
					DEBUG_INIT_S("Phy Power up did't finished\n");
					return MV_ERROR;
				}
	    case SERDES_UNIT_UNCONNECTED:
	    default:
			break;
        }
    }
#endif

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

    for (pexIf = 0; pexIf < 2; pexIf++)  // only pexIf 0 on
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
					if ((mvPexConfigRead(pexIf, first_busno, 0, 0, addr + 0xC) & 0xF) >= 0x2) {
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

	/* Step 8: Configure PON on lane3 */
	mvPONPhyInit();

#ifndef CONFIG_ALP_A375_ZX_REV
	mvUsb2PhyInit(0);
	mvUsb2PhyInit(1);
#endif

	DEBUG_INIT_S(ENDED_OK);
	DEBUG_INIT_S("\n");

    return MV_OK;
}

#ifndef CONFIG_ALP_A375_ZX_REV
/* USB Phy init specific for 40nm LP (88F6660) */
MV_STATUS mvUsb2PhyInit(MV_U32 dev)
{
	MV_U32 regVal;

	/*Set USB PHY config selector to USB 2.0*/
	regVal = MV_REG_READ(USB_CLUSTER_CONTROL_REG);
	regVal &= ~0x1;
	MV_REG_WRITE(USB_CLUSTER_CONTROL_REG, regVal);

	/* Set the PLL clocks to 450 MHz. Our ref clock is 25 Mhz so to
	 * achieve 480 MHz we set divider = 5 multiplier = 96 */
	regVal = MV_REG_READ(MV_USB2_PHY_CHANNEL_REG(dev, 1));
	regVal = (regVal & (~0x1FF)) | (96);
	regVal = (regVal & (~0x1E00)) | (5 << 9);
	MV_REG_WRITE(MV_USB2_PHY_CHANNEL_REG(dev, 1), regVal);

	/* Turn on the PLL and wait 200 usec */
	regVal = MV_REG_READ(MV_USB2_PHY_CHANNEL_REG(dev, 2));
	regVal |= BIT13;
	MV_REG_WRITE(MV_USB2_PHY_CHANNEL_REG(dev, 2), regVal);
	mvOsUDelay(2);

	/* Enable the analog part of the PHY */
	regVal = MV_REG_READ(MV_USB2_PHY_CHANNEL_REG(dev, 13));
	regVal |= BIT14;
	MV_REG_WRITE(MV_USB2_PHY_CHANNEL_REG(dev, 13), regVal);
	mvOsUDelay(2);

	/* Turn on the VCO calibration */
	regVal = MV_REG_READ(MV_USB2_PHY_CHANNEL_REG(dev, 2));
	regVal |= BIT2;
	MV_REG_WRITE(MV_USB2_PHY_CHANNEL_REG(dev, 2), regVal);
	mvOsUDelay(10);

	/* Perform Impedance calibration for 40 usec */
	regVal = MV_REG_READ(MV_USB2_PHY_CHANNEL_REG(dev, 4));
	regVal |= BIT13;
	MV_REG_WRITE(MV_USB2_PHY_CHANNEL_REG(dev, 4), regVal);
	mvOsUDelay(1);

	regVal = MV_REG_READ(MV_USB2_PHY_CHANNEL_REG(dev, 4));
	regVal &= ~BIT13;
	MV_REG_WRITE(MV_USB2_PHY_CHANNEL_REG(dev, 4), regVal);
	mvOsUDelay(400);

	/* Check if the PHY is ready */
	regVal = MV_REG_READ(MV_USB2_PHY_CHANNEL_REG(dev, 2));
	if ((regVal & BIT15) == 0) {
		DEBUG_INIT_S("Error: USB2 UTMI PHY not ready\n");
		return MV_NOT_READY;
	}

	DEBUG_INIT_S("USB2 UTMI PHY initialized succesfully\n");

	return MV_OK;
}
#endif

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

/*****************************************************************:**************
* mvBoardUpdateSerdesTopology
*
* DESCRIPTION: Updates the topology structure of a specific board according to dynamic configuration
*
* INPUT:  None
*
* OUTPUT: None
*
* RETURN: NONE
*
*******************************************************************************/
MV_STATUS mvBoardUpdateBoardTopologyConfig(MV_U32 boardId)
{
	/* this routine must be implemented, in order to use dynamic Serdes /SGMII settings configuration.
		1. scan requested configuration 2. update boardTopologyConfig[] according to config */
	return MV_OK;
}

MV_STATUS mvPONPhyInit()
{
	/*TBD - make the sequence clear*/
	MV_REG_WRITE(0x184f4, 0x53000238);
	MV_REG_WRITE(0x184f8, 0x1);
	MV_REG_WRITE(0x184f4, 0x53000210);
	MV_REG_WRITE(0x184f4, 0x53001210);
	MV_REG_WRITE(0x18754, 0x8);
	MV_REG_WRITE(0x18754, 0xA);
	MV_REG_WRITE(0x18754, 0xE);
	MV_REG_WRITE(0x32004, 0xf400);
	MV_REG_WRITE(0x32144, 0x104);
	MV_REG_WRITE(0x320f4, 0x400);
	MV_REG_WRITE(0x3208c, 0x0000);
	MV_REG_WRITE(0x184f4, 0x53001217);
	MV_REG_WRITE(0x184f4, 0x52001247);
	MV_REG_WRITE(0x184f4, 0x52001207);
	MV_REG_WRITE(0x32098, 0x0);

	return MV_OK;
}



