/*
 * ***************************************************************************
 * Copyright (C) 2015 Marvell International Ltd.
 * ***************************************************************************
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

.section .text
.thumb

.global _start
_start:
	.word	reset				/*  1 */
	.word	nmi_exception			/*  2 */
	.word	hard_fault_exception		/*  3 */
	.word	mem_manage_exception		/*  4 */
	.word	bus_fault_exception		/*  5 */
	.word	usage_fault_exception		/*  6 */
	.word	reserved			/*  7 */
	.word	reserved			/*  8 */
	.word	reserved			/*  9 */
	.word	reserved			/* 10 */
	.word	svc				/* 11 */
	.word	debug_monitor			/* 12 */
	.word	reserved			/* 13 */
	.word	pendable_service_request	/* 14 */
	.word	sys_tick_handler		/* 15 */
	.word	external_interrupt		/* 16 */
	.word	external_interrupt		/* 17 */
	.word	external_interrupt		/* 18 */
	.word	external_interrupt		/* 19 */
	.word	external_interrupt		/* 20 */
	.word	external_interrupt		/* 21 */
	.word	external_interrupt		/* 22 */
	.word	external_interrupt		/* 23 */
	.word	external_interrupt		/* 24 */
	.word	external_interrupt		/* 25 */
	.word	external_interrupt		/* 26 */
	.word	external_interrupt		/* 27 */
	.word	external_interrupt		/* 28 */
	.word	external_interrupt		/* 29 */
	.word	external_interrupt		/* 30 */
	.word	external_interrupt		/* 31 */

/***********************************************************/
.global reset
reset:
	ldr	r2, =stack_top
	msr	psp, r2				/* Init Process stack pointer */
	msr	msp, r2				/* Init main stack pointer */
	ldr	r0, =0x0
	ldr	r1, =main
	bx	r1


/***********************************************************/
.global nmi_exception
nmi_exception:
	ldr	r0, =0x2
	ldr	r1, =main
	bx	r1

.global hard_fault_exception
hard_fault_exception:
	ldr	r0, =0x3
	ldr	r1, =main
	bx	r1

.global mem_manage_exception
mem_manage_exception:
	ldr	r0, =0x4
	ldr	r1, =main
	bx	r1

.global bus_fault_exception
bus_fault_exception:
	ldr	r0, =0x5
	ldr	r1, =main
	bx	r1

.global usage_fault_exception
usage_fault_exception:
	ldr	r0, =0x6
	ldr	r1, =main
	bx	r1

.global reserved
reserved:
	ldr	r0, =0x7
	ldr	r1, =main
	bx	r1

.global svc
svc:
	ldr	r0, =0xB
	ldr	r1, =main
	bx	r1

.global debug_monitor
debug_monitor:
	ldr	r0, =0xC
	ldr	r1, =main
	bx	r1

.global pendable_service_request
pendable_service_request:
	ldr	r0, =0xE
	ldr	r1, =main
	bx	r1

.global sys_tick_handler
sys_tick_handler:
	ldr	r0, =0xF
	ldr	r1, =main
	bx	r1

.global external_interrupt
external_interrupt:
	ldr	r0, =0x10
	ldr	r1, =main
	bx	r1


.end
