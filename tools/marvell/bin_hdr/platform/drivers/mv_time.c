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
#include "config_marvell.h"     /* Required to identify SOC and Board */
#include "mv_os.h"
#if defined(MV88F78X60)
#include "ddr3_axp.h"
#elif defined(MV88F6710)
#include "ddr3_a370.h"
extern MV_U32 mvCpuL2ClkGet(MV_VOID);
#elif defined(MV88F68XX)
#include "ddr3_a38x.h"
#elif defined(MV88F69XX)
#include "ddr3_a39x.h"
#elif defined(MV88F66XX)
#include "ddr3_alp.h"
extern MV_U32 mvCpuL2ClkGet(MV_VOID);
#elif defined(MV88F672X)
#include "ddr3_a375.h"
extern MV_U32 mvCpuL2ClkGet(MV_VOID);
#elif defined(MV_MSYS_BC2)
#include "ddr3_msys_bc2.h"
#elif defined(MV_MSYS_AC3)
#include "ddr3_msys_ac3.h"
#else
#error "No SOC define for uart in binary header."
#endif
#define UBOOT_CNTR              0       /* counter to use for uboot timer  0,1 */


void __udelay(unsigned long usec)
{
    unsigned long delayticks;
    unsigned int cntmrCtrl;

    /* In case udelay is called before timier was initialized */
    delayticks = (usec * (MV_BOARD_REFCLK / 1000000));
    /* init the counter */
    MV_REG_WRITE(CNTMR_RELOAD_REG(UBOOT_CNTR),delayticks);
    MV_REG_WRITE(CNTMR_VAL_REG(UBOOT_CNTR),delayticks);

    /* set control for timer \ cunter and enable */
    /* read control register */
    cntmrCtrl = MV_REG_READ(CNTMR_CTRL_REG(UBOOT_CNTR));
    cntmrCtrl &= ~CTCR_ARM_TIMER_AUTO_EN(UBOOT_CNTR);
    cntmrCtrl |= CTCR_ARM_TIMER_EN(UBOOT_CNTR);
    cntmrCtrl |= CTCR_ARM_TIMER_25MhzFRQ_EN(UBOOT_CNTR);
    MV_REG_WRITE(CNTMR_CTRL_REG(UBOOT_CNTR),cntmrCtrl);

    while(MV_REG_READ(CNTMR_VAL_REG(UBOOT_CNTR)));

    /* disable times*/
    cntmrCtrl &= ~CTCR_ARM_TIMER_EN(UBOOT_CNTR);
    MV_REG_WRITE(CNTMR_CTRL_REG(UBOOT_CNTR),cntmrCtrl);
}
void __timerSet(unsigned long usec)
{
    unsigned int cntmrCtrl;
    unsigned long startTicks;

    /* In case udelay is called before timier was initialized */
    startTicks = (usec * (MV_BOARD_REFCLK / 1000000));
    /* init the counter */
    MV_REG_WRITE(CNTMR_RELOAD_REG(UBOOT_CNTR),startTicks);
    MV_REG_WRITE(CNTMR_VAL_REG(UBOOT_CNTR),startTicks);

    /* set control for timer \ cunter and enable */
    /* read control register */
    cntmrCtrl = MV_REG_READ(CNTMR_CTRL_REG(UBOOT_CNTR));
    cntmrCtrl &= ~CTCR_ARM_TIMER_AUTO_EN(UBOOT_CNTR);
    cntmrCtrl |= CTCR_ARM_TIMER_EN(UBOOT_CNTR);
    cntmrCtrl |= CTCR_ARM_TIMER_25MhzFRQ_EN(UBOOT_CNTR);
    MV_REG_WRITE(CNTMR_CTRL_REG(UBOOT_CNTR),cntmrCtrl);

}
MV_U32 __timerGet(void)
{
    return MV_REG_READ(CNTMR_VAL_REG(UBOOT_CNTR));
}

void __timerDisable(void)
{
    unsigned int cntmrCtrl;
    cntmrCtrl = MV_REG_READ(CNTMR_CTRL_REG(UBOOT_CNTR));
    cntmrCtrl &= ~CTCR_ARM_TIMER_AUTO_EN(UBOOT_CNTR);
    cntmrCtrl |= CTCR_ARM_TIMER_25MhzFRQ_EN(UBOOT_CNTR);
    /* disable times*/
    cntmrCtrl &= ~CTCR_ARM_TIMER_EN(UBOOT_CNTR);
    MV_REG_WRITE(CNTMR_CTRL_REG(UBOOT_CNTR),cntmrCtrl);
}
