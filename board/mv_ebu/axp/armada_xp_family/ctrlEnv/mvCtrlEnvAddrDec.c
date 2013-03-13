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

/*******************************************************************************
* mvCtrlEnvAddrDec.h - Marvell controller address decode library
*
* DESCRIPTION:
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

/* includes */
#include "mvCommon.h"
#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "ctrlEnv/mvCtrlEnvAddrDec.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "sys/mvCpuIf.h"

#undef MV_DEBUG

/* defines  */
#ifdef MV_DEBUG
#define DB(x)	x
#else
#define DB(x)
#endif

/* Default Attributes array */
MV_TARGET_ATTRIB mvTargetDefaultsArray[] = TARGETS_DEF_ARRAY;
extern MV_TARGET sampleAtResetTargetArray[];

/*******************************************************************************
* mvCtrlAttribGet -
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
*
*******************************************************************************/
MV_STATUS mvCtrlAttribGet(MV_TARGET target, MV_TARGET_ATTRIB *targetAttrib)
{
	targetAttrib->attrib = mvTargetDefaultsArray[MV_CHANGE_BOOT_CS(target)].attrib;
	targetAttrib->targetId = mvTargetDefaultsArray[MV_CHANGE_BOOT_CS(target)].targetId;

	return MV_OK;
}
/*******************************************************************************/
MV_STATUS mvCtrlAttribSet(MV_TARGET target, MV_TARGET_ATTRIB *targetAttrib)
{
	mvTargetDefaultsArray[MV_CHANGE_BOOT_CS(target)].attrib  = targetAttrib->attrib;
	mvTargetDefaultsArray[MV_CHANGE_BOOT_CS(target)].targetId= targetAttrib->targetId;

	return MV_OK;
}

/*******************************************************************************
* mvCtrlGetAttrib -
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
*
*******************************************************************************/
MV_TARGET mvCtrlTargetGet(MV_TARGET_ATTRIB *targetAttrib)
{
	MV_TARGET target;
	MV_TARGET x;
	for (target = SDRAM_CS0; target < MAX_TARGETS; target++) {
		x = MV_CHANGE_BOOT_CS(target);
		if ((mvTargetDefaultsArray[x].attrib == targetAttrib->attrib) &&
		    (mvTargetDefaultsArray[MV_CHANGE_BOOT_CS(target)].targetId == targetAttrib->targetId)) {
			/* found it */
			break;
		}
	}

	return target;
}

/*******************************************************************************
* mvCtrlTargetByWinInfoGet -
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
*
*******************************************************************************/
MV_TARGET mvCtrlTargetByWinInfoGet(MV_UNIT_WIN_INFO *unitWinInfo)
{
	MV_TARGET target;
	MV_TARGET x;
	for (target = SDRAM_CS0; target < MAX_TARGETS; target++) {
		x = MV_CHANGE_BOOT_CS(target);
		if ((mvTargetDefaultsArray[x].attrib == unitWinInfo->attrib) &&
		    (mvTargetDefaultsArray[MV_CHANGE_BOOT_CS(target)].targetId == unitWinInfo->targetId)) {
			/* found it */
			break;
		}
	}

	return target;
}

/*******************************************************************************
* mvCtrlAddrWinMapBuild
*
* DESCRIPTION:
*	Build the windows address decoding table, to be used for initializing
*	the unit's address decoding windows.
*
* INPUT:
*	pAddrWinMap: An array to hold the address decoding windows parameters.
*	len: Number of entries in pAddrWinMap.
*
* OUTPUT:
*	pAddrWinMap: Address window information.
*
* RETURN:
*	MV_BAD_PARAM: input array is smaller than needed to store all window
*	addresses.
*	MV_ERROR: Otherwise.
*
*******************************************************************************/
MV_STATUS mvCtrlAddrWinMapBuild(MV_UNIT_WIN_INFO *pAddrWinMap, MV_U32 len)
{
	MV_CPU_DEC_WIN cpuAddrDecWin;
	MV_U32 i, j;
	MV_TARGET_ATTRIB targetAttrib;
	MV_STATUS status;
	MV_U64 startAddr, endAddr;
	MV_UNIT_WIN_INFO ioDdrWin[MV_DRAM_MAX_CS];
	MV_U32 base;
	MV_U64 size;

	/* Check size of CPU address win table */
	if (len <= MAX_TARGETS) {
		mvOsPrintf("mvCtrlAddrWinMapBuild() - Table size too small.\n");
		return MV_BAD_PARAM;
	}

	/* Prepare an array of DRAM info */
	base = 0x0;
	j = 0;
	for (i = SDRAM_CS0; i <= SDRAM_CS3; i++) {
		status = mvCpuIfTargetWinGet(i, &cpuAddrDecWin);
		if (status != MV_OK) {
			if (status == MV_NO_SUCH) {
				ioDdrWin[i].enable = MV_FALSE;
				continue;
			} else {
				mvOsPrintf("mvCtrlAddrWinMapBuild() - mvCpuIfTargetWinGet() failed.\n");
				return MV_ERROR;
			}
		}

		/* As all IO address decode windows support only 32-bit
		** addresses, limit the DRAM base / size to 4GB max.
		*/
		startAddr = (MV_U64)((((MV_U64)cpuAddrDecWin.addrWin.baseHigh << 32ll)) +
				(MV_U64)cpuAddrDecWin.addrWin.baseLow);
		endAddr = (MV_U64)(startAddr + (MV_U64)cpuAddrDecWin.addrWin.size) - 1;
		if (endAddr > 0xFFFFFFFFll) {
			if (startAddr <= 0xFFFFFFFFll)
				cpuAddrDecWin.addrWin.size = (0x100000000ll -
						cpuAddrDecWin.addrWin.baseLow);
			else
				cpuAddrDecWin.enable = MV_FALSE;
		}

		if (cpuAddrDecWin.enable == MV_FALSE)
			continue;

		/* If the endAddr passes minBase, then we need to split
		** this window to several windows up to minBase.
		** For example: minBase=0xE0000000, and CS0=2, CS1=2G,
		** Then we need to split the windwos as follows:
		** Win0: CS-0, 2GB (Base 0x0)
		** win1: CS-1, 1GB (Base 0x80000000)
		** Win2: CS-1, 0.5GB (Base 0xC0000000)
		*/
		if (endAddr > MV_DRAM_IO_RESERVE_BASE)
			/* Need to cut down this CS to IO reserve base
			** address.
			*/
			size = MV_DRAM_IO_RESERVE_BASE -
				cpuAddrDecWin.addrWin.baseLow;
		else
			size = cpuAddrDecWin.addrWin.size;

		if (mvCtrlAttribGet(i, &targetAttrib) != MV_OK) {
			mvOsPrintf("mvCtrlAddrWinMapBuild() - "
					"mvCtrlAttribGet() failed.\n");
			return MV_ERROR;
		}
		/* Now, spread the last CS into several windows, and make sure
		** that each of has a power-of-2 size.
		*/
		while (size != 0) {
			ioDdrWin[j].enable = MV_TRUE;
			ioDdrWin[j].attrib = targetAttrib.attrib;
			ioDdrWin[j].targetId = targetAttrib.targetId;
			ioDdrWin[j].addrWin.baseHigh = 0;
			if (MV_IS_POWER_OF_2(size))
				ioDdrWin[j].addrWin.size = size;
			else
				ioDdrWin[j].addrWin.size = (MV_U64)(1ll << (MV_U64)mvLog2(size));
			size -= ioDdrWin[j].addrWin.size;
			ioDdrWin[j].addrWin.baseLow = base;
			base += ioDdrWin[j].addrWin.size;
			j++;
		}
		/* Support only up to 4 DRAM address decode windows in the
		** units. */
		if (j == MV_DRAM_MAX_CS)
			break;
	}

	for (; j < MV_DRAM_MAX_CS; j++)
		ioDdrWin[j].enable = MV_FALSE;

	/* Fill in the pAddrWinMap fields       */
	for (i = 0; i < MAX_TARGETS; i++) {
		if (MV_TARGET_IS_DRAM(i)) {
			pAddrWinMap[i].addrWin.baseLow = ioDdrWin[i].addrWin.baseLow;
			pAddrWinMap[i].addrWin.baseHigh = ioDdrWin[i].addrWin.baseHigh;
			pAddrWinMap[i].addrWin.size = ioDdrWin[i].addrWin.size;
			pAddrWinMap[i].enable = ioDdrWin[i].enable;
			pAddrWinMap[i].attrib = ioDdrWin[i].attrib;
			pAddrWinMap[i].targetId = ioDdrWin[i].targetId;
		} else {
			status = mvCpuIfTargetWinGet(i, &cpuAddrDecWin);
			if (status != MV_OK) {
				if (status == MV_NO_SUCH) {
					pAddrWinMap[i].enable = MV_FALSE;
					continue;
				} else {
					mvOsPrintf("mvCtrlAddrWinMapBuild()"
					" - mvCpuIfTargetWinGet() failed.\n");
					return MV_ERROR;
				}
			}

			pAddrWinMap[i].addrWin.baseLow =
				cpuAddrDecWin.addrWin.baseLow;
			pAddrWinMap[i].addrWin.baseHigh =
				cpuAddrDecWin.addrWin.baseHigh;
			pAddrWinMap[i].addrWin.size =
				cpuAddrDecWin.addrWin.size;
			pAddrWinMap[i].enable = cpuAddrDecWin.enable;

			if (mvCtrlAttribGet(i, &targetAttrib) != MV_OK) {
				mvOsPrintf("mvCtrlAddrWinMapBuild() - "
						"mvCtrlAttribGet() failed.\n");
				return MV_ERROR;
			}
			pAddrWinMap[i].attrib = targetAttrib.attrib;
			pAddrWinMap[i].targetId = targetAttrib.targetId;
		}
	}
	pAddrWinMap[i].addrWin.baseLow = TBL_TERM;
	pAddrWinMap[i].addrWin.baseHigh = TBL_TERM;
	pAddrWinMap[i].addrWin.size = TBL_TERM;
	pAddrWinMap[i].enable = TBL_TERM;
	pAddrWinMap[i].attrib = TBL_TERM;
	pAddrWinMap[i].targetId = TBL_TERM;

	return MV_OK;
}

MV_STATUS mvCtrlAddrWinInfoGet(MV_UNIT_WIN_INFO *pAddrWinInfo, MV_ULONG physAddr)
{
	MV_CPU_DEC_WIN cpuAddrDecWin;
	MV_U32 i;
	MV_TARGET_ATTRIB targetAttrib;
	MV_STATUS status;

	for (i = 0; i < MAX_TARGETS; i++) {
		status = mvCpuIfTargetWinGet(i, &cpuAddrDecWin);
		if (status != MV_OK)
			continue;

		if ((physAddr >= cpuAddrDecWin.addrWin.baseLow) &&
		    (physAddr < cpuAddrDecWin.addrWin.baseLow + cpuAddrDecWin.addrWin.size)) {
			/* Found */
			pAddrWinInfo->addrWin.baseLow = cpuAddrDecWin.addrWin.baseLow;
			pAddrWinInfo->addrWin.baseHigh = cpuAddrDecWin.addrWin.baseHigh;
			pAddrWinInfo->addrWin.size = cpuAddrDecWin.addrWin.size;

			if (mvCtrlAttribGet(i, &targetAttrib) != MV_OK) {
				mvOsPrintf("mvCtrlAddrWinMapBuild() - mvCtrlAttribGet() failed.\n");
				return MV_ERROR;
			}
			pAddrWinInfo->attrib = targetAttrib.attrib;
			pAddrWinInfo->targetId = targetAttrib.targetId;
			return MV_OK;
		}
	}
	/* not found */
	return MV_NOT_FOUND;
}
