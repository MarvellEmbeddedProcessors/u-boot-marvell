/*
 * Copyright (C) 2013 - ARM Ltd
 * Author: Marc Zyngier <marc.zyngier@arm.com>
 *
 * Based on code by Carl van Schaik <carl@ok-labs.com>.
 *
 * Copyright (C) 2015 Marvell International Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ***************************************************************************
*/

#include <config.h>
#include <asm/psci.h>
#include <asm/macro.h>

#define MVEBU_CCU_HTC_ASET_REG	0x4264
#define MVEBU_PRIVATE_UID_REG	0x30
#define MVEBU_IO_AFFINITY	0xF00
#define MVEBU_RFU_GLOBL_SW_RST	0x84

#define MVEBU_CCU_RVBAR(i)	(0x640 + (i * 4))

.pushsection ._secure.text, "ax"

enable_affinity:
	/* get current CPU */
	mrs 	x2, MPIDR_EL1   	/* get current CPU - Use affinity level 1 */
	asr 	x2, x2, #8
	and 	x2, x2, #0xff

	/* Activate Affinity between current CPU */
	mov	x0, #(MVEBU_REGS_BASE)
	mov     x3, #(MVEBU_CCU_HTC_ASET_REG)
	orr     x0, x3, x0
	mov	x3, #0x1
	lsl     x1, x3, x2
	orr	x1, x1, #(MVEBU_IO_AFFINITY)
	str     w1, [x0]

	/* Activate Affinity in CA-57 configuration
	 * Enable the SMPEN bit in CPUECTLR_EL1 */
	mrs x0, S3_1_c15_c2_1
	orr x0, x0, #0x40
	msr S3_1_c15_c2_1, x0
	ret

l2_setup:

#ifdef CONFIG_MVEBU_LLC_EXCLUSIVE_EN
	mrs x5, s3_1_c15_c0_0 /* L2 Aux Ctrl */
	orr x5, x5, #(1 << 14) /* Enable UniqueClean evictions with data */
	msr s3_1_c15_c0_0, x5 /* L2 Aux Ctrl */
#endif
	ret

.globl	psci_arch_init
psci_arch_init:
	add     x29, x30, 0 /* keep return address */
	bl	enable_affinity
	bl	l2_setup
	bl	psci_build_stack
#ifdef CONFIG_MVEBU_LLC_ENABLE
	bl	llc_enable
#endif
	ret	x29

psci_build_stack:

	mrs     x5, SCR_EL3
	bic	x5, x5, #1	/* Secure mode */
	msr	SCR_EL3, x5
	isb

	mrs 	x4, MPIDR_EL1	/* get current CPU - Use affinity level 1 */
	asr 	x4, x4, #8
	and 	x4, x4, #0xff

	mov	x5, #400		/* 1kB of stack per CPU */
	mul	x4, x4, x5

	adr	x5, text_end		/* end of text */
	add	x5, x5, #0x2000		/* Skip two pages */
	lsr	x5, x5, #12		/* Align to start of page */
	lsl	x5, x5, #12
	sub	sp, x5, x4		/* here's our stack! */

	ret

.globl	psci_0_2_system_reset
psci_0_2_system_reset:
	mov	x0, #(MVEBU_RFU_BASE)
	add	x3, x0, #(MVEBU_RFU_GLOBL_SW_RST)
	mov	w0, #0
	str	w0, [x3]
	ret

	/* x1 = target CPU */
	/* x2 = target PC */
.globl	psci_0_2_cpu_on_64
psci_0_2_cpu_on_64:

	adr	x0, _target_pc
	str	x2, [x0]

	dsb     sy

	/* get cpu number - use CPU ID */
	and 	x5, x1, #0x3

	/* get cluster number - use affinity level 1 */
	asr 	x1, x1, #8
	and 	x1, x1, #0xff

	/* set CPU private UID */
	mov	x0, #(MVEBU_REGS_BASE)
	add	x3, x0, #(MVEBU_PRIVATE_UID_REG)
	add     x2, x1, #0x4
	str 	w2, [x3]

	/* set the cpu start address */
	mov	x2, #(MVEBU_CCU_RVBAR(0))
	add	x2, x2, x5, lsl #2
	add	x3, x0, x2

	/* CPU reset vector address - must be aligned to 0x10000
	use __secure_start - _start as PSCI first address
	first instruction - jump to _armada8k_cpu_entry */
	adr     x2, _secure_base
	ldr     x2, [x2]
	adr     x4, _start_base
	ldr     x4, [x4]
	sub	x2, x2, x4
	lsr	x2, x2, #16	/* align to 0x10000 */
	str 	w2, [x3]

	/* Save the Linux out of reset address to unused register.
	** We used the RVBAR for CPU's 2-3 to store the Linux return address
	** for CPUs 0 & 1. */
	adr	x1, _target_pc
	ldr	x1, [x1]
	mov	x2, #(MVEBU_CCU_RVBAR(2))
	add	x2, x2, x5, lsl #2
	add	x3, x0, x2
	str     w1, [x3]

	/* get the cpu out of reset */
	mov	x2, #0x650
	add	x2, x2, x5, lsl #2
	add	x3, x0, x2
	movz 	x2, #0x1, LSL #16
	add	x2, x2, #0x1
	str 	w2, [x3]

	/* return success */
	mov	x0, #ARM_PSCI_RET_SUCCESS	/* Return PSCI_RET_SUCCESS */
	ret

.global _armada8k_cpu_entry
_armada8k_cpu_entry:

	/* get current CPU */
	mrs 	x2, MPIDR_EL1   	/* get current CPU - Use affinity level 1 */
	asr 	x2, x2, #8
	and 	x2, x2, #0xff

	/* set CPU private UID */
	mov	x0, #(MVEBU_REGS_BASE)
	add	x3, x0, #(MVEBU_PRIVATE_UID_REG)
	add     x1, x2, #0x4
	str 	w1, [x3]

	bl	enable_affinity

	bl	l2_setup
	isb

	/*
	 * Could be EL3/EL2/EL1, Initial State:
	 * Little Endian, MMU Disabled, i/dCache Disabled
	 */
	adr	x0, vectors
	switch_el x1, 3f, 2f, 1f
3:	msr	vbar_el3, x0
	mrs	x0, scr_el3
	orr	x0, x0, #0xf			/* SCR_EL3.NS|IRQ|FIQ|EA */
	msr	scr_el3, x0
	msr	cptr_el3, xzr			/* Enable FP/SIMD */
	ldr	x0, =COUNTER_FREQUENCY
	msr	cntfrq_el0, x0			/* Initialize CNTFRQ */
	b	0f
2:	msr	vbar_el2, x0
	mov	x0, #0x33ff
	msr	cptr_el2, x0			/* Enable FP/SIMD */
	b	0f
1:	msr	vbar_el1, x0
	mov	x0, #3 << 20
	msr	cpacr_el1, x0			/* Enable FP/SIMD */
0:

#if defined(CONFIG_GICV3)
	ldr	x0, =GICR_BASE
	bl	gic_init_secure_percpu
#elif defined(CONFIG_GICV2)
	ldr	x0, =GICD_BASE
	ldr	x1, =GICC_BASE
	bl	gic_init_secure_percpu
#endif

	bl	psci_build_stack

	bl	armv8_switch_to_el2
#ifdef CONFIG_ARMV8_SWITCH_TO_EL1
	bl	armv8_switch_to_el1
#endif

	/* Get linux start address - we use RVBARR for CPU's 2-3 as a WA. */
	mrs 	x2, MPIDR_EL1   	/* get current CPU - Use affinity level 1 */
	and	x5, x2, #0x3

	mov	x0, #(MVEBU_REGS_BASE)
	mov	x2, #(MVEBU_CCU_RVBAR(2))
	add	x2, x2, x5, lsl #2
	orr     x2, x2, x0
	ldr     w0, [x2]

	br	x0

	/* 64 bit alignment for elements accessed as data */
	.align 4
_target_pc:
	.quad 0x0

_secure_base:
	.quad __secure_start

_start_base:
	.quad	_start

text_end:
	.popsection
