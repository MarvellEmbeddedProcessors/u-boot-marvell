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
#include "include/config.h"

#define MV_ASMLANGUAGE

#ifdef CONFIG_ALP_HW_SEMAPHORE_WA
.globl lock_hw_mutex
lock_hw_mutex:

	ldr r1, _mv_sema_reg_base
	ldrexb r2, [r1]		   /* Read lock */
	and r2, r2, #0xFF
	/* If semaphore free, lock it */
	cmp r2, #0xFF
	bne lock_wait		/* If not, wait */
	mov r3, #1
	strexb r0, r3, [r1]	/* If free lock it */
	cmp r0, #0		/* Test if store'ex suceeded */
	bne lock_hw_mutex	/* Retry if not */
	mov pc, lr		/* Exit the routine */

lock_wait:
	ldr r1, _mv_sema_wait
1:	sub r1, r1, #1		/* Wait "_mv_sema_wait" loops */
	cmp r1, #0
	bne 1b
	b  lock_hw_mutex 	/* Try getting lock again*/


.globl unlock_hw_mutex
unlock_hw_mutex:

	ldr r1, _mv_sema_reg_base
	ldr r2, _mv_sema_release
	str r2, [r1] /* release semaphore */
	mov pc, lr

#else

.globl lock_hw_mutex
lock_hw_mutex:

	ldr r1, _mv_sema_reg_base
	add r1, r1, r0  /* Add semaphore number offset */
	ldrb r2, [r1]		   /* read lock */
	mrc p15, 0, r3, c0, c0, 5  /* read core ID */
	and r3, r3, #0xF
	and r2, r2, #0xFF
	cmp r2, r3		/* if lock == core ID the lock is ours*/
	/*bne lock_hw_mutex*/
	moveq pc, lr		/*Exit the routine */
	ldr r1, _mv_sema_wait
lock_wait:
	sub r1, r1, #1		/* Wait "_mv_sema_wait" loops */
	cmp r1, #0
	bne lock_wait
	b  lock_hw_mutex 	/* Try getting lock again*/


.globl unlock_hw_mutex
unlock_hw_mutex:

	ldr r1, _mv_sema_reg_base
	add r1, r1, r0
	ldr r2, _mv_sema_release
	strb r2, [r1] /* release HW semaphore */
	mov pc, lr
#endif


.globl linuxMicroLoader

/*.section ".reset_vector_sect",#alloc, #execinstr*/
linuxMicroLoader:


	mrs r0, cpsr
	/* Disable all interrupts, IRQ and FIQ */
	orr r0, r0, #0xC0
	/* set Supervisor (SVC) state CPSR[0:4] = 10011*/
	orr r0, r0, #0x13
	bic r0, r0, #0xC
	msr cpsr_c, r0

	/* Now disable i-cache, D-cache, MMU and aligned acces check */
	mrc 	p15, 0, r0, c1, c0, 0
	bic     r0, r0, #0x1000
	bic     r0, r0, #0x7
	mcr 	p15, 0, r0, c1, c0, 0

	/*
	 * This linuxMicroLoader is the first routine that is called after
	 * waking up CPU from reset. After reset invalidation of each cache
	 * must be done before enabling it.
	 */
	mcr	p15, 0, r0, c7, c5, 0	/* invalidate I-Cache*/
#ifdef CONFIG_AVANTA_LP
	/* After return from v7_invalidate_l1_dcache r0-r6 corrupted */
	bl	v7_invalidate_l1_dcache	/* invalidate D-Cache*/
#endif

	/* Boot barrier start*/
	ldr r0, _amp_sync_boot
	ldr r0, [r0]
	cmp r0, #0	/* Check is boot sync with other OS is needed*/
	beq boot

	ldr r0, _amp_hw_lock_id
	ldr r0, [r0]
	bl lock_hw_mutex  /* Aquire mutex */

	ldr r1, _amp_barrier 	/* Start of protected section */
	ldr r2, [r1]
	sub r2, r2, #1		/* decrement barrier */
	str r2, [r1]		/* End of protected section */

	bl unlock_hw_mutex /* Release mutex */

	/* Wait for barrier to reach zero */
	ldr r1, _amp_barrier
barrier_wait:
	ldr r2, [r1]
	cmp r2, #0
	bne barrier_wait
	/* Boot barrier end*/

boot:
	/* Set r0 = 0, r1 = machine_id, r2 = ATAGS ptr */
	mov r0, #0
	ldr r1, _machine_id
	ldr r1, [r1]
	ldr r2, _image_param_addr
	ldr r2, [r2]
	ldr r3, _image_load_addr
	ldr r3, [r3]
	mov pc, r3


.globl vxWorksMicroLoader
vxWorksMicroLoader:

	/* Boot barrier start*/
	ldr r0, _amp_sync_boot
	ldr r0, [r0]
	cmp r0, #0	/* Check is boot sync with other OS is needed*/
	beq vx_boot

	ldr r0, _amp_hw_lock_id
	ldr r0, [r0]
	bl lock_hw_mutex  /* Aquire mutex */

	ldr r1, _amp_barrier 	/* Start of protected section */
	ldr r2, [r1]
	sub r2, r2, #1		/* decrement barrier */
	str r2, [r1]		/* End of protected section */

	bl unlock_hw_mutex /* Release mutex */

	/* Wait for barrier to reach zero */
	ldr r1, _amp_barrier

vx_barrier_wait:
	ldr r2, [r1]
	cmp r2, #0
	bne vx_barrier_wait
	/* Boot barrier end*/

vx_boot:
	ldr r3, _image_load_addr
	ldr r3, [r3]
	mov pc, r3

#ifdef CONFIG_AVANTA_LP
/* After return from v7_invalidate_l1_dcache r4-r6 corrupted */
v7_invalidate_l1_dcache:
	mov	r0, #0
	mcr	p15, 2, r0, c0, c0, 0
	mrc	p15, 1, r0, c0, c0, 0

	ldr	r1, =0x7fff
	and	r2, r1, r0, lsr #13

	ldr	r1, =0x3ff

	and	r3, r1, r0, lsr #3  @ NumWays - 1
	add	r2, r2, #1          @ NumSets

	and	r0, r0, #0x7
	add	r0, r0, #4          @ SetShift

	clz	r1, r3              @ WayShift
	add	r4, r3, #1          @ NumWays
1:	sub	r2, r2, #1          @ NumSets--
	mov	r3, r4              @ Temp = NumWays
2:	subs	r3, r3, #1          @ Temp--
	mov	r5, r3, lsl r1
	mov	r6, r2, lsl r0
	orr	r5, r5, r6          @ Reg = (Temp<<WayShift)|(NumSets<<SetShift)
	mcr	p15, 0, r5, c7, c6, 2
	bgt	2b
	cmp	r2, #0
	bgt	1b
	dsb
	isb
	mov	pc, lr
#endif


_machine_id: .word machine_id
_image_param_addr: .word image_param_addr
_image_load_addr: .word image_load_addr
_amp_sync_boot: .word amp_sync_boot
_amp_hw_lock_id: .word amp_hw_lock_id
_amp_barrier: .word amp_barrier
#ifdef CONFIG_ALP_HW_SEMAPHORE_WA
_mv_sema_reg_base: .word amp_soft_sem
#else
_mv_sema_reg_base: .word 0xF1020500
#endif
_mv_sema_release: .word 0xFF
_mv_sema_wait: .word 0x400
