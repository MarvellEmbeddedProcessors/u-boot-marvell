/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

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

*******************************************************************************/

#define MV_ASMLANGUAGE
#include "mvOsAsm.h"
#include <config.h>
#include <version.h>
#include "mvBoardEnvSpec.h"
#include "mvCtrlEnvSpec.h"
#include "mvAhbToMbusRegs.h"
#include "ddr2_3/mvDramIfRegs.h"
#include "mvCtrlEnvAsm.h"

.globl lowlevel_init

/************************************************/
/*              lowlevel_init                   *
/************************************************/

lowlevel_init:
	mov	r2, lr

	/* Multicore support */
	mrc     p15, 0, r0, c0, c0, 5
	/* Check if we are CPU0 or CPU1 */
	cmp     r0, #0x0
	bne     done_cpu1

	bl	done

done:

#ifdef CFU_DRAM_BYPASS
	/* allow accessing to DRAM from XBAR */
	ldr r4, =0x1fff0e01			/* Set DRAM CS[0] size in P-unit (default) */
	MV_REG_WRITE_ASM(r4, r1, 0x200e8)
	ldr r4, =0x20000000			/* Set DRAM CS[1] base in P-unit */
	MV_REG_WRITE_ASM(r4, r1, 0x200e4)
	ldr r4, =0x1fff0d01			/* Set DRAM CS[1] size in P-unit */
	MV_REG_WRITE_ASM(r4, r1, 0x200e0)
#endif

	mov	lr, r2

done_cpu1:
	mov 	pc, lr

/*
 *************************************************************************
 *
 * Flush DCache
 *
 *************************************************************************
 */

.globl _dcache_index_max
_dcache_index_max:
	.word 0x0

.globl _dcache_index_inc
_dcache_index_inc:
	.word 0x0

.globl _dcache_set_max
_dcache_set_max:
	.word 0x0

.globl _dcache_set_index
_dcache_set_index:
         .word 0x0


#define s_max   r0
#define s_inc   r1
#define i_max   r2
#define i_inc   r3

.globl cpu_dcache_flush_all
cpu_dcache_flush_all:
        stmdb	sp!, {r0-r3,ip}

        ldr i_max, _dcache_index_max
        ldr i_inc, _dcache_index_inc
        ldr s_max, _dcache_set_max
        ldr s_inc, _dcache_set_index

Lnext_set_inv:
        orr     ip, s_max, i_max
Lnext_index_inv:
        mcr     p15, 0, ip, c7, c14, 2  /* Purge D cache SE with Set/Index */
        sub     ip, ip, i_inc
        tst     ip, i_max               /* Index 0 is last one */
        bne     Lnext_index_inv         /* Next index */
        mcr     p15, 0, ip, c7, c14, 2  /* Purge D cache SE with Set/Index */
        subs    s_max, s_max, s_inc
        bpl     Lnext_set_inv           /* Next set */
        ldmia	sp!, {r0-r3,ip}

        mov	pc, lr   /* back to my caller */

.globl cpu_icache_flush_invalidate_all
cpu_icache_flush_invalidate_all:
        stmdb	sp!, {r0}

        ldr     r0,=0
        mcr     p15, 0, r0, c7, c5, 0   /* Flush Invalidate D and I caches */
        ldmia	sp!, {r0}

        mov	pc, lr   /* back to my caller */
