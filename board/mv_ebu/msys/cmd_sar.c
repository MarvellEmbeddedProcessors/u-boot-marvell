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
#include <common.h>
#if defined(CONFIG_CMD_SAR)
#include "cpu/mvCpu.h"
#include "ctrlEnv/mvCtrlEnvRegs.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "boardEnv/mvBoardEnvLib.h"

#if defined CONFIG_ALLEYCAT3
	extern MV_BOARD_INFO *marvellAC3BoardInfoTbl[];
	MV_BOARD_INFO **mvMsysBoardInfoTbl = marvellAC3BoardInfoTbl;
#elif defined CONFIG_BOBCAT2
	extern MV_BOARD_INFO *marvellBC2BoardInfoTbl[];
	MV_BOARD_INFO **mvMsysBoardInfoTbl = marvellBC2BoardInfoTbl;
#elif defined CONFIG_BOBK
	extern MV_BOARD_INFO *marvellBOBKBoardInfoTbl[];
	MV_BOARD_INFO **mvMsysBoardInfoTbl = marvellBOBKBoardInfoTbl;
#endif
/*
   bc2 sample and reset register
#     4f       #     4e       #     4d       #      4c      #
#              #              #              #              #
#--|--|--|--|--#--|--|--|--|--#--|--|--|--|--#--|--|--|--|--#
#  |  |  |  |  #  |  |  |  |  #  |  |  |  |  #  |  |  |  |  #
#--|--|--|--|--#--|--|--|--|--#--|--|--|--|--#--|--|--|--|--#
#  |           #  |        |  #     |        #              #
#-P|-R|bootsel-#-R|-TM-f---|-CPU-f--|-CORE-f-#-----devid----#
*/

enum { /* Update defaultValue[] if any change to this enum has made!*/
	CMD_CORE_CLK_FREQ = 0,
#if defined(CONFIG_BOBK)
	CMD_BYPASS_CORE_CLK_FREQ,
#endif
	CMD_CPU_DDR_REQ,
#if defined(CONFIG_BOBCAT2) || defined(CONFIG_BOBK)
	CMD_TM_FREQ,
	CMD_JTAG_CPU,
#endif
#if defined CONFIG_BOBCAT2/* TODO */
	CMD_PTP_PLL,
	CMD_OOB0_CON,
	CMD_OOB1_CON,
	CMD_PEX_GEN1,
#endif
#if defined(CONFIG_ALLEYCAT3)
	CMD_PCIE_CLOCK,
	CMD_PLL_CLOCK,
	CMD_AVS_MODE,
	CMD_SLAVE_ADDR,
	CMD_DEVICE_NUM,
	CMD_DDR_ECC_EN,
#endif
	CMD_BOARD_ID,
	CMD_PCIE_MODE,
	CMD_BOOTSRC,
	CMD_DEVICE_ID,
	CMD_DUMP,
	CMD_DEFAULT,
	CMD_UNKNOWN
};


#ifdef CONFIG_BOBCAT2
	int defaultValue[] = { 0,	/* Core clock */
						   3,	/* CPU/DDR clock */
						   3,	/* TM frequency */
						   1,	/* JTAG CPU 1 == MSYS */
						   0,	/* PTP PLL */
						   0,	/* OOB0 connection */
						   0,	/* OOB1 connection */
						   0,	/* Force AMC RC GEN1 PCIe */
						   0,	/* Board ID */
						   0,	/* PCIe mode */
						   3,	/* Boot source */
						   0 };	/* Device ID */
	MV_U32 coreClockTbl[] = MV_CORE_CLK_TBL_BC2;
	MV_CPUDDR_MODE cpuDdrClkTbl[] = MV_CPU_DDR_CLK_TBL_BC2;
	MV_U32 coreClockTblSize = ARRAY_SIZE(coreClockTbl);
	MV_U32 cpuDdrClkTblSize = ARRAY_SIZE(cpuDdrClkTbl);
#elif defined(CONFIG_BOBK)
	int defaultValue[] = { 0,	/* Core clock */
						   0,	/* bypass_coreclock: if core clock is set to bypass(7), this field select the bypass clock */
						   3,	/* CPU/DDR clock */
						   2,	/* TM frequency */
						   1,	/* JTAG CPU 1 == MSYS */
						   0,	/* Board ID */
						   0,	/* PCIe mode */
						   3,	/* Boot source */
						   0 };	/* Device ID */
	MV_U32 coreClockTblBobkCetus[] = MV_CORE_CLK_TBL_BOBK_CETUS;
	MV_U32 coreClockTblBobkCaelum[] = MV_CORE_CLK_TBL_BOBK_CAELUM;
	MV_U32 bypassCoreClockTbl[] = MV_BYPASS_CORE_CLK_TBL_BOBK;
	MV_CPUDDR_MODE cpuDdrClkTblBobkCetus[] = MV_CPU_DDR_CLK_TBL_BOBK_CETUS;
	MV_CPUDDR_MODE cpuDdrClkTblBobkCaelum[] = MV_CPU_DDR_CLK_TBL_BOBK_CAELUM;
	MV_U32 *coreClockTbl;
	MV_CPUDDR_MODE *cpuDdrClkTbl;
	MV_U32 coreClockTblSize;
	MV_U32 cpuDdrClkTblSize;
#elif defined(CONFIG_ALLEYCAT3)
	int defaultValue[] = { 4,	/* Core clock */
						   3,	/* CPU/DDR clock */
						   0,	/* PCIe clock */
						   1,	/* PLL clock */
						   1,	/* AVS Mode */
						   1,	/* I2C/SMI Slave address */
						   0,	/* Device number */
						   0,	/* DDR ECC enable */
						   0,	/* Board ID */
						   0,	/* PCIe mode */
						   3,	/* Boot source */
						   0 };	/* Device ID */

	MV_U32 coreClockTbl[] = MV_CORE_CLK_TBL_AC3;
	MV_CPUDDR_MODE cpuDdrClkTbl[] = MV_CPU_DDR_CLK_TBL_AC3;
	MV_U32 coreClockTblSize = ARRAY_SIZE(coreClockTbl);
	MV_U32 cpuDdrClkTblSize = ARRAY_SIZE(cpuDdrClkTbl);
#else
#error "Unknown MSYS family!"
#endif

MV_TM_MODE tmClkTbl[] = MV_TM_CLK_TBL;

typedef struct {
	char bootstr[80];
	MV_STATUS internalFreq;
} MV_BOOT_SRC;

MV_BOOT_SRC bootSrcTbl[] = {
	{ "BootROM enabled, Boot from Device (NOR) flash",			MV_FALSE },
	{ "BootROM enabled, Boot from NAND flash (on DEV_CSn[0])",	MV_FALSE },
	{ "BootROM enabled, Boot from UART",						MV_FALSE },
	{ "BootROM enabled, Boot from SPI0 (CS0)",					MV_FALSE },
	{ "Reserved. BootROM enabled, Boot from PCIe",				MV_TRUE	},
	{ "BootROM enabled,Standby slave. Must set PCI-E as endpoint",	MV_FALSE },
	{ "BootROM enabled, UART debug prompt mode",				MV_FALSE },
	{ "Reserved BootROM disabled, Boot from SPI 0(CS0)",		MV_TRUE	}
};

void SatR_usage(void);

static int sar_cmd_get(const char *cmd)
{
#if defined CONFIG_ALLEYCAT3
	MV_U32 revId = mvCtrlRevGet();
#endif

	if (strcmp(cmd, "coreclock") == 0)
		return CMD_CORE_CLK_FREQ;
#if defined CONFIG_BOBK
	if (strcmp(cmd, "bypass_coreclock") == 0)
		return CMD_BYPASS_CORE_CLK_FREQ;
#endif
	if (strcmp(cmd, "freq") == 0)
		return CMD_CPU_DDR_REQ;
#if defined(CONFIG_BOBCAT2) || defined(CONFIG_BOBK)
	if (strcmp(cmd, "tmfreq") == 0)
		return CMD_TM_FREQ;
	if (strcmp(cmd, "jtagcpu") == 0)
		return CMD_JTAG_CPU;
#endif
#if defined CONFIG_BOBCAT2
	if (strcmp(cmd, "ptppll") == 0)
		return CMD_PTP_PLL;
	if (strcmp(cmd, "oob0con") == 0)
		return CMD_OOB0_CON;
	if (strcmp(cmd, "oob1con") == 0)
		return CMD_OOB1_CON;
	if (strcmp(cmd, "pciegen1") == 0)
		return CMD_PEX_GEN1;
#endif
#if defined CONFIG_ALLEYCAT3
	if (strcmp(cmd, "pciclock") == 0)
		return CMD_PCIE_CLOCK;
	if (revId == MV_ALLEYCAT3_A0_ID) {
		if (strcmp(cmd, "pllclock") == 0)
			return CMD_PLL_CLOCK;
	} else {
		if (strcmp(cmd, "avsmode") == 0)
			return CMD_AVS_MODE;
	}
	if (strcmp(cmd, "slaveaddr") == 0)
		return CMD_SLAVE_ADDR;
	if (strcmp(cmd, "devicenum") == 0)
		return CMD_DEVICE_NUM;
	if (strcmp(cmd, "ddreccenable") == 0)
		return CMD_DDR_ECC_EN;
#endif
	if (strcmp(cmd, "boardid") == 0)
		return CMD_BOARD_ID;
	if (strcmp(cmd, "pcimode") == 0)
		return CMD_PCIE_MODE;
	if (strcmp(cmd, "bootsrc") == 0)
		return CMD_BOOTSRC;
	if (strcmp(cmd, "deviceid") == 0)
		return CMD_DEVICE_ID;
	if (strcmp(cmd, "dump") == 0)
		return CMD_DUMP;
	if (strcmp(cmd, "default") == 0)
		return CMD_DEFAULT;
	return CMD_UNKNOWN;
}

static int do_sar_list(int mode)
{
	int i;
#if defined(CONFIG_ALLEYCAT3) || defined(CONFIG_BOBK)
	MV_U32 revId = mvCtrlRevGet();
#endif
	MV_U32 stepping =
#if defined CONFIG_ALLEYCAT3
		mvCtrlStepGet();
#else
		0;
#endif

	switch (mode) {
	case CMD_CORE_CLK_FREQ:
		printf("Determines the core clock frequency:\n");
		printf("\t| ID  | Core clock (MHz) |\n");
		printf("\t--------------------------\n");
		for (i = 0; i < coreClockTblSize; i++)
			printf("\t| %2d  |      %4d        |\n", i, coreClockTbl[i]);
#if defined CONFIG_BOBK
		if (revId == MV_BOBK_A0_ID)
			/* Bypass mode is reserved(out of table), print it separately */
			printf("\t| %2d  |   Bypass Mode    |\n", 7);
#endif
		printf("\t--------------------------\n");
		break;
#if defined CONFIG_BOBK
	case CMD_BYPASS_CORE_CLK_FREQ:
		if (revId != MV_BOBK_A0_ID) {
			printf("Bypass coreclock is not supported\n");
			break;
		}
	printf("Determines the core clock frequency used in bypass mode(when \"coreclock\"=7):\n");
	printf("\t| ID  | Core clock (MHz) |\n");
	printf("\t--------------------------\n");
	for (i = 0; i < ARRAY_SIZE(bypassCoreClockTbl); i++)
		printf("\t| %2d  |      %4d        |\n", i, bypassCoreClockTbl[i]);
	printf("\t--------------------------\n");
	break;
#endif
	case CMD_CPU_DDR_REQ:
		printf("Determines the CPU and DDR frequency:\n");
		printf("\t| ID  | CPU Freq (MHz) | DDR Freq (MHz) |\n");
		printf("\t-----------------------------------------\n");
		for (i = 0; i < cpuDdrClkTblSize; i++) {
			if (cpuDdrClkTbl[i].internalFreq ||
			   (stepping && (cpuDdrClkTbl[i].ddrFreq >= 800)))
				continue;
			printf("\t| %2d  |      %4d      |      %4d      |\n",
			       i, cpuDdrClkTbl[i].cpuFreq, cpuDdrClkTbl[i].ddrFreq);
		}
		printf("\t-----------------------------------------\n");
		break;
#if defined(CONFIG_BOBCAT2) || defined(CONFIG_BOBK)
	case CMD_TM_FREQ:
		printf("Determines the TM frequency:\n");
		printf("\t| ID  | TM Freq (MHz) | DDR Freq (MHz) |\n");
		printf("\t----------------------------------------\n");
		printf("\t|  0  |   Disabled    |                |\n");
		for (i = 1; i < ARRAY_SIZE(tmClkTbl); i++) {
			if (tmClkTbl[i].internalFreq)
				continue;
			printf("\t| %2d  |      %4d     |      %4d      |\n",
			       i, tmClkTbl[i].tmFreq, tmClkTbl[i].ddr3Freq);
		}
		printf("\t----------------------------------------\n");
		break;
	case CMD_JTAG_CPU:
		printf("Determines the JTAG to CPU connection:\n");
		printf("\t| ID  | JTAG connection |\n");
		printf("\t------------------------\n");
		printf("\t|  0  |  Cortex-M3      |\n");
		printf("\t|  1  |  MSYS (PJ4B)    |\n");
		printf("\t------------------------\n");
		break;
#endif
#if defined CONFIG_BOBCAT2
	case CMD_PTP_PLL:
		printf("Determines the PTP PLL multiplier configurtation:\n");
		printf("\t| ID  |  PTP PLL Multiplier |\n");
		printf("\t----------------------------\n");
		printf("\t|  0  |      20             |\n");
		printf("\t|  1  |      21.875         |\n");
		printf("\t----------------------------\n");
		break;
	case CMD_OOB0_CON:
	case CMD_OOB1_CON:
		printf("Determines the OOB port 0/1 to Physical port connection:\n");
		printf("\t| ID  | OOB port | Physical port   |\n");
		printf("\t-----------------------------------\n");
		printf("\t|  0  |    0     | 20 (back panel) |\n");
		printf("\t|  1  |    0     | 0 (front panel) |\n");
		printf("\t|  2  |    0     | Not connected   |\n");
		printf("\t-----------------------------------\n");
		printf("\t|  0  |    1     | 21 (back panel) |\n");
		printf("\t|  1  |    1     | 1 (front panel) |\n");
		printf("\t|  2  |    1     | Not connected   |\n");
		printf("\t-----------------------------------\n");
		break;
	case CMD_PEX_GEN1:
		printf("Determines the PCIe GEN1 enforcement mode om AMC RC:\n");
		printf("\t| ID  |   Force PCIe GEN1   |\n");
		printf("\t----------------------------\n");
		printf("\t|  0  |   Do not force      |\n");
		printf("\t|  1  |   Force GEN1        |\n");
		printf("\t----------------------------\n");
		break;
#endif

#if defined CONFIG_ALLEYCAT3
	case CMD_DDR_ECC_EN:
		printf("Determines the DDR ECC status:\n");
		printf("\t| ID  |  ECC status    |\n");
		printf("\t------------------------\n");
		printf("\t|  0  |  ECC Disabled  |\n");
		printf("\t|  1  |  ECC Enabled   |\n");
		printf("\t------------------------\n");
		break;
	case CMD_PCIE_CLOCK:
		printf("Determines the PCI-E clock source:\n");
		printf("\t| ID  |           Clock source            |\n");
		printf("\t-------------------------------------------\n");
		printf("\t|  0  |  Internally generated by PLL      |\n");
		printf("\t|  1  |  External 100 MHz from PEX_CLK_P  |\n");
		printf("\t-------------------------------------------\n");
		break;
	case CMD_PLL_CLOCK:
		if (revId == MV_ALLEYCAT3_A0_ID) {
			printf("Determines the PLL VCO clock frequency:\n");
			printf("\t| ID  | Clock freq (GHz) |\n");
			printf("\t--------------------------\n");
			printf("\t|  0  |          1       |\n");
			printf("\t|  1  |         2.5      |\n");
			printf("\t--------------------------\n");
		} else
			printf("The filed is not available on this SoC revision\n");
		break;
	case CMD_AVS_MODE:
		if (revId != MV_ALLEYCAT3_A0_ID) {
			printf("Determines the AVS operation mode:\n");
			printf("\t| ID  |  Mode  |\n");
			printf("\t----------------\n");
			printf("\t|  0  | Slave  |\n");
			printf("\t|  1  | Master |\n");
			printf("\t----------------\n");
		} else
			printf("The filed is not available on this SoC revision\n");
		break;
	case CMD_SLAVE_ADDR:
		printf("Determines the CPU I2C/SMI address:\n");
		printf("\t| ID  | CPU_SMI_I2C_ADDR[2] |\n");
		printf("\t-----------------------------\n");
		printf("\t|  0  |          0          |\n");
		printf("\t|  1  |          1          |\n");
		printf("\t-----------------------------\n");
		break;
	case CMD_DEVICE_NUM:
		printf("Determines the device number (0-3)\n");
		break;
#endif
	case CMD_BOARD_ID:
		printf("Determines the board ID (0-7)\n");
		printf("\t| ID  |      Board               |\n");
		printf("\t----------------------------------\n");
		for (i = 0; i < MV_MARVELL_BOARD_NUM ; i++)
			printf("\t|  %d  |  %-22s  |\n", i, mvMsysBoardInfoTbl[i]->boardName );
		printf("\t----------------------------------\n");
		break;
	case CMD_PCIE_MODE:
		printf("Determines the PCI-E mode:\n");
		printf("\t| ID  |       Mode     |\n");
		printf("\t------------------------\n");
		printf("\t|  0  |  Endpoint      |\n");
		printf("\t|  1  |  Root Complex  |\n");
		printf("\t------------------------\n");
		break;
	case CMD_BOOTSRC:
		printf("Determines the Boot source device:\n");
		printf("\t| ID  | Boot Source description                                 |\n");
		printf("\t-----------------------------------------------------------------\n");
		for (i = 0; i < 7; i++) {
			if (bootSrcTbl[i].internalFreq)
				continue;
			printf("\t| %2d  | %s\n", i, bootSrcTbl[i].bootstr);
		}
		printf("\t-----------------------------------------------------------------\n");
		break;
	case CMD_DEVICE_ID:
		printf("Determines the device ID (0-31)\n");
		break;
	case CMD_UNKNOWN:
	default:
		printf("Usage: sar list [options] (see help) \n");
		return 1;
	}
	return 0;
}

static int do_sar_read(int mode)
{
	MV_U8 tmp, i;
#if defined(CONFIG_ALLEYCAT3) || defined(CONFIG_BOBK)
	MV_U32 revId = mvCtrlRevGet();
#endif

	switch (mode) {
	case CMD_CORE_CLK_FREQ:
		if (mvBoardCoreFreqGet(&tmp) == MV_OK) {
#if defined CONFIG_BOBK
			if (tmp == 7 && (revId == MV_BOBK_A0_ID))
				printf("coreclock\t\t= %d ==>  bypass mode, refer to bypass_coreclock\n", tmp);
			else
#endif
				printf("coreclock\t\t= %d ==>  Core @ %dMHz\n", tmp, coreClockTbl[tmp]);
		} else
			printf("coreclock Error: failed reading Core Clock Frequency (PLL_0)\n");
		break;

#if defined CONFIG_BOBK
	case CMD_BYPASS_CORE_CLK_FREQ:
		if (revId != MV_BOBK_A0_ID) {
			printf("Bypass coreclock is not supported\n");
			break;
		}
		if (mvBoardBypassCoreFreqGet(&tmp) == MV_OK) {
			printf("bypass_coreclock\t= %d ==>  Core @ %dMHz valid only when \"coreclock\"=7(bypass mode)\n",
				tmp, bypassCoreClockTbl[tmp]);
		} else
			printf("bypass_coreclock Error: failed reading Core Clock Frequency in bypass mode\n");
		break;
#endif

	case CMD_CPU_DDR_REQ:
		if (mvBoardCpuFreqGet(&tmp) == MV_OK)
			printf("freq\t\t\t= %d ==>  CPU @ %dMHz DDR @ %dMHz\n", tmp,
			       cpuDdrClkTbl[tmp].cpuFreq, cpuDdrClkTbl[tmp].ddrFreq);
		else
			printf("freq Error: failed reading CPU/DDR Clocks Frequency (PLL_1)\n");
		break;

#if defined(CONFIG_BOBCAT2) || defined(CONFIG_BOBK)
	case CMD_TM_FREQ:
		if (mvBoardTmFreqGet(&tmp) == MV_OK)
			printf("tmfreq\t\t\t= %d ==>  TM @ %dMHz DDR3 @ %dMHz \n", tmp, tmClkTbl[tmp].tmFreq, tmClkTbl[tmp].ddr3Freq);
		else
			printf("tmfreq Error: failed reading TM Clock Frequency (PLL_2)\n");
		break;

	case CMD_JTAG_CPU:
		if (mvBoardJtagCpuGet(&tmp) == MV_OK)
			printf("jtagcpu \t\t= %d ==>  %s\n", tmp, ((tmp == 0) ? "Cortex-M3" : "MSYS (PJ4B)"));
		else
			printf("jtagcpu Error: failed reading JTAG connection type\n");
		break;
#endif

#if defined CONFIG_BOBCAT2
	case CMD_PTP_PLL:
		if (mvBoardPtpPllGet(&tmp) == MV_OK)
			printf("ptppll \t\t= %d ==>  %s\n", tmp, ((tmp == 0) ? "20" : "21.875"));
		else
			printf("ptppll Error: failed reading PTP PLL multiplier\n");
		break;

	case CMD_OOB0_CON:
	case CMD_OOB1_CON:
		i = mode == CMD_OOB0_CON ? 0 : 1;
		if (mvBoardOobPortConnectionGet(i, &tmp) == MV_OK) {
			if (tmp == 2)
				printf("oob%dcon \t\t= %d ==>  OOB port is disconnected\n", i, tmp);
			else
				printf("oob%dcon \t\t= %d ==>  Physical port %d\n", i, tmp, ((tmp == 1) ? i : (20 + i)));
		} else
			printf("oob%dcon Error: failed reading PTP PLL multiplier\n", i);
		break;

	case CMD_PEX_GEN1:
		if (mvBoardForcePexGen1Get(&tmp) == MV_OK)
			printf("pciegen1 \t\t= %d ==>  %s\n", tmp, ((tmp == 0) ? "Do not enforce GEN1" : "Force GEN1 connection"));
		else
			printf("pciegen1 Error: failed reading PCIe GEN1 enforcement mode\n");
		break;
#endif

#if defined CONFIG_ALLEYCAT3
	case CMD_DDR_ECC_EN:
		if (mvBoardDdrEccEnableGet(&tmp) == MV_OK)
			printf("ddreccenable \t\t= %d ==>  %s\n", tmp,
				   ((tmp == 0) ? "ECC Disabled" : "ECC Enabled"));
		else
			printf("ddreccenable Error: failed reading DDR Bus Width\n");
		break;

	case CMD_PCIE_CLOCK:
		if (mvBoardPcieClockGet(&tmp) == MV_OK)
			printf("pciclock \t\t= %d ==>  %s\n", tmp,
				   ((tmp == 0) ? "Internally generated by PLL" : "External 100MHz from PEX_CLK_P"));
		else
			printf("pciclock Error: failed reading PCI-E clock\n");
		break;

	case CMD_PLL_CLOCK:
		if (revId == MV_ALLEYCAT3_A0_ID) {
			if (mvBoardPllClockGet(&tmp) == MV_OK)
				printf("pllclock (PLL2 VCO)\t= %d ==>  %s\n", tmp, ((tmp == 0) ? "1GHz" : "2.5GHz"));
			else
				printf("pllclock (PLL2 VCO) Error: failed reading PLL VCO clock\n");
		} else
			printf("pllclock (PLL2 VCO)\t    ==>  <<Not available in this SoC revision>>\n");
		break;

	case CMD_AVS_MODE:
		if (revId != MV_ALLEYCAT3_A0_ID) {
			if (mvBoardAvsModeGet(&tmp) == MV_OK)
				printf("avsmode \t\t= %d ==>  %s\n", tmp, ((tmp == 0) ? "Slave" : "Master"));
			else
				printf("avsmode Error: failed reading AVS mode\n");
		} else
			printf("avsmode \t\t    ==>  <<Not available on this SoC revision>>\n");
		break;

	case CMD_SLAVE_ADDR:
		if (mvBoardSmiI2c2AddrGet(&tmp) == MV_OK)
			printf("slaveaddr \t\t= %d ==>  CPU_SMI_I2C_ADDR[2]=%d\n", tmp, tmp);
		else
			printf("slaveaddr Error: failed reading CPU I2C/SMI Slave address\n");
		break;

	case CMD_DEVICE_NUM:
		if (mvBoardDeviceNumGet(&tmp) == MV_OK)
			printf("devicenum\t\t= %d \n", tmp);
		else
			printf("devicenum Error: failed reading devicenum\n");
		break;

#endif
	case CMD_BOARD_ID:
		if (mvBoardSarBoardIdGet(&tmp) == MV_OK)
			printf("boardid\t\t\t= %d ==>  %s\n", tmp, mvMsysBoardInfoTbl[tmp]->boardName);
		else
			printf("boardid Error: failed reading boardid\n");
		break;

	case CMD_PCIE_MODE:
		if (mvBoardPcieModeGet(&tmp) == MV_OK)
			printf("pcimode \t\t= %d ==>  %s\n", tmp, ((tmp == 0) ? "Endpoint" : "Root Complex"));
		else
			printf("pcimode Error: failed reading PCI-E mode\n");
		break;

	case CMD_BOOTSRC:
		if (mvBoardBootDevGet(&tmp) == MV_OK)
			printf("bootsrc\t\t\t= %d ==>  %s\n", tmp, bootSrcTbl[tmp].bootstr);
		else
			printf("bootsrc Error: failed reading Boot Source\n");
		break;
	case CMD_DEVICE_ID:
		if (mvBoardDeviceIdGet(&tmp) == MV_OK)
			printf("deviceid\t\t= %d \n", tmp);
		else
			printf("deviceid Error: failed reading deviceid\n");
		break;

	case CMD_DUMP:
	case CMD_UNKNOWN:
		for (i = 0 ; i < CMD_DUMP; i++)
			do_sar_read(i);
		break;
	case CMD_DEFAULT:
		return 0;
	default:
		printf("Usage: sar list [options] (see help)\n");
		return 1;

	}
	return 0;
}

static int do_sar_write(int mode, int value)
{
	MV_U8 tmp, i;
	MV_STATUS rc = MV_OK;
#if defined(CONFIG_ALLEYCAT3) || defined(CONFIG_BOBK)
	MV_U32 revId = mvCtrlRevGet();
#endif

	tmp = (MV_U8)value;
	switch (mode) {
	case CMD_CORE_CLK_FREQ:
#if defined CONFIG_BOBK
		/* In Bobk (only A0 Chip), we need to valid the bypass mode(coreclock=7) to enble the
		CorePLL WA, and the index 7 is out of table limit. For A1 chip, there is no CorePLL WA*/
		if ((!(value == 7 && revId == MV_BOBK_A0_ID)) && ((value < 0)
			 || (value >= coreClockTblSize))) {
#else
		if ((value < 0) || (value >= coreClockTblSize)) {
#endif
			mvOsPrintf("S@R incorrect value for Core Clock: %d\n", value);
			rc = MV_ERROR;
			break;
		}
		rc = mvBoardCoreFreqSet(tmp);
		break;
#if defined CONFIG_BOBK
	case CMD_BYPASS_CORE_CLK_FREQ:
		if (revId != MV_BOBK_A0_ID) {
			mvOsPrintf("Bypass coreclock is not supported\n\n");
			rc = MV_NOT_SUPPORTED;
			break;
		}
		if ((value < 0) || (value >= ARRAY_SIZE(bypassCoreClockTbl))) {
			mvOsPrintf("S@R incorrect value for Core Clock in bypass mode: %d\n", value);
			rc = MV_ERROR;
			break;
		}
		rc = mvBoardBypassCoreFreqSet(tmp);
		break;
#endif

	case CMD_CPU_DDR_REQ:
		if (((value < 0) || (value >= cpuDdrClkTblSize)) ||
			(cpuDdrClkTbl[value].internalFreq != MV_FALSE)) {
			mvOsPrintf("S@R incorrect value for Freq: %d\n", value);
			rc = MV_ERROR;
			break;
		}
		rc = mvBoardCpuFreqSet(tmp);
		break;
#if defined(CONFIG_BOBCAT2) || defined(CONFIG_BOBK)
	case CMD_TM_FREQ:
		if (((value < 0) || (value >= ARRAY_SIZE(tmClkTbl))) ||
			(tmClkTbl[value].internalFreq != MV_FALSE)) {
			mvOsPrintf("S@R incorrect value for TM Freq: %d\n", value);
			rc = MV_ERROR;
			break;
		}
		rc = mvBoardTmFreqSet(tmp);
		break;
	case CMD_JTAG_CPU:
		rc = mvBoardJtagCpuSet(tmp);
		break;
#endif

#if defined CONFIG_BOBCAT2
	case CMD_PTP_PLL:
		rc = mvBoardPtpPllSet(tmp);
		break;
	case CMD_OOB0_CON:
	case CMD_OOB1_CON:
		rc = mvBoardOobPortConnectionSet((mode == CMD_OOB0_CON ? 0 : 1), tmp);
		break;
	case CMD_PEX_GEN1:
		rc = mvBoardForcePexGen1Set(tmp);
		break;
#endif

#if defined CONFIG_ALLEYCAT3
	case CMD_DDR_ECC_EN:
		rc = mvBoardDdrEccEnableSet(tmp);
		break;
	case CMD_PCIE_CLOCK:
		rc = mvBoardPcieClockSet(tmp);
		break;
	case CMD_PLL_CLOCK:
		if (revId == MV_ALLEYCAT3_A0_ID)
			rc = mvBoardPllClockSet(tmp);
		break;
	case CMD_AVS_MODE:
		if (revId != MV_ALLEYCAT3_A0_ID)
			rc = mvBoardAvsModeSet(tmp);
		break;
	case CMD_SLAVE_ADDR:
		rc = mvBoardSmiI2c2AddrSet(tmp);
		break;
	case CMD_DEVICE_NUM:
		rc = mvBoardDeviceNumSet(tmp);
		break;
#endif
	case CMD_BOARD_ID:
		rc = mvBoardSarBoardIdSet(tmp);
		break;
	case CMD_PCIE_MODE:
		rc = mvBoardPcieModeSet(tmp);
		break;
	case CMD_BOOTSRC:
		rc = mvBoardBootDevSet(tmp);
		break;
	case CMD_DEVICE_ID:
		rc = mvBoardDeviceIdSet(tmp);
		break;
	case CMD_DEFAULT:
		for (i = 0 ; i < CMD_DUMP; i++) {
			if (i == CMD_BOARD_ID) {
				MV_U32 brdId = mvBoardIdGet();
				if ((brdId < MARVELL_BOARD_ID_BASE) || (brdId >= MV_MAX_MARVELL_BOARD_ID))
					mvOsPrintf("Bad Board ID returned - %d! Assigning default value!\n", brdId);
				else
					defaultValue[i] = brdId - MARVELL_BOARD_ID_BASE; /* Update default value with real board ID*/
			}
			if (1 == do_sar_write(i, defaultValue[i]))
				rc = MV_FALSE;
			do_sar_read(i);
		}
		if (rc == MV_OK)
			mvOsPrintf("\nRestored all S@R default values\n");
		break;
	case CMD_UNKNOWN:
	default:
		printf("Usage: sar list [options] (see help) \n");
		return 1;
	}
	if (rc != MV_OK) {
		mvOsPrintf("Write S@R failed!\n");
		return 1;
	}
	return 0;
}

int do_sar(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	const char *cmd;
	int mode, value;

/* For BobK boards(Cetus & Caelum), Some of the settings are different,
we can't select the settings with "ifdef", so update it in runtime*/
#if defined CONFIG_BOBK
	static int setBoardSarEnv;

	if (setBoardSarEnv == 0) {
		switch (mvBoardIdGet()) {
		case BOBK_CETUS_DB_ID:
		case BOBK_CETUS_CUSTOMER_BOARD_ID0:
		case BOBK_LEWIS_RD_ID:	/*the setting of Lewis is the same with Cetus*/
			coreClockTbl = coreClockTblBobkCetus;
			coreClockTblSize = ARRAY_SIZE(coreClockTblBobkCetus);
			cpuDdrClkTbl = cpuDdrClkTblBobkCetus;
			cpuDdrClkTblSize = ARRAY_SIZE(cpuDdrClkTblBobkCetus);
			break;
		case BOBK_CAELUM_DB_ID:
		case BOBK_CAELUM_CUSTOMER_BOARD_ID1:
		case BOBK_CYGNUS_RD_ID:		/*the setting of Cygnus is the same with Caelum*/
			coreClockTbl = coreClockTblBobkCaelum;
			coreClockTblSize = ARRAY_SIZE(coreClockTblBobkCaelum);
			cpuDdrClkTbl = cpuDdrClkTblBobkCaelum;
			cpuDdrClkTblSize = ARRAY_SIZE(cpuDdrClkTblBobkCaelum);
			break;
		default:
			mvOsPrintf("ERROR: Unknown BoardID %d, Set Board SAR env failed\n", mvBoardIdGet());
			return 1;
		}
		setBoardSarEnv = 1;
	}
#endif
	/* need at least two arguments */
	if (argc < 2)
		goto usage;

	cmd = argv[1];
	mode = sar_cmd_get(argv[2]);

	if (mode != CMD_BOARD_ID) {
		/* SatR support BC2-DB, AC3-DB, BOBK-CETUS-DB, BOBK-CAELUM-DB */
		switch (mvBoardIdGet()) {
		case DB_AC3_ID:
		case DB_DX_BC2_ID:
		case BOBK_CETUS_DB_ID:
		case BOBK_CAELUM_DB_ID:
			break;
		default:
			mvOsPrintf("Error: Sample at reset supports modifying only 'boardid' field for current board\n\n");
			goto usage;
		}
	}

	if (strcmp(cmd, "list") == 0)
		return do_sar_list(mode);
	else if (strcmp(cmd, "write") == 0) {
		value = simple_strtoul(argv[3], NULL, 10);
		if (do_sar_write(mode, value) == 0)
			do_sar_read(mode);
		return 0;

	} else if (strcmp(cmd, "read") == 0)
		return do_sar_read(mode);

usage:
	cmd_usage(cmdtp);
	return 1;
}

U_BOOT_CMD(SatR, 6, 1, do_sar,
	"Sample At Reset sub-system",
     "list <field>          - list configuration options for <field>\n\n"
"SatR read dump             - print all SatR configuration values\n"
"SatR read <field>          - print the requested <field> value\n\n"
"SatR write default         - restore all SatR fields to their default values\n"
"SatR write <field> <val>   - write the requested <field> <value>\n\n"

"\tHW SatR fields\n"
"\t--------------\n"
"coreclock                  - Core frequency\n"
"freq                       - CPU DDR frequency\n"
"pcimode                    - PCIe mode (EP/RC)\n"
#if defined(CONFIG_BOBCAT2) || defined(CONFIG_BOBK)
"tmfreq                     - TM frequency\n"
"jtagcpu                    - JTAG CPU connection\n"
#endif
#if defined(CONFIG_BOBCAT2)
"ptppll                     - PTP PLL multiplier\n"
"oob0con                    - OOB-0 to physical port connection\n"
"oob1con                    - OOB-1 to physical port connection\n"
"pciegen1					- Force PCIe GEN1 on AMC RC connection\n"
#endif
#if defined(CONFIG_ALLEYCAT3)
"pciclock                   - PCIe reference clock source\n"
"avsmode                    - Adaptive Voltage Scaling mode. Not valid for rev.A0\n"
"slaveaddr                  - I2C/SMI Slave Address\n"
"devicenum                  - Devicenum\n"
#endif
"bootsrc                    - Boot source\n"
"deviceid                   - Device ID\n"
"\n\tSW SatR fields\n"
"\t--------------\n"
"boardid                    - Board ID\n"
#if defined CONFIG_BOBK
"bypass_coreclock			- Core frequency used in bypass mode(when coreclock=7) only for A0 chip\n"
#endif
#ifdef CONFIG_ALLEYCAT3
"ddreccenable               - DDR ECC modes\n"
"\t----Legacy fileds----\n"
"pllclock                   - PLL2 VCO clock frequency. Valid for rev.A0 only\n"
#endif
);
#endif /*defined(CONFIG_CMD_SAR)*/
