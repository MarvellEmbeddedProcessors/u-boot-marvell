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

#define	_ASMLANGUAGE
#define MV_ASMLANGUAGE
#include "mvSysHwConfig.h"
#include "mvOsAsm.h"
#include "mvBoardEnvSpec.h"
#include "mvCpuIfRegs.h"
#include "mvDramIfConfig.h"
#include "mvDramIfRegs.h"
#include "pex/mvPexRegs.h"
#include "mvCtrlEnvSpec.h"
#include "mvCtrlEnvAsm.h"
#include "mvCommon.h"

/* defines */

#if defined(MV_STATIC_DRAM_ON_BOARD)

.globl dramBoot1
dramBoot1:
        .word   0

	/* For   1GB DDR3 SODIM */
	#define STATIC_SDRAM_CONFIG	     	0x7301C0C3 /*	0x1400  */
	#define STATIC_DUNIT_CTRL_LOW		0x3630B800 /*   0x1404  */
	#define STATIC_SDRAM_TIME_CTRL_LOW	0x43149887 /*   0x1408  */ /* 1600 => 0x5415baab */
	#define STATIC_SDRAM_TIME_CTRL_HI	0x38583F98 /*   0x140c  */ /* 1600 => 0x38d83fff */
	#define STATIC_SDRAM_ADDR_CTRL		0x100F0000 /*   0x1410  */ /* 1600 => 0x110F0000 */
	#define STATIC_DUNIT_CTRL_HI		0x0000d3ff /*   0x1424  default */
	#define	STATIC_DDR_ODT_TIMING_LOW	0x000f8830 /*   0x1428  New */
	#define	STATIC_DDR3_TIMING		0x014c2f38 /*   0x142C  New */
	#define STATIC_SDRAM_DDR2_TIMING_HI	0x0000c671 /*   0x147C  default */
	#define STATIC_SDRAM_DUNIT_ODT_CTRL    	0x00000001 /*   0x149c  */
	#define STATIC_DRAM_AXI_CTRL		0x00000101 /*   0x14a8  */
	#define	STATIC_DDR_IO			0x00008800 /*   0x1524  New */
	#define	STATIC_DDR_DFS			0x00032af0 /*   0x1528  New */
	#define	STATIC_DDR_MR0			0x00000620 /*   0x15D0  New - MR0 */
	#define	STATIC_DDR_MR1			0x00000001 /*   0x15D4  New - MR1 */
	#define	STATIC_DDR_MR2			0x00000008 /*   0x15D8  New - MR2 */
	#define STATIC_DDR_MR3			0x00000000 /*   0x15DC  New - MR3 */
	#define	STATIC_DDR3_RANK_CTRL		0x00000001 /*   0x15E0  New */
	#define	STATIC_ZQC_CONFIG		0x00203C18 /*   0x15E4  New - ZQC Config */
	#define STATIC_DRAM_PHY_CONFIG		0xD8000025 /*   0x15EC  - SBen */


	#define	STATIC_MAIN_PADS_CALIB_CTRL	0x3d090009 /*   0X14cc  */
	#define	STATIC_TRAINING_DEBUG_3		0x3d090009 /*   0X15C8  */


	/* T-Domain */
	#define	STATIC_READ_DATA_SMPL_DLY	0x00000808 /*   0x1538  New */
	#define	STATIC_READ_DATA_RDY_DLY	0x00000d0d /*   0x153C  New */
	/* Phy */
	#define	STATIC_PHY_LOCK_MASK		0x00000fff /*   0x1670  New */
	/* DLB */
	#define STATIC_DRAM_DLB_CONTROL		0x2000005D /*   0x1700  */
	#define STATIC_DRAM_DLB_BUS_OPT_WEIT	0x0018C01E /*   0x1704  */
	#define STATIC_DRAM_DLB_AGING		0x00F7F07F /*   0x1708  */
	#define STATIC_DRAM_DLB_EVICT_CTRL	0x0000008D /*   0x170C  */
	#define STATIC_DRAM_DLB_EVICT_TIMR	0x00FF3C1F /*   0x1710  */
	#define STATIC_DRAM_DLB_USR_CMD		0x00000000 /*   0x1714  */

	/* CIB 2to1 */
	#define CDI_CONFIG                      0x00000007 /*   0x20220 */
	#define CDI_CONFIG2                     0x00000001 /*   0x20220 */

	.globl _mvDramIfStaticInit
_mvDramIfStaticInit:

	mov     r11, LR     		/* Save link register */
	mov	r10, r2

	ldr	r6, = STATIC_SDRAM_CONFIG
	MV_32BIT_WRITE_ASM(r6, r5, 0xf1001400)
	ldr	r6, = STATIC_DUNIT_CTRL_LOW
	MV_32BIT_WRITE_ASM(r6, r5, 0xf1001404)
	ldr	r6, = STATIC_SDRAM_TIME_CTRL_LOW
	MV_32BIT_WRITE_ASM(r6, r5, 0xf1001408)
	ldr	r6, = STATIC_SDRAM_TIME_CTRL_HI
	MV_32BIT_WRITE_ASM(r6, r5, 0xf100140C)
	ldr	r6, = STATIC_SDRAM_ADDR_CTRL
	MV_32BIT_WRITE_ASM(r6, r5, 0xf1001410)
	ldr	r6, = STATIC_DUNIT_CTRL_HI
	MV_32BIT_WRITE_ASM(r6, r5, 0xf1001424)
	ldr	r6, = STATIC_DDR_ODT_TIMING_LOW
	MV_32BIT_WRITE_ASM(r6, r5, 0xf1001428)
	ldr	r6, = STATIC_DDR3_TIMING
	MV_32BIT_WRITE_ASM(r6, r5, 0xf100142C)

	ldr	r6, = STATIC_SDRAM_DDR2_TIMING_HI
	MV_32BIT_WRITE_ASM(r6, r5, 0xf100147C)
	ldr	r6, = STATIC_DRAM_AXI_CTRL
	MV_32BIT_WRITE_ASM(r6, r5, 0xf10014A8)
	ldr	r6, = STATIC_DDR_IO
	MV_32BIT_WRITE_ASM(r6, r5, 0xf1001524)
	ldr	r6, = STATIC_DDR_DFS
	MV_32BIT_WRITE_ASM(r6, r5, 0xf1001528)


	/* T-Domain */
	ldr	r6, = STATIC_READ_DATA_SMPL_DLY
	MV_32BIT_WRITE_ASM(r6, r5, 0xf1001538)
	ldr	r6, = STATIC_READ_DATA_RDY_DLY
	MV_32BIT_WRITE_ASM(r6, r5, 0xf100153C)

	ldr	r6, = STATIC_DDR_MR0
	MV_32BIT_WRITE_ASM(r6, r5, 0xf10015D0)
	ldr	r6, = STATIC_DDR_MR1
	MV_32BIT_WRITE_ASM(r6, r5, 0xf10015D4)
	ldr	r6, = STATIC_DDR_MR2
	MV_32BIT_WRITE_ASM(r6, r5, 0xf10015D8)
	ldr	r6, = STATIC_DDR_MR3
	MV_32BIT_WRITE_ASM(r6, r5, 0xf10015DC)
	ldr	r6, = STATIC_DDR3_RANK_CTRL
	MV_32BIT_WRITE_ASM(r6, r5, 0xf10015E0)
	ldr	r6, = STATIC_ZQC_CONFIG
	MV_32BIT_WRITE_ASM(r6, r5, 0xf10015E4)
	ldr	r6, = STATIC_DRAM_PHY_CONFIG
	MV_32BIT_WRITE_ASM(r6, r5, 0xf10015EC)


	ldr	r6, = STATIC_PHY_LOCK_MASK
	MV_32BIT_WRITE_ASM(r6, r5, 0xf1001670)

	/* Phase Select */
	ldr	r6, = 0xC8003400
	ldr	r2, = FPGA_DDR3_DELAY_PHASE
	MV_32BIT_WRITE_ASM(r6, r5, 0xf10016A0)
	ldr	r6, = 0xC8043400
	MV_32BIT_WRITE_ASM(r6, r5, 0xf10016A0)
	ldr	r6, = 0xC8000000
	orr	r6, r6, r2
	MV_32BIT_WRITE_ASM(r6, r5, 0xf10016A0)
	ldr	r6, = 0xC8010000
	orr	r6, r6, r2
	MV_32BIT_WRITE_ASM(r6, r5, 0xf10016A0)
	ldr	r6, = 0xC8020000
	orr	r6, r6, r2
	MV_32BIT_WRITE_ASM(r6, r5, 0xf10016A0)
	ldr	r6, = 0xC8030000
	orr	r6, r6, r2
	MV_32BIT_WRITE_ASM(r6, r5, 0xf10016A0)
	ldr	r6, = 0xC8040000
	orr	r6, r6, r2
	MV_32BIT_WRITE_ASM(r6, r5, 0xf10016A0)
	ldr	r6, = 0xC8050000
	orr	r6, r6, r2
	MV_32BIT_WRITE_ASM(r6, r5, 0xf10016A0)
	ldr	r6, = 0xC8060000
	orr	r6, r6, r2
	MV_32BIT_WRITE_ASM(r6, r5, 0xf10016A0)
	ldr	r6, = 0xC8070000
	orr	r6, r6, r2
	MV_32BIT_WRITE_ASM(r6, r5, 0xf10016A0)
	ldr	r6, = 0xC8090000
	orr	r6, r6, r2
	MV_32BIT_WRITE_ASM(r6, r5, 0xf10016A0)
	ldr	r6, = 0xC80a0000
	orr	r6, r6, r2
	MV_32BIT_WRITE_ASM(r6, r5, 0xf10016A0)

#ifdef MV88F66XX
	ldr	r6, = STATIC_MAIN_PADS_CALIB_CTRL
	MV_32BIT_WRITE_ASM(r6, r5, 0xf10014cc)	
	ldr	r6, = STATIC_TRAINING_DEBUG_3
	MV_32BIT_WRITE_ASM(r6, r5, 0xf10015C8)	
#else
	 /* DLB */
	/* Enable DLB in DSMP-A0 */
	ldr	r6, = STATIC_DRAM_DLB_CONTROL
	MV_32BIT_WRITE_ASM(r6, r5, 0xf1001700)
	ldr	r6, = STATIC_DRAM_DLB_BUS_OPT_WEIT
	MV_32BIT_WRITE_ASM(r6, r5, 0xf1001704)
	ldr	r6, = STATIC_DRAM_DLB_AGING
	MV_32BIT_WRITE_ASM(r6, r5, 0xf1001708)
	ldr	r6, = STATIC_DRAM_DLB_EVICT_CTRL
	MV_32BIT_WRITE_ASM(r6, r5, 0xf100170C)
	ldr	r6, = STATIC_DRAM_DLB_EVICT_TIMR
	MV_32BIT_WRITE_ASM(r6, r5, 0xf1001710)
	ldr	r6, = STATIC_DRAM_DLB_USR_CMD
	MV_32BIT_WRITE_ASM(r6, r5, 0xf1001714)
	ldr	r6, = 0x00010F0F
	MV_32BIT_WRITE_ASM(r6, r5, 0xf102042C)
#endif

	/* CDI Config  2to1 */
	ldr	r6, = CDI_CONFIG
	MV_32BIT_WRITE_ASM(r6, r5, 0xF1020220)

#ifdef MV88F66XX
	ldr	r6, = CDI_CONFIG2
	MV_32BIT_WRITE_ASM(r6, r5, 0xF1020220)
#endif

     	/*DDR SDRAM Initialization Control */
	ldr	r6, =DSICR_INIT_EN
	MV_REG_WRITE_ASM (r6, r1, DDR_SDRAM_INIT_CTRL_REG)
2:	MV_REG_READ_ASM (r6, r1, DDR_SDRAM_INIT_CTRL_REG)
         and    r6, r6, #DSICR_INIT_EN
         cmp    r6, #0
         bne 2b


	/* set all dram windows to 0 */
	mov	r6, #0
	MV_REG_WRITE_ASM(r6, r5, SDRAM_WIN_CTRL_REG(0))
	MV_REG_WRITE_ASM(r6, r5, SDRAM_WIN_CTRL_REG(1))
	MV_REG_WRITE_ASM(r6, r5, SDRAM_WIN_CTRL_REG(2))
	MV_REG_WRITE_ASM(r6, r5, SDRAM_WIN_CTRL_REG(3))


	ldr	r0, =0

	mov 	r2, r10
	mov     PC, r11         	/* r11 is saved link register */

#endif /* MV_STATIC_DRAM_ON_BOARD */
