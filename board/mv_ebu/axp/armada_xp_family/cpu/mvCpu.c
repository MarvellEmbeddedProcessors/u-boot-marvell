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

#include "cpu/mvCpu.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "ctrlEnv/mvCtrlEnvRegs.h"
#include "ctrlEnv/sys/mvCpuIfRegs.h"
#include "boardEnv/mvBoardEnvLib.h"

/* defines  */
#ifdef MV_DEBUG
#define DB(x)	x
#else
#define DB(x)
#endif

/* locals */

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
*       32bit clock cycles in MHertz.
*
*******************************************************************************/
MV_U32 mvCpuPclkGet(MV_VOID)
{
	MV_U32 idx;
	MV_U32 cpuClk[] = MV_CPU_CLK_TBL;

	if (mvBoardIdGet() == FPGA_88F78XX0_ID)
		return MV_FPGA_CLK; /* FPGA is limited to 25Mhz */

	idx = MSAR_CPU_CLK_IDX(MV_REG_READ(MPP_SAMPLE_AT_RESET(0)), MV_REG_READ(MPP_SAMPLE_AT_RESET(1)));

	return cpuClk[idx] * 1000000;
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

	if (mvBoardIdGet() == FPGA_88F78XX0_ID)
		return MV_FPGA_CLK; /* FPGA is limited to 25Mhz */

	idx = MSAR_DDR_L2_CLK_RATIO_IDX(MV_REG_READ(MPP_SAMPLE_AT_RESET(0)), MV_REG_READ(MPP_SAMPLE_AT_RESET(1)));

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

/*******************************************************************************
* mvCpuNameGet - Get CPU name
*
* DESCRIPTION:
*       This function returns a string describing the CPU model and revision.
*
* INPUT:
*       None.
*
* OUTPUT:
*       pNameBuff - Buffer to contain board name string. Minimum size 32 chars.
*
* RETURN:
*       None.
*******************************************************************************/
MV_VOID mvCpuNameGet(char *pNameBuff)
{
	MV_U32 cpuModel;
	MV_U32 archType;

	cpuModel = mvOsCpuPartGet();
	archType = mvOsCpuThumbEEGet();
	/* The CPU module is indicated in the Processor Version Register (PVR) */
	switch (cpuModel & 0xfff) {
	case CPU_PART_ARM_V6UP:
	case CPU_PART_ARM_V7UP:
	case CPU_PART_MRVLPJ4B_UP:
		if (archType == 0x1)
			mvOsSPrintf(pNameBuff, "%s (Rev %d)", "Marvell PJ4B (581) v7", mvOsCpuRevGet());
		else
			mvOsSPrintf(pNameBuff, "%s (Rev %d)", "Marvell PJ4B (581) v6", mvOsCpuRevGet());
		break;
	case CPU_PART_MRVLPJ4B_MP:
	case CPU_PART_ARM_V6MP:
			if (archType == 0x1)
			mvOsSPrintf(pNameBuff, "%s (Rev %d)", "Marvell PJ4B (584) v7", mvOsCpuRevGet());
		else
			mvOsSPrintf(pNameBuff, "%s (Rev %d)", "Marvell PJ4B (584) v6", mvOsCpuRevGet());
		break;
	default:
		mvOsSPrintf(pNameBuff, "??? (0x%04x) (Rev %d)", cpuModel, mvOsCpuRevGet());
		break;
	}			/* switch  */

	return;
}

#define MV_PROC_STR_SIZE 50

static void mvCpuIfGetL2EccMode(MV_8 *buf)
{
	MV_U32 regVal = MV_REG_READ(CPU_L2_AUX_CTRL_REG);
	if ((regVal & CL2ACR_ECC_MASK) == CL2ACR_ECC_EN)
		mvOsSPrintf(buf, "L2 ECC Enabled");
	else
		mvOsSPrintf(buf, "L2 ECC Disabled");
}

static void mvCpuIfGetL2ParityMode(MV_8 *buf)
{
	MV_U32 regVal = MV_REG_READ(CPU_L2_AUX_CTRL_REG);
	if ((regVal & CL2ACR_PARITY_MASK) == CL2ACR_PARITY_EN)
		mvOsSPrintf(buf, "L2 Parity Enabled");
	else
		mvOsSPrintf(buf, "L2 Parity Disabled");
}

static void mvCpuIfGetL2Mode(MV_8 *buf)
{
	MV_U32 regVal = MV_REG_READ(CPU_L2_CTRL_REG);
	if (regVal & CL2CR_L2_EN_MASK)
		mvOsSPrintf(buf, "L2 Enabled");
	else
		mvOsSPrintf(buf, "L2 Disabled");
}

static void mvCpuIfGetL2PrefetchMode(MV_8 *buf)
{				/* valid for PJ4B as well */
	MV_U32 regVal = MV_REG_READ(CPU_L2_AUX_CTRL_REG);
	if ((regVal & CL2ACR_PFU_MASK) == CL2ACR_PFU_DIS)
		mvOsSPrintf(buf, "L2 Prefetch Disabled");
	else
		mvOsSPrintf(buf, "L2 Prefetch Enabled");
}

static void mvCpuIfGetWriteAllocMode(MV_8 *buf)
{
	MV_U32 regVal = MV_REG_READ(CPU_L2_AUX_CTRL_REG);
	if ((regVal & CL2ACR_FORCE_WA_MASK) == CL2ACR_FORCE_NO_WA)
		mvOsSPrintf(buf, "L2 Write Allocate Disabled");
	else
		mvOsSPrintf(buf, "L2 Write Allocate Enabled");
}

static void mvCpuIfGetCpuStreamMode(MV_8 *buf)
{				/* valid for PJ4B as well */
	MV_U32 regVal = 0;
	__asm volatile ("mrc p15, 1, %0, c15, c1, 0" : "=r" (regVal));	/* Read Marvell extra features register */
	if (regVal & BIT29)
		mvOsSPrintf(buf, "CPU Streaming Enabled");
	else
		mvOsSPrintf(buf, "CPU Streaming Disabled");
}

static void mvCpuIfPrintCpuRegs(void)
{
	MV_U32 regVal = 0;

	__asm volatile ("mrc p15, 1, %0, c15, c1, 0" : "=r" (regVal));	/* Read Marvell extra features register0 */
	mvOsPrintf("Extra Features Reg[0] = 0x%x\n", regVal);

	__asm volatile ("mrc p15, 1, %0, c15, c1, 1" : "=r" (regVal));	/* Read Marvell extra features register1 */
	mvOsPrintf("Extra Features Reg[1] = 0x%x\n", regVal);

	__asm volatile ("mrc	p15, 0, %0, c1, c0, 0" : "=r" (regVal));	/* Read Control register */
	mvOsPrintf("Control Reg = 0x%x\n", regVal);

	__asm volatile ("mrc	p15, 0, %0, c0, c0, 0" : "=r" (regVal));	/* Read Main ID register */
	mvOsPrintf("Main ID Reg = 0x%x\n", regVal);

	__asm volatile ("mrc	p15, 0, %0, c0, c0, 1" : "=r" (regVal));	/* Read Cache Type register */
	mvOsPrintf("Cache Type Reg = 0x%x\n", regVal);

	regVal = MV_REG_READ(CPU_L2_CTRL_REG);
	mvOsPrintf("L2 Control Reg = 0x%x\n", regVal);

	regVal = MV_REG_READ(CPU_L2_AUX_CTRL_REG);
	mvOsPrintf("L2 Auxilary Control Reg = 0x%x\n", regVal);

}

MV_U32 mvCpuIfPrintSystemConfig(MV_8 *buffer, MV_U32 index)
{
	MV_U32 count = 0;

	MV_8 L2_ECC_str[MV_PROC_STR_SIZE];
	MV_8 L2_En_str[MV_PROC_STR_SIZE];
	MV_8 L2_Prefetch_str[MV_PROC_STR_SIZE];
	MV_8 Write_Alloc_str[MV_PROC_STR_SIZE];
	MV_8 Cpu_Stream_str[MV_PROC_STR_SIZE];
	MV_8 L2_Parity_str[MV_PROC_STR_SIZE];

	mvCpuIfGetL2Mode(L2_En_str);
	mvCpuIfGetL2EccMode(L2_ECC_str);
	mvCpuIfGetL2ParityMode(L2_Parity_str);
	mvCpuIfGetL2PrefetchMode(L2_Prefetch_str);
	mvCpuIfGetWriteAllocMode(Write_Alloc_str);
	mvCpuIfGetCpuStreamMode(Cpu_Stream_str);
	mvCpuIfPrintCpuRegs();

	count += mvOsSPrintf(buffer + count + index, "%s\n", L2_En_str);
	count += mvOsSPrintf(buffer + count + index, "%s\n", L2_ECC_str);
	count += mvOsSPrintf(buffer + count + index, "%s\n", L2_Parity_str);
	count += mvOsSPrintf(buffer + count + index, "%s\n", L2_Prefetch_str);
	count += mvOsSPrintf(buffer + count + index, "%s\n", Write_Alloc_str);
	count += mvOsSPrintf(buffer + count + index, "%s\n", Cpu_Stream_str);
	return count;
}

/*******************************************************************************
* whoAmI - Get the CPU ID
*
* DESCRIPTION:
*       This function returns CPU ID in multiprocessor system
*
* INPUT:
*       None.
*
* OUTPUT:
*       none.
*
* RETURN:
*       CPU ID.
*******************************************************************************/
unsigned int whoAmI(void)
{
	MV_U32 value;

	__asm__ __volatile__("mrc p15, 0, %0, c0, c0, 5   @ read CPUID reg\n" : "=r"(value) : : "memory");
	return (value & 0x7);
}
