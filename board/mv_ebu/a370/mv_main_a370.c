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
#include "mvSysHwConfig.h"
#include <asm/arch-armv7/vfp.h>
#include <asm/arch-armv7/vfpinstr.h>

void envVerifyAndSet(char* envName, char* value1, char* value2, int defaultValue);
void envSetDefault(char* envName, char* defaultValue);

int mv_get_arch_number(void)
{
	switch (mvBoardIdGet()) {
	case DB_88F6710_BP_ID:
		return 3038;
		break;
	default:
		return 3038;
		break;
	}
}

void setBoardEnv(void)
{
	envVerifyAndSet("enaFPU", "no", "yes",1);
	envSetDefault("mvNetConfig", "mv_net_config=1,(00:50:43:11:11:11,0:1:2:3:4),mtu=1500");
	envSetDefault("pxe_files_load", ":default.arm-armada370-db:default.arm-armadaxp:default.arm");
}

void mv_cpu_init(void)
{
	char *env;
	volatile unsigned int temp;

	/* enable access to CP10 and CP11 */
	temp = 0x00f00000;
	__asm__ __volatile__("mcr p15, 0, %0, c1, c0, 2" :: "r" (temp));

	env = getenv("enaFPU");
	if(env && ((strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0))){
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

	__asm__ __volatile__("mrc p15, 1, %0, c15, c1, 1" : "=r" (temp));
	temp |= BIT16; /* Disable reac clean intv */
	__asm__ __volatile__("mcr p15, 1, %0, c15, c1, 1\n" \
			"mcr p15, 0, %0, c7, c5, 4": :"r" (temp)); /*imb*/

	__asm__ __volatile__("mrc p15, 1, %0, c15, c1, 2" : "=r" (temp));
	temp |= (BIT25 | BIT27 | BIT29 | BIT30);

	/* removed BIT23 in order to enable fast LDR bypass */
	__asm__ __volatile__("mcr p15, 1, %0, c15, c1, 2\n" \
			"mcr p15, 0, %0, c7, c5, 4": :"r" (temp)); /*imb*/

	/* Enable speculative read miss from L1 to "line fill" L1 */
	__asm__ __volatile__("mrc p15, 1, %0, c15, c2, 0" : "=r" (temp));

	env = getenv("L1SpeculativeEn");
	if( (strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) )
		temp |= BIT7;
	else{
		temp &= ~BIT7;
	}

	__asm__ __volatile__("mcr p15, 1, %0, c15, c2, 0\n" \
			"mcr p15, 0, %0, c7, c5, 4": :"r" (temp)); /*imb*/

	/* Set L2C WT mode */
	temp = MV_REG_READ(CPU_L2_AUX_CTRL_REG) & ~CL2ACR_WB_WT_ATTR_MASK;
	env = getenv("setL2CacheWT");
	if(!env || ((strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0))) {
		temp |= CL2ACR_WB_WT_ATTR_WT;
	}
	MV_REG_WRITE(CPU_L2_AUX_CTRL_REG, temp);

	/* enable L2C */
	temp = MV_REG_READ(CPU_L2_CTRL_REG);
	env = getenv("disL2Cache");
	if((!env || (strcmp(env,"no") == 0) || (strcmp(env,"No") == 0)) && enaMonExt())
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
		if( env && ((strcmp(env,"WB") == 0) || (strcmp(env,"wb") == 0)) )
			temp |= CL2ACR_WB_WT_ATTR_WB;
		else if( env && ((strcmp(env,"WT") == 0) || (strcmp(env,"wt") == 0)) )
			temp |= CL2ACR_WB_WT_ATTR_WT;
		else
			temp |= CL2ACR_WB_WT_ATTR_PAGE;

		/* Set "Force Write Allocate" field */
		env = getenv("L2forceWrAlloc");
		if( env && ((strcmp(env,"no") == 0) || (strcmp(env,"No") == 0)) )
			temp |= CL2ACR_FORCE_NO_WA;
		else if( env && ((strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0)) )
			temp |= CL2ACR_FORCE_WA;
		else
			temp |= CL2ACR_FORCE_WA_DISABLE;

		/* Set "ECC" */
		env = getenv("L2EccEnable");
		if(!env || ( (strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) ) )
			temp &= ~CL2ACR_ECC_EN;
		else
			temp |= CL2ACR_ECC_EN;

		/* Set other L2 configurations */
		temp |= (CL2ACR_PARITY_EN | CL2ACR_INVAL_UCE_EN);

		/* Set L2 algorithm to semi_pLRU */
		temp &= ~CL2ACR_REP_STRGY_MASK;
		temp |= CL2ACR_REP_STRGY_PLRU_MASK;

		/* Write to L2 Auxilary control register */
		MV_REG_WRITE(CPU_L2_AUX_CTRL_REG, temp);

		env = getenv("L2SpeculativeRdEn");
		if(env && ((strcmp(env,"no") == 0) || (strcmp(env,"No") == 0)) )
			MV_REG_BIT_SET(0x20228, ((0x1 << 5)));
		else
			MV_REG_BIT_RESET(0x20228, ((0x1 << 5)));

	}

	/* Enable i cache */
	asm ("mrc p15, 0, %0, c1, c0, 0":"=r" (temp));
	temp |= BIT12;
	/* Change reset vector to address 0x0 */
	temp &= ~BIT13;
	asm ("mcr p15, 0, %0, c1, c0, 0\n" \
		"mcr p15, 0, %0, c7, c5, 4": :"r" (temp)); /* imb */

	/* Disable MBUS Err Prop - inorder to avoid data aborts */
	MV_REG_BIT_RESET(SOC_COHERENCY_FABRIC_CTRL_REG, BIT8);
	/* Enable IOCC */
	env = getenv("cacheShare");
	if(((strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0)) && enaMonExt()) {

		__asm__ __volatile__("mrc p15, 1, %0, c15, c1, 1" : "=r" (temp));
		temp |= BIT7; /* @ v7 IO coherency support (Single core) */
		__asm__ __volatile__("mcr p15, 1, %0, c15, c1, 1\n" \
				"mcr p15, 0, %0, c7, c5, 4": :"r" (temp)); /*imb*/

		MV_REG_BIT_SET(SOC_COHERENCY_FABRIC_CTRL_REG, BIT24);
	}
}

/* Set unit in power off mode acording to the detection of MPP/SERDES */
#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
void mv_set_power_scheme(void)
{
	MV_U32 boardId = mvBoardIdGet();
	MV_U32 mppGrp1 = mvBoardMppModulesCfgGet(1);
	MV_U32 mppGrp2 = mvBoardMppModulesCfgGet(2);
	MV_U32 srdsCfg = mvBoardSerdesModeGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mv_set_power_scheme: Board unknown.\n");
		return;
	}

	mvOsOutput("Shutting down unused interfaces:\n");

	/* PCI-E */
	if (!(srdsCfg & SRDS_MOD_PCIE0_LANE0)) {
		mvCtrlPwrClckSet(PEX_UNIT_ID, 0, MV_FALSE);
		mvOsOutput("       PEX0\n");
	}
	if (!(srdsCfg & SRDS_MOD_PCIE1_LANE1)) {
		mvCtrlPwrClckSet(PEX_UNIT_ID, 1, MV_FALSE);
		mvOsOutput("       PEX1\n");
	}

	/* SATA */
	/* Disable SATA 0 only if SATA 1 is not required as well */
	if (!(srdsCfg & SRDS_MOD_SATA1_LANE3) || (mvCtrlSataMaxPortGet() < 2)) {
		mvCtrlPwrClckSet(SATA_UNIT_ID, 1, MV_FALSE);
		if (mvCtrlSataMaxPortGet() == 2)
			mvOsOutput("       SATA1\n");

		if (!(srdsCfg & (SRDS_MOD_SATA0_LANE0 | SRDS_MOD_SATA0_LANE2))) {
			mvCtrlPwrClckSet(SATA_UNIT_ID, 0, MV_FALSE);
			mvOsOutput("       SATA0\n");
		}
	}

	/* GBE - SMI Bus is connected to EGIGA0 so we only shut down EGIGA1 if needed */
	if (!(srdsCfg & SRDS_MOD_SGMII1_LANE3)) {
		if (!(mppGrp1 & (MV_BOARD_GMII0 | MV_BOARD_RGMII1))) {
			mvCtrlPwrClckSet(ETH_GIG_UNIT_ID, 1, MV_FALSE);
			mvOsOutput("       GBE1\n");
		}
	}

	/* SDIO */
	if (!(mppGrp1 & MV_BOARD_SDIO)) {
		mvCtrlPwrClckSet(SDIO_UNIT_ID, 0, MV_FALSE);
		mvOsOutput("       SDIO\n");
	}

	/* Audio */
	if (!((mppGrp1 & MV_BOARD_I2S) || (mppGrp2 & MV_BOARD_I2S))) {
		mvCtrlPwrClckSet(AUDIO_UNIT_ID, 0, MV_FALSE);
		if (mvCtrlAudioSupport())
			mvOsOutput("       AUDIO\n");
	}
	
	/* TDM */
	if(!((mppGrp1 & MV_BOARD_TDM) || (mppGrp2 & MV_BOARD_TDM)) || !(mvCtrlTdmSupport())) {
		mvCtrlPwrClckSet(TDM_2CH_UNIT_ID, 0, MV_FALSE);
		if (mvCtrlTdmSupport())
			mvOsOutput("       TDM\n");
	}
}
#endif /* defined(MV_INCLUDE_CLK_PWR_CNTRL) */