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
//#define SERDES_LOCAL_DEBUG
//#define MV_DEBUG_INIT_FULL
#include "mv_os.h"
#include "config_marvell.h"  	/* Required to identify SOC and Board */
#include "mvUart.h"
#include "util.h"

#include "mvHighSpeedEnvSpec.h"
#include "mvBHboardEnvSpec.h"

#define LINK_WAIT_CNTR	10000
#define LINK_WAIT_SLEEP	100
#ifdef SERDES_LOCAL_DEBUG
#define MV_BH_REG_WRITE(r,v) 	{	\
	DEBUG_INIT_S("Write Reg: 0x");  \
	DEBUG_INIT_D((r), 8);           \
	DEBUG_INIT_S("= ");             \
	DEBUG_INIT_D((v), 8);           \
	DEBUG_INIT_S("\n");             \
	MV_REG_WRITE(r,v);               \
}
#define MV_BH_REG_READ(r,v)	{			\
			v = MV_REG_READ(r);		\
			DEBUG_INIT_S("Read  Reg: 0x");  \
			DEBUG_INIT_D((r), 8);           \
			DEBUG_INIT_S("= ");             \
			DEBUG_INIT_D(v, 8);           \
			DEBUG_INIT_S("\n");             \
}
#else
#define MV_BH_REG_WRITE(r,v) 	MV_REG_WRITE(r,v)
#define MV_BH_REG_READ(r,v)	v = MV_REG_READ(r)
#endif

static const MV_U8 serdesCfg[MV_SERDES_MAX_LANES][8] = SERDES_CFG;

MV_U32	pexSpeed = PEX_SPEED_GEN2;


/*************************** Functions declarations ***************************/
/* Local functions */
MV_VOID	mvBhSerdesInit(MV_VOID);
MV_VOID	mvBhPexTestLink(MV_VOID);
MV_U32	mvBoardTclkGet(MV_VOID);
MV_U32	mvBoardIdGet(MV_VOID);
MV_U16	mvCtrlModelGet(MV_VOID);
MV_STATUS mvBoardTwsiGet(MV_U8 devAddr, MV_U8 regOffset, MV_BOOL moreThen256, MV_U8 *pData);
/*************************** Functions declarations ***************************/

MV_STATUS mvCtrlHighSpeedSerdesPhyConfig(MV_VOID)
{
	MV_U32 boardId;

	MV_TWSI_ADDR slave;

	mvTwsiInit(0, CONFIG_SYS_I2C_SPEED, CONFIG_SYS_TCLK, &slave, 0);

	boardId = mvBoardIdGet();


	switch (boardId) {
	case RD_NAS_68XX_ID:
/*
Lane	Lane options		Lane settings
L0	PCIe0			PCIe0  1
L1	PCIe0/SATA0		SATA0  3
L2	PCIe1/SATA1		SATA1  3
L3	SATA3			SATA3  3
L4	SGMII(SFP+)/USB3.0	USB3.0 4
L5	SATA2			SATA2  2
*/
	//print "STEP-0 Configure the MUX select for PCIe-1 on Serdes Lane 2"
	DEBUG_INIT_S("Initializing RD NAS: \n");
	DEBUG_INIT_S("0- PCIe0, 1- SATA0, 2- SATA1, 3- SATA3, 4- USB3.0, 5- SATA2\n");
	MV_BH_REG_WRITE(COMMON_PHYS_SELECTORS_REG, 0x146D9);

		break;
	case RD_AP_68XX_ID:
/*
L0	PCIe0			NA      0
L1	PCIe0/SATA0		PCIe0	1
L2	PCIe1/SATA1		PCIe1	1
L3	SATA3			SATA3	3
L4	SGMII(SFP+)/USB3.0	SFP+	3
L5	SATA2			SATA2	2

*/
	DEBUG_INIT_S("Initializing RD AP: \n");
	DEBUG_INIT_S("0- NA, 1- PCIe(0), 2- PCIe(1), 3- SATA3, 4- USB3.0, 5- SATA2\n");
	MV_BH_REG_WRITE(COMMON_PHYS_SELECTORS_REG, 0x14648);
		break;
	default:
	DEBUG_INIT_S("Warning: BOARD ID failed init as DB!!!!!!!! \n");

	case DB_68XX_ID:
	DEBUG_INIT_S("Initializing DB-A38x board: \n");
	DEBUG_INIT_S("0-SATA, 1- PCIe(0), 2- PCIe(1), 3- SATA(3), 4-USB3(0), 5 USB3(1)\n");
	MV_BH_REG_WRITE(COMMON_PHYS_SELECTORS_REG,  0x0002464A); //  Bits [17:15]=0x2.
		break;
	}
	mvBhSerdesInit();
	mvBhPexTestLink();

	return MV_OK;
}

/*******************************************************************************
* mvBhSerdesInit - init serdes lans
*
* DESCRIPTION: read serdes selector configuration and initialize serdes lane
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*
*******************************************************************************/
MV_VOID	mvBhSerdesInit(MV_VOID)
{
	MV_U32 i, port, unit;
	MV_U32 temp;
	MV_U32 socCtrlreg;
	MV_U32 commPhyConfigReg, comPhyCfg, serdesNum, serdesCongigField, maxSerdesLane;


	maxSerdesLane = MV_SERDES_MAX_LANES;
	if (MV_6810_DEV_ID == mvCtrlModelGet())
		maxSerdesLane = MV_SERDES_MAX_LANES_6810;


	MV_BH_REG_READ(COMMON_PHYS_SELECTORS_REG, commPhyConfigReg);
	MV_BH_REG_READ(SOC_CTRL_REG, socCtrlreg);

	for (serdesNum = 0; serdesNum < maxSerdesLane; serdesNum++) {
		serdesCongigField = (commPhyConfigReg & COMPHY_SELECT_MASK(serdesNum)) >> COMPHY_SELECT_OFFS(serdesNum);
		comPhyCfg = serdesCfg[serdesNum][serdesCongigField];

		DEBUG_INIT_S("SERDES ");
		DEBUG_INIT_D(serdesNum,1);
		DEBUG_INIT_S(" configure as ");

		port = comPhyCfg & SERDES_PORT_MASK;
		switch (comPhyCfg & SERDES_UNIT_MASK ) {
		case SERDES_UNIT_PEX:
			DEBUG_INIT_S("PEX  ");
			DEBUG_INIT_D(port,1);
			DEBUG_INIT_S("\n");

			MV_BH_REG_READ(PEX_LINK_CAPABILITIES_REG(port), temp );
			temp &= ~(0x3FF); /*  bit 4:9 and bit 0:3    */
			if ((port == PEX0_IF) && (commPhyConfigReg & PCIE0_X4_EN_MASK)) {
				socCtrlreg &= ~(PEX_QUADX4_EN);
				temp |= (MAX_LNK_WDTH_X4 << 4);
			}
			else {
				socCtrlreg |= (PEX_QUADX4_EN);
				temp |= (MAX_LNK_WDTH_X1 << 4);
			}
			/* Pcie CLKOUT Enable config the desire value per required chip configuration  */
			socCtrlreg |= (PEX0_CLK_EN | PEX1_CLK_EN | PEX2_CLK_EN | PEX3_CLK_EN);
			if (pexSpeed == PEX_SPEED_GEN2)
				temp |= MAX_LNK_SPEED_5GBS;
			else
				temp |= MAX_LNK_SPEED_2_5GBS;

			MV_BH_REG_WRITE(PEX_LINK_CAPABILITIES_REG(port), temp);
			temp = socCtrlreg & ~(PEX0_EN | PEX1_EN | PEX2_EN | PEX3_EN);
			MV_BH_REG_WRITE(SOC_CTRL_REG, temp);

			MV_BH_REG_READ(PEX_LINK_CTRL_STATUS_REG(port), temp );
			temp |= COMMON_REF_CLK;
			MV_BH_REG_WRITE(PEX_LINK_CTRL_STATUS_REG(port), temp);

/********************* SERDES INIT **************************/
			MV_BH_REG_WRITE(COMMON_PHY_CONFIGURATION1_REG(serdesNum), 0x04471804);
			MV_BH_REG_WRITE(COMMON_PHY_CONFIGURATION2_REG(serdesNum), 0x00000058);
			MV_BH_REG_WRITE(COMMON_PHY_CONFIGURATION4_REG(serdesNum), 0x0000000d);
			MV_BH_REG_WRITE(COMMON_PHY_CONFIGURATION1_REG(serdesNum), 0x04476004);
			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum,0x1c1), 0x25); 	/*  offset 1c1 */
			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum,1), 0x0000FC60); 	/* reference clock select and phy mode select */
			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum,0x25), 0x000017FF); 	/* [11:10]-Maximal PHY Generation Setting */
			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum,0x4F), 0xA08A); 	/*  offset 93c*/
			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum,0x52), 0xE409); 	/*  offset 948 */
			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum,0x52), 0xE008); 	/*  offset 948 */
			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum,0x1c1), 0x24); 	/*  offset 1c1 - soft reset*/
			mvOsDelay(5);		/*wait 5ms*/

/*****************************************************************************/
			MV_BH_REG_READ(PEX_CAPABILITIES_REG(port), temp);
			temp &= ~PEX_CFG_MODE_MASK;
			temp |= PEX_CFG_MODE_RC_MODE;  /*  RC/EP mode Rc is decided  */
			MV_BH_REG_WRITE(PEX_CAPABILITIES_REG(port), temp);
#ifdef PEX_END_POINT
			MV_BH_REG_READ(PEX_DBG_CTRL_REG(port), temp);
			temp &= ~(BIT19 | BIT16);
			MV_BH_REG_WRITE(PEX_DBG_CTRL_REG(port), temp);
#endif
			socCtrlreg |=  PEXx_EN(port);
			break;
		case SERDES_UNIT_SATA:
			DEBUG_INIT_S("SATA  ");
			DEBUG_INIT_D(port,1);
			DEBUG_INIT_S("\n");
			unit = port/2;
			MV_BH_REG_WRITE((MV_SATA3_UNIT_OFFSET(port) + 0xA0), 0);
			temp = 0xC40000 | ((port & 1) ? (1 << 14) : (1 << 6));
			MV_BH_REG_WRITE((MV_SATA3_UNIT_OFFSET(port) + 0xA4), temp);
			MV_BH_REG_WRITE(COMMON_PHY_CONFIGURATION1_REG(serdesNum), 0xE1802);
			MV_BH_REG_WRITE(COMMON_PHY_CONFIGURATION1_REG(serdesNum), 0x88E1802);
			MV_BH_REG_WRITE(COMMON_PHY_CONFIGURATION1_REG(serdesNum), 0x88E6002);
			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum,01), 0xFC01); 	/*  offset 4*/
			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum,0x25), 0x00001BFF); 	/* [11:10]-Maximal PHY Generation Setting Configure only in SATA mode */
			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum,0x23), 0x4);
			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum,0x4F), 0xA44A);

			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum, 0x52), 0xE409); 	/*   */
			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum, 0x52), 0xE008); 	/*   */
			MV_BH_REG_WRITE(COMMON_PHY_CONFIGURATION1_REG(serdesNum), 0x8876002);
			MV_BH_REG_WRITE((MV_SATA3_UNIT_OFFSET(unit) + 0xa0), 0); 	/*   */
			MV_BH_REG_WRITE((MV_SATA3_UNIT_OFFSET(unit) + 0xA4), 0xC40000);
			MV_BH_REG_WRITE((MV_SATA3_UNIT_OFFSET(unit) + 0xa0), 4); 	/*   */
			MV_BH_REG_WRITE((MV_SATA3_UNIT_OFFSET(unit) + 0xA4), 0x0);

			mvOsDelay(5);		/*wait 5ms*/
			for (i=0; i<LINK_WAIT_CNTR; i++) {
				temp = MV_REG_READ(COMMON_PHY_STATUS1_REG(serdesNum));
				if ((temp & PHY_PLL_READY) == PHY_PLL_READY)
					break;
				mvOsUDelay(LINK_WAIT_SLEEP);

			}
			MV_BH_REG_READ(COMMON_PHY_STATUS1_REG(serdesNum), temp );
			if ((temp & PHY_PLL_READY) == PHY_PLL_READY)
				DEBUG_INIT_S(" SATA PLL ready. \n");
			else
				DEBUG_INIT_S(" SATA PLL not ready.\n");

			break;
		case SERDES_UNIT_GBE:
			DEBUG_INIT_S("SGMII  ");
			DEBUG_INIT_D(port,1);
			DEBUG_INIT_S("\n");

			MV_BH_REG_WRITE(COMMON_PHY_CONFIGURATION1_REG(serdesNum), 0xE1802);
			MV_BH_REG_WRITE(COMMON_PHY_CONFIGURATION1_REG(serdesNum), 0x198E1802);
			MV_BH_REG_WRITE(COMMON_PHY_CONFIGURATION1_REG(serdesNum), 0x198E6002);
			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum,01), 0xFC81); 	/*  offset 4*/
			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum,0x26), 0x00000166);
			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum,23), 0x2);
			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum,0x4F), 0xA40A);
			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum,0x50), 0x1800);

			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum, 0x52), 0xE409); 	/*   */
			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum, 0x52), 0xE008); 	/*   */
			MV_BH_REG_WRITE(COMMON_PHY_CONFIGURATION1_REG(serdesNum), 0x19876002);

			mvOsDelay(5);		/*wait 5ms*/
			for (i=0; i<LINK_WAIT_CNTR; i++) {
				temp = MV_REG_READ(COMMON_PHY_STATUS1_REG(serdesNum));
				if ((temp & PHY_PLL_READY) == PHY_PLL_READY)
					break;
				mvOsUDelay(LINK_WAIT_SLEEP);
			}
			MV_BH_REG_READ(COMMON_PHY_STATUS1_REG(serdesNum), temp );
			if ((temp & PHY_PLL_READY) == PHY_PLL_READY)
				DEBUG_INIT_S(" SGMII PLL ready. \n");
			else
				DEBUG_INIT_S(" SGMII PLL not ready.\n");

			break;
		case SERDES_UNIT_USB:
		case SERDES_UNIT_USB_H:
			DEBUG_INIT_S("USB ");
			DEBUG_INIT_D(port,1);
			DEBUG_INIT_S("\n");
			MV_BH_REG_WRITE(COMMON_PHY_CONFIGURATION1_REG(serdesNum), 0x4479804);
			MV_BH_REG_WRITE(COMMON_PHY_CONFIGURATION2_REG(serdesNum), 0x00000058);
			MV_BH_REG_WRITE(COMMON_PHY_CONFIGURATION4_REG(serdesNum), 0x0000000d);
			MV_BH_REG_WRITE(COMMON_PHY_CONFIGURATION1_REG(serdesNum), 0x0447e004);
			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum,0x1c1), 0x21); 	/*  offset 1c1 */
			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum,1), 0x0000FCA0); 	/* reference clock select and phy mode select */
			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum,0x25), 0x000017FF); 	/* [11:10]-Maximal PHY Generation Setting */
			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum,0x4F), 0xA0CA); 	/*  offset 93c*/
			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum,0x52), 0xE409); 	/*  offset 948 */
			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum,0x52), 0xE008); 	/*  offset 948 */
			MV_BH_REG_WRITE(COMPHY_H_PIPE3_28LP(serdesNum,0x1c1), 0x20); 	/*  offset 1c1 - soft reset*/
			mvOsDelay(5);		/*wait 5ms*/

			break;
		case SERDES_UNIT_NA:
			DEBUG_INIT_S("NA\n ");
		}
	}

	MV_BH_REG_WRITE(SOC_CTRL_REG, socCtrlreg);

	mvOsDelay(150);		/*wait 150ms*/
}
/*******************************************************************************
* mvBhPexTestLink - test pex port if link up
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*
*******************************************************************************/
MV_VOID	mvBhPexTestLink(MV_VOID)
{
	MV_U32 port, temp;
	MV_U32 socCtrlreg;
	MV_U32 commPhyConfigReg, comPhyCfg, serdesNum, serdesCongigField, maxSerdesLane;

	maxSerdesLane = MV_SERDES_MAX_LANES;
	if (MV_6810_DEV_ID == mvCtrlModelGet())
		maxSerdesLane = MV_SERDES_MAX_LANES_6810;

	MV_BH_REG_READ(COMMON_PHYS_SELECTORS_REG, commPhyConfigReg );
	MV_BH_REG_READ(SOC_CTRL_REG, socCtrlreg );
	for (serdesNum = 0; serdesNum < maxSerdesLane; serdesNum++) {
		serdesCongigField = (commPhyConfigReg & COMPHY_SELECT_MASK(serdesNum)) >> COMPHY_SELECT_OFFS(serdesNum);
		comPhyCfg = serdesCfg[serdesNum][serdesCongigField];
		port = comPhyCfg & SERDES_PORT_MASK ;
		if ((comPhyCfg & SERDES_UNIT_MASK ) != SERDES_UNIT_PEX)
			continue;
		if (0 == (socCtrlreg & (1 << port))) {
			DEBUG_INIT_S("Disconnect PCIe-");
		}
		else {
			MV_BH_REG_READ(PEX_DBG_STATUS_REG(port), temp);
			if ((temp & 0xFF) == 0x7E)
				DEBUG_INIT_S("Link up PCIe-");
			else
				DEBUG_INIT_S("Link down PCIe");
		}
		DEBUG_INIT_D(port,1);
		DEBUG_INIT_S("\n");
	}
}
/***************************************************************************/
MV_U32 mvBoardTclkGet(MV_VOID)
{
	MV_U32 tclk;
	tclk = (MV_REG_READ(MPP_SAMPLE_AT_RESET));
	tclk = ((tclk & (CORE_CLK_FREQ_MASK)) >> CORE_CLK_FREQ_OFFS);
	switch (tclk) {
	case 0:
		return MV_BOARD_TCLK_250MHZ;
	case 1:
		return MV_BOARD_TCLK_200MHZ;
	default:
		return MV_BOARD_TCLK_250MHZ;
	}
}
MV_U32 mvBoardIdGet(MV_VOID)
{
	MV_U8 gBoardId;

	if (mvBoardTwsiGet(BOARD_ID_GET_ADDR, 0, MV_TRUE, &gBoardId) != MV_OK) {
		DEBUG_INIT_S("Error: Read from TWSI failed\n");
		DEBUG_INIT_S("Set default board ID to DB-88F6820-BP");
		gBoardId = DB_68XX_ID;
	}
	gBoardId &= MV_BOARD_ID_MASK;

	if (gBoardId >= MV_MAX_BOARD_ID) {
		DEBUG_INIT_S("Error: read wrong board ID = ");
		DEBUG_INIT_D(gBoardId,8);
		DEBUG_INIT_S("\n");
		return MV_INVALID_BOARD_ID;
	}
	return gBoardId;
}
MV_STATUS mvBoardTwsiGet(MV_U8 devAddr, MV_U8 regOffset, MV_BOOL moreThen256, MV_U8 *pData)
{
        MV_TWSI_SLAVE twsiSlave;

        twsiSlave.slaveAddr.address = devAddr;
        twsiSlave.slaveAddr.type = ADDR7_BIT;
        twsiSlave.validOffset = MV_TRUE;
        twsiSlave.offset = regOffset;	        /* Use offset as command */
        twsiSlave.moreThen256 = moreThen256;
        if (MV_OK != mvTwsiRead(0, &twsiSlave, pData, 1)) {
                return MV_ERROR;
        }
        return MV_OK;
}
/*******************************************************************************
* mvCtrlModelGet - Get Marvell controller device model (Id)
*
* DESCRIPTION:
*       This function returns 16bit describing the device model (ID) as defined
*       in Vendor ID configuration register
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       16bit desscribing Marvell controller ID
*
*******************************************************************************/
MV_U16 mvCtrlModelGet(MV_VOID)
{
	MV_U32	ctrlId = MV_REG_READ(DEV_ID_REG);
	ctrlId = (ctrlId & (DEVICE_ID_MASK)) >> DEVICE_ID_OFFS;
	if (ctrlId == 0x6820)
		return MV_6820_DEV_ID;
	if (ctrlId == 0x6810)
		return MV_6810_DEV_ID;

	return MV_INVALID_DEV_ID;
}
