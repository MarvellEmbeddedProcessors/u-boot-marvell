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

enum {
	CMD_DUMP = MV_SATR_MAX_OPTION+1,
	CMD_DEFAULT,
	CMD_UNKNOWN
};

MV_FREQ_MODE cpuDdrClkTbl[] = MV_SAR_FREQ_MODES;


typedef struct {
	char bootstr[80];
	MV_STATUS internalFreq;
} MV_BOOT_SRC;
extern MV_SATR_BOOT_TABLE satrBootSrcTable[];
extern MV_BOARD_SATR_INFO boardSatrInfo[];
extern MV_BOARD_INFO *marvellBoardInfoTbl[];

typedef struct _boardSatrDefault {
	MV_SATR_TYPE_ID satrId;
	MV_U32 defauleValueForBoard[MV_MARVELL_BOARD_NUM];
} MV_BOARD_SATR_DEFAULT;
#define MAX_DEFAULT_ENTRY	2

MV_BOARD_SATR_DEFAULT boardSatrDefault[MAX_DEFAULT_ENTRY] = {
/* 	defauleValueForBoard[] = RD_69xx,	DB_68xx */
{ MV_SATR_CPU_DDR_L2_FREQ,	{0x0c,		0x0c,		} },
{ MV_SATR_CORE_CLK_SELECT,	{0,		0,		} },
};

typedef struct _deviceIdEntry {
	char *devIdStr;
	MV_U16 devIdFlavor;
	MV_U16 devIdValue;
} MV_BOARD_SATR_DEVICE_ID;

MV_BOARD_SATR_DEVICE_ID devIdArr[2] = {
/* Device ID STR	ID Flavor	ID HW Value*/
{ "6920 (A395)",	0x6920,		0x4 },
{ "6928 (A398)",	0x6928,		0x6 },
};

MV_BOOL mvVerifyRequest(void)
{
	readline(" ");
	if(strlen(console_buffer) == 0 || /* if pressed Enter */
		strcmp(console_buffer,"n") == 0 ||
		strcmp(console_buffer,"N") == 0 ) {
		printf("\n");
		return MV_FALSE;
	}
	return MV_TRUE;
}

int do_sar_default(void)
{
	MV_U32 i, rc, defaultValue, boardId = mvBoardIdIndexGet(mvBoardIdGet());
	MV_SATR_TYPE_ID satrClassId;
	MV_BOARD_SATR_INFO satrInfo;

	for (i = 0; i < MAX_DEFAULT_ENTRY; i++) {
		satrClassId = boardSatrDefault[i].satrId;
		if (mvBoardSatrInfoConfig(satrClassId, &satrInfo) != MV_OK)
			continue;
		defaultValue = boardSatrDefault[i].defauleValueForBoard[boardId];
		rc = mvBoardSatRWrite(satrClassId, defaultValue);
		if (rc == MV_ERROR) {
			mvOsPrintf("Error write S@R for id=%d\n", satrClassId);
		}
	}

	/* set default Device ID - if MV_SATR_DEVICE_ID field is relevant on board */
	if (mvBoardSatrInfoConfig(MV_SATR_DEVICE_ID, &satrInfo) == MV_OK) {
		for (i = 0; i < ARRAY_SIZE(devIdArr); i++) {
			if (devIdArr[i].devIdFlavor == mvCtrlModelGet())
				if (mvBoardSatRWrite(MV_SATR_DEVICE_ID, devIdArr[i].devIdValue) == MV_ERROR)
					mvOsPrintf("Error writing default Device ID ('devid') =%d\n", devIdArr[i].devIdValue);
		}
	}

	printf("\nSample at Reset values were restored to default.\n");
	return 0;
}

int sar_cmd_get(const char *cmd)
{
	int i;
	MV_BOARD_SATR_INFO satrInfo;

	for (i = MV_SATR_CPU_DDR_L2_FREQ; i < MV_SATR_MAX_OPTION; i++) {
		if (i == MV_SATR_BOOT2_DEVICE)
			continue;
		if (mvBoardSatrInfoConfig(i, &satrInfo) != MV_OK)
			continue;
		if (strcmp(cmd, satrInfo.name) == 0)
			return satrInfo.satrId;
	}
	if (strcmp(cmd, "dump") == 0)
		return CMD_DUMP;
	if (strcmp(cmd, "default") == 0)
		return CMD_DEFAULT;
	return CMD_UNKNOWN;
}

int do_sar_list(MV_BOARD_SATR_INFO *satrInfo)
{
	int i;

	switch (satrInfo->satrId) {
	case MV_SATR_CPU_DDR_L2_FREQ:
		mvOsPrintf("cpufreq options - Determines the frequency of CPU/DDR/L2:\n\n");
		mvOsPrintf("| ID | CPU Freq (MHz) | L2 Freq (MHz)  | SDRAM Freq (MHz) |\n");
		mvOsPrintf("|----|----------------|----------------|------------------|\n");
		for (i=0; i <= MV_SAR_FREQ_MODES_EOT; i++) {
			if (cpuDdrClkTbl[i].id == MV_SAR_FREQ_MODES_EOT)
				break;
			if (cpuDdrClkTbl[i].isDisplay)
				mvOsPrintf("| %2d |      %4d      |      %d       |      %d         | \n",
					   cpuDdrClkTbl[i].id,
					   cpuDdrClkTbl[i].cpuFreq,
					   cpuDdrClkTbl[i].ddrFreq,
					   cpuDdrClkTbl[i].l2Freq);
		}
		mvOsPrintf("-----------------------------------------------------------\n");
		break;
	case MV_SATR_CORE_CLK_SELECT:
		mvOsPrintf("Determines the Core clock frequency:\n");
		mvOsPrintf("\t0 = 250MHz\n");
		mvOsPrintf("\t1 = 200MHz\n");
		break;
	case MV_SATR_BOOT_DEVICE:
		mvOsPrintf("Determines the Boot source device (BootROM is Enabled if not stated the opposite):\n");
		for (i = 0; i < BOOT_SRC_TABLE_SIZE; i++) {
			if (satrBootSrcTable[i].bootSrc == -1)
				continue;
			mvOsPrintf("\t%02d (%#04x), Boot from %s\n", i, i, satrBootSrcTable[i].name);
		}
		break;
	case MV_SATR_DEVICE_ID:
		mvOsPrintf("Determines the Device ID:\n");
		for (i = 0; i < ARRAY_SIZE(devIdArr); i++)
			mvOsPrintf("\t\t %d = %s (ID value = %x)\n" , i ,devIdArr[i].devIdStr, devIdArr[i].devIdValue);
		break;
	default:
		mvOsPrintf("Usage: sar list [options] (see help)\n");
		return 1;
	}
	return 0;
}

int do_sar_read(MV_U32 mode, MV_BOARD_SATR_INFO *satrInfo)
{
	MV_U32 i, tmp;

	if (mode != CMD_DUMP) {
	    tmp = mvBoardSatRRead(satrInfo->satrId);
	    if (tmp == MV_ERROR) {
		mvOsPrintf("Error reading from TWSI\n");
		return 1;
	    }
	}
	switch (mode) {
	case MV_SATR_CPU_DDR_L2_FREQ:
		for (i=0; i <= MV_SAR_FREQ_MODES_EOT; i++) {
			if (cpuDdrClkTbl[i].id == MV_SAR_FREQ_MODES_EOT)
				break;
			if (cpuDdrClkTbl[i].id == tmp) {
				mvOsPrintf("\nCurrent freq configuration:\n");
				mvOsPrintf("| ID | CPU Freq (MHz) | L2 Freq (MHz)  | SDRAM Freq (MHz) |\n");
				mvOsPrintf("|----|----------------|----------------|------------------|\n");
				mvOsPrintf("| %2d |      %4d      |      %d       |      %d         |\n",
					   cpuDdrClkTbl[i].id,
					   cpuDdrClkTbl[i].cpuFreq,
					   cpuDdrClkTbl[i].ddrFreq,
					   cpuDdrClkTbl[i].l2Freq);
				mvOsPrintf("-----------------------------------------------------------\n");
				break;
			}
		}
		break;
	case MV_SATR_CORE_CLK_SELECT:
		mvOsPrintf("\ncoreclock\t\t= %d  ==> %sMhz\n", tmp, (tmp == 0x0) ? "250" : "200");
		break;
	case MV_SATR_BOOT_DEVICE:
		if (tmp < BOOT_SRC_TABLE_SIZE)
			mvOsPrintf("bootsrc\t\t\t= %d ==> Boot From %s\n", tmp, satrBootSrcTable[tmp].name);
		break;
	case MV_SATR_DEVICE_ID:
		for (i = 0; i < ARRAY_SIZE(devIdArr); i++) {
			if (devIdArr[i].devIdValue == tmp)
				mvOsPrintf("devid\t\t= %d  ==> Device ID: %s (ID value = %x)\n", i,
				devIdArr[i].devIdStr, devIdArr[i].devIdValue);
		}
		break;
	case CMD_DUMP:
		{
			MV_BOARD_SATR_INFO satrInfo;

			for (i = MV_SATR_CPU_DDR_L2_FREQ; i < MV_SATR_MAX_OPTION; i++) {
				if (i == MV_SATR_BOOT2_DEVICE)
					continue;
				if (mvBoardSatrInfoConfig(i, &satrInfo) != MV_OK)
					continue;
				do_sar_read(i, &satrInfo);
			}
		}
		break;


	case CMD_UNKNOWN:
	default:
		mvOsPrintf("Usage: sar list [options] (see help)\n");
		return 1;
	}
	return 0;
}

int do_sar_write(MV_BOARD_SATR_INFO *satrInfo, int value)
{
	MV_STATUS rc = MV_TRUE;
	MV_U32 i, freqValueInvalid;

	/* if field is read only, or field is board id for DB boards - modification not supported */
	if (satrInfo->status & SATR_READ_ONLY) {
		mvOsPrintf("S@R ID = %d is read only for this board\n", satrInfo->satrId);
		mvOsPrintf("Write S@R failed!\n");
		return 1;
	}

        if (satrInfo->satrId == MV_SATR_CPU_DDR_L2_FREQ) {
		freqValueInvalid = 1;
		for (i=0; i <= MV_SAR_FREQ_MODES_EOT; i++) {
			if (cpuDdrClkTbl[i].id == MV_SAR_FREQ_MODES_EOT)
				break;
			if (cpuDdrClkTbl[i].id == value) {
				freqValueInvalid = 0;
				break;
			}
		}
		if (freqValueInvalid) {
			mvOsPrintf("S@R incorrect value for Freq %d\n", value);
			mvOsPrintf("Write S@R failed!\n");
			return 1;
		}
	}

	/* verify requested entry is valid */
	if (satrInfo->satrId == MV_SATR_DEVICE_ID) {
		if (value <= ARRAY_SIZE(devIdArr))
			value = devIdArr[value].devIdValue;
		else {
			printf("%s: Error: requested invalid DEVICE_ID value (%x)\n", __func__, value);
			return 1;
		}
	}

	rc = mvBoardSatRWrite(satrInfo->satrId, value);
	if (rc == MV_ERROR) {
		mvOsPrintf("Error write to TWSI\n");
		return 1;
	}

	return 0;
}

int do_sar(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	const char *cmd;
	int value, mode = -1;
	MV_BOARD_SATR_INFO satrInfo;
	MV_U32 boardId = mvBoardIdGet();

	cmd = argv[1];

	/* SatR write/list/default are supported only on Marvell boards */
	if (!(boardId >= MARVELL_BOARD_ID_BASE && boardId <= MV_MAX_MARVELL_BOARD_ID)) {
		printf("\nError: S@R configuration is not supported on current board\n");
		return 1;
	}

	/* is requested 'SatR read' --> Dump all */
	if (argc > 1 && argc < 3 && strcmp(cmd, "read") == 0)
		mode = CMD_DUMP;
	else if (argc < 2)	/* need at least two arguments */
		goto usage;
	else
		mode = sar_cmd_get(argv[2]);

	if (mode == CMD_UNKNOWN)
		goto usage;

	if ((mode != CMD_DUMP)  && (mode != CMD_DEFAULT)) {
		if (mvBoardSatrInfoConfig(mode, &satrInfo) != MV_OK)
			goto usage;
	}

	if (strcmp(cmd, "list") == 0)
		return do_sar_list(&satrInfo);
	else if ((strcmp(cmd, "write") == 0) && (mode == CMD_DEFAULT)) {
		if (do_sar_default() == 0) {
			do_sar_read(CMD_DUMP, &satrInfo);
			printf("\nChanges will be applied after reset.\n\n");
			return 0;
		}
		else
			return 1;
	}
	else if (strcmp(cmd, "write") == 0) {
		value = simple_strtoul(argv[3], NULL, 10);
		if (do_sar_write(&satrInfo, value) == 0){
			mvOsDelay(100);
			do_sar_read(mode, &satrInfo);
		}
		return 0;

	} else if (strcmp(cmd, "read") == 0)
		return do_sar_read(mode, &satrInfo);

usage:
	cmd_usage(cmdtp);
	return 1;
}

U_BOOT_CMD(SatR, 6, 1, do_sar,
	"Sample At Reset sub-system",
"list <field>          - list configuration options for <field>\n\n"
"SatR read                  - print all SatR configuration values\n"
"SatR read <field>          - print the requested <field> value\n\n"
"SatR write default         - restore all SatR fields to their default values\n"
"SatR write <field> <val>   - write the requested <field> <value>\n\n"

"\tHW SatR fields\n"
"\t--------------\n"
"devid			- Device ID flavor\n"
"coreclock		- core frequency\n"
"freq			- CPU DDR frequency\n"
"bootsrc		- boot source\n"
);

#endif /*defined(CONFIG_CMD_SAR)*/
