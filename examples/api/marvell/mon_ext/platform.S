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
#include <config.h>
#include <version.h>
#include "mvBoardEnvSpec.h"
#include "mvCtrlEnvSpec.h"
#include "mvAhbToMbusRegs.h"
#include "ddr2_3/mvDramIfRegs.h"
#include "mvCtrlEnvAsm.h"

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

#ifndef MV88F78X60_Z1
	.align 5	
.global armv7_mmu_cache_flush
armv7_mmu_cache_flush:
		mrc	p15, 0, r10, c0, c1, 5	@ read ID_MMFR1
		tst	r10, #0xf << 16		@ hierarchical cache (ARMv7)
		mov	r10, #0
		beq	hierarchical
		mcr	p15, 0, r10, c7, c6, 0	@ clean+invalidate D
		b	iflush
hierarchical:
		mcr	p15, 0, r10, c7, c10, 5	@ DMB
		stmfd	sp!, {r0-r7, r9-r11}
		mrc	p15, 1, r0, c0, c0, 1	@ read clidr
		ands	r3, r0, #0x7000000	@ extract loc from clidr
		mov	r3, r3, lsr #23		@ left align loc bit field
		beq	finished		@ if loc is 0, then no need to clean
		mov	r10, #0			@ start clean at cache level 0
loop1:
		add	r2, r10, r10, lsr #1	@ work out 3x current cache level
		mov	r1, r0, lsr r2		@ extract cache type bits from clidr
		and	r1, r1, #7		@ mask of the bits for current cache only
		cmp	r1, #2			@ see what cache we have at this level
		blt	skip			@ skip if no cache, or just i-cache
		mcr	p15, 2, r10, c0, c0, 0	@ select current cache level in cssr
		mcr	p15, 0, r10, c7, c5, 4	@ isb to sych the new cssr&csidr
		mrc	p15, 1, r1, c0, c0, 0	@ read the new csidr
		and	r2, r1, #7		@ extract the length of the cache lines
		add	r2, r2, #4		@ add 4 (line length offset)
		ldr	r4, =0x3ff
		ands	r4, r4, r1, lsr #3	@ find maximum number on the way size
		clz	r5, r4			@ find bit position of way size increment
		ldr	r7, =0x7fff
		ands	r7, r7, r1, lsr #13	@ extract max number of the index size
loop2:
		mov	r9, r4			@ create working copy of max way size
loop3:
		orr	r11, r10, r9, lsl r5	@ factor way and cache number into r11
		orr	r11, r11, r7, lsl r2	@ factor index number into r11
		mcr	p15, 0, r11, c7, c14, 2	@ clean & invalidate by set/way
		subs	r9, r9, #1		@ decrement the way
		bge	loop3
		subs	r7, r7, #1		@ decrement the index
		bge	loop2
skip:
		add	r10, r10, #2		@ increment cache number
		cmp	r3, r10
		bgt	loop1
finished:
		ldmfd	sp!, {r0-r7, r9-r11}
		mov	r10, #0			@ swith back to cache level 0
		mcr	p15, 2, r10, c0, c0, 0	@ select current cache level in cssr
iflush:
		mcr	p15, 0, r10, c7, c10, 4	@ DSB
		mcr	p15, 0, r10, c7, c5, 0	@ invalidate I+BTB
		mcr	p15, 0, r10, c7, c10, 4	@ DSB
		mcr	p15, 0, r10, c7, c5, 4	@ ISB
		mov	pc, lr
#endif



