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

enum {
	CMD_CORE_CLK_FREQ = 0,
	CMD_CPU_DDR_REQ,
	CMD_TM_FREQ,
	CMD_BOOTSRC,
	CMD_DEVICE_ID,
	CMD_DUMP,
	CMD_DEFAULT,
	CMD_UNKNOWN
};

int defaultValue[] = { 0, 3, 3, 3 };

MV_U32 coreClockTbl[] = MV_CORE_CLK_TBL;

MV_CPUDDR_MODE cpuDdrClkTbl[] = MV_CPU_DDR_CLK_TBL;

MV_TM_MODE tmClkTbl[] = MV_TM_CLK_TBL;

typedef struct {
	char bootstr[80];
	MV_STATUS internalFreq;
} MV_BOOT_SRC;

MV_BOOT_SRC bootSrcTbl[] = {
	{ "BootROM enabled, Boot from Device (NOR) flash",	       MV_FALSE },
	{ "BootROM enabled, Boot from NAND flash (on DEV_CSn[0])",     MV_FALSE },
	{ "BootROM enabled, Boot from UART",			       MV_FALSE },
	{ "BootROM enabled, Boot from SPI0 (CS0)",		       MV_FALSE },
	{ "Reserved. BootROM enabled, Boot from PCIe",		       MV_TRUE	},
	{ "BootROM enabled,Standby slave. Must set PCI-E as endpoint", MV_FALSE },
	{ "BootROM enabled, UART debug prompt mode",		       MV_FALSE },
	{ "Reserved BootROM disabled, Boot from SPI 0(CS0)",	       MV_TRUE	}
};

void SatR_usage(void);

static int sar_cmd_get(const char *cmd)
{
	if (strcmp(cmd, "corefreq") == 0)
		return CMD_CORE_CLK_FREQ;
	if (strcmp(cmd, "freq") == 0)
		return CMD_CPU_DDR_REQ;
	if (strcmp(cmd, "tmfreq") == 0)
		return CMD_TM_FREQ;
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
		for (i = 0; i < 7; i++)
			printf("\t0x%x = %dMhz\n", i, coreClockTbl[i]);
		break;
	case CMD_CPU_DDR_REQ:
		printf("Determines the CPU and DDR frequency:\n");
		printf("\t| val| CPU freq | DDR freq|\n");
		for (i = 0; i < 7; i++) {
			if (cpuDdrClkTbl[i].internalFreq)
				continue;
			printf("\t| 0x%d| %dMHz  | %dMHz\n",
			       i, cpuDdrClkTbl[i].cpuFreq, cpuDdrClkTbl[i].ddrFreq);
		}
		break;
	case CMD_TM_FREQ:
		printf("Determines the TM frequency:\n");
		printf("\t| val| TM freq \n");
		printf("\t| 0  | TM clock is disabled\n");
		for (i = 1; i < 7; i++) {
			if (tmClkTbl[i].internalFreq)
				continue;
			printf("\t| %d  | TM runs %dMHz DDR3 runs %dMHz\n",
			       i, tmClkTbl[i].tmFreq, tmClkTbl[i].ddr3Freq);
		}
		break;
	case CMD_BOOTSRC:
		printf("Determines the Boot source device:\n");
		for (i = 0; i < 7; i++) {
			if (bootSrcTbl[i].internalFreq)
				continue;
			printf("\t| 0x%d =  %s\n", i, bootSrcTbl[i].bootstr);
		}
		break;
	case CMD_DEVICE_ID:
		printf("Determines the device ID (0-31):\n");
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
			printf("corefreq = %d ==> @ %dMHz\n", tmp, coreClockTbl[tmp]);
		else
			printf("corefreq Error: failed reading Core Clock Frequency (PLL_0)\n");
		break;

	case CMD_CPU_DDR_REQ:
		if (mvBoardCpuFreqGet(&tmp) == MV_OK)
			printf("freq     = %d ==> CPU @ %dMHz DDR @ %dMHz \n", tmp,
			       cpuDdrClkTbl[tmp].cpuFreq, cpuDdrClkTbl[tmp].ddrFreq);
		else
			printf("freq Error: failed reading CPU/DDR Clocks Frequency (PLL_1)\n");
		break;

	case CMD_TM_FREQ:
		if (mvBoardTmFreqGet(&tmp) == MV_OK)
			printf("TM clock = %d ==> TM @ %dMHz DDR3 @ %dMHz \n", tmp,
			       tmClkTbl[tmp].tmFreq, tmClkTbl[tmp].ddr3Freq);
		else
			printf("TM clock Error: failed reading TM Clock Frequency (PLL_2)\n");
		break;

	case CMD_BOOTSRC:
                if (mvBoardBootDevGet(&tmp) == MV_OK)
			printf("bootsrc  = %d ==> %s\n", tmp, bootSrcTbl[tmp].bootstr);
		else
			printf("bootsrc Error: failed reading Boot Source\n");
		break;
	case CMD_DEVICE_ID:
                if (mvBoardDeviceIdGet(&tmp) == MV_OK)
			printf("deviceid = %d \n", tmp);
		else
			printf("deviceid Error: failed reading deviceid\n");
		break;

	case CMD_DUMP:
		for (i = 0 ; i < CMD_DUMP; i++)
			do_sar_read(i);
		break;
	case CMD_DEFAULT:
		return 0;
	case CMD_UNKNOWN:
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
	case CMD_TM_FREQ:
		rc = mvBoardTmFreqSet(tmp);
		break;
	case CMD_BOOTSRC:
		rc = mvBoardBootDevSet(tmp);
		break;
	case CMD_DEVICE_ID:
                rc = mvBoardDeviceIdSet(tmp);
		break;
	case CMD_DEFAULT:
		for (i = 0 ; i < CMD_DUMP; i++) {
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
	"Sample At Reset sub-system\n",

	"list corefreq		- prints the S@R modes list\n"
	"SatR list freq		- prints the S@R modes list\n"
	"SatR list tmfreq	- prints the S@R modes list\n"
	"SatR list bootsrc	- prints the S@R modes list\n"
	"SatR list deviceid	- prints the S@R modes list\n\n"

	"SatR read corefreq	- read and print the core frequency S@R value\n"
	"SatR read freq		- read and print the CPU DDR frequency S@R value\n"
	"SatR read tmfreq	- read and print the TM frequency S@R value\n"
	"SatR read bootsrc	- read and print the Boot source S@R value\n"
	"SatR read deviceid	- read and print the deviceid S@R value\n\n"
	"SatR read dump		- read and print the SAR register \n\n"

	"SatR write corefreq <val>	- write the S@R with core frequency value\n"
	"SatR write freq     <val>	- write the S@R with CPU DDR frequency value\n"
	"SatR write tmfreq   <val>	- write the S@R with TM frequency value\n"
	"SatR write bootsrc  <val>	- write the S@R with Boot source value\n"
	"SatR write deviceid <val>	- write the S@R with device ID value\n"
	"SatR write default		- write all S@R values to their default\n"
);
#endif /*defined(CONFIG_CMD_SAR)*/
