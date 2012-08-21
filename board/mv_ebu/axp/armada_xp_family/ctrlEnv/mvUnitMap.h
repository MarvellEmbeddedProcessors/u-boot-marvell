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
#ifndef mvUnitMap_h
#define mvUnitMap_h

#ifdef MV_VXWORKS
#include "common/mvTypes.h"
#include "config.h"
#endif

typedef enum
{
	UART0=0,
	UART1,
	PEX0,
	PEX1,
	PEX2,
	PEX3,
	ETH0,
	ETH1,
	ETH2,
	ETH3,
	XOR0,
	XOR1,
	USB0,
	USB1,
	USB2,
	I2C0,
	I2C1,
	SATA,
	CESA,
	NOR,
	NAND,
	SPI,
	TDM,
	SDIO,
	LCD,
	HWMON,
	RTC,
	GPIO,
	MSTR,
	MAX_UNITS
} MV_SOC_UNIT;

/* binary flags for mvSocUnitMapFillTableFormBitMap */
/*#define UART0_T0_CPU1	0x0001
#define UART1_TO_CPU1	0x0002
#define PEX0_TO_CPU1	0x0004
#define PEX1_TO_CPU1	0x0008
#define GIGA0_TO_CPU1	0x0010
#define GIGA1_TO_CPU1	0x0020
#define GIGA2_TO_CPU1	0x0040
#define GIGA3_TO_CPU1	0x0080
#define SATA_TO_CPU1	0x0100
#define XOR_TO_CPU1		0x0200
#define IDMA_TO_CPU1	0x0400
#define USB0_TO_CPU1	0x0800
#define USB1_TO_CPU1    0x1000
#define USB2_TO_CPU1    0x2000
#define CESA_TO_CPU1	0x4000
#define NOR_TO_CPU1		0x8000
#define NAND_TO_CPU1	0x10000
#define SPI_TO_CPU1		0x20000
#define TDM_TO_CPU1		0x40000

#define CPU1_DEFAULT_INTERFACE (UART1_TO_CPU1 | PEX1_TO_CPU1 | GIGA2_TO_CPU1 | GIGA3_TO_CPU1 | IDMA_TO_CPU1 | USB1_TO_CPU1)*/

typedef struct __MV_RES_MAP
{
	int	isMine;
	char*	unitName;
} MV_RES_MAP;

typedef char *(*STRSTR_FUNCPTR)(const char *s1, const char *s2);

#ifdef CONFIG_MV_AMP_ENABLE

MV_BOOL mvUnitMapIsMine(MV_SOC_UNIT unitIdx);
MV_BOOL mvUnitMapIsPexMine(int pciIf);
MV_VOID mvUnitMapSetMine(MV_SOC_UNIT unitIdx);
MV_BOOL mvUnitMapSetup(char* p, STRSTR_FUNCPTR strstr_func);
MV_VOID mvUnitMapSetAllMine(void);
MV_VOID mvUnitMapPrint(void);
MV_BOOL mvUnitMapIsRsrcLimited(void);
MV_VOID mvUnitMapSetRsrcLimited(MV_BOOL isLimited);
#else /* CONFIG_MV_AMP_ENABLE */
#define mvUnitMapIsMine(rsrc) 		MV_TRUE
#define mvUnitMapIsPexMine(pciIf) 	MV_TRUE
#define mvUnitMapIsRsrcLimited 	  	MV_TRUE
#define mvUnitMapSetRsrcLimited(limit)
#define mvUnitMapSetMine(rsrc)
#define mvUnitMapSetAllMine
#define mvUnitMapPrint
#define mvUnitMapSetup(str, strstr_func) MV_TRUE
#endif /* CONFIG_MV_AMP_ENABLE */

#endif /* mvUnitMap_h */
