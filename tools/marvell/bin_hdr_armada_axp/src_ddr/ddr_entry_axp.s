/* @
@ 	Entry point for the binary code to be executed as a BootROM extension
@
*/
	.globl _start
	_start:

	stmfd	sp!, {r0 - r12, lr}	/* @ save registers on stack */
	bl		ddr3Init
	mov	r0, #0x0				/* @ return value: 0x0 NO_ERR */
	ldmfd	sp!, {r0 - r12, pc}	/* @ restore regs and return */
	

/*************************************/
/* cache_inv - invalidate Cache line  */
/* r0 - dest		*/

	.global cache_inv
	cache_inv:
	
	stmfd   sp!, {r1-r12}

	mcr     p15, 0, r0, c7, c6, 1

	ldmfd   sp!, {r1-r12}
	bx      lr


/*************************************/
/* flush_l1_v6 - l1 cache clean invalidate  */
/* r0 - dest		*/

	.global flush_l1_v6
	flush_l1_v6:
	
	stmfd   sp!, {r1-r12}

	mcr     p15, 0, r0, c7, c10, 5          /* @ data memory barrier */
	mcr     p15, 0, r0, c7, c14, 1          /* @ clean & invalidate D line */
	mcr     p15, 0, r0, c7, c10, 4          /* @ data sync barrier */

	ldmfd   sp!, {r1-r12}
	bx      lr


/*************************************/
/* flush_l1_v7 - l1 cache clean invalidate  */
/* r0 - dest		*/

	.global flush_l1_v7
	flush_l1_v7:
	
	stmfd   sp!, {r1-r12}

	dmb 									/* @data memory barrier */
	mcr     p15, 0, r0, c7, c14, 1          /* @ clean & invalidate D line */
	dsb 									/* @data sync barrier */

	ldmfd   sp!, {r1-r12}
	bx      lr

/**********************************************************************/
/* changeResetVecBase - change reset vector offset 0x0 / 0xFFFF0000  	*/
/* r0 - direction														*/
			
	.global changeResetVecBase
	changeResetVecBase:
	mrc     p15, 0, r1, c1, c0, 0
	and		r1, r1, #0xFFFFDFFF		/*@ BIT13 = 0:  */
	orr		r1, r1, r0
	mcr     p15, 0, r1, c1, c0, 0
	dsb
	bx      lr

			
/******************************************/
/* setCPSR - set/reset CPSR register bits  */
/* r0 - required bit 						*/
			
			
	.global setCPSR
	setCPSR:
	mrs 	r1, cpsr
	bic		r1, r1, #0x100
	orr		r1, r1, r0
	msr		cpsr_x, r1
	bx      lr
