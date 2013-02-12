/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

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
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer.

    *   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

    *   Neither the name of Marvell nor the names of its contributors may be
        used to endorse or promote products derived from this software without
        specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#ifndef __INCmvOsAsmh
#define __INCmvOsAsmh


#include "mvCommon.h"

#if defined(MV_MIPS)
 #define CPU_FAMILY   MIPS
 #include "asm.h"

#elif defined (MV_PPC)
 #define CPU_FAMILY   PPC
 #include <config.h>
 #include <74xx_7xx.h>
 #include <ppc_asm.tmpl>
 #include <asm/cache.h>
 #include <asm/mmu.h>
 #include <ppc_defs.h>

#elif defined (MV_ARM)
 #define CPU_FAMILY   ARM
 #include <config.h>

/* BE/ LE swap for Asm */
#if defined(MV_CPU_LE)

#define htoll(x)        x
#define HTOLL(sr,tr)

#elif defined(MV_CPU_BE)

#define htoll(x) ((((x) & 0x00ff) << 24) | \
                  (((x) & 0xff00) <<  8) | \
                  (((x) >> 8)  & 0xff00) | \
                  (((x) >> 24) & 0x00ff))


#define HTOLL(sr,temp)                  /*sr   = A  ,B  ,C  ,D    */\
        eor temp, sr, sr, ROR #16 ;     /*temp = A^C,B^D,C^A,D^B  */\
        bic temp, temp, #0xFF0000 ;     /*temp = A^C,0  ,C^A,D^B  */\
        mov sr, sr, ROR #8 ;            /*sr   = D  ,A  ,B  ,C    */\
        eor sr, sr, temp, LSR #8        /*sr   = D  ,C  ,B  ,A    */

#endif

#define MV_REG_READ_ASM(toReg, tmpReg, regOffs)         \
        ldr     tmpReg, =(INTER_REGS_BASE + regOffs) ;  \
        ldr     toReg, [tmpReg]                      ;  \
        HTOLL(toReg,tmpReg)

#define MV_REG_WRITE_ASM(fromReg, tmpReg, regOffs)      \
        HTOLL(fromReg,tmpReg)                        ;  \
        ldr     tmpReg, =(INTER_REGS_BASE + regOffs) ;  \
        str     fromReg, [tmpReg]

#define MV_DV_REG_READ_ASM(toReg, tmpReg, regOffs)      \
        ldr     tmpReg, =(MV_DFL_REGS + regOffs) ;  \
        ldr     toReg, [tmpReg]                      ;  \
        HTOLL(toReg,tmpReg)

#define MV_DV_REG_WRITE_ASM(fromReg, tmpReg, regOffs)  	\
        HTOLL(fromReg,tmpReg)                        ;	\
        ldr     tmpReg, =(MV_DFL_REGS + regOffs) ;	\
        str     fromReg, [tmpReg]

#define MV_32BIT_READ_ASM(toReg, tmpReg, addr)       	\
        ldr     tmpReg, =(addr) ;  			\
        ldr     toReg, [tmpReg]                      ;  \
        HTOLL(toReg,tmpReg)

#define MV_32BIT_WRITE_ASM(fromReg, tmpReg, addr)	\
        HTOLL(fromReg,tmpReg)                        ;  \
        ldr     tmpReg, =(addr) 		     ;	\
        str     fromReg, [tmpReg]

#else
 #error "CPU type not selected"
#endif

#endif /* __INCmvOsAsmh */
