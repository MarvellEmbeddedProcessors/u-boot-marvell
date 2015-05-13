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

#include "mvOs.h"
#include "mvSwitch.h"
#include "eth-phy/mvEthPhy.h"
#include "mvSwitchRegs.h"
#include "mvCtrlEnvLib.h"
#include "mvBoardEnvLib.h"

static void switchVlanInit(MV_U32 ethPortNum,
						   MV_U32 switchCpuPort,
					   MV_U32 switchMaxPortsNum,
					   MV_U32 switchPortsOffset,
					   MV_U32 switchEnabledPortsMask);
static MV_U16 mvEthSwitchGetDeviceIDInMultiorManualMode(void);
static MV_U16 mvEthSwitchGetDeviceIDInAutoScanMode(MV_BOOL *highSmiDevAddr);

void mvEthSwitchRegWrite(MV_U32 ethPortNum, MV_U32 phyAddr,
                                 MV_U32 regOffs, MV_U16 data);

void mvEthSwitchRegRead(MV_U32 ethPortNum, MV_U32 phyAddr,
                             MV_U32 regOffs, MV_U16 *data);

int switchMultiChipMode = 0xdeadbeef;

static MV_32 switchDeviceID = -1;	/*-1 : initialized, 0 : no switch, other values are switch device ID*/
static MV_U32 switchAccessMode = SMI_MANUAL_MODE;
#ifdef MV_SWITCH_SMI_ACCESS
static MV_32 switchPhyAddr = -1;
#endif

/*******************************************************************************
* mvEthSwitchGetDeviceIDInMultiAddMode -
*
* DESCRIPTION:
*	get switch device ID if switch is in SMI muti address access mode
*
* INPUT:
*	None
*
* OUTPUT:
*	None
*
* RETURN:   switch device ID
*
*******************************************************************************/
static MV_U16 mvEthSwitchGetDeviceIDInMultiorManualMode(void)
{
	MV_U16 data;

	/*read switch device ID, first try the case that port register offset is 8*/
	data = 0;
	mvEthSwitchRegRead(0, PORT_REGS_START_ADDR, QD_REG_SWITCH_ID, &data);

	switch (data & 0xFF00) {
	case 0x0200:
	case 0x0300:
	case 0x0500:
	case 0x0600:
	case 0x1500:
	case 0xF500:
	case 0xF900:
	case 0x0700:	    /* Spinnaker */
	case 0x2200:	    /* Spinnaker */
	case 0x2500:	    /* Spinnaker */
		return data;
	case 0xC000:	    /* Melody, Now it could be 0xc00 - 0xc07 */
		return data&0xFF0F;
	default:
	    break;
	}

	/*read switch device ID, second try the case that port register offset is 0x10*/
	data = 0;
	mvEthSwitchRegRead(0, PORT_REGS_START_ADDR_8PORT, QD_REG_SWITCH_ID, &data);

	switch (data & 0xFF00) {
	case 0x0800:
	case 0x1A00:
	case 0x1000:
	case 0x0900:
	case 0x0400:
	case 0x1200:
	case 0x1400:
	case 0x1600:
	case 0x3200:
	case 0x1700:
	case 0x3700:
	case 0x2400:	/* Agate */
	case 0x3500:	/* Agate */
	case 0x1100:	/* Pearl */
	case 0x3100:	/* Pearl */
	case 0xc100:	/* ALP Fix */
		return data;
	default:
	    break;
	}

	return 0;
}

/*******************************************************************************
* mvEthSwitchGetDevIDByPortRegStartInAuto -
*
* DESCRIPTION:
*	get switch device ID by the switch port register start address if switch is in SMI auto scan access mode
*
* INPUT:
*	portRegsStartAddr - switch port register start address
*
* OUTPUT:
*	None
*
* RETURN:   switch device ID
*
*******************************************************************************/
static MV_U16 mvEthSwitchGetDevIDByPortRegStartInAuto(MV_U32 portRegsStartAddr)
{
	MV_U16 data, data1;

	/*read switch device ID from switch port 0 and 1, if the 2 values are equal and meaningful, then it's valid*/
	data = 0;
	mvEthSwitchRegRead(0, portRegsStartAddr, QD_REG_SWITCH_ID, &data);
	data1 = 0;
	mvEthSwitchRegRead(0, portRegsStartAddr + 1, QD_REG_SWITCH_ID, &data1);

	switch (data & 0xFF00) {
	case 0x0200:
	case 0x0300:
	case 0x0500:
	case 0x0600:
	case 0x1500:
	case 0xC000:		/* Melody */
	case 0x0700:		/* Spinnaker */
	case 0x2200:		/* Spinnaker */
	case 0x2500:		/* Spinnaker */
	case 0xF500:
	case 0xF900:
		if (data == data1)
			return data;
	default:
	    break;
	}

	return 0;

}

/*******************************************************************************
* mvEthSwitchGetDeviceIDInAutoScanMode -
*
* DESCRIPTION:
*	get switch device ID if switch is in SMI auto scan access mode
*
* INPUT:
*	None
*
* OUTPUT:
*	highSmiDevAddr - whether high SMI device address is used
*
* RETURN:   switch device ID
*
*******************************************************************************/
static MV_U16 mvEthSwitchGetDeviceIDInAutoScanMode(MV_BOOL *highSmiDevAddr)
{
	MV_U16 data, data1;

	/*read switch device ID, first try the case that port register offset is 8*/
	data = mvEthSwitchGetDevIDByPortRegStartInAuto(PORT_REGS_START_ADDR);
	if (data != 0) {
		*highSmiDevAddr = MV_FALSE;
		return data;
	}

	/*read switch device ID, second try the case that port register offset is 0x18*/
	data = mvEthSwitchGetDevIDByPortRegStartInAuto(PORT_REGS_START_ADDR + 0x10);
	if (data != 0) {
		*highSmiDevAddr = MV_TRUE;
		return data;
	}

	/*read switch device ID, finally try the case that port register offset is 0x10*/
	data = 0;
	mvEthSwitchRegRead(0, PORT_REGS_START_ADDR_8PORT, QD_REG_SWITCH_ID, &data);
	data1 = 0;
	mvEthSwitchRegRead(0, PORT_REGS_START_ADDR_8PORT + 1, QD_REG_SWITCH_ID, &data1);

	switch (data & 0xFF00) {
	case 0x0800:
	case 0x1A00:
	case 0x1000:
	case 0x0900:
	case 0x0400:
	case 0x1200:
	case 0x1400:
	case 0x1600:
	case 0x1700:
	case 0x3200:
	case 0x3700:
	case 0x2400:    /* Agate */
	case 0x3500:    /* Agate */
	case 0x1100:    /* Pearl */
	case 0x3100:    /* Pearl */
		if (data == data1) {
			*highSmiDevAddr = MV_FALSE;
			return data;
		}
	default:
	    break;
	}

	return 0;
}


/*******************************************************************************
* mvEthSwitchGetDeviceID -
*
* DESCRIPTION:
*	If switch device ID is not initialized, then detect switch device, find out switch device ID,
*	access mode and PHY address, otherwise return switch device ID directly
*
* INPUT:
*	None
*
* OUTPUT:
*	highSmiDevAddr - Indicates whether to use the high device register
*		addresses when accessing switch's registers (of all kinds)
*		i.e, the devices registers range is 0x10 to 0x1F, or to
*		use the low device register addresses (range 0x0 to 0xF).
*		GT_TRUE     - use high addresses (0x10 to 0x1F).
*		GT_FALSE    - use low addresses (0x0 to 0xF).
*
* RETURN:   switch device ID, 0 means no switch detected
*
*******************************************************************************/
MV_U16 mvEthSwitchGetDeviceID(void)
{
	MV_BOOL		highSmiDevAddr;

	/*if switchDeviceID has been initialized, then return it directly*/
	if (switchDeviceID >= 0)
		return (MV_U16)switchDeviceID;

	/*if switch module is not detected, then reurn 0 - no switch detected*/
	if (mvBoardIsSwitchConnected() == MV_FALSE) {
		switchDeviceID = 0;
		return (MV_U16)switchDeviceID;
	}

	switchAccessMode = mvBoardSmiScanModeGet(0);
#ifdef MV_SWITCH_SMI_ACCESS
	switchPhyAddr = mvBoardSwitchPhyAddrGet(0);
	if ((switchPhyAddr == -1) && (switchAccessMode == SMI_MANUAL_MODE)) {
		mvOsPrintf("switchPhyAddr value %d is error!\n", switchPhyAddr);
		switchDeviceID = 0;
		return switchDeviceID;
	}
#endif

	if (switchAccessMode == SMI_MULTI_ADDR_MODE || switchAccessMode == SMI_MANUAL_MODE) {
		switchDeviceID = mvEthSwitchGetDeviceIDInMultiorManualMode();
		return (MV_U16)switchDeviceID;
	} else if (switchAccessMode == SMI_AUTO_SCAN_MODE) {
		switchDeviceID = mvEthSwitchGetDeviceIDInAutoScanMode(&highSmiDevAddr);
		return (MV_U16)switchDeviceID;
	} else {
		switchDeviceID = 0;
		return (MV_U16)switchDeviceID;
	}
}


/*******************************************************************************
* mvEthE6065_61PhyBasicInit -
*
* DESCRIPTION:
*	Do a basic Init to the Phy , including reset
*
* INPUT:
*       ethPortNum - Ethernet port number
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
MV_VOID		mvEthE6065_61SwitchBasicInit(MV_U32 ethPortNum)
{
	switchVlanInit(ethPortNum,
			   MV_E6065_CPU_PORT,
			   MV_E6065_MAX_PORTS_NUM,
			   MV_E6065_PORTS_OFFSET,
			   MV_E6065_ENABLED_PORTS);
}

/*******************************************************************************
* mvEthE6063PhyBasicInit -
*
* DESCRIPTION:
*	Do a basic Init to the Phy , including reset
*
* INPUT:
*       ethPortNum - Ethernet port number
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
MV_VOID		mvEthE6063SwitchBasicInit(MV_U32 ethPortNum)
{
	switchVlanInit(ethPortNum,
			   MV_E6063_CPU_PORT,
			   MV_E6063_MAX_PORTS_NUM,
			   MV_E6063_PORTS_OFFSET,
			   MV_E6063_ENABLED_PORTS);
}

/*******************************************************************************
* mvEthE6131PhyBasicInit -
*
* DESCRIPTION:
*	Do a basic Init to the Phy , including reset
*
* INPUT:
*       ethPortNum - Ethernet port number
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
MV_VOID		mvEthE6131SwitchBasicInit(MV_U32 ethPortNum)
{

	MV_U16 reg;

	/*Enable Phy power up*/
	mvEthPhyRegWrite (0,0,0x9140);
	mvEthPhyRegWrite (1,0,0x9140);
	mvEthPhyRegWrite (2,0,0x9140);


	/*Enable PPU*/
	mvEthPhyRegWrite (0x1b,4,0x4080);


	/*Enable Phy detection*/
	mvEthPhyRegRead (0x13,0,&reg);
	reg &= ~(1<<12);
	mvEthPhyRegWrite (0x13,0,reg);

	mvOsDelay(100);
	mvEthPhyRegWrite (0x13,1,0x33);


	switchVlanInit(ethPortNum,
			    MV_E6131_CPU_PORT,
			   MV_E6131_MAX_PORTS_NUM,
			   MV_E6131_PORTS_OFFSET,
			   MV_E6131_ENABLED_PORTS);

}


/*******************************************************************************
* mvEthE6161PhyBasicInit -
*
* DESCRIPTION:
*	Do a basic Init to the Phy , including reset
*
* INPUT:
*       ethPortNum - Ethernet port number
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
MV_VOID		mvEthE6161SwitchBasicInit(MV_U32 ethPortNum)
{

    MV_U32 prt;
    MV_U16 reg;
    volatile MV_U32 timeout;

    /* The 6161/5 needs a delay */
    mvOsDelay(1000);

    /* Init vlan */
    switchVlanInit(ethPortNum,
		    MV_E6161_CPU_PORT,
		    MV_E6161_MAX_PORTS_NUM,
		    MV_E6161_PORTS_OFFSET,
		    MV_E6161_ENABLED_PORTS);

    /* Enable RGMII delay on Tx and Rx for CPU port */
    mvEthSwitchRegWrite (ethPortNum, 0x14,0x1a,0x81e7);
    mvEthSwitchRegRead (ethPortNum, 0x15,0x1a,&reg);
    mvEthSwitchRegWrite (ethPortNum, 0x15,0x1a,0x18);
    mvEthSwitchRegWrite (ethPortNum, 0x14,0x1a,0xc1e7);

    for(prt=0; prt < MV_E6161_MAX_PORTS_NUM; prt++)
    {
	if (prt != MV_E6161_CPU_PORT)
	{
	    /*Enable Phy power up*/
	    mvEthSwitchRegWrite (ethPortNum, MV_E6161_GLOBAL_2_REG_DEV_ADDR,
				MV_E6161_SMI_PHY_DATA, 0x3360);
	    mvEthSwitchRegWrite (ethPortNum, MV_E6161_GLOBAL_2_REG_DEV_ADDR,
				MV_E6161_SMI_PHY_COMMAND, (0x9410 | (prt << 5)));

	    /*Make sure SMIBusy bit cleared before another SMI operation can take place*/
	    timeout = E6161_PHY_TIMEOUT;
	    do
            {
		mvEthSwitchRegRead(ethPortNum, MV_E6161_GLOBAL_2_REG_DEV_ADDR,
				MV_E6161_SMI_PHY_COMMAND,&reg);
		if(timeout-- == 0)
		{
			mvOsPrintf("mvEthPhyRegRead: SMI busy timeout\n");
			return;
		}
	    }while (reg & E6161_PHY_SMI_BUSY_MASK);

	    mvEthSwitchRegWrite (ethPortNum, MV_E6161_GLOBAL_2_REG_DEV_ADDR,
				MV_E6161_SMI_PHY_DATA,0x1140);
	    mvEthSwitchRegWrite (ethPortNum, MV_E6161_GLOBAL_2_REG_DEV_ADDR,
				MV_E6161_SMI_PHY_COMMAND,(0x9400 | (prt << 5)));

	    /*Make sure SMIBusy bit cleared before another SMI operation can take place*/
	    timeout = E6161_PHY_TIMEOUT;
	    do
            {
		mvEthSwitchRegRead(ethPortNum, MV_E6161_GLOBAL_2_REG_DEV_ADDR,
				MV_E6161_SMI_PHY_COMMAND,&reg);
		if(timeout-- == 0)
		{
			mvOsPrintf("mvEthPhyRegRead: SMI busy timeout\n");
			return;
		}
	    }while (reg & E6161_PHY_SMI_BUSY_MASK);

	}

	/*Enable port*/
	mvEthSwitchRegWrite (ethPortNum, MV_E6161_PORTS_OFFSET + prt, 4, 0x7f);
    }

    /*Force CPU port to RGMII FDX 1000Base*/
    mvEthSwitchRegWrite (ethPortNum, MV_E6161_PORTS_OFFSET + MV_E6161_CPU_PORT, 1, 0x3e);
}

#if defined (MV88F66XX)
MV_VOID mvAlpBoardSwitchBasicInit(MV_U32 enabledPorts)
{
	MV_U16 regVal;
	MV_U32 port, forceMask, switchCpuPort = mvBoardSwitchCpuPortGet(0);

	/* Force link, speed, duplex for switch CPU port */
	mvEthSwitchRegWrite(0, 0x10 + switchCpuPort, 0x1, 0x3e);

	forceMask = mvBoardSwitchPortForceLinkGet(0);
	for (port = 0; port < 6; port++) {
		MV_U32 toForce = forceMask & (1 << port);
		if (toForce) {
			mvEthSwitchRegWrite(0, MV_SWITCH_PORT_OFFSET(port),
						MV_SWITCH_PHYS_CONTROL_REG, 0x3e);

			/* Disable PHY polling for forced link ports - clear bit 12 */
			mvEthSwitchRegRead(0, MV_SWITCH_PORT_OFFSET(port),
						MV_SWITCH_PORT_STATUS_REG, &regVal);
			mvEthSwitchRegWrite(0, MV_SWITCH_PORT_OFFSET(port),
						MV_SWITCH_PORT_STATUS_REG, regVal & ~BIT12);
		}
	}

	switchVlanInit(0, switchCpuPort, MV_ALP_SW_MAX_PORTS_NUM,
		       MV_ALP_SW_PORTS_OFFSET, enabledPorts);
}
#endif /* MV88F66XX */

/* This initial function is valid for E6176 switch */
MV_VOID	mvEthE6171SwitchBasicInit(MV_U32 ethPortNum)
{

	MV_U32 prt;
	MV_U16 reg;
	volatile MV_U32 timeout;
	MV_U32 cpuPort =  mvBoardSwitchCpuPortGet(0);
	MV_U32 swicthPhyAddr = mvBoardSwitchPhyAddrGet(0);
	/* The 6171 needs a delay */
	mvOsDelay(1000);

	/* Init vlan of switch 1 and enable all ports */
	switchVlanInit(ethPortNum, cpuPort, MV_E6171_MAX_PORTS_NUM, swicthPhyAddr, MV_E6171_ENABLED_PORTS);

	if (mvBoardSwitchCpuPortIsRgmii(0)) {
		/* Enable RGMII delay on Tx and Rx for port 5 switch 1 */
		mvEthSwitchRegRead(ethPortNum, swicthPhyAddr + cpuPort, MV_E6171_SWITCH_PHIYSICAL_CTRL_REG, &reg);
		mvEthSwitchRegWrite(ethPortNum, swicthPhyAddr + cpuPort, MV_E6171_SWITCH_PHIYSICAL_CTRL_REG,
				(reg|0xC000));
	} else {
		/* If the CPU port is not connected to RGMII, so it connected
		   to SGMII via SerDes lane, this code power-up the SerDes in switch */
		/* Change to page #1 to access SerDes control register */
		mvEthSwitchPhyRegWrite(ethPortNum, MV_E6171_SERDES_REG, MV_E6171_PAGE_REG, BIT0);
		/* Read SerDes power down status of the switch */
		mvEthSwitchPhyRegRead(ethPortNum, MV_E6171_SERDES_REG, MV_E6171_SERDES_CONTROL_REG, &reg);
		/* Clear Bit 11 to power up the switch SerDes */
		reg &= ~BIT11;
		mvEthSwitchPhyRegWrite(ethPortNum, MV_E6171_SERDES_REG, MV_E6171_SERDES_CONTROL_REG, reg);
	}

	/* Power up PHYs */
	for (prt = 0; prt < MV_E6171_MAX_PORTS_NUM - 2; prt++)	{
		if (prt != cpuPort) {
			/*Enable Phy power up for switch 1*/
			mvEthSwitchRegWrite(ethPortNum, MV_E6171_GLOBAL_2_REG_DEV_ADDR, MV_E6171_SMI_PHY_DATA, 0x3360);
			mvEthSwitchRegWrite(ethPortNum, MV_E6171_GLOBAL_2_REG_DEV_ADDR, MV_E6171_SMI_PHY_COMMAND,
								(0x9410 | (prt << 5)));
			/*Make sure SMIBusy bit cleared before another SMI operation can take place*/
			timeout = E6171_PHY_TIMEOUT;

			do {
				mvEthSwitchRegRead(ethPortNum, MV_E6171_GLOBAL_2_REG_DEV_ADDR, MV_E6171_SMI_PHY_COMMAND,&reg);
				if (timeout-- == 0) {
					mvOsPrintf("mvEthPhyRegRead: SMI busy timeout\n");
					return;
				}
			} while (reg & E6171_PHY_SMI_BUSY_MASK);
 
			mvEthSwitchRegWrite(ethPortNum, MV_E6171_GLOBAL_2_REG_DEV_ADDR, MV_E6171_SMI_PHY_DATA,0x9140);
			mvEthSwitchRegWrite(ethPortNum, MV_E6171_GLOBAL_2_REG_DEV_ADDR,
								MV_E6171_SMI_PHY_COMMAND,(0x9400 | (prt << 5)));

			/*Make sure SMIBusy bit cleared before another SMI operation can take place*/
			timeout = E6171_PHY_TIMEOUT;

			do {
				mvEthSwitchRegRead(ethPortNum, MV_E6171_GLOBAL_2_REG_DEV_ADDR,
								   MV_E6171_SMI_PHY_COMMAND,&reg);
				if (timeout-- == 0) {
					mvOsPrintf("mvEthPhyRegRead: SMI busy timeout\n");
					return;
				}
			} while (reg & E6171_PHY_SMI_BUSY_MASK);
		}
	}

	mvEthSwitchRegWrite(ethPortNum, swicthPhyAddr + cpuPort, 0x4, 0x7f);

	/* Init LEDs on RD-6282 */
	/* Move all LEDs to special */
	mvEthSwitchRegWrite(ethPortNum, swicthPhyAddr, MV_E6171_LED_CONTROL, (BIT15|0x67));
	mvEthSwitchRegWrite(ethPortNum, swicthPhyAddr, MV_E6171_LED_CONTROL, (BIT15|BIT12|0x32));

	/* Port 0 LED special activity link */
	mvEthSwitchRegWrite(ethPortNum, swicthPhyAddr, MV_E6171_LED_CONTROL, (BIT15|BIT14|BIT13|BIT12|BIT0));

	/* Port 1 LED special activity link */
	mvEthSwitchRegWrite(ethPortNum, swicthPhyAddr + 1, MV_E6171_LED_CONTROL, (BIT15|BIT14|BIT13|BIT12|BIT1));

	/* Port 2 LED special activity link */
	mvEthSwitchRegWrite(ethPortNum, swicthPhyAddr + 2, MV_E6171_LED_CONTROL, (BIT15|BIT14|BIT13|BIT12|BIT2));

	/* Port 3 LED special activity link */
	mvEthSwitchRegWrite(ethPortNum, swicthPhyAddr + 3, MV_E6171_LED_CONTROL, (BIT15|BIT14|BIT13|BIT12|BIT3));

#ifdef RD_88F6710
	mvEthSwitchRegWrite(ethPortNum, swicthPhyAddr, MV_E6171_LED_CONTROL, 0xf00f);
#endif

}

MV_VOID	mvEthE6172SwitchBasicInit(MV_U32 ethPortNum)
{

	MV_U32 prt;
	MV_U16 reg;
	MV_U32 timeout;
	MV_32 cpuPort =  MV_E6172_CPU_PORT;/*mvBoardSwitchCpuPortGet(0);*/
	MV_U16 smiReg, data;

	/* The 6172 needs a delay */
	mvOsDelay(1000);

	/* Init vlan of switch 1 and enable all ports */
	switchVlanInit(ethPortNum, MV_E6172_CPU_PORT, MV_E6172_MAX_PORTS_NUM, MV_E6172_PORTS_OFFSET,
				   MV_E6172_ENABLED_PORTS);

	/* Enable RGMII delay on Tx and Rx for cpu port, force duplex, link up,  flow control, and speed to 1Gps*/
	mvEthSwitchRegWrite(ethPortNum, MV_E6172_PORTS_OFFSET + cpuPort, MV_E6172_SWITCH_PHIYSICAL_CTRL_REG,
	QD_PCS_RGMII_RX_TIMING_MASK | QD_PCS_RGMII_TX_TIMING_MASK | QD_PCS_FLOW_CONTROL_VALUE_MASK
	| QD_PCS_FORCED_FLOW_CONTROL_MASK | QD_PCS_LINK_VALUE_MASK | QD_PCS_FORCED_LINK_MASK
	| QD_PCS_DUPLEX_VALUE_MASK | QD_PCS_FORCED_DUPLEX_MASK | QD_PCS_FORCE_SPEED_1G);
	mvEthSwitchRegWrite(ethPortNum, MV_E6172_PORTS_OFFSET + cpuPort, MV_E6172_SWITCH_PORT_CTRL_REG,
	QD_PC_VLAN_TUNNEL_BY_PASS | QD_PC_INITIAL_PRI_IP | QD_PC_EGRESS_FLOODS_ALL | QD_PC_PORT_STATE_FORWARDING);

	/* Power up PHYs */
	for (prt = 0; prt < MV_E6172_MAX_PORTS_NUM - 2; prt++)	{
		if (prt != MV_E6172_CPU_PORT) {
			/*Enable Phy power up for switch 1*/
			data = QD_PHY_MDI_CROSS_AUTO | QD_PHY_ENERGY_DETECT_SENSE_PERIODIC_TX_NLP |
			QD_PHY_DOWNSHIFT_COUNTER;
			mvEthSwitchRegWrite(ethPortNum, MV_E6172_GLOBAL_2_REG_DEV_ADDR,
								MV_E6172_SMI_PHY_DATA, data);
			smiReg = (QD_SMI_BUSY | (prt << QD_SMI_DEV_ADDR_BIT) | (QD_SMI_WRITE << QD_SMI_OP_BIT) |
			(QD_PHY_SPEC_CONTROL_REG << QD_SMI_REG_ADDR_BIT) | (QD_SMI_CLAUSE22 << QD_SMI_MODE_BIT));
			mvEthSwitchRegWrite(ethPortNum, MV_E6172_GLOBAL_2_REG_DEV_ADDR,
								MV_E6172_SMI_PHY_COMMAND, smiReg);
			/*Make sure SMIBusy bit cleared before another SMI operation can take place*/
			timeout = E6172_PHY_TIMEOUT;

			do {
				mvEthSwitchRegRead(ethPortNum, MV_E6172_GLOBAL_2_REG_DEV_ADDR,
					MV_E6172_SMI_PHY_COMMAND, &reg);
				if (timeout-- == 0) {
					mvOsPrintf("mvEthPhyRegRead: SMI busy timeout\n");
					return;
				}
			} while (reg & E6172_PHY_SMI_BUSY_MASK);

			data = QD_PHY_RESET | QD_PHY_AUTONEGO | QD_PHY_DUPLEX | QD_PHY_SPEED_MSB;
			mvEthSwitchRegWrite(ethPortNum, MV_E6172_GLOBAL_2_REG_DEV_ADDR, MV_E6172_SMI_PHY_DATA,
				data);
			smiReg = (QD_SMI_BUSY | (prt << QD_SMI_DEV_ADDR_BIT) | (QD_SMI_WRITE << QD_SMI_OP_BIT) |
			(QD_PHY_CONTROL_REG << QD_SMI_REG_ADDR_BIT) | (QD_SMI_CLAUSE22 << QD_SMI_MODE_BIT));
			mvEthSwitchRegWrite(ethPortNum, MV_E6172_GLOBAL_2_REG_DEV_ADDR,
								MV_E6172_SMI_PHY_COMMAND, smiReg);

			/*Make sure SMIBusy bit cleared before another SMI operation can take place*/
			timeout = E6172_PHY_TIMEOUT;

			do {
				mvEthSwitchRegRead(ethPortNum, MV_E6172_GLOBAL_2_REG_DEV_ADDR,
					MV_E6172_SMI_PHY_COMMAND, &reg);
				if (timeout-- == 0) {
					mvOsPrintf("mvEthPhyRegRead: SMI busy timeout\n");
					return;
				}
			} while (reg & E6172_PHY_SMI_BUSY_MASK);
		}
	}

}

static void switchVlanInit(MV_U32 ethPortNum, MV_U32 switchCpuPort, MV_U32 switchMaxPortsNum,
			   MV_U32 switchPortsOffset, MV_U32 switchEnabledPortsMask)
{
	MV_U32 port;
	MV_U16 reg;

	/*enable only appropriate ports to forwarding mode - and disable the others*/
	for(port=0; port < switchMaxPortsNum; port++) {
		if ((1 << port)& switchEnabledPortsMask)	{
			mvEthSwitchRegRead(ethPortNum, MV_SWITCH_PORT_OFFSET(port),
					   MV_SWITCH_PORT_CONTROL_REG,&reg);
			reg |= 0x3;
			mvEthSwitchRegWrite(ethPortNum, MV_SWITCH_PORT_OFFSET(port),
					    MV_SWITCH_PORT_CONTROL_REG,reg);
		} else {
			/* Disable port */
			mvEthSwitchRegRead(ethPortNum, MV_SWITCH_PORT_OFFSET(port),
					   MV_SWITCH_PORT_CONTROL_REG,&reg);
			reg &= ~0x3;
			mvEthSwitchRegWrite(ethPortNum, MV_SWITCH_PORT_OFFSET(port),
					   MV_SWITCH_PORT_CONTROL_REG,reg);
		}
	}
}

#ifdef MV_SWITCH_DIRECT_ACCESS
void mvEthSwitchRegWrite(MV_U32 ethPortNum, MV_U32 switchPort,
				MV_U32 switchReg, MV_U16 data)
{
	MV_U32 offset = 0;

	if (mvBoardIsSwitchConnected() == MV_FALSE) {
		mvOsPrintf("No Switch.\n");
		return;
	}

	offset |= (0x3 << 16); /* [19:16] - APB bridge on XBar port #3 */
	/* [15:13] - Switch is target 0 on ABP splitter */
	offset |= (switchPort << 7); /* [11:7] - Switch port */
	offset |= (switchReg << 2);  /* [6:2] - Switch Register */
	MV_REG_WRITE(offset, data);
}

void mvEthSwitchRegRead(MV_U32 ethPortNum, MV_U32 switchPort,
				MV_U32 switchReg, MV_U16 *data)
{
	MV_U32 result, reg = 0;

	if (mvBoardIsSwitchConnected() == MV_FALSE) {
		mvOsPrintf("No Switch.\n");
		return;
	}

	reg |= (0x3 << 16); /* [19:16] - APB bridge on XBar port #3 */
	/* [15:13] - Switch is target 0 on ABP splitter */
	reg |= (switchPort << 7); /* [11:7] - Switch port */
	reg |= (switchReg << 2);  /* [6:2] - Switch Register */
	result = MV_REG_READ(reg);
	*data = result;
}

#else	/*defined(MV_SWITCH_SMI_ACCESS)*/
/*******************************************************************************
* mvEthSwitchWaitSMICommandRegFree -
*
* DESCRIPTION:
*	wait SMI command register to be free
*
* INPUT:
*	timeOut - the wait time, in 100MS units
*
* OUTPUT:
*	None
*
* RETURN:   true - SMI command register is free, false - SMI command register is not free
*
*******************************************************************************/
static MV_BOOL mvEthSwitchWaitSMICommandRegFree(MV_U32 timeOut)
{
	MV_U16 smiReg;
	int i;

	/* first check that it is not busy */
	if (mvEthPhyRegRead(switchPhyAddr, (MV_U32)QD_REG_SMI_COMMAND, &smiReg) != MV_OK)
		return MV_FALSE;

	timeOut = QD_SMI_ACCESS_LOOP; /* initialize the loop count */

	if (smiReg & QD_SMI_BUSY) {
		for (i = 0; i < QD_SMI_TIMEOUT; i++)
			;
		do {
			if (timeOut-- < 1)
				return MV_FALSE;
			if (mvEthPhyRegRead(switchPhyAddr, (MV_U32)QD_REG_SMI_COMMAND, &smiReg) != MV_OK)
				return MV_FALSE;
		} while (smiReg & QD_SMI_BUSY);
	}

	return MV_TRUE;
}

void mvEthSwitchRegWrite(MV_U32 ethPortNum, MV_U32 switchPort,
				MV_U32 switchReg, MV_U16 data)
{
	MV_U16 smiReg;

	if (mvBoardIsSwitchConnected() == MV_FALSE) {
		mvOsPrintf("No Switch.\n");
		return;
	}

	if (switchAccessMode != SMI_MULTI_ADDR_MODE) {
		mvEthPhyRegWrite(switchPort, switchReg, data);
		return;
	}

	/* wait SMI command register to be free */
	if (mvEthSwitchWaitSMICommandRegFree(QD_SMI_ACCESS_LOOP) == MV_FALSE)
		return;

	if (mvEthPhyRegWrite(switchPhyAddr, (MV_U32)QD_REG_SMI_DATA, data) != MV_OK)
		return;

	smiReg = QD_SMI_BUSY | (switchPort << QD_SMI_DEV_ADDR_BIT) | (QD_SMI_WRITE << QD_SMI_OP_BIT) |
		(switchReg << QD_SMI_REG_ADDR_BIT) | (QD_SMI_CLAUSE22 << QD_SMI_MODE_BIT);

	if (mvEthPhyRegWrite(switchPhyAddr, (MV_U32)QD_REG_SMI_COMMAND, smiReg) != MV_OK)
		return;

	return;
}


void mvEthSwitchRegRead(MV_U32 ethPortNum, MV_U32 switchPort,
				MV_U32 switchReg, MV_U16 *data)
{
	MV_U16 smiReg;

	if (mvBoardIsSwitchConnected() == MV_FALSE) {
		mvOsPrintf("No Switch.\n");
		return;
	}

	if (switchAccessMode != SMI_MULTI_ADDR_MODE) {
		mvEthPhyRegRead(switchPort, switchReg, &smiReg);
		*data = smiReg;
		return;
	}

	/* wait SMI command register to be free */
	if (mvEthSwitchWaitSMICommandRegFree(QD_SMI_ACCESS_LOOP) == MV_FALSE)
		return;

	smiReg =  QD_SMI_BUSY | (switchPort << QD_SMI_DEV_ADDR_BIT) | (QD_SMI_READ << QD_SMI_OP_BIT) |
		(switchReg << QD_SMI_REG_ADDR_BIT) | (QD_SMI_CLAUSE22 << QD_SMI_MODE_BIT);

	if (mvEthPhyRegWrite(switchPhyAddr, (MV_U32)QD_REG_SMI_COMMAND, smiReg) != MV_OK)
		return;

	/* wait SMI command register to be free */
	if (mvEthSwitchWaitSMICommandRegFree(QD_SMI_ACCESS_LOOP) == MV_FALSE)
		return;

	if (mvEthPhyRegRead(switchPhyAddr, (MV_U32)QD_REG_SMI_DATA, &smiReg) != MV_OK)
		return;

	*data = smiReg;

	return;
}
#endif

void mvEthSwitchPhyRegWrite(MV_U32 ethPortNum, MV_U16 prt,
                                 MV_U16 regOffs, MV_U16 data)
{
	MV_U16 reg;
	volatile MV_U32 timeout;

	/*Make sure SMIBusy bit cleared before another SMI operation can take place*/
	timeout = MV_SW_PHY_TIMEOUT;
	do {
		mvEthSwitchRegRead(ethPortNum, MV_SW_GLOBAL_2_REG_DEV_ADDR, MV_SW_SMI_PHY_COMMAND, &reg);
		if (timeout-- == 0) 
		{
			mvOsPrintf("mvEthPhyRegRead: SMI busy timeout\n");
			return;
		}
	} while (reg & MV_SW_PHY_SMI_BUSY_MASK);

	mvEthSwitchRegWrite(ethPortNum, MV_SW_GLOBAL_2_REG_DEV_ADDR, MV_SW_SMI_PHY_DATA, data);

	mvEthSwitchRegWrite(ethPortNum, MV_SW_GLOBAL_2_REG_DEV_ADDR,
			MV_SW_SMI_PHY_COMMAND,(0x9400 | (prt << 5) | regOffs));

	/*Make sure SMIBusy bit cleared before another SMI operation can take place*/
	timeout = MV_SW_PHY_TIMEOUT;
	do {
		mvEthSwitchRegRead(ethPortNum, MV_SW_GLOBAL_2_REG_DEV_ADDR, MV_SW_SMI_PHY_COMMAND, &reg);
		if(timeout-- == 0) {
			mvOsPrintf("mvEthPhyRegRead: SMI busy timeout\n");
			return;
		}
	} while (reg & MV_SW_PHY_SMI_BUSY_MASK);
}

void mvEthSwitchPhyRegRead(MV_U32 ethPortNum, MV_U16 prt,
                             MV_U16 regOffs, MV_U16 *data)
{
	MV_U16 reg;
	volatile MV_U32 timeout;

	/*Make sure SMIBusy bit cleared before another SMI operation can take place*/
	timeout = MV_SW_PHY_TIMEOUT;
	do {
		mvEthSwitchRegRead(ethPortNum, MV_SW_GLOBAL_2_REG_DEV_ADDR, MV_SW_SMI_PHY_COMMAND, &reg);
		if(timeout-- == 0) 
		{
			mvOsPrintf("mvEthPhyRegRead: SMI busy timeout\n");
			return;
		}
	} while (reg & MV_SW_PHY_SMI_BUSY_MASK);

	mvEthSwitchRegWrite(ethPortNum, MV_SW_GLOBAL_2_REG_DEV_ADDR,
			MV_SW_SMI_PHY_COMMAND,(0x9800 | (prt << 5) | regOffs));

	/*Make sure SMIBusy bit cleared before another SMI operation can take place*/
	timeout = MV_SW_PHY_TIMEOUT;
	do {
		mvEthSwitchRegRead(ethPortNum, MV_SW_GLOBAL_2_REG_DEV_ADDR, MV_SW_SMI_PHY_COMMAND, &reg);
		if(timeout-- == 0) 
		{
			mvOsPrintf("mvEthPhyRegRead: SMI busy timeout\n");
			return;
		}
	} while (reg & MV_SW_PHY_SMI_BUSY_MASK);

	mvEthSwitchRegRead(ethPortNum, MV_SW_GLOBAL_2_REG_DEV_ADDR, MV_SW_SMI_PHY_DATA, data);
}
