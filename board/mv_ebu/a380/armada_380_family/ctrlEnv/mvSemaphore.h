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
#ifndef mvsemaphore_h
#define mvsemaphore_h

#ifdef MV_VXWORKS
#include "common/mvTypes.h"
#include "config.h"
#endif

#define MV_AMP_GLOBAL_REG(x)	(0x20980 + (4*x))
#define ADR_WIN_EN_REG			0

#define MV_SEMA_REG_BASE	(0x20500)
#define MV_MAX_SEMA 	    	128
#define MV_SEMA_SMI 		50
#define MV_SEMA_RTC 		51
#define MV_SEMA_NOR_FLASH 	0
#define MV_SEMA_BOOT 		1
#define MV_SEMA_PEX0 		2
#define MV_SEMA_BRIDGE 		3
#define MV_SEMA_IRQ 		4
#define MV_SEMA_CLOCK		5
#define MV_SEMA_L2		6
#define MV_SEMA_TWSI		7
#define MV_SEMA_ADR_WIN		8

#define MV_SEMA_BARRIER(cpu)	(50 + cpu)


MV_BOOL mvSemaLock(MV_32 num);
MV_BOOL mvSemaTryLock(MV_32 num);
MV_BOOL mvSemaUnlock(MV_32 num);
MV_32   mvReadAmpReg(int regId);
MV_32 	mvWriteAmpReg(int regId, MV_32 value);

/* Turn on HW semapores only if AMP is enabled */
#ifndef CONFIG_MV_AMP_ENABLE
#define mvSemaLock
#define mvSemaTryLock
#define mvSemaUnlock
#define mvHwBarrier
#endif /* CONFIG_MV_AMP_ENABLE */

#endif /* mvsemaphore_h */
