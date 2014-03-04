/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
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
#include "mvCommon.h"
#include "mvTypes.h"
#include "mvBoardEnvLib.h"
#include "mvCtrlEnvLib.h"
#include "cpu/mvCpu.h"
#include "mvSysHwConfig.h"
#include "gpp/mvGpp.h"
#include <asm/arch-armv7/vfp.h>
#include <asm/arch-armv7/vfpinstr.h>

void envVerifyAndSet(char* envName, char* value1, char* value2, int defaultValue);
void envSetDefault(char* envName, char* defaultValue);

int mv_get_arch_number(void)
{
	switch (mvBoardIdGet()) {
	case RD_DX_BC2_ID:
	case DB_DX_BC2_ID:
		return 3035;
	default:
		return 3036;
	}
}

void setBoardEnv(void)
{
	char *env;

	envSetDefault("mvNetConfig", "mv_net_config=4,(00:50:43:11:11:11,0:1:2:3),mtu=1500");
	envSetDefault("lcd0_enable", "0");
	envSetDefault("lcd0_params", "640x480-16@60");
	envSetDefault("lcd_panel", "0");
	/* CPU streaming */
	envVerifyAndSet("enaCpuStream", "no", "yes", 1);
	envVerifyAndSet("MPmode", "SMP", "AMP", 1);
	/* Make address 0x80000000-0x8fffffff shared (set 'S' in pgd) */

	env = getenv("disL2Prefetch");
	if (!env || ( (strcmp(env, "yes") == 0) || (strcmp(env, "Yes") == 0) ) ) {
		setenv("disL2Prefetch", "yes");
		/* ICache Prefetch */
		envVerifyAndSet("enaICPref", "no", "yes", 2);
		/* DCache Prefetch */
		envVerifyAndSet("enaDCPref", "no", "yes", 2);
	} else {
		setenv("disL2Prefetch", "no");
		setenv("enaICPref", "no");
		setenv("enaDCPref", "no");
	}
	envVerifyAndSet("enaFPU", "no", "yes", 2);
	envSetDefault("pxe_files_load", ":default.arm-armadaxp-db:default.arm-armadaxp:default.arm");
}

void mv_cpu_init(void)
{
	char *env;
	volatile unsigned int temp;

	/* enable access to CP10 and CP11 */
	temp = 0x00f00000;
	__asm__ __volatile__ ("mcr p15, 0, %0, c1, c0, 2" :: "r" (temp));

	env = getenv("enaFPU");
	if (env && ((strcmp(env, "yes") == 0) || (strcmp(env, "Yes") == 0))) {
		/* init and Enable FPU to Run Fast Mode */
		printf("FPU initialized to Run Fast Mode.\n");
		/* Enable */
		temp = FPEXC_ENABLE;
		fmxr(FPEXC, temp);
		/* Run Fast Mode */
		temp = fmrx(FPSCR);
		temp |= (FPSCR_DEFAULT_NAN | FPSCR_FLUSHTOZERO);
		fmxr(FPSCR, temp);
	}else{
		printf("FPU not initialized\n");
		/* Disable */
		temp = fmrx(FPEXC);
		temp &= ~FPEXC_ENABLE;
		fmxr(FPEXC, temp);
	}

	__asm__ __volatile__ ("mrc p15, 1, %0, c15, c1, 2" : "=r" (temp));
	temp |= (BIT25 | BIT27 | BIT29 | BIT30);
	/* removed BIT23 in order to enable fast LDR bypass */
	__asm__ __volatile__ ("mcr p15, 1, %0, c15, c1, 2\n" \
			      "mcr p15, 0, %0, c7, c5, 4" : : "r" (temp)); /*imb*/

	/* Set L2C WT mode */
	temp = MV_REG_READ(CPU_L2_AUX_CTRL_REG) & ~CL2ACR_WB_WT_ATTR_MASK;
	env = getenv("setL2CacheWT");
	if (!env || ((strcmp(env, "yes") == 0) || (strcmp(env, "Yes") == 0)))
		temp |= CL2ACR_WB_WT_ATTR_WT;
	 /* Set L2 algorithm to semi_pLRU */
	temp &= ~CL2ACR_REP_STRGY_MASK;

	if (mvCtrlRevGet() == 1)
		temp |= CL2ACR_REP_STRGY_semiPLRU_MASK;
	else{
		temp |= CL2ACR_REP_STRGY_semiPLRU_WA_MASK;
		temp |= CL2_DUAL_EVICTION;
		temp |= CL2_PARITY_ENABLE;
		temp |= CL2_InvalEvicLineUCErr;
	} /* MV88F78X60_B0  */

	MV_REG_WRITE(CPU_L2_AUX_CTRL_REG, temp);

	/* enable L2C */
	temp = MV_REG_READ(CPU_L2_CTRL_REG);

	env = getenv("disL2Cache");
	if (!env || ((strcmp(env, "no") == 0) || (strcmp(env, "No") == 0)))
		temp |= CL2CR_L2_EN_MASK;
	else
		temp &= ~CL2CR_L2_EN_MASK;

	MV_REG_WRITE(CPU_L2_CTRL_REG, temp);

	/* Configure L2 options if L2 exists */
	if (MV_REG_READ(CPU_L2_CTRL_REG) & CL2CR_L2_EN_MASK) {
		/* Read L2 Auxilary control register */
		temp = MV_REG_READ(CPU_L2_AUX_CTRL_REG);
		/* Clear fields */
		temp &= ~(CL2ACR_WB_WT_ATTR_MASK | CL2ACR_FORCE_WA_MASK);

		/* Set "Force write policy" field */
		env = getenv("L2forceWrPolicy");
		if ( env && ((strcmp(env, "WB") == 0) || (strcmp(env, "wb") == 0)) )
			temp |= CL2ACR_WB_WT_ATTR_WB;
		else if ( env && ((strcmp(env, "WT") == 0) || (strcmp(env, "wt") == 0)) )
			temp |= CL2ACR_WB_WT_ATTR_WT;
		else
			temp |= CL2ACR_WB_WT_ATTR_PAGE;

		/* Set "Force Write Allocate" field */
		env = getenv("L2forceWrAlloc");
		if ( env && ((strcmp(env, "no") == 0) || (strcmp(env, "No") == 0)) )
			temp |= CL2ACR_FORCE_NO_WA;
		else if ( env && ((strcmp(env, "yes") == 0) || (strcmp(env, "Yes") == 0)) )
			temp |= CL2ACR_FORCE_WA;
		else
			temp |= CL2ACR_FORCE_WA_DISABLE;

		/* Set "ECC" */
		env = getenv("L2EccEnable");
		if (!env || ( (strcmp(env, "no") == 0) || (strcmp(env, "No") == 0) ) )
			temp &= ~CL2ACR_ECC_EN;
		else
			temp |= CL2ACR_ECC_EN;

		/* Set other L2 configurations */
		temp |= (CL2ACR_PARITY_EN | CL2ACR_INVAL_UCE_EN);

		/* Set L2 algorithm to semi_pLRU */
		temp &= ~CL2ACR_REP_STRGY_MASK;
		temp |= CL2ACR_REP_STRGY_semiPLRU_MASK;

		/* Write to L2 Auxilary control register */
		MV_REG_WRITE(CPU_L2_AUX_CTRL_REG, temp);

		env = getenv("L2SpeculativeRdEn");
		if (env && ((strcmp(env, "no") == 0) || (strcmp(env, "No") == 0)) )
			MV_REG_BIT_SET(0x20228, ((0x1 << 5)));
		else
			MV_REG_BIT_RESET(0x20228, ((0x1 << 5)));

	}

	/* Enable i cache */
	asm ("mrc p15, 0, %0, c1, c0, 0" : "=r" (temp));
	temp |= BIT12;
	/* Change reset vector to address 0x0 */
	temp &= ~BIT13;
	asm ("mcr p15, 0, %0, c1, c0, 0\n" \
	     "mcr p15, 0, %0, c7, c5, 4" : : "r" (temp));  /* imb */

	/* Disable MBUS Err Prop - inorder to avoid data aborts */
	MV_REG_BIT_RESET(SOC_COHERENCY_FABRIC_CTRL_REG, BIT8);
}

/* Set unit in power off mode acording to the detection of MPP/SERDES */
#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
void mv_set_power_scheme(void)
{
	return;
}

#endif /* defined(MV_INCLUDE_CLK_PWR_CNTRL) */
