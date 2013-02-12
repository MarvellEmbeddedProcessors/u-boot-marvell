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

	/* Multicore support - read MPIDR register (Multiprocessor Affinity) */
	mrc	p15, 0, r0, c0, c0, 5
#if defined(CONFIG_AVANTA_LP_FPGA)
	bic	r0, r0, #0xFFFFFFF0
	cmp	r0, #0x0
	bne	exit
	/* If run from DRAM, then no need to init DRAM, => return */
	adr	r0, lowlevel_init	/* run-time address of function */
	ldr	r1, _lowlevel_init_val	/* link-time address of function */
	cmp	r1, r0
	beq	exit			/* exit if run from DRAM */

	/* Write to CP15:1 - point exception vector to flash (0xffff0000) */
	mrc 	p15, 0, r0, c1, c0, 0
	orr 	r0, r0, #0x00002000 	/* bit 13 - Exception Vector on flash 	*/
	orr	r0, r0, #0x00001000	/* bit 12 - re-enable I-cache 		*/
	mcr 	p15, 0, r0, c1, c0, 0

	/* descrease size of CS[2] to prevent overlap with 0xf1000000*/ 
	ldr	r4, =0x000f3b11
	MV_DV_REG_WRITE_ASM(r4, r1, 0x200B0)

	/* Mask L2 End address filtering - forward CPU transactions to XBAR */
	ldr	r4, =0x0
	MV_DV_REG_WRITE_ASM(r4, r1, 0x08c04)

#if defined(MV_STATIC_DRAM_ON_BOARD)
	bl	_mvDramIfStaticInit
#endif

	/* re-open XBAR window */
	ldr	r4, =0x0fff0e01
	MV_REG_WRITE_ASM(r4, r1, 0x200e8)

	/* Enable CS0 bank */
	ldr	r4, =0x1
	MV_DV_REG_WRITE_ASM(r4, r1, 0x015e0)

#else /* if not (CONFIG_AVANTA_LP_FPGA) */
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
#endif /* CFU_DRAM_BYPASS */

#endif /* CONFIG_AVANTA_LP_FPGA) */

exit:
	mov	lr, r2
done_cpu1:
	mov 	pc, lr

.globl _lowlevel_init_val
_lowlevel_init_val:
	.word lowlevel_init

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
#if !defined(CONFIG_MACH_AVANTA_LP_FPGA)
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
#endif

        mov	pc, lr   /* back to my caller */

.globl cpu_icache_flush_invalidate_all
cpu_icache_flush_invalidate_all:
#if !defined(CONFIG_MACH_AVANTA_LP_FPGA)
        stmdb	sp!, {r0}

        ldr     r0,=0
        mcr     p15, 0, r0, c7, c5, 0   /* Flush Invalidate D and I caches */
        ldmia	sp!, {r0}
#endif

        mov	pc, lr   /* back to my caller */
