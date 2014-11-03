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

extern MV_BOARD_INFO *marvellAC3BoardInfoTbl[];

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
	CMD_CPU_DDR_REQ,
#ifdef CONFIG_BOBCAT2
	CMD_TM_FREQ,
#elif defined(CONFIG_ALLEYCAT3)
	CMD_PCIE_CLOCK,
	CMD_PLL_CLOCK,
	CMD_DEVICE_NUM,
	CMD_BOARD_ID,
	CMD_DDR_ECC_EN,
#endif
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
						   1,	/* PCIe mode */
						   3,	/* Boot source */
						   0 };	/* Device ID */
	MV_U32 coreClockTbl[] = MV_CORE_CLK_TBL_BC2;
	MV_CPUDDR_MODE cpuDdrClkTbl[] = MV_CPU_DDR_CLK_TBL_BC2;
#elif defined(CONFIG_ALLEYCAT3)
	int defaultValue[] = { 4,	/* Core clock */
						   3,	/* CPU/DDR clock */
						   0,	/* PCIe clock */
						   1,	/* PLL clock */
						   0,	/* Device number */
						   0,	/* Board ID */
						   0,	/* DDR ECC enable */
						   0,	/* PCIe mode */
						   3,	/* Boot source */
						   0 };	/* Device ID */

	MV_U32 coreClockTbl[] = MV_CORE_CLK_TBL_AC3;
	MV_CPUDDR_MODE cpuDdrClkTbl[] = MV_CPU_DDR_CLK_TBL_AC3;
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
	if (strcmp(cmd, "coreclock") == 0)
		return CMD_CORE_CLK_FREQ;
	if (strcmp(cmd, "freq") == 0)
		return CMD_CPU_DDR_REQ;
#ifdef CONFIG_BOBCAT2
	if (strcmp(cmd, "tmfreq") == 0)
		return CMD_TM_FREQ;
#elif defined CONFIG_ALLEYCAT3
	if (strcmp(cmd, "pciclock") == 0)
		return CMD_PCIE_CLOCK;
	if (strcmp(cmd, "pllclock") == 0)
		return CMD_PLL_CLOCK;
	if (strcmp(cmd, "devicenum") == 0)
		return CMD_DEVICE_NUM;
	if (strcmp(cmd, "boardid") == 0)
		return CMD_BOARD_ID;
	if (strcmp(cmd, "ddreccenable") == 0)
		return CMD_DDR_ECC_EN;
#endif
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

	switch (mode) {
	case CMD_CORE_CLK_FREQ:
		printf("Determines the core clock frequency:\n");
		printf("\t| ID  | Core clock (MHz) |\n");
		printf("\t--------------------------\n");
		for (i = 0; i < 7; i++)
			printf("\t| %2d  |      %4d        |\n", i, coreClockTbl[i]);
		printf("\t--------------------------\n");
		break;
	case CMD_CPU_DDR_REQ:
		printf("Determines the CPU and DDR frequency:\n");
		printf("\t| ID  | CPU Freq (MHz) | DDR Freq (MHz) |\n");
		printf("\t-----------------------------------------\n");
		for (i = 0; i < 7; i++) {
			if (cpuDdrClkTbl[i].internalFreq)
				continue;
			printf("\t| %2d  |      %4d      |      %4d      |\n",
			       i, cpuDdrClkTbl[i].cpuFreq, cpuDdrClkTbl[i].ddrFreq);
		}
		printf("\t-----------------------------------------\n");
		break;
#ifdef CONFIG_BOBCAT2
	case CMD_TM_FREQ:
		printf("Determines the TM frequency:\n");
		printf("\t| ID  | TM Freq (MHz) | DDR Freq (MHz) |\n");
		printf("\t----------------------------------------\n");
		printf("\t|  0  |   Disabled    |                |\n");
		for (i = 1; i < 7; i++) {
			if (tmClkTbl[i].internalFreq)
				continue;
			printf("\t| %2d  |      %4d     |      %4d      |\n",
			       i, tmClkTbl[i].tmFreq, tmClkTbl[i].ddr3Freq);
		}
		printf("\t----------------------------------------\n");
		break;
#elif defined CONFIG_ALLEYCAT3
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
		printf("Determines the PLL VCO clock frequency:\n");
		printf("\t| ID  | Clock freq (GHz) |\n");
		printf("\t--------------------------\n");
		printf("\t|  0  |          1       |\n");
		printf("\t|  1  |         2.5      |\n");
		printf("\t--------------------------\n");
		break;
	case CMD_BOARD_ID:
		printf("Determines the board ID (0-7)\n");
		printf("\t| ID  |      Board               |\n");
		printf("\t----------------------------------\n");
		for (i = 0; i < AC3_MARVELL_BOARD_NUM ; i++)
			printf("\t|  %d  |  %-22s  |\n", i, marvellAC3BoardInfoTbl[i]->boardName );
		printf("\t----------------------------------\n");
		break;
	case CMD_DEVICE_NUM:
		printf("Determines the device number (0-3)\n");
		break;
#endif
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

	switch (mode) {
	case CMD_CORE_CLK_FREQ:
		if (mvBoardCoreFreqGet(&tmp) == MV_OK)
			printf("coreclock\t\t= %d ==>  Core @ %dMHz\n", tmp, coreClockTbl[tmp]);
		else
			printf("coreclock Error: failed reading Core Clock Frequency (PLL_0)\n");
		break;

	case CMD_CPU_DDR_REQ:
		if (mvBoardCpuFreqGet(&tmp) == MV_OK)
			printf("freq\t\t\t= %d ==>  CPU @ %dMHz DDR @ %dMHz \n", tmp,
			       cpuDdrClkTbl[tmp].cpuFreq, cpuDdrClkTbl[tmp].ddrFreq);
		else
			printf("freq Error: failed reading CPU/DDR Clocks Frequency (PLL_1)\n");
		break;

#ifdef CONFIG_BOBCAT2

	case CMD_TM_FREQ:
		if (mvBoardTmFreqGet(&tmp) == MV_OK)
			printf("tmfreq\t\t\t= %d ==>  TM @ %dMHz DDR3 @ %dMHz \n", tmp, tmClkTbl[tmp].tmFreq, tmClkTbl[tmp].ddr3Freq);
		else
			printf("tmfreq Error: failed reading TM Clock Frequency (PLL_2)\n");
		break;

#elif defined CONFIG_ALLEYCAT3
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
		if (mvBoardPllClockGet(&tmp) == MV_OK)
			printf("pllclock (PLL2 VCO)\t\t= %d ==>  %s\n", tmp, ((tmp == 0) ? "1GHz" : "2.5GHz"));
		else
			printf("pllclock (PLL2 VCO) Error: failed reading PLL VCO clock\n");
		break;
	case CMD_DEVICE_NUM:
		if (mvBoardDeviceNumGet(&tmp) == MV_OK)
			printf("devicenum\t\t= %d \n", tmp);
		else
			printf("devicenum Error: failed reading devicenum\n");
		break;

	case CMD_BOARD_ID:
		if (mvBoardSarBoardIdGet(&tmp) == MV_OK)
			printf("boardid\t\t\t= %d ==>  %s\n", tmp, marvellAC3BoardInfoTbl[tmp]->boardName);
		else
			printf("boardid Error: failed reading boardid\n");
		break;

#endif
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

	tmp = (MV_U8)value;
	switch (mode) {
	case CMD_CORE_CLK_FREQ:
		rc = mvBoardCoreFreqSet(tmp);
		break;
	case CMD_CPU_DDR_REQ:
		rc = mvBoardCpuFreqSet(tmp);
		break;
#ifdef CONFIG_BOBCAT2
	case CMD_TM_FREQ:
		rc = mvBoardTmFreqSet(tmp);
		break;
#elif defined CONFIG_ALLEYCAT3
	case CMD_DDR_ECC_EN:
		rc = mvBoardDdrEccEnableSet(tmp);
		break;
	case CMD_PCIE_CLOCK:
		rc = mvBoardPcieClockSet(tmp);
		break;
	case CMD_PLL_CLOCK:
		rc = mvBoardPllClockSet(tmp);
		break;
	case CMD_BOARD_ID:
		rc = mvBoardSarBoardIdSet(tmp);
		break;
	case CMD_DEVICE_NUM:
		rc = mvBoardDeviceNumSet(tmp);
		break;
#endif
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
#if defined CONFIG_ALLEYCAT3
			if (i == CMD_BOARD_ID) {
				MV_U32 brdId = mvBoardIdGet();
				if ((brdId < AC3_MARVELL_BOARD_ID_BASE) || (brdId >= AC3_MARVELL_MAX_BOARD_ID))
					mvOsPrintf("Bad Board ID returned - %d! Assigning default value!\n", brdId);
				else
					defaultValue[i] = brdId - AC3_MARVELL_BOARD_ID_BASE; /* Update default value with real board ID*/
			}
#endif /* CONFIG_ALLEYCAT3 */
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

	/* need at least two arguments */
	if (argc < 2)
		goto usage;

	cmd = argv[1];
	mode = sar_cmd_get(argv[2]);

#if defined CONFIG_ALLEYCAT3
	if(mvBoardIdGet() != DB_AC3_ID && mode != CMD_BOARD_ID) {
		mvOsPrintf("Error: Sample at reset supports modifying only 'boardid' field for current board\n\n");
		goto usage;
	}
#endif

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
#ifdef CONFIG_BOBCAT2
"tmfreq                     - TM frequency\n"
#elif defined CONFIG_ALLEYCAT3
"pciclock                   - PCIe reference clock source\n"
"pllclock                   - PLL2 VCO clock frequency\n"
"devicenum                  - Devicenum\n"
#endif
"bootsrc                    - Boot source\n"
"deviceid                   - Device ID\n"
#ifdef CONFIG_ALLEYCAT3
"\n\tSW SatR fields\n"
"\t--------------\n"
"ddreccenable               - DDR ECC modes\n"
"boardid                    - Board ID\n"
#endif
);
#endif /*defined(CONFIG_CMD_SAR)*/
