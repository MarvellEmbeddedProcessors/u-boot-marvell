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

/* includes */
#include "mv_os.h"
#include "config_marvell.h"  	/* Required to identify SOC and Board */
#include "mvUart.h"
#include "util.h"
#include "mvSiliconIf.h"
#include "generalInit.h"

#if defined(MV_MSYS_AC3)
#include "ddr3_msys_ac3.h"
#endif

#define SWITCH_MBUS_WIN_CTRL_VAL			((0x3FF << 16) | (0x3 << 4) | 0x1) /* 64MB window, Target = Switching core */
#define SWITCH_MBUS_WIN_BASE_VAL			0xA8000000
#define SWITCH_MBUS_WIN_RMAP_VAL			0x0

#define USB_MBUS_WIN_CTRL_VAL				((0xF << 16) | (0x5 << 4) | 0x1) /* 1MB window, Target = USB */
#define USB_MBUS_WIN_BASE_VAL				0xAF000000
#define USB_MBUS_WIN_RMAP_VAL				0

#define SERVER_MBUS_WIN_CTRL_VAL			(0xF0081)


/*****************************************************************************/
static inline MV_VOID mvMbusWinConfig()
{
	const MV_U32	mmuTableBase = 0x40000000;
	MV_U32			idx, mmuTableEntry, mmuTableEntryAddr;

	/* open DFX server window - required to derive Tclk for UART init (mvBoardTclkGet)
	   The server window base has been already configured by BootROM and should not be changed */
	MV_REG_WRITE(AHB_TO_MBUS_WIN_CTRL_REG(SERVER_WIN_ID), SERVER_MBUS_WIN_CTRL_VAL);

	/* Configure memory window for SERDES switch access */
	MV_REG_WRITE(AHB_TO_MBUS_WIN_BASE_REG(SWITCH_WIN_ID),  SWITCH_MBUS_WIN_BASE_VAL);
	if (AHB_TO_MBUS_WIN_REMAP_LOW_REG(SWITCH_WIN_ID)) {
		MV_REG_WRITE(AHB_TO_MBUS_WIN_REMAP_LOW_REG(SWITCH_WIN_ID),  SWITCH_MBUS_WIN_RMAP_VAL);
		MV_REG_WRITE(AHB_TO_MBUS_WIN_REMAP_HIGH_REG(SWITCH_WIN_ID), 0);
	}
	MV_REG_WRITE(AHB_TO_MBUS_WIN_CTRL_REG(SWITCH_WIN_ID),  SWITCH_MBUS_WIN_CTRL_VAL);

#if defined(MV_MSYS_AC3)
	/* Configure memory window for USB registers access */
	MV_REG_WRITE(AHB_TO_MBUS_WIN_BASE_REG(USB_WIN_ID),  USB_MBUS_WIN_BASE_VAL);
	if (AHB_TO_MBUS_WIN_REMAP_LOW_REG(USB_WIN_ID)) {
		MV_REG_WRITE(AHB_TO_MBUS_WIN_REMAP_LOW_REG(USB_WIN_ID),  USB_MBUS_WIN_RMAP_VAL);
		MV_REG_WRITE(AHB_TO_MBUS_WIN_REMAP_HIGH_REG(USB_WIN_ID), 0);
	}
	MV_REG_WRITE(AHB_TO_MBUS_WIN_CTRL_REG(USB_WIN_ID),  USB_MBUS_WIN_CTRL_VAL);
#endif

	/* Configure MMU translation table (see VMSAv7 documentation for details)
	   The MSYS BootROM uses 1 translation table entry (4 bytes) per 1MB of memory section
	   with the following address distribution:

	   0x00000000 - 0x3FFFFFFF - DRAM, 1024 descriptors (1GB)
	   0x40000000 - 0x400FFFFF - SRAM, one descriptor (1MB)
	   0x40100000 - 0x7FFFFFFF - Reserved1, 1023MB
	   0x80000000 - 0xA7FFFFFF - PCI, 640MB
	   0xA8000000 - 0xC7FFFFFF - Reserved2, 512MB
	   0xC8000000 - 0xC80FFFFF - CESA SRAM, 1MB
	   0xC8100000 - 0xCFFFFFFF - Reserved3, 127MB
	   0xD0000000 - 0xD00FFFFF - Internal registers, 1MB
	   0xD0100000 - 0xD01FFFFF - DFX server, 1MB
	   0xD0200000 - 0xD37FFFFF - Reserved4, 55MB
	   0xD3800000 - 0xFFEFFFFF - Device bus, 711MB
	   0xFFF00000 - 0xFFFFFFFF - BootROM, 1MB

	   The Switch memory window fits the Reserved2 region (started from 2688th descriptor),
	   which has to be switched from "not accessible" memory type to "strongly ordered, privileged RW access" one
	   */
	mmuTableEntryAddr = mmuTableBase + (SWITCH_MBUS_WIN_BASE_VAL >> 18); /* Should be inside the Reserved2 region */
	/* Fill in 64 descriptors for 64MB switch window */
	for (idx = 0; idx < 64; idx++) {
		mmuTableEntry = MV_MEMIO_LE32_READ(mmuTableEntryAddr);
		mmuTableEntry &= ~0xFFF;
		mmuTableEntry |= 0x402; /* Set section access permissions for privileged access */
		MV_MEMIO_LE32_WRITE(mmuTableEntryAddr, mmuTableEntry);
		mmuTableEntryAddr +=  4;
	}

#if defined(MV_MSYS_AC3)
	/* Fill in single descriptor for 1MB USB window */
	mmuTableEntryAddr = mmuTableBase + (USB_MBUS_WIN_BASE_VAL >> 18); /* Should be inside the Reserved2 region */
	mmuTableEntry = MV_MEMIO_LE32_READ(mmuTableEntryAddr);
	mmuTableEntry &= ~0xFFF;
	mmuTableEntry |= 0x402; /* Set section access permissions for privileged access */
	MV_MEMIO_LE32_WRITE(mmuTableEntryAddr, mmuTableEntry);
#endif

	/* Invalidate the TLB for re-loading the translation table */
	asm (
		"mov	r1, #0\n\t"
		"mcr	p15, 0, r1, c8, c7, 0\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop"
	);
}

/*****************************************************************************/
MV_STATUS mvGeneralInit(void)
{
	mvMbusWinConfig();
#if !defined(MV_NO_PRINT)
	mvUartInit();
	DEBUG_INIT_S("\n\nGeneral initialization - Version: " GENERAL_VERION "\n");
#endif

	return MV_OK;
}
