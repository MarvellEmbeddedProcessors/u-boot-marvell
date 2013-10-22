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

MV_FREQ_MODE coreClockTbl[] = MV_USER_SAR_FREQ_MODES;

MV_FREQ_MODE cpuDdrClkTbl[] = MV_SAR_FREQ_MODES;


typedef struct {
	char bootstr[80];
	MV_STATUS internalFreq;
} MV_BOOT_SRC;

MV_BOOT_SRC bootSrcTbl[] = {
	{ "00  |    NOR 8 bit ", MV_TRUE },
	{ "01  |    NOR 8 bit ", MV_TRUE },
	{ "02  |    NOR 8 bit ", MV_TRUE },
	{ "03  |    NOR 8 bit ", MV_TRUE },
	{ "04  |    NOR 16 bit", MV_TRUE },
	{ "05  |    NOR 16 bit", MV_TRUE },
	{ "06  |    NOR 16 bit", MV_TRUE },
	{ "07  |    NOR 16 bit", MV_TRUE },
	{ "08  |    NAND 8 bit |Page size 512B ", MV_TRUE },
	{ "09  |    NAND 8 bit |Page size 512B ", MV_TRUE },
	{ "0A  |    NAND 8 bit |Page size 2KB, ECC 12BIT", MV_TRUE },
	{ "0B  |    NAND 8 bit |Page size 2KB  ECC 16BIT", MV_TRUE },
	{ "0C  |    NAND 8 bit |Page size 2KB  ECC 4BIT ", MV_TRUE },
	{ "0D  |    NAND 8 bit |Page size 2KB  ECC 8BIT ", MV_TRUE },
	{ "0E  |    NAND 8 bit |Page size 4KB  ECC 12BIT", MV_TRUE },
	{ "0F  |    NAND 8 bit |Page size 4KB  ECC 16BIT", MV_TRUE },
	{ "10  |    NAND 8 bit |Page size 4KB  ECC 12BIT", MV_TRUE },
	{ "11  |    NAND 8 bit |Page size 4KB  ECC 16BIT", MV_TRUE },
	{ "12  |    NAND 8 bit |Page size 8KB  ECC 4BIT ", MV_TRUE },
	{ "13  |    NAND 8 bit |Page size 8KB  ECC 8BIT ", MV_TRUE },
	{ "14  |    NAND 8 bit |Page size 8KB  ECC 12BIT", MV_TRUE },
	{ "15  |    NAND 8 bit |Page size 8KB  ECC 16BIT", MV_TRUE },
	{ "16  |    NA", MV_FALSE },
	{ "17  |    NA", MV_FALSE },
	{ "18  |    NAND 16    |Page size 512B          ", MV_TRUE },
	{ "19  |    NAND 16    |Page size 512B          ", MV_TRUE },
	{ "1A  |    NAND 16    |Page size 2KB  ECC 4BIT ", MV_TRUE },
	{ "1B  |    NAND 16    |Page size 2KB  ECC 8BIT ", MV_TRUE },
	{ "1C  |    NAND 16    |Page size 2KB  ECC 12BIT", MV_TRUE },
	{ "1D  |    NAND 16    |Page size 2KB  ECC 16BIT", MV_TRUE },
	{ "1E  |    NAND 16    |Page size 4KB  ECC 4BIT ", MV_TRUE },
	{ "1F  |    NAND 16    |Page size 4KB  ECC 8BIT ", MV_TRUE },
	{ "20  |    NAND 16    |Page size 4KB  ECC 12BIT", MV_TRUE },
	{ "21  |    NAND 16    |Page size 4KB  ECC 16BIT", MV_TRUE },
	{ "22  |    NAND 16    |Page size 8KB  ECC 4BIT ", MV_TRUE },
	{ "23  |    NAND 16    |Page size 8KB  ECC 8BIT ", MV_TRUE },
	{ "24  |    NAND 16    |Page size 8KB  ECC 12BIT", MV_TRUE },
	{ "25  |    NAND 16    |Page size 8KB  ECC 16BIT", MV_TRUE },
	{ "28  |    UART   Port0                        ", MV_TRUE },
	{ "29  |    UART   Port1                        ", MV_TRUE },
	{ "2A  |    SATA   Lan0                         ", MV_TRUE },
	{ "2B  |    SATA   Lan1                         ", MV_TRUE },
	{ "2C  |    PEX0   Lan0                         ", MV_TRUE },
	{ "2D  |    PEX0   Lan1                         ", MV_TRUE },
	{ "2E  |    I2C                                 ", MV_TRUE },
	{ "2F  |    NA", MV_FALSE },
	{ "30  |    SDIO   Port0                        ", MV_TRUE },
	{ "31  |    SDIO   Port0                        ", MV_TRUE },
	{ "32  |    SPI 24 bit address                  ", MV_TRUE },
	{ "33  |    SPI 32 bit address                  ", MV_TRUE },
	{ "34  |    SPI 24 bit address                  ", MV_TRUE },
	{ "35  |    SPI 32 bit address                  ", MV_TRUE },
	{ "36  |    SPI 24 bit address                  ", MV_TRUE },
	{ "37  |    SPI 32 bit address                  ", MV_TRUE },
	{ "38  |    SPI 24 bit address                  ", MV_TRUE },
	{ "39  |    SPI 32 bit address                  ", MV_TRUE },
};
typedef struct _boardSatrDefault {
	MV_SATR_TYPE_ID satrId;
	MV_U32 defauleValueForBoard[MV_MAX_BOARD_ID];
} MV_BOARD_SATR_DEFAULT;

MV_BOARD_SATR_DEFAULT boardSatrDefault[] = {
/* 	defauleValueForBoard[] = RD_68xx,	DB_68xx*/
{ MV_SATR_CPU_DDR_L2_FREQ,	{6,	       6}  },
{ MV_SATR_CORE_CLK_SELECT,	{1,	       1}  },
{ MV_SATR_CPU1_ENABLE,	  	{MV_FALSE,     MV_FALSE} },
{ MV_SATR_SSCG_DISABLE,	  	{MV_FALSE,     MV_FALSE} },
{ MV_SATR_DDR_ECC_ENABLE,	{MV_FALSE,     MV_FALSE} },
{ MV_SATR_RD_LANE1_2_CFG,	{0,	       0} },
{ MV_SATR_RD_LANE4_CFG,		{0,	       0} },
{ MV_SATR_RD_LANE0_CFG,		{0,	       0} },
{ MV_SATR_RD_APPS_CFG,		{0,	       0} },
};
static int do_sar_default(void)
{
	MV_U32 i, rc, defaultValue, boardId = mvBoardIdGet();
	MV_SATR_TYPE_ID satrClassId;
	MV_BOARD_SATR_INFO satrInfo;

	if (boardId != RD_68XX_ID && boardId != DB_68XX_ID) {
		printf("\nError: S@R fields are readable only for current board\n");
		return 1;
	}
	for (i = 0; i < 9; i++) {
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

void SatR_usage(void);

static int sar_cmd_get(const char *cmd)
{
	if (strcmp(cmd, "corefreq") == 0)
		return MV_SATR_CORE_CLK_SELECT;
	if (strcmp(cmd, "cpufreq") == 0)
		return MV_SATR_CPU_DDR_L2_FREQ;
	if (strcmp(cmd, "cpusnum") == 0)
		return MV_SATR_CPU1_ENABLE;
	if (strcmp(cmd, "sscg") == 0)
		return MV_SATR_SSCG_DISABLE;
	if (strcmp(cmd, "ddr4select") == 0)
		return MV_SATR_DDR4_SELECT;
	if (strcmp(cmd, "ddrbuswidth,") == 0)
		return MV_SATR_DDR_BUS_WIDTH;
	if (strcmp(cmd, "ddreccenable") == 0)
		return MV_SATR_DDR_ECC_ENABLE;
	if (strcmp(cmd, "nanddetect") == 0)
		return MV_SATR_NAND_DETECT;
	if (strcmp(cmd, "rd_lane1_2_cfg") == 0)
		return MV_SATR_RD_LANE1_2_CFG;
	if (strcmp(cmd, "rd_lane4_cfg") == 0)
		return MV_SATR_RD_LANE4_CFG;
	if (strcmp(cmd, "rd_lane0_cfg") == 0)
		return MV_SATR_RD_LANE0_CFG;
	if (strcmp(cmd, "rd_apps_cfg") == 0)
		return MV_SATR_RD_APPS_CFG;
	if (strcmp(cmd, "boot_device") == 0)
		return MV_SATR_BOOT_DEVICE;
	if (strcmp(cmd, "board_id") == 0)
		return MV_SATR_BOARD_ID;
	if (strcmp(cmd, "EcoVersion") == 0)
		return MV_SATR_BOARD_ECO_VERSION;
	if (strcmp(cmd, "dump") == 0)
		return CMD_DUMP;
	return CMD_UNKNOWN;
}

static int do_sar_list(MV_BOARD_SATR_INFO *satrInfo)
{
	MV_FREQ_MODE pFreqModes[] = MV_USER_SAR_FREQ_MODES;
	int i, maxFreqModes = mvBoardFreqModesNumGet();

	switch (satrInfo->satrId) {
	case MV_SATR_CPU_DDR_L2_FREQ:
		mvOsPrintf("cpufreq options - Determines the frequency of CPU/DDR/L2:\n");
		mvOsPrintf("\n| ID  | CPU Freq (Mhz) | DDR Freq (Mhz) | L2 Freq (Mhz) |\n");
		mvOsPrintf("---------------------------------------------------------\n");
		for (i=0; i < maxFreqModes; i++) {
			mvOsPrintf("|  %2d |      %4d      |      %d       |      %d      | \n",
				pFreqModes[i].id,
				pFreqModes[i].cpuFreq,
				pFreqModes[i].ddrFreq,
				pFreqModes[i].l2Freq);
		}
		mvOsPrintf("---------------------------------------------------------\n");
		break;
	case MV_SATR_CORE_CLK_SELECT:
		mvOsPrintf("Determines the Core clock frequency:\n");
		mvOsPrintf("0x0 - 166MHz\n");
		mvOsPrintf("0x1 - 200MHz\n");
		break;
	case MV_SATR_CPU1_ENABLE:
		mvOsPrintf("cpusnum options - Determines the number of CPU cores:\n");
		mvOsPrintf("\t0x0 = Single CPU\n");
		mvOsPrintf("\t0x1 = Dual CPU\n");
		break;
	case MV_SATR_SSCG_DISABLE:
		printf("Determines the SSCG  mode:\n");
		printf("\t0x0 = SSCG Enabled\n");
		printf("\t0x1 = SSCG Disabled\n");
		break;
	case MV_SATR_DDR4_SELECT:
		mvOsPrintf("Determines the DDR3 DDR4  :\n");
		mvOsPrintf("0 = DDR3 module mounted.\n");
		mvOsPrintf("1 = DDR4 module mounted. \n ");
		break;
	case MV_SATR_DDR_BUS_WIDTH:
		mvOsPrintf("Determines the DDR BUS width 16/32 bit :\n");
		mvOsPrintf("0x0 = 16 bit \n");
		mvOsPrintf("0x1 = 32 bit \n ");
		break;
	case MV_SATR_DDR_ECC_ENABLE:
		mvOsPrintf("Determines the DDR ECC:\n");
		mvOsPrintf("0x0 = No ECC      \n");
		mvOsPrintf("0x1 = ECC enabled  \n ");
		break;
	case MV_SATR_NAND_DETECT:
		mvOsPrintf("Determines NAND Device  detect:\n");
		mvOsPrintf("0x0 = No NAND      \n");
		mvOsPrintf("0x1 = NAND device detected\n ");

		break;
	case MV_SATR_RD_LANE1_2_CFG:
		mvOsPrintf("Determines the SERDES lane 1 and 2 configuring\n");
		mvOsPrintf("0x0 = Configuring lane 1 and lane 2 to PEX\n");
		mvOsPrintf("0x1 = Configuring lane 1 and lane 2 to SATA\n");
		break;
	case MV_SATR_RD_LANE4_CFG:
		mvOsPrintf("Determines the SERDES lane 4 configuring\n");
		mvOsPrintf("0x0 = Configuring lane 4 USB3\n");
		mvOsPrintf("0x1 = Configuring lane 4 SGMII\n");
		break;
	case MV_SATR_RD_LANE0_CFG:
		mvOsPrintf("Determines the SERDES lane 0 configuring\n");
		mvOsPrintf("0x0 = No configuring lane 0\n");
		mvOsPrintf("0x1 = Configuring lane 0 to PEX0\n");
		break;
	case MV_SATR_BOOT_DEVICE:
		mvOsPrintf("Determines the Boot source device:\n");
		for (i = 0; i < 7; i++) {
			if (bootSrcTbl[i].internalFreq)
				continue;
			mvOsPrintf("%s\n", bootSrcTbl[i].bootstr);
		}
		break;
	case MV_SATR_RD_APPS_CFG:
		mvOsPrintf("Determines the RD application:\n");
		mvOsPrintf("0x0 = NAS\n");
		mvOsPrintf("0x1 = AP\n");
		break;
	case MV_SATR_BOARD_ID:
		mvOsPrintf("Determines the board ID\n");
		break;
	case MV_SATR_BOARD_ECO_VERSION:
		mvOsPrintf("Determines the ECO version\n");
		break;

	default:
		mvOsPrintf("Usage: sar list [options] (see help) \n");
		return 1;
	}
	return 0;
}

static int do_sar_read(MV_U32 mode, MV_BOARD_SATR_INFO *satrInfo)
{
	MV_U32 tmp = mvBoardSatRRead(satrInfo->satrId);
	if (tmp == MV_ERROR)
	{
		mvOsPrintf("Error reading from TWSI\n");
		return 1;
	}
	switch (mode) {
	case MV_SATR_CPU_DDR_L2_FREQ:
		mvOsPrintf("value= %d (CPU freq= %dMhz, DDR freq= %dMhz, L2 freq= %dMhz\n", \
				   tmp, 															\
				   cpuDdrClkTbl[tmp].cpuFreq,   										\
				   cpuDdrClkTbl[tmp].ddrFreq, 										\
				   cpuDdrClkTbl[tmp].l2Freq);

		break;
	case MV_SATR_CORE_CLK_SELECT:
		mvOsPrintf("Core clock frequency= %sMHz\n", (tmp == 0)? "0x0 - 166" : "0x1 - 200");
		break;
	case MV_SATR_CPU1_ENABLE:
		mvOsPrintf("CPU 1 %s\n", (tmp == 0)? "Disabled" : "Enabled");
		break;
	case MV_SATR_SSCG_DISABLE:
		mvOsPrintf("sscg = %d ==> %s \n", tmp, (tmp == 0)? "Disabled" : "Enabled");
		break;
	case MV_SATR_DDR4_SELECT:
		mvOsPrintf("DDR%d module mounted.\n", (tmp + 3));
		break;
	case MV_SATR_DDR_BUS_WIDTH:
		mvOsPrintf("Select DDR BUS width %d bit :\n", ((tmp+1) * 16) );
		break;
	case MV_SATR_DDR_ECC_ENABLE:
		mvOsPrintf("DDR ECC: ");
		if (tmp)
			mvOsPrintf("0x0 = No ECC      \n");
		else
			mvOsPrintf("0x1 = ECC enabled  \n ");
		break;
	case MV_SATR_NAND_DETECT:
		mvOsPrintf("NAND Device detect: %s\n", (tmp == 0) ? "0x0 = No NAND":"0x1 = NAND device detected");
		break;
	case MV_SATR_RD_LANE1_2_CFG:
		mvOsPrintf("SERDES lane 1 and 2 configuring to %s\n", (tmp == 0) ? "PEX" : "SATA");
		break;
	case MV_SATR_RD_LANE4_CFG:
		mvOsPrintf("SERDES lane 4 configuring to %s\n", (tmp == 0) ? "USB3" : "SGMII");
		break;
	case MV_SATR_RD_LANE0_CFG:
		mvOsPrintf("SERDES lane 0 configuring to %s\n", (tmp == 0) ? "NONE" : "PEX");
		break;
	case MV_SATR_BOOT_DEVICE:
		mvOsPrintf("Boot source device:\n");
		if (bootSrcTbl[tmp].internalFreq)
			mvOsPrintf("%s\n", bootSrcTbl[tmp].bootstr);
		break;
	case MV_SATR_RD_APPS_CFG:
		mvOsPrintf("RD application configure to %s\n", (tmp == 0) ? "NAS" : "AP");
		break;
	case MV_SATR_BOARD_ID:
		mvOsPrintf("Board ID = 0x%x\n", tmp);
		break;
	case MV_SATR_BOARD_ECO_VERSION:
		mvOsPrintf("Determines the ECO version %d.%d\n", (tmp & 0x0f), ((tmp >> 4) & 0x0f));
		break;

		case CMD_DUMP:
		{ //TODO
		}
		break;

		case CMD_UNKNOWN:
		default:
			mvOsPrintf("Usage: sar list [options] (see help) \n");
			return 1;
	}
	return 0;
}

static int do_sar_write(MV_BOARD_SATR_INFO *satrInfo, int value)
{
	MV_STATUS rc = MV_TRUE;
	if (satrInfo->status & BOARD_SATR_READ_ONLY)
	{
		mvOsPrintf("S@R ID = %d is read only\n", satrInfo->satrId);
		mvOsPrintf("Write S@R failed!\n");
		return 1;
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
	int mode, value;
	MV_BOARD_SATR_INFO satrInfo;

	/* need at least two arguments */
	if (argc < 2)
		goto usage;

	cmd = argv[1];
	mode = sar_cmd_get(argv[2]);
	if (mode == CMD_UNKNOWN)
		goto usage;
	if (mode != CMD_DUMP) {
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
		if (do_sar_write(&satrInfo, value) == 0)
			do_sar_read(mode, &satrInfo);
		return 0;

	} else if (strcmp(cmd, "read") == 0)
		return do_sar_read(mode, &satrInfo);

usage:
	cmd_usage(cmdtp);
	return 1;
}

U_BOOT_CMD(SatR, 6, 1, do_sar,
	"Sample At Reset sub-system\n",

	"list corefreq  - prints the S@R modes list\n"
	"SatR list cpufreq	- prints the S@R modes list\n"
	"SatR list cpusnum	- prints the S@R modes list\n"
    "SatR list sscg		- prints the S@R modes list\n\n"
	"SatR list ddr4select	- prints the S@R modes list\n"
	"SatR list ddrbuswidth	- prints the S@R modes list\n"
	"SatR list ddreccenable	- prints the S@R modes list\n"
	"SatR list nanddetect	- prints the S@R modes list\n"
    "SatR list rd_lane1_2_cfg - (RD only) prints the S@R modes list\n"
    "SatR list rd_lane4_cfg - (RD only) prints the S@R modes list\n"
    "SatR list rd_lane0_cfg - (RD only) prints the S@R modes list\n"
	"SatR list rd_apps_cfg - (RD only) prints the S@R modes list\n"
	"SatR list boot_device - (RD only) prints the S@R modes list\n"

	"SatR read corefreq	- read and print the core frequency S@R value\n"
	"SatR read cpuddrfreq	- read and print the CPU DDR frequency S@R value\n"
	"SatR read cpu1enable       - read and print the TM frequency S@R value\n"
	"SatR read ddr4select       - read and print the TM frequency S@R value\n"
	"SatR read ddrbuswidth      - read and print the TM frequency S@R value\n"
	"SatR read ddreccenable     - read and print the TM frequency S@R value\n"
	"SatR read nanddetect       - read and print the TM frequency S@R value\n"
	"SatR read rd_lane1_2_cfg     - read and print the TM frequency S@R value\n"
	"SatR read rd_lane4_cfg      - read and print the TM frequency S@R value\n"
	"SatR read rd_lane0_cfg      - read and print the TM frequency S@R value\n"
	"SatR read rd_apps_cfg -     - read and print the TM frequency S@R value\n"
	"SatR read boot_device -     - read and print the TM frequency S@R value\n"
	"SatR read board_id     - read and print the TM frequency S@R value\n"
	"SatR read EcoVersion     - read and print the TM frequency S@R value\n"

	"SatR write corefreq   <val>	- write the S@R with core frequency value\n"
	"SatR write cpuddrfreq <val>	- write the S@R with CPU DDR frequency value\n"
	"SatR write tmfreq     <val>	- write the S@R with TM frequency value\n"
	"SatR write bootsrc <val>	- write the S@R with Boot source value\n"
);
#endif /*defined(CONFIG_CMD_SAR)*/
