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
#include "switchingServices/switchingServices.h"

enum {
	CMD_DUMP = MV_SATR_MAX_OPTION+1,
	CMD_DEFAULT,
	CMD_UNKNOWN,
	CMD_NOT_ACTIVE	// Command that requests valid but not active field on current board
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
#define MAX_DEFAULT_ENTRY	23
MV_BOARD_SATR_DEFAULT boardSatrDefault[MAX_DEFAULT_ENTRY] = {
/* 	defauleValueForBoard[] = RD_NAS_68xx,	DB_BP_68xx,	RD_WAP_68xx,	DB_AP_68xx , DB_GP_68xx,  DB_BP_6821,	DB-AMC */
{ MV_SATR_CPU_DDR_L2_FREQ,	{0x0c,		0x0c,		0x0c,		0x0c,		0x0c,	  0x4,		0x0c}},
{ MV_SATR_CORE_CLK_SELECT,	{0,		0,		0,		0,		0,	  1,		1}},
{ MV_SATR_CPU1_ENABLE,	  	{MV_TRUE,	MV_TRUE,	MV_TRUE,	MV_TRUE,	MV_TRUE,  MV_TRUE,	MV_TRUE}},
{ MV_SATR_SSCG_DISABLE,	  	{MV_FALSE,	MV_FALSE,	MV_FALSE,	MV_FALSE,	MV_FALSE, MV_FALSE,	MV_FALSE}},
{ MV_SATR_SGMII_SPEED,		{0,		0,		0,		0,		0,	  0,		0}},
{ MV_SATR_DDR_BUS_WIDTH,	{1,		1,		1,		1,		1,	  0,		1}},
{ MV_SATR_RD_SERDES4_CFG,	{0,		0,		0,		0,		0,	  0,		0}},
{ MV_SATR_GP_SERDES5_CFG,	{0,		0,		0,		0,		0,	  0,		0}},
{ MV_SATR_GP_SERDES1_CFG,	{0,		0,		0,		0,		0,	  0,		0}},
{ MV_SATR_GP_SERDES2_CFG,	{0,		0,		0,		0,		0,	  0,		0}},
{ MV_SATR_DB_SERDES1_CFG,	{1,		1,		1,		1,		1,	  1,		0}},
{ MV_SATR_DB_SERDES2_CFG,	{1,		1,		1,		1,		1,	  1,		0}},
{ MV_SATR_DB_USB3_PORT0,	{0,		0,		0,		0,		0,	  0,		0}},
{ MV_SATR_DB_USB3_PORT1,	{0,		0,		0,		0,		0,	  0,		0}},
{ MV_SATR_DDR_ECC_ENABLE,	{0,		0,		0,		0,		0,	  0,		1}},
{ MV_SATR_DDR_ECC_PUP_SEL,	{0,		0,		0,		0,		0,	  0,		1}},
{ MV_SATR_FULL_FLAVOR,		{0,		1,		0,		1,		1,	  1,		0} },
{ MV_SATR_TDM_CONNECTED,	{0,		1,		0,		0,		0,	  0,		0} },
{ MV_SATR_TDM_PLACE_HOLDER,	{0,		1,		0,		0,		0,	  0,		0} },
{ MV_SATR_BOARD_SPEED,		{0,		0x1,		0,		0x1,		0x1,	  0,		0x1} },
{ MV_SATR_AVS_SKIP,		{0,		0x1,		0,		0x1,		0x1,	  0x1,		0x1} },
{MV_SATR_BOOT_DEVICE,           {0,             0,              0,		0,		0,	  0,		0} },/* Dummy entry: default value taken from S@R register */
{MV_SATR_BOOT2_DEVICE,          {0,             0,              0,		0,		0,	  0,		0} },/* Dummy entry: default value taken from S@R register */
};

char *lane1Arr[7] = {	"Unconnected" ,
			"PCIe Port 0",
			"SATA3 Port 0",
			"SGMII-0",
			"SGMII-1",
			"USB3.0 Port 0",
			"QSGMII" };

char *lane2Arr[4] = {	"Unconnected" ,
			"PCIe Port 1",
			"SATA3 Port 1",
			"SGMII-1" };

char *devIdArr[4] = {
			"6810 (A380)",
			"6820 (A385)",
			"N/A",
			"6828 (A388)" };

static MV_U32 getBoardSpeed(MV_U32 boardspeed)
{
	switch (boardspeed) {
	case 1:
		return 2000;
	case 2:
		return 1866;
	case 3:
		return 1600;
	default:
		return 0;
	}
}
/*
 * when each CPU is checked, max CPU speed is determined for each board
 * To enable the speed limitation for each board, the according value is written
 * in SatR field 'boardspeed':
 * -0x1 for max value 2000Mhz
 * -0x2 for max value 1866MHz
 * -0x3 for max value 1600MHz
 *
 * The function returns MV_TRUE if the cpuMode is not limited on this board
 * by checking the 'boardspeed' field, otherwise returns MV_FALSE
 */
static MV_BOOL isCpuModeSupportedForBoard(int index, MV_BOOL printError)
{
	MV_U32 tmp;

	if (cpuDdrClkTbl[index].isLimited == MV_TRUE) {
		tmp = mvBoardSatRRead(MV_SATR_BOARD_SPEED);
		if (tmp == MV_ERROR) {
			mvOsPrintf("Failed reading 'boardspeed' SatR field\n");
			return MV_FALSE;
		}

		if (cpuDdrClkTbl[index].cpuFreq > getBoardSpeed(tmp)) {
			if (printError == MV_TRUE)
				mvOsPrintf("Maximum supported CPU speed is %uMHz\n", getBoardSpeed(tmp));
			return MV_FALSE;
		}
	}
	return MV_TRUE;
}

int do_sar_default(void)
{
	MV_U32 i, rc, defaultValue, boardId = mvBoardIdIndexGet(mvBoardIdGet());
	MV_SATR_TYPE_ID satrClassId;
	MV_BOARD_SATR_INFO satrInfo;
	MV_U32 satrBootDeviceValue = mvCtrlbootSrcGet(), tmp;
	for (i = 0; i < MAX_DEFAULT_ENTRY; i++) {
		satrClassId = boardSatrDefault[i].satrId;
		if (mvBoardSatrInfoConfig(satrClassId, &satrInfo) != MV_OK)
			continue;
		if (satrClassId == MV_SATR_BOOT_DEVICE)
			boardSatrDefault[i].defauleValueForBoard[boardId] = satrBootDeviceValue & satrInfo.mask;
		if (satrClassId == MV_SATR_BOOT2_DEVICE) {
			tmp = satrBootDeviceValue >> MV_SATR_BOOT2_VALUE_OFFSET;
			boardSatrDefault[i].defauleValueForBoard[boardId] = tmp & MV_SATR_BOOT2_VALUE_MASK;
		}
		defaultValue = boardSatrDefault[i].defauleValueForBoard[boardId];
		rc = mvBoardSatRWrite(satrClassId, defaultValue);
		if (rc == MV_ERROR) {
			mvOsPrintf("Error write S@R for id=%d\n", satrClassId);
		}
	}

	/* set default Device ID - if MV_SATR_DEVICE_ID field is relevant on board */
	/* A383/A384 are virtual device ID's - not represented as HW device ID values in S@R@0x18600.
	A383/A384 are configured in SW EEPROM with FULL_FLAVOR field: if false override device ID */

	if (!(mvCtrlModelGet() == MV_6W22_DEV_ID
			|| mvCtrlModelGet() ==  MV_6W23_DEV_ID)) { /* 6W22=A383, 6W23=A384 */
		if (mvBoardSatrInfoConfig(MV_SATR_DEVICE_ID, &satrInfo) == MV_OK) {
			if (mvBoardSatRWrite(MV_SATR_DEVICE_ID, mvCtrlDevIdIndexGet(mvCtrlModelGet())) == MV_ERROR)
				mvOsPrintf("Error writing default Device ID ('devid') =%d\n", i);
		}
	}
	printf("\nSample at Reset values were restored to default.\n");
	return 0;
}

int sar_cmd_get(const char *cmd)
{
	int i;
	MV_BOARD_SATR_INFO satrInfo;
	MV_STATUS readResult;

	for (i = MV_SATR_CPU_DDR_L2_FREQ; i < MV_SATR_MAX_OPTION; i++) {
		if (i == MV_SATR_BOOT2_DEVICE || i == MV_SATR_DEVICE_ID2)
			continue;
		readResult = mvBoardSatrInfoConfig(i, &satrInfo);
		if (readResult != MV_ERROR && strcmp(cmd,satrInfo.name) == 0) {
			if (readResult == MV_BAD_VALUE)
				return CMD_NOT_ACTIVE;
			else
				return satrInfo.satrId;
		}
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
		mvOsPrintf("| ID | CPU Freq (MHz) | DDR Freq (MHz)   |  L2 freq(MHz)  |\n");
		mvOsPrintf("|----|----------------|------------------|----------------|\n");
		for (i=0; i <= MV_SAR_FREQ_MODES_EOT; i++) {
			if (cpuDdrClkTbl[i].id == MV_SAR_FREQ_MODES_EOT)
				break;
			if (cpuDdrClkTbl[i].isDisplay && isCpuModeSupportedForBoard(i, MV_FALSE) == MV_TRUE)
				mvOsPrintf("| %2d |      %4d      |      %4d        |      %4d      | \n",
					   cpuDdrClkTbl[i].id,
					   cpuDdrClkTbl[i].cpuFreq,
					   cpuDdrClkTbl[i].ddrFreq,
					   cpuDdrClkTbl[i].l2Freq);
		}
		mvOsPrintf("-----------------------------------------------------------\n");
		break;
	case MV_SATR_CORE_CLK_SELECT:
		mvOsPrintf("Determines the Core clock frequency:\n");
		if (mvCtrlModelGet() == MV_6811_DEV_ID)
			mvOsPrintf("\t0 = 166MHz\n");	/* device 381/2 (6811/21) use 166MHz instead of 250MHz */
		else
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
	case MV_SATR_GP_SERDES1_CFG:
		mvOsPrintf("Determines the GP SERDES lane #1 configuration:\n");
		mvOsPrintf("\t0 = SATA0\n");
		mvOsPrintf("\t1 = PCIe0 (mini PCIe)\n ");
		break;
	case MV_SATR_GP_SERDES2_CFG:
		mvOsPrintf("Determines the GP SERDES lane #2 configuration:\n");
		mvOsPrintf("\t0 = SATA1\n");
		mvOsPrintf("\t1 = PCIe1 (mini PCIe)\n ");
		break;
	case MV_SATR_DB_SERDES1_CFG:
		mvOsPrintf("Determines the DB SERDES lane #1 configuration:\n");
		for (i = 0; i <  ARRAY_SIZE(lane1Arr); i++)
			mvOsPrintf("\t %d = %s\n" , i ,lane1Arr[i]);
		break;
	case MV_SATR_DB_SERDES2_CFG:
		mvOsPrintf("Determines the DB SERDES lane #2 configuration:\n");
		for (i = 0; i < ARRAY_SIZE(lane2Arr); i++)
			mvOsPrintf("\t %d = %s\n" , i ,lane2Arr[i]);
		break;
	case MV_SATR_SGMII_MODE:
		mvOsPrintf("Determines the SGMII negotiation mode:\n");
		mvOsPrintf("\t0 = In-band\n");
		mvOsPrintf("\t1 = Out-of-band (PHY)\n ");
		break;
	case MV_SATR_DEVICE_ID:
		mvOsPrintf("Determines the Device ID:\n");
		for (i = 0; i < ARRAY_SIZE(devIdArr); i++)
			mvOsPrintf("\t %d = %s\n", i, devIdArr[i]);
		break;
	case MV_SATR_FULL_FLAVOR:
		mvOsPrintf("Determines whether to use full flavor capabilites:\n");
		mvOsPrintf("\t0 = Reduced Flavor\n");
		mvOsPrintf("\t1 = Full Flavor\n ");
		break;
	case MV_SATR_TDM_CONNECTED:
		mvOsPrintf("Indicates whether TDM module is connected or not:\n");
		mvOsPrintf("\t0 = Connected\n");
		mvOsPrintf("\t1 = Not connected\n ");
		break;
	case MV_SATR_AVS_SKIP:
		mvOsPrintf("Indicates whether to skip AVS update from EFUSE or not:\n");
		mvOsPrintf("\t0 = Do not skip AVS update from EFUSE\n");
		mvOsPrintf("\t1 = Skip AVS update from EFUSE\n");
		break;
	case MV_SATR_BOARD_SPEED:
		mvOsPrintf("Determines the max supported CPU speed:\n");
		mvOsPrintf("\t1 = %uMHz\n", getBoardSpeed(1));
		mvOsPrintf("\t2 = %uMHz\n", getBoardSpeed(2));
		mvOsPrintf("\t3 = %uMHz\n", getBoardSpeed(3));
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
	char core_clk_value0[4];

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
				mvOsPrintf("| ID | CPU Freq (MHz) | DDR Freq (MHz)   |  L2 freq(MHz)  |\n");
				mvOsPrintf("|----|----------------|------------------|----------------|\n");
				mvOsPrintf("| %2d |      %4d      |      %4d        |      %4d      |\n",
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
		/* device 381/2 (6811/21) use 166MHz instead of 250MHz */
		sprintf(core_clk_value0, mvCtrlModelGet() == MV_6811_DEV_ID ? "166" : "250");
		mvOsPrintf("\ncoreclock\t= %d  ==> %sMhz\n", tmp, (tmp == 0x0) ? core_clk_value0 : "200");
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
	case MV_SATR_GP_SERDES1_CFG:
		mvOsPrintf("gpserdes1\t= %d  ==> GP SERDES Lane #1: %s\n", tmp, (tmp == 0) ? "SATA0" : "PCIe0 (mini PCIe)");
		break;
	case MV_SATR_GP_SERDES2_CFG:
		mvOsPrintf("gpserdes2\t= %d  ==> GP SERDES Lane #2: %s\n", tmp, (tmp == 0) ? "SATA1" : "PCIe1 (mini PCIe)");
		break;
	case MV_SATR_DB_SERDES1_CFG:
		mvOsPrintf("dbserdes1\t= %d  ==> DB SERDES Lane #1: %s\n", tmp, lane1Arr[tmp]);
		break;
	case MV_SATR_DB_SERDES2_CFG:
		mvOsPrintf("dbserdes2\t= %d  ==> DB SERDES Lane #2: %s\n", tmp, lane2Arr[tmp]);
		break;
	case MV_SATR_SGMII_MODE:
		mvOsPrintf("sgmiimode\t= %d  ==> SGMII mode: %s\n", tmp, (tmp == 0) ? "In-band" : "Out-of-band (PHY)");
		break;
	case MV_SATR_DEVICE_ID:
		mvOsPrintf("devid\t\t= %d  ==> Device ID: %s\n", tmp, devIdArr[tmp]);
		break;
	case MV_SATR_FULL_FLAVOR:
		mvOsPrintf("flavor\t\t= %d  ==> %s Flavor", tmp, (tmp == 0) ? "Reduced" : "Full");
		if (!tmp) {
			if (mvBoardIdGet() == DB_GP_68XX_ID)
				mvOsPrintf(" (A384 on DB-GP)");
			else if (mvBoardIdGet() == DB_BP_6821_ID)
				mvOsPrintf(" (A383 on DB-88F6821-BP)");
		}
		mvOsPrintf("\n");
		break;
	case MV_SATR_TDM_CONNECTED:
		mvOsPrintf("tdm\t\t= %d  ==> TDM module is %s\n", tmp, (tmp == 0) ? "connected" : "not connected");
		break;
	case MV_SATR_AVS_SKIP:
		mvOsPrintf("avsskip\t\t= %d  ==> %sskip AVS update from EFUSE\n", tmp, (tmp == 0) ? "Do not " : "");
		break;
	case MV_SATR_BOARD_SPEED:
		mvOsPrintf("boardspeed\t\t= %d  ==> Max CPU speed is %uMHz\n", tmp, getBoardSpeed(tmp));
		break;
	case CMD_DUMP:
		{
			MV_BOARD_SATR_INFO satrInfo;

			for (i = MV_SATR_CPU_DDR_L2_FREQ; i < MV_SATR_MAX_OPTION; i++) {
				if (i == MV_SATR_BOOT2_DEVICE || i == MV_SATR_DEVICE_ID2)
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
	MV_U32 i, freqValueInvalid;

	/* if field is read only, or field is board id for DB boards - modification not supported */
	if (satrInfo->status & SATR_READ_ONLY ||
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

	if (satrInfo->satrId == MV_SATR_CPU_DDR_L2_FREQ) {
		/* DB-6821-BP maximum CPU/DDR frequency mode is 8 : 1332/666 MHz */
		if (boardId == DB_BP_6821_ID && value > 0x8) {
			mvOsPrintf("Maximum supported CPU/DDR mode for DB-6821-BP is 0x8\n");
			return 1;
		}
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
		if (isCpuModeSupportedForBoard(i, MV_TRUE) == MV_FALSE)
			return 1;
	}

	/* verify requested entry is valid and map it's ID value */
	if (satrInfo->satrId == MV_SATR_DEVICE_ID) {
		if (value > ARRAY_SIZE(devIdArr) || value == 2) { /* devid = 2 (6811/21) : not supported via SatR */
			printf("%s: Error: requested invalid Device ID value (%x)\n", __func__, value);
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
		readline(" ");
		if(strlen(console_buffer) != 0 && /* if pressed Enter */
			strcmp(console_buffer,"n") != 0 &&
			strcmp(console_buffer,"N") != 0 )
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

	/* Check if switch is connected to AMC board */
	if (boardId == DB_AMC_6820_ID) {
		static SILICON_TYPE silt = SILT_NOT_DETECT;

		if (silt == SILT_NOT_DETECT)
			silt = get_attached_silicon_type();
		if (silt == SILT_BC2)
			return do_sar_bc2(cmdtp, flag, argc, argv);
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

	if (mode == CMD_NOT_ACTIVE){
		printf("\nRequested SatR field is not active on current board (type 'SatR' for valid options) \n");
		return 1;
	}

	mvBoardSatrInfoConfig(mode, &satrInfo);
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
"coreclock                  - DB-88F6820-BP, DB-88F6821-BP, DB-GP: core frequency\n"
"freq                       - CPU DDR frequency\n"
"cpusnum                    - DB-88F6821-BP only: number of CPU cores\n"
"sscg                       - DB-88F6820-BP, DB-88F6821-BP, DB-GP: SSCG modes\n"
"bootsrc                    - DB-88F6820-BP, DB-88F6821-BP: boot source\n"
"devid			   - DB-88F6820-BP, DB-GP:      Device ID flavor\n"

"\tSW SatR fields\n"
"\t--------------\n"
"ddrbuswidth 		   - DB-88F6820-BP, DB-GP, DB-AP: DDR bus width\n"
"ddreccenable		   - DDR ECC enable\n"
"ddreccpupselect		   -DB-88F6820-BP: DDR ECC PUP selection\n"
"sgmiispeed		   - SGMII speed\n"
"sgmiimode		   - SGMII negotiation mode\n"
"ddr4select		   - DB-88F6820-BP: DDR3/4		(read only)\n"
"ecoversion		   - DB-88F6820-BP: ECO version	(read only)\n"
"boardid			   - board ID		(read only)\n"
"boardspeed			   - MAX validated CPU mode for current chip		(read only)\n"
"avsskip			   - Skip AVS from EFUSE update\n"

"\n\t Board Specific SW fields\n"
"\t------------------------\n"
"gpserdes1		   - DB-GP:			SerDes lane #1\n"
"gpserdes2		   - DB-GP:			SerDes lane #2\n"
"gpserdes5		   - DB-GP:			SerDes lane #5\n"
"dbserdes1		   - DB-GP, DB-88F6821-BP:	SerDes lane #1\n"
"dbserdes2		   - DB-GP, DB-88F6821-BP:	SerDes lane #2\n"
"usb3port0		   - DB-GP, DB-88F6821-BP:	USB3-Port0 mode\n"
"usb3port1		   - DB-GP, DB-88F6821-BP:	USB3-Port1 mode\n"
"flavor			   - DB-GP, DB-88F6821-BP: Is full flavor (for A383/4 simulation)\n"
"tdm			   - DB-BP:			is TDM module connected\n\n"


);
#endif /*defined(CONFIG_CMD_SAR)*/
