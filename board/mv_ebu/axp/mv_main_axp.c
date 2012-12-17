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

int mv_get_arch_number(void)
{
	switch (mvBoardIdGet()) {
	case RD_78460_NAS_ID:
		return 3500;
		break;
	case DB_784MP_GP_ID:
		return 3035;
		break;
	case RD_78460_SERVER_ID:
	case RD_78460_SERVER_REV2_ID:
		return 3037;
	case DB_88F78XX0_BP_ID:
	default:
		return 3036;
		break;
	}
}

/* Set unit in power off mode acording to the detection of MPP/SERDES */
#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
void mv_set_power_scheme(void){

	char name[30];
	mvBoardNameGet(name);

	/*start with shutting the power to any unused cores
	this is done via mpps on the DB board*/
	if(strcmp(name,"DB-78460-BP")== 0){
		//MV_U32 soc_type=mvCtrlModelRevGet();
		switch (mvCtrlGetCpuNum()+1) {
			case 1:
				/*in the DB board GPIO 40 * 57 control the power to CPU1 & CPU 2&3 */
				/*make sure GPIO40 & 57 are enabled as output first*/
			printf(" shutting the power to CORES 1,2 & 3\n");
			mvGppTypeSet(1, (BIT8 | BIT25),((MV_GPP_OUT & BIT8) | (MV_GPP_OUT & BIT25)) );
			mvGppValueSet(1, (BIT8 | BIT25) ,(BIT8 | BIT25));
				break;


			case 2:
				/*turn off the power to core 2&3 - GPIO 57*/
				printf("shutting the power to CORES 2 & 3\n");
				mvGppTypeSet(1, BIT25 , ( MV_GPP_OUT & BIT25 ) );
				mvGppValueSet(1, BIT25 ,BIT25 );
				break;
			case 3: /*core 2 &3 are tied togther and can't be seperated*/
			case 4:
			default:
				break;
		}
	}
}
#endif /* defined(MV_INCLUDE_CLK_PWR_CNTRL) */