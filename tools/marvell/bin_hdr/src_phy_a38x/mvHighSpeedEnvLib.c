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
#include "mvBHboardEnvSpec.h"
//#include "mvCtrlPex.h"

MV_U32 _MV_REG_READ(MV_U32 regAddr)
{
  DEBUG_INIT_FULL_S(" >>>       MV_REG_READ.  regAddr=0x");
  DEBUG_INIT_FULL_D(INTER_REGS_BASE | (regAddr), 8);
  DEBUG_INIT_FULL_S(" regData=0x");

  MV_U32 regData = MV_MEMIO_LE32_READ((INTER_REGS_BASE | (regAddr)));

  DEBUG_INIT_FULL_D(regData, 8);
  DEBUG_INIT_FULL_S(" - Done\n");

  return regData;
}

MV_VOID _MV_REG_WRITE(MV_U32 regAddr, MV_U32 regData)
{
  DEBUG_INIT_FULL_S(" >>>       MV_REG_WRITE. regAddr=0x");
  DEBUG_INIT_FULL_D(INTER_REGS_BASE | (regAddr), 8);
  DEBUG_INIT_FULL_S(" regData=0x");
  DEBUG_INIT_FULL_D(regData, 8);

  MV_MEMIO_LE32_WRITE((INTER_REGS_BASE | (regAddr)), (regData));
  DEBUG_INIT_FULL_S(" - Done \n\n");
}

//#define SERDES_LOCAL_DEBUG
#ifdef SERDES_LOCAL_DEBUG
#define MV_REG_WRITE 	_MV_REG_WRITE
#define MV_REG_READ 	_MV_REG_READ
#endif

#define LINK_WAIT_CNTR	100
#define LINK_WAIT_SLEEP	100

MV_STATUS mvCtrlHighSpeedSerdesPhyConfig(MV_VOID)
{
	MV_U32 i;

	DEBUG_INIT_S("Initializing PCIe-0 on lane 1: \n");

	//print "Set the capabilities register"
	MV_REG_WRITE(0x8006c, 0x7ac12);

	//rint "STEP-0 Configure the MUX select for PCIe-1 on Serdes Lane 2"
	MV_REG_WRITE(0x183fc, 0x8);

	//;print "STEP-1 Set values that should be constant from beginning of sequence"
	MV_REG_WRITE(0x18328, 0x04470004);
	MV_REG_WRITE(0x1832c, 0x00000058);
	MV_REG_WRITE(0x18334, 0x0000000d);

	//print "STEP-2 Assert reset"
	// Nothing to do as both bit 13, 14 are asserted to reset by default

	//print "STEP-3 Deassert reset"
	MV_REG_WRITE(0x18328, 0x04476004);

	//print "STEP-6"
	MV_REG_WRITE(0xA0F04, 0x00000025);

	//print "STEP-4"
	MV_REG_WRITE(0xA0804, 0x0000fc60);

	//print "STEP-5 PHY gen max"
	MV_REG_WRITE(0xA0894, 0x000017ff);

	//7. Irrelevent - USB device mode only

	//print "STEP-8, not needed - only for simulation"
	//data.out 0xD00A1200 %LONG 0x00001000

	//print "STEP-9 "
	MV_REG_WRITE(0xA093c, 0x0000a08a);	//bit 7 = 1.bit 6=0

	//print "STEP-10, not needed - only for simulation"
	//;data.out 0xD00A1144 %LONG 0x00000304 ; check if we need to write to this register

	//print "Release phy reset"
	MV_REG_WRITE(0xA0F04, 0x00000024);

	//print "Enable PCIe interface"
	MV_REG_WRITE(0x18204, 0x0707c0f1);

	for (i=0; i<LINK_WAIT_CNTR; i++) {
		if ((MV_REG_READ(0x81a64) & 0xFF) == 0x7E) {
			DEBUG_INIT_S("LINK UP ;-)\n");
			break;
		}
		mvOsUDelay(LINK_WAIT_SLEEP);
	}

	if (i==LINK_WAIT_CNTR)
		DEBUG_INIT_S("NO LINK\n");

	DEBUG_INIT_S("Initializing PCIe-1 on lane 2:\n");

	//print "Set the capabilities register"
	MV_REG_WRITE(0x4006c, 0x7ac12);

	//rint "STEP-0 Configure the MUX select for PCIe-1 on Serdes Lane 2"
	MV_REG_WRITE(0x183fc, 0x48); // Keeping also PCI-0

	//;print "STEP-1 Set values that should be constant from beginning of sequence"
	MV_REG_WRITE(0x18350, 0x04470004);
	MV_REG_WRITE(0x18354, 0x00000058);
	MV_REG_WRITE(0x1835c, 0x0000000d);

	//print "STEP-2 Assert reset"
	// Nothing to do as both bit 13, 14 are asserted to reset by default

	//print "STEP-3 Deassert reset"
	MV_REG_WRITE(0x18350, 0x04476004);

	//print "STEP-4"
	MV_REG_WRITE(0xA1004, 0x0000fc60);

	//print "STEP-5 PHY gen max"
	MV_REG_WRITE(0xA1094, 0x000017ff);

	//print "STEP-6"
	MV_REG_WRITE(0xA1704, 0x00000025);

	//7. Irrelevent - USB device mode only

	//print "STEP-8, not needed - only for simulation"
	//data.out 0xD00A1200 %LONG 0x00001000

	//print "STEP-9 "
	MV_REG_WRITE(0xA113c, 0x0000a08a);	//bit 7 = 1.bit 6=0

	//print "STEP-10, not needed - only for simulation"
	//;data.out 0xD00A1144 %LONG 0x00000304 ; check if we need to write to this register

	//print "Release phy reset"
	MV_REG_WRITE(0xA1704, 0x00000024);

	//print "Enable PCIe interface"
	MV_REG_WRITE(0x18204, 0x0707c0f3); // Keeping also PCI-0

	for (i=0; i<LINK_WAIT_CNTR; i++) {
		if ((MV_REG_READ(0x41a64) & 0xFF) == 0x7E) {
			DEBUG_INIT_S("LINK UP ;-)\n");
			break;
		}
		mvOsUDelay(LINK_WAIT_SLEEP);
	}

	if (i==LINK_WAIT_CNTR)
		DEBUG_INIT_S("NO LINK\n");

	DEBUG_INIT_S("Initializing SATA:\n");

	MV_REG_WRITE(0xa80a0, 0x00000000);
	MV_REG_WRITE(0xa80a4, 0x00000000);
	MV_REG_WRITE(0xa80a0, 0x00000000);
	//bits [3:0] => GenI 5 ,GENII 6,GENIII - 7
	MV_REG_WRITE(0xa80a4, 0x00000046);
	MV_REG_WRITE(0xa80a0, 0x00000004);
	MV_REG_WRITE(0xa80a4, 0x00000000);


	MV_REG_WRITE(0x183fC, 0x0000004A);
	MV_REG_WRITE(0x18300, 0x08881802);
	MV_REG_WRITE(0x1830C, 0x0000000F);
	MV_REG_WRITE(0x18300, 0x08886002);
	MV_REG_WRITE(0xa0004, 0x0000FC01);
	MV_REG_WRITE(0xa0094, 0x00001BFF);
	MV_REG_WRITE(0xa008C, 0x00000074);
	MV_REG_WRITE(0xa013C, 0x0000044A);
	MV_REG_WRITE(0x18300, 0x08876002);

	MV_REG_WRITE(0xa80a0, 0x00000000);
	//bits [3:0] => GenI 5 ,GENII 6,GENIII - 7
	MV_REG_WRITE(0xa80a4, 0x00C40006);

	for (i=0; i<LINK_WAIT_CNTR; i++) {
		if ((MV_REG_READ(0x18318) & 0xD) == 0xD) {
			DEBUG_INIT_S("PLL READY ;-)\n");
			break;
		}
		mvOsUDelay(LINK_WAIT_SLEEP);
	}

	if (i==LINK_WAIT_CNTR)
		DEBUG_INIT_S("PLL NOT READY\n");


	DEBUG_INIT_S("Initializing USB3 port 0 \n");

	MV_REG_WRITE(0x183fc,  0x0000404A); // Bits [14:12] =0x4.
	MV_REG_WRITE(0x183A0,  0x04479804);
	MV_REG_WRITE(0x183a4,  0x00000058);
	MV_REG_WRITE(0x183AC,  0x0000000d);
	MV_REG_WRITE(0x183A0,  0x0447E004);
	MV_REG_WRITE(0xA2704,  0x00000021);
	MV_REG_WRITE(0xA2004,  0x0000FCA0);
	MV_REG_WRITE(0xA2094,  0x000017ff);
	MV_REG_WRITE(0xA213C,  0x0000a08a);
	MV_REG_WRITE(0xA2704,  0x00000020);
	mvOsUDelay(10000);

	if( 0 != (MV_REG_READ(0xA2004) & 0x100))
		DEBUG_INIT_S("PLL UNLOCK ;-)\n");
	else
		DEBUG_INIT_S("PLL LOCKED ;-|\n");

	if( 0xD == (MV_REG_READ(0x183B8) & 0xD))
		DEBUG_INIT_S("PLL READY ;-)\n");
	else
		DEBUG_INIT_S("PLL NOT READY ;-|\n");

	DEBUG_INIT_S("Initializing USB3 port 1 \n");

	MV_REG_WRITE(0x183fc,  0x0002404A); //  Bits [17:15]=0x2.
	MV_REG_WRITE(0x183C8,  0x04479804);
	MV_REG_WRITE(0x183CC,  0x00000058);
	MV_REG_WRITE(0x183D4,  0x0000000d);
	MV_REG_WRITE(0x183C8,  0x0447E004);
	MV_REG_WRITE(0xA2F04,  0x00000021);
	MV_REG_WRITE(0xA2804,  0x0000FCA0);
	MV_REG_WRITE(0xA2894,  0x000017ff);
	MV_REG_WRITE(0xA293C,  0x0000a08a);
	MV_REG_WRITE(0xA2F04,  0x00000020);
	mvOsUDelay(10000);

	if( 0 != (MV_REG_READ(0xA2804) & 0x100))
		DEBUG_INIT_S("PLL UNLOCK ;-)\n");
	else
		DEBUG_INIT_S("PLL LOCKED ;-|\n");

	if( 0xD == (MV_REG_READ(0x183E0) & 0xD))
		DEBUG_INIT_S("PLL READY ;-)\n");
	else
		DEBUG_INIT_S("PLL NOT READY ;-|\n");

	return MV_OK;
}

/***************************************************************************/
MV_U32 mvBoardTclkGet(MV_VOID)
{
	MV_U32 tclk;
	tclk = (MV_REG_READ(MPP_SAMPLE_AT_RESET));
	tclk = ((tclk & (1 << 15)) >> 15);
	switch (tclk) {
	case 0:
		return MV_BOARD_TCLK_250MHZ;
	case 1:
		return MV_BOARD_TCLK_200MHZ;
	default:
		return MV_BOARD_TCLK_250MHZ;
	}
}
