/*
 * (C) Copyright 2014
 * Arnab Basu <arnab.basu@freescale.com>
 * (C) Copyright 2015
 * Arnab Basu <arnab_basu@rocketmail.com>
 *
 * Based on arch/arm/cpu/armv7/psci.S
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <linux/linkage.h>
#include <asm/psci.h>
#include <asm/armv8/esr.h>

#define PSCI_FN(__id, __fn) \
        .quad __id; \
        .quad __fn

.pushsection ._startofsecure.text, "ax"

#ifdef CONFIG_TARGET_ARMADA_8K
/* WA reset adderrs - CPU reset vector address - must be alined to 0x10000 */
ENTRY(armv8_psci_reset_addr)
	b _armada8k_cpu_entry
ENDPROC(armv8_psci_reset_addr)
#endif

ENTRY(psci_0_2_cpu_suspend_64)
ENTRY(psci_0_2_cpu_on_64)
ENTRY(psci_0_2_affinity_info_64)
ENTRY(psci_0_2_migrate_64)
ENTRY(psci_0_2_migrate_info_up_cpu_64)
ENTRY(psci_0_2_system_reset)
	mov	x0, #ARM_PSCI_RET_NI	/* Return -1 (Not Implemented) */
	ret
ENDPROC(psci_0_2_cpu_suspend_64)
ENDPROC(psci_0_2_cpu_on_64)
ENDPROC(psci_0_2_affinity_info_64)
ENDPROC(psci_0_2_migrate_64)
ENDPROC(psci_0_2_migrate_info_up_cpu_64)
ENDPROC(psci_0_2_system_reset)
.weak psci_0_2_cpu_suspend_64
.weak psci_0_2_cpu_on_64
.weak psci_0_2_affinity_info_64
.weak psci_0_2_migrate_64
.weak psci_0_2_migrate_info_up_cpu_64
.weak psci_0_2_system_reset

ENTRY(psci_0_2_psci_version)
	mov	x0, #2			/* Return Major = 0, Minor = 2*/
	ret
ENDPROC(psci_0_2_psci_version)

.align 4
_psci_0_2_table:
	PSCI_FN(PSCI_0_2_FN_PSCI_VERSION, psci_0_2_psci_version)
	PSCI_FN(PSCI_0_2_FN_SYSTEM_RESET, psci_0_2_system_reset)
	PSCI_FN(PSCI_0_2_FN64_CPU_SUSPEND, psci_0_2_cpu_suspend_64)
	PSCI_FN(PSCI_0_2_FN64_CPU_ON, psci_0_2_cpu_on_64)
	PSCI_FN(PSCI_0_2_FN64_AFFINITY_INFO, psci_0_2_affinity_info_64)
	PSCI_FN(PSCI_0_2_FN64_MIGRATE, psci_0_2_migrate_64)
	PSCI_FN(PSCI_0_2_FN64_MIGRATE_INFO_UP_CPU, psci_0_2_migrate_info_up_cpu_64)
	PSCI_FN(0, 0)

.macro	psci_enter
	stp	x29, x30, [sp, #-16]!
	stp	x27, x28, [sp, #-16]!
	stp	x25, x26, [sp, #-16]!
	stp	x23, x24, [sp, #-16]!
	stp	x21, x22, [sp, #-16]!
	stp	x19, x20, [sp, #-16]!
        str     x18, [sp, #-16]!
        mrs     x16, sp_el0
        mrs     x15, elr_el3
	stp	x15, x16, [sp, #-16]!

	/* Switching to Secure State to Execute U-Boot */
	mrs	x4, scr_el3
	bic	x4, x4, #1
	msr	scr_el3, x4
.endm

.macro	psci_return
	/* Switching to Non-Secure State to Execute OS */
	mrs	x4, scr_el3
	orr	x4, x4, #1
	msr	scr_el3, x4

        ldp     x15, x16, [sp], #16
        msr     elr_el3, x15
        msr     sp_el0, x16
        ldr     x18, [sp], #16
	ldp	x19, x20, [sp], #16
	ldp	x21, x22, [sp], #16
	ldp	x23, x24, [sp], #16
	ldp	x25, x26, [sp], #16
	ldp	x27, x28, [sp], #16
	ldp	x29, x30, [sp], #16
	eret
.endm

ENTRY(_smc_psci)
	psci_enter
	adr	x4, _psci_0_2_table
1:	ldp	x5, x6, [x4]	      /* Load PSCI function ID and target PC */
	cbz	x5, fn_not_found      /* If reach the end, bail out */
	cmp	x0, x5		      /* If not matching, try next entry */
	b.eq	fn_call
	add	x4, x4, #16
	b	1b

fn_call:
	blr	x6
	psci_return

fn_not_found:
	mov	x0, #ARM_PSCI_RET_NI    /* Return -1 (Not Supported) */
	psci_return
ENDPROC(_smc_psci)

ENTRY(unhandled_exception)
/* Returning to the place that caused the exception has the potential to cause
 * an endless loop of taking the same exception over and over again. Looping
 * here seems marginally better
 */
1:      b       1b
ENDPROC(unhandled_exception)

__handle_sync:
	str 	x4, [sp, #-16]!
	mrs	x4, esr_el3
	ubfx	x4, x4, #26, #6
	cmp	x4, #ESR_EC_SMC64
	b.eq	smc_found
	ldr	x4, [sp], #16
	b	unhandled_exception
smc_found:
	ldr     x4, [sp], #16
	b	_smc_psci

/*
 * PSCI Exception vectors.
 */
	.align	11
	.globl	psci_vectors
psci_vectors:
	.align	7
	b	unhandled_exception	/* Current EL Synchronous Thread */
	.align	7
	b	unhandled_exception	/* Current EL IRQ Thread */
	.align	7
	b	unhandled_exception	/* Current EL FIQ Thread */
	.align	7
	b	unhandled_exception	/* Current EL Error Thread */
	.align	7
	b	unhandled_exception	/* Current EL Synchronous Handler */
	.align	7
	b	unhandled_exception	/* Current EL IRQ Handler */
	.align	7
	b	unhandled_exception	/* Current EL FIQ Handler */
	.align	7
	b	unhandled_exception	/* Current EL Error Handler */
	.align	7
	b	__handle_sync		/* Lower EL Synchronous (64b) */
	.align	7
	b	unhandled_exception	/* Lower EL IRQ (64b) */
	.align	7
	b	unhandled_exception	/* Lower EL FIQ (64b) */
	.align	7
	b	unhandled_exception	/* Lower EL Error (64b) */

.popsection

ENTRY(fixup_vectors)
        adr     x0, psci_vectors
        msr     vbar_el3, x0
        ret
ENDPROC(fixup_vectors)
