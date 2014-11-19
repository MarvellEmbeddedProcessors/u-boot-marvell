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

#ifndef _MV_RTC2_H
#define _MV_RTC2_H

#include <mvOs.h>

#define MV_RTC2_REGS_OFFSET		0xA3800
#define MV_RTC2_SOC_OFFSET		0x184A0

/* The RTC DRS revision 1.2 indicates that firmware should wait 5us after every register write
   to the RTC hard macro, so that the required update can occur without holding off the system bus
*/
#define RTC_READ_REG(reg)			MV_REG_READ(MV_RTC2_REGS_OFFSET + reg)
#define RTC_WRITE_REG(val, reg)		{ MV_REG_WRITE(MV_RTC2_REGS_OFFSET + reg, val); udelay(5); }

#define RTC_0HZ		0x00
#define RTC_1HZ		0x04
#define RTC_2HZ		0x08
#define RTC_4HZ		0x10
#define RTC_8HZ		0x20

#define RTC_NOMINAL_TIMING			0x20

#define RTC_STATUS_REG_OFFS			0x0
#define RTC_IRQ_1_CONFIG_REG_OFFS	0x4
#define RTC_IRQ_2_CONFIG_REG_OFFS	0x8
#define RTC_TIME_REG_OFFS			0xC
#define RTC_ALARM_1_REG_OFFS		0x10
#define RTC_ALARM_2_REG_OFFS		0x14
#define RTC_CLOCK_CORR_REG_OFFS		0x18
#define RTC_TEST_CONFIG_REG_OFFS	0x1C

#define RTC_SZ_STATUS_ALARM1_MASK			0x1
#define RTC_SZ_STATUS_ALARM2_MASK			0x2
#define RTC_SZ_TIMING_RESERVED1_MASK		0xFFFF0000
#define RTC_SZ_INTERRUPT1_INT1AE_MASK		0x1
#define RTC_SZ_INTERRUPT1_RESERVED1_MASK	0xFFFFFFC0
#define RTC_SZ_INTERRUPT2_INT2FE_MASK		0x2
#define RTC_SZ_INTERRUPT2_RESERVED1_MASK	0xFFFFFFC0

#endif /* _MV_RTC2_H */
