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
	ldr r4, _mv_sema_wait
lock_wait:
	sub r4, r4, #1		/* Wait "_mv_sema_wait" loops */
	cmp r4, #0
	bne lock_wait
	b  lock_hw_mutex 	/* Try getting lock again*/


.globl unlock_hw_mutex
unlock_hw_mutex:

	ldr r1, _mv_sema_reg_base
	add r1, r1, r0
	ldr r2, _mv_sema_release
	strb r2, [r1] /* release HW semaphore */
	mov pc, lr

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


_machine_id: .word machine_id
_image_param_addr: .word image_param_addr
_image_load_addr: .word image_load_addr
_amp_sync_boot: .word amp_sync_boot
_amp_hw_lock_id: .word amp_hw_lock_id
_amp_barrier: .word amp_barrier
_mv_sema_reg_base: .word 0xD0020500
_mv_sema_release: .word 0xFF
_mv_sema_wait: .word 0x400
