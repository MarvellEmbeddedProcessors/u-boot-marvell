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
#define MAX_DEFAULT_ENTRY	5
MV_BOARD_SATR_DEFAULT boardSatrDefault[MAX_DEFAULT_ENTRY] = {
/* 	defauleValueForBoard[] = RD_NAS_68xx,	DB_BP_68xx,	RD_WAP_68xx,	DB_AP_68xx */
{ MV_SATR_CPU_DDR_L2_FREQ,	{0x0c,		0x0c,		0x0c,		0x0c}},
{ MV_SATR_CORE_CLK_SELECT,	{1,		1,		1,		1}},
{ MV_SATR_CPU1_ENABLE,	  	{MV_TRUE,	MV_TRUE,	MV_TRUE,	MV_TRUE}},
{ MV_SATR_SSCG_DISABLE,	  	{MV_FALSE,	MV_FALSE,	MV_FALSE,	MV_FALSE}},
{ MV_SATR_SGMII_SPEED,		{0,		0,		0,		0}},
};

char* lane1Arr[7] = { 	"Unconnected" ,
			"PCIe Port 0",
			"SATA3 Port 0",
			"SGMII-0",
			"SGMII-1",
			"USB3.0 Port 0",
			"QSGMII" };

char* lane2Arr[4] = { 	"Unconnected" ,
			"PCIe Port 1",
			"SATA3 Port 1",
			"SGMII-1" };

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
	case MV_SATR_CPU1_ENABLE:
		mvOsPrintf("cpusnum options - Determines the number of CPU cores:\n");
		mvOsPrintf("\t0 = Single CPU\n");
		mvOsPrintf("\t1 = Dual CPU\n");
		break;
	case MV_SATR_SSCG_DISABLE:
		printf("Determines the SSCG  mode:\n");
		printf("\t0 = SSCG Enabled\n");
		printf("\t1 = SSCG Disabled\n");
		break;
	case MV_SATR_DDR4_SELECT:
		mvOsPrintf("Determines the DDR3 DDR4  :\n");
		mvOsPrintf("\t0 = DDR3 module mounted.\n");
		mvOsPrintf("\t1 = DDR4 module mounted.\n ");
		break;
	case MV_SATR_DDR_BUS_WIDTH:
		mvOsPrintf("Determines the DDR BUS width 16/32 bit :\n");
		mvOsPrintf("\t0 = 16 bit\n");
		mvOsPrintf("\t1 = 32 bit\n");
		break;
	case MV_SATR_DDR_ECC_ENABLE:
		mvOsPrintf("Determines the DDR ECC:\n");
		mvOsPrintf("\t0 = No ECC\n");
		mvOsPrintf("\t1 = ECC enabled\n ");
		break;
	case MV_SATR_DDR_ECC_PUP_SEL:
		mvOsPrintf("Determines the DDR ECC PUP selection:\n");
		mvOsPrintf("\t0 = PUP 3\n");
		mvOsPrintf("\t1 = PUP 4\n ");
		break;
	case MV_SATR_SGMII_SPEED:
		mvOsPrintf("Determines the SGMII Speed configuration:\n");
		mvOsPrintf("\t0 = 1G\n");
		mvOsPrintf("\t1 = 2.5G\n");
		break;
	case MV_SATR_BOOT_DEVICE:
		mvOsPrintf("Determines the Boot source device (BootROM is Enabled if not stated the opposite):\n");
		for (i = 0; i < BOOT_SRC_TABLE_SIZE; i++) {
			if (satrBootSrcTable[i].bootSrc == -1)
				continue;
			mvOsPrintf("\t%02d (%#04x), Boot from %s\n", i, i, satrBootSrcTable[i].name);
		}
		break;
	case MV_SATR_BOARD_ID:
		if (mvBoardIdGet() == RD_NAS_68XX_ID || mvBoardIdGet() == RD_AP_68XX_ID) {
			mvOsPrintf("Determines the board ID\n");
			mvOsPrintf("\t0 - %s\n", marvellBoardInfoTbl[0]->boardName);
			mvOsPrintf("\t2 - %s\n", marvellBoardInfoTbl[2]->boardName);
		} else
			mvOsPrintf("board Id modification is not supported for current board\n");
		break;
	case MV_SATR_BOARD_ECO_VERSION:
		mvOsPrintf("Determines the ECO version\n");
		break;

	case MV_SATR_DB_USB3_PORT0:
		mvOsPrintf("Determines the USB3 Port0 Mode:\n");
		mvOsPrintf("\t0 = Host\n");
		mvOsPrintf("\t1 = Device\n ");
		break;
	case MV_SATR_DB_USB3_PORT1:
		mvOsPrintf("Determines the USB3 Port1 Mode:\n");
		mvOsPrintf("\t0 = Host\n");
		mvOsPrintf("\t1 = Device\n ");
		break;
	case MV_SATR_RD_SERDES4_CFG:
		mvOsPrintf("Determines the RD-NAS SERDES lane #4 configuration:\n");
		mvOsPrintf("\t0 = USB3\n");
		mvOsPrintf("\t1 = SGMII\n ");
		break;
	case MV_SATR_GP_SERDES5_CFG:
		mvOsPrintf("Determines the GP SERDES lane #5 configuration:\n");
		mvOsPrintf("\t0 = USB3.0 Port 1\n");
		mvOsPrintf("\t1 = SGMII-2\n ");
		break;
	case MV_SATR_DB_SERDES1_CFG:
		mvOsPrintf("Determines the DB SERDES lane #1 configuration:\n");
		for (i = 0; i < 7; i++)
			mvOsPrintf("\t %d = %s\n" , i ,lane1Arr[i]);
		break;
	case MV_SATR_DB_SERDES2_CFG:
		mvOsPrintf("Determines the DB SERDES lane #2 configuration:\n");
		for (i = 0; i < 4; i++)
			mvOsPrintf("\t %d = %s\n" , i ,lane2Arr[i]);
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
		mvOsPrintf("\ncoreclock\t= %d  ==> %sMhz\n", tmp, (tmp == 0x0) ? "250" : "200");
		break;
	case MV_SATR_CPU1_ENABLE:
		mvOsPrintf("cpusnum\t\t= %d  ==> %s CPU\n", tmp, (tmp == 0) ? "Single" : "Dual");
		break;
	case MV_SATR_SSCG_DISABLE:
		mvOsPrintf("sscg \t\t= %d  ==> %s\n", tmp, (tmp == 1) ? "Disabled" : "Enabled");
		break;
	case MV_SATR_DDR4_SELECT:
		mvOsPrintf("ddr4select\t= %d  ==> DDR%d module\n", tmp, (tmp == 0) ? 3 : 4);
		break;
	case MV_SATR_DDR_BUS_WIDTH:
		mvOsPrintf("ddrbuswidth\t= %d  ==> DDR BUS width %d bit\n", tmp, (tmp == 0) ? 16 : 32);
		break;
	case MV_SATR_DDR_ECC_ENABLE:
		mvOsPrintf("ddreccenable\t= %d  ==> ECC %s\n", tmp, (tmp == 0) ? "Disabled" : "Enabled");
		break;
	case MV_SATR_DDR_ECC_PUP_SEL:
		mvOsPrintf("ddreccpupselect\t= %d  ==> PUP %d\n", tmp, (tmp == 0) ? 3 : 4);
		break;
	case MV_SATR_SGMII_SPEED:
		mvOsPrintf("sgmiispeed\t= %d  ==> %s\n", tmp, (tmp == 0) ? "1G" : "2.5G");
		break;
	case MV_SATR_BOOT_DEVICE:
		if (tmp < BOOT_SRC_TABLE_SIZE)
			mvOsPrintf("bootsrc\t\t= %d ==> Boot From %s\n", tmp, satrBootSrcTable[tmp].name);
		break;
	case MV_SATR_BOARD_ID:
		mvOsPrintf("boardid\t\t= %d  ==> %s\n", tmp, marvellBoardInfoTbl[tmp]->boardName);
		break;
	case MV_SATR_BOARD_ECO_VERSION:
		mvOsPrintf("ecoversion\t= %d  ==> ECO v%d.%d\n", tmp, (tmp & 0x0f), ((tmp >> 4) & 0x0f));
		break;
	case MV_SATR_DB_USB3_PORT0:
		mvOsPrintf("usb3port0\t= %d  ==> USB3.0 port0: %s Mode\n", tmp, (tmp == 0) ? "Host" : "Device");
		break;
	case MV_SATR_DB_USB3_PORT1:
		mvOsPrintf("usb3port1\t= %d  ==> USB3.0 port0: %s Mode\n", tmp, (tmp == 0) ? "Host" : "Device");
		break;
	case MV_SATR_RD_SERDES4_CFG:
		mvOsPrintf("rdserdes4\t= %d  ==> RD SERDES Lane #4: %s\n", tmp, (tmp == 0) ? "USB3" : "SGMII");
	case MV_SATR_GP_SERDES5_CFG:
		mvOsPrintf("gpserdes5\t= %d  ==> GP SERDES Lane #5: %s\n", tmp, (tmp == 0) ? "USB3.0 port 1" : "SGMII-2");
		break;
	case MV_SATR_DB_SERDES1_CFG:
		mvOsPrintf("dbserdes1\t= %d  ==> DB SERDES Lane #1: %s\n", tmp, lane1Arr[tmp]);
		break;
	case MV_SATR_DB_SERDES2_CFG:
		mvOsPrintf("dbserdes2\t= %d  ==> DB SERDES Lane #2: %s\n", tmp, lane2Arr[tmp]);
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
			mvOsPrintf("Usage: sar list [options] (see help) \n");
			return 1;
	}
	return 0;
}

int do_sar_write(MV_BOARD_SATR_INFO *satrInfo, int value)
{
	MV_STATUS rc = MV_TRUE;
	MV_U32 boardId = mvBoardIdGet();

	/* if field is read only, or field is board id for DB boards - modification not supported */
	if (satrInfo->status & BOARD_SATR_READ_ONLY ||
	    (MV_SATR_BOARD_ID == satrInfo->satrId  && (boardId != RD_NAS_68XX_ID && boardId != RD_AP_68XX_ID))) {
		mvOsPrintf("S@R ID = %d is read only for this board\n", satrInfo->satrId);
		mvOsPrintf("Write S@R failed!\n");
		return 1;
	}
	/* only RD-AP/RD-NAS boards support modifying board id value */
	if ((MV_SATR_BOARD_ID == satrInfo->satrId) && (boardId == RD_NAS_68XX_ID || boardId == RD_AP_68XX_ID)) {
		/* Adding MARVELL_BOARD_ID_BASE, since Marvell board IDs are virtually shifted by MARVELL_BOARD_ID_BASE */
		if ((value + MARVELL_BOARD_ID_BASE != RD_NAS_68XX_ID)
			&& (value + MARVELL_BOARD_ID_BASE != RD_AP_68XX_ID)) {
			mvOsPrintf("S@R incorrect value for board ID %d\n", value);
			mvOsPrintf("Write S@R failed!\n");
			return 1;
		}
	}

	rc = mvBoardSatRWrite(satrInfo->satrId, value);
	if (rc == MV_ERROR) {
		mvOsPrintf("Error write to TWSI\n");
		return 1;
	}

	if (MV_SATR_BOARD_ID == satrInfo->satrId) {
		mvOsPrintf("\nBoard ID update requires new default environment variables.\n");
		mvOsPrintf(" Reset environment for %s ? [y/N]" ,marvellBoardInfoTbl[value]->boardName);
		if (mvVerifyRequest() == MV_TRUE)
			run_command("resetenv", 0);
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
"coreclock                  - core frequency\n"
"freq                       - CPU DDR frequency\n"
"cpusnum                    - number of CPU cores\n"
"sscg                       - SSCG modes\n"
"bootsrc                    - boot source\n"
"usb3port0                  - USB3-Port0 mode\n"
"usb3port1                  - USB3-Port1 mode\n\n"

"\tSW SatR fields\n"
"\t--------------\n"
"ddrbuswidth                - DDR bus width\n"
"ddreccenable               - DDR ECC enable\n"
"ddreccpupselect            - DDR ECC PUP selection\n"
"boardid                    - board ID\n"
"sgmiispeed                 - SGMII speed\n"
"rdserdes4                  - RD-NAS: SerDes lane #4\n"
"gpserdes5                  - DB-GP:  SerDes lane #5\n"
"dbserdes1                  - DB:     SerDes lane #1\n"
"dbserdes2                  - DB:     SerDes lane #2\n"
"ddr4select                 - DDR3/4 (read only) \n"
"ecoversion                 - ECO version (read only)\n"
);
#endif /*defined(CONFIG_CMD_SAR)*/
