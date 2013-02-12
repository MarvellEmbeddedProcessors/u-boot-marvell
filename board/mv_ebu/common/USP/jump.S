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

#include <config.h>

.section ".reset_vector_sect", #alloc, #execinstr

/*
 * This code should be run only from Flash.
 * Its main purpose to enable CPU to reach the reset vector code (_start symbol).
 */
jumpStart:
	
	adr	r4, jumpStart		/* r4 <- current position of code   */
	ldr	r5, __start		/* r5 <- linker results for _start   */
	ldr	r2, _jumpStart		/* r2 <- linker results reset vector */
	sub	r8, r2, r5		/* r8 <- (reset vector address - start address) */
	sub	r8, r4, r8		/* r8 <- absolute address to jump to */
					/* r8 <- (current code address -      */

	/*
	 * Stack is still unavailable here, it will be set before
	 * jumping to first C-code function board_init_f()
	 */

	mov	lr, r8
	mov	pc, lr

__start:
	.word	_start

_jumpStart:
	.word	.reset_vector_sect

.section ".dummy", #alloc, #execinstr
	.long	0xffffffff
	.long	0xffffffff
	.long	0xffffffff
	.long	0xffffffff
