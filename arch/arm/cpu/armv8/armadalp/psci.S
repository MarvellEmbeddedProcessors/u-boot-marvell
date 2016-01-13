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
#include <linux/linkage.h>
#include <asm/psci.h>
#include <asm/gic.h>
#include <asm/macro.h>

#define MVEBU_CPU_1_RESET_VECTOR 0x14044
#define MVEBU_CPU_1_RESET_REG 0xD00C
#define MVEBU_CPU_1_RESET_BIT 31
#define MVEBU_NB_REGS_BASE 0x13000
#define MVEBU_WARM_RESET_REG 0x840
#define MVEBU_WARM_RESET_MAGIC 0x1D1E

/*
  * Now there is only very limited support for PSCI feature.
  * We only support wake up CPU1 from CPU0 to test SMP
  * feature in Kernel.
  *
  * TODO: Add the rest of PSCI feature which includes
  * will be added, including suspend/on any cpu from other
  * cpu and get affinity_info routine.
  */


.pushsection ._secure.text, "ax"

enable_affinity:
	/* Activate Affinity in CA-53 configuration
	 * Enable the SMPEN bit in CPUECTLR_EL1 */
	mrs x0, S3_1_c15_c2_1
	orr x0, x0, #0x40
	msr S3_1_c15_c2_1, x0
	ret

.globl	psci_arch_init
psci_arch_init:
	add     x29, x30, 0 /* keep return address */
	bl	enable_affinity
	bl	psci_build_stack
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
	mov	x0, #(MVEBU_REGS_BASE)
	add	x3, x0, #(MVEBU_NB_REGS_BASE)
	add	x3, x3, #(MVEBU_WARM_RESET_REG)
	mov	w0, #(MVEBU_WARM_RESET_MAGIC)
	str	w0, [x3]
	ret

	/* x1 = target CPU */
	/* x2 = target PC */
.globl	psci_0_2_cpu_on_64
psci_0_2_cpu_on_64:

	adr	x0, _target_pc
	str	x2, [x0]

	dsb     sy

	mov	x0, #(MVEBU_REGS_BASE)

	/* set the cpu_1 start address */
	ldr x3, =MVEBU_CPU_1_RESET_VECTOR
	add	x3, x0, x3
	adr x2, _armadalp_cpu_entry
	lsr x2, x2, #2 /* 0x14044 is bit[33:2] of start address	 */
	str 	w2, [x3]

	/* get the cpu out of reset */
	ldr x3, =MVEBU_CPU_1_RESET_REG
	add	x3, x0, x3
	mov 	w4, #1
	lsl w4, w4, #MVEBU_CPU_1_RESET_BIT
	mvn		w5, w4
	ldr w2, [x3]
	and	w2, w2, w5
	str w2, [x3]
	ldr w2, [x3]
	orr 	w2, w2, w4
	str 	w2, [x3]

	/* return success */
	mov	x0, #ARM_PSCI_RET_SUCCESS	/* Return PSCI_RET_SUCCESS */
	ret

.global _armadalp_cpu_entry
_armadalp_cpu_entry:

	bl	enable_affinity

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

	adr	x0, _target_pc
	ldr x0, [x0]
	br	x0

	/* 64 bit alignment for elements accessed as data */
	.align 4
_target_pc:
	.quad 0x0

text_end:
	.popsection
