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

#include "ctrlEnv/mvCtrlEnvLib.h"
#include "ctrlEnv/sys/mvCpuIf.h"
#include "cpu/mvCpu.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "ctrlEnv/mvUnitMap.h"

static MV_BOOL mv_rsrc_limited = MV_FALSE;
static MV_RES_MAP mv_res_table[] = {
	/* Dividable units */
	{0, "uart0"},
	{0, "uart1"},
	{0, "pex0"},
	{0, "pex1"},
	{0, "pex2"},
	{0, "pex3"},
	{0, "eth0"},
	{0, "eth1"},
	{0, "eth2"},
	{0, "eth3"},
	{0, "xor0"},
	{0, "xor1"},
	{0, "usb0"},
	{0, "usb1"},
	{0, "usb2"},
	{0, "i2c0"},
	{0, "i2c1"},
	/* Single Allocation units */
	{0, "sata"},
	{0, "cesa"},
	{0, "nor"},
	{0, "nand"},
	{0, "spi"},
	{0, "tdm"},
	{0, "sdio"},
	{0, "lcd"},
	{0, "hwmon"},
	{0, "rtc"},
	{0, "gpio"},
	{0, "mstr"},
	{-1, "last"}
};

MV_BOOL mvUnitMapIsRsrcLimited(void)
{
	return mv_rsrc_limited;
}

MV_VOID mvUnitMapSetRsrcLimited(MV_BOOL isLimited)
{
	mv_rsrc_limited = isLimited;
}

MV_BOOL mvUnitMapIsMine(MV_SOC_UNIT unitIdx)
{
	return mv_res_table[unitIdx].isMine;
}

MV_BOOL mvUnitMapIsPexMine(int pciIf)
{
	MV_SOC_UNIT unitIdx;

	/* Map line Number to PEX unit number */
	/* This is compatible to mvCtrlSerdesPhyConfig in BoardEnvLib.c */
	if(pciIf < PEX1_0x4)
		unitIdx = PEX0;
	else if(pciIf < PEX2_0x4)
		unitIdx = PEX1;
	else if(pciIf < PEX3_0x4)
		unitIdx = PEX2;
	else
		unitIdx = PEX3;

	return mv_res_table[unitIdx].isMine;
}

MV_VOID mvUnitMapSetMine(MV_SOC_UNIT unitIdx)
{
	if (MV_TRUE == mv_res_table[unitIdx].isMine)
		return;

	mv_res_table[unitIdx].isMine = 1;
}

MV_BOOL mvUnitMapSetup(char* cmdLine, STRSTR_FUNCPTR strstr_func)
{
	int unitIdx;
	char* match;

	for (unitIdx = 0; mv_res_table[unitIdx].isMine != (-1); unitIdx++) {
		char *unitName = mv_res_table[unitIdx].unitName;
		int len;
		match = strstr_func(cmdLine, unitName);
		if (!match) continue;
		/*Look for start delimiter*/
		if (match > cmdLine) {
			if (match[-1] != ' ' && match[-1] != ':') {
				continue;
			}
		}
		/* Calc string length without using strlen() */
		while (*unitName++ != '\0');
		len = unitName - mv_res_table[unitIdx].unitName - 1;
		/*Look for end delimiter*/
		if (match[len] != ' ' && match[len] != ':' && match[len] != '\0') {
			continue;
		}
		mvUnitMapSetMine(unitIdx);
	}
	return MV_TRUE;
}

/*MV_BOOL mvSocUnitMapFillTableFormBitMap(MV_U32 flag)
{
	int i,bit,cpuId;
	for (i = 0; mv_res_table[i].cpuId != -1; i++)
	{
		switch (i)
		{
		case UART0:  bit=UART0_T0_CPU1;   break;
		case UART1:  bit=UART1_TO_CPU1;   break;
		case PEX00:  bit=PEX0_TO_CPU1;    break;
		case PEX10:  bit=PEX1_TO_CPU1;    break;
		case GIGA0:  bit=GIGA0_TO_CPU1;   break;
		case GIGA1:  bit=GIGA1_TO_CPU1;   break;
		case GIGA2:  bit=GIGA2_TO_CPU1;   break;
		case GIGA3:  bit=GIGA3_TO_CPU1;   break;
		case SATA:   bit=SATA_TO_CPU1;    break;
		case XOR:    bit=XOR_TO_CPU1;  	  break;
		case IDMA:   bit=IDMA_TO_CPU1;    break;
		case USB0:   bit=USB0_TO_CPU1;    break;
		case USB1:   bit=USB1_TO_CPU1;    break;
		case USB2:   bit=USB2_TO_CPU1;    break;
		case CESA:   bit=CESA_TO_CPU1;    break;
		case NOR_FLASH:	bit=NOR_TO_CPU1;    break;
		case NAND_FLASH: bit=NAND_TO_CPU1;    break;
		case SPI_FLASH:bit=SPI_TO_CPU1;    break;
		case TDM: bit=TDM_TO_CPU1;    break;
		default: bit=0;
			break;
		}
		//cpuId = (flag & bit) ? SLAVE_CPU:MASTER_CPU;
		mvSocUnitMapSet(i, cpuId);
	}
	return MV_TRUE;
}

MV_U32 mvSocUnitMapFillFlagFormTable(void)
{
	int i;
	MV_U32 flag = 0;
	for (i = 0; mv_res_table[i].cpuId != -1; i++)
	{
	    if (mvSocUnitMapGet(i) == 0)//SLAVE_CPU)
	    {
		switch (i)
		{
		case UART0:  flag |= UART0_T0_CPU1;   	break;
		case UART1:  flag |= UART1_TO_CPU1;   	break;
		case PEX00:  flag |= PEX0_TO_CPU1;    	break;
		case PEX10:  flag |= PEX1_TO_CPU1;    	break;
		case GIGA0:  flag |= GIGA0_TO_CPU1;   	break;
		case GIGA1:  flag |= GIGA1_TO_CPU1;   	break;
		case GIGA2:  flag |= GIGA2_TO_CPU1;   	break;
		case GIGA3:  flag |= GIGA3_TO_CPU1;   	break;
		case SATA:   flag |= SATA_TO_CPU1;    	break;
		case XOR:    flag |= XOR_TO_CPU1;	break;
		case IDMA:   flag |= IDMA_TO_CPU1;    	break;
		case USB0:   flag |= USB0_TO_CPU1;    	break;
		case USB1:   flag |= USB1_TO_CPU1;    	break;
		case USB2:   flag |= USB2_TO_CPU1;    	break;
		case CESA:   flag |= CESA_TO_CPU1;    	break;
		case NOR_FLASH: flag |= NOR_TO_CPU1;   break;
		case NAND_FLASH: flag |= NAND_TO_CPU1;   break;
		case SPI_FLASH: flag |= SPI_TO_CPU1;   break;
		case TDM: flag |= TDM_TO_CPU1;   break;
		default:
			break;
		}
	    }
	}

	return flag;
}*/
MV_VOID mvUnitMapSetAllMine()
{
	int unitIdx;
	for (unitIdx = 0; mv_res_table[unitIdx].isMine != (-1); unitIdx++) {
		mvUnitMapSetMine(unitIdx);
	}
}

MV_VOID mvUnitMapPrint()
{
	int unitIdx;
	mvOsPrintf("  AMP: Resources ");
	for (unitIdx = 0; mv_res_table[unitIdx].isMine != -1; unitIdx++) {
		if (mv_res_table[unitIdx].isMine) {
			mvOsPrintf("- %s ", mv_res_table[unitIdx].unitName);
		}
	}
	mvOsPrintf("\n");
}
