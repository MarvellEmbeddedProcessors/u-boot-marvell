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

extern int do_sar(cmd_tbl_t *cmdtb, int flag, int argc, char *const argv[]);

typedef struct _boardSatrDefault {
	MV_SATR_TYPE_ID satrId;
	MV_U32 defauleValueForBoard[MV_MARVELL_BOARD_NUM];
} MV_BOARD_SATR_DEFAULT;

MV_BOARD_SATR_DEFAULT boardSatrDefault[] = {
/* 	defauleValueForBoard[] ={ DB_6720	 }*/
{ MV_SATR_WRITE_CPU_FREQ,	{ 0x15 		}},
{ MV_SATR_WRITE_CORE_CLK_SELECT,{ _200MHz	}},
{ MV_SATR_WRITE_CPU1_ENABLE,	{ 1		}},
{ MV_SATR_WRITE_SSCG_DISABLE,	{ 0		}},
{ MV_SATR_WRITE_DDR_BUS_WIDTH,	{ 0		}},
};
static int do_sar_default(void)
{
	MV_U32 temp, defaultValue, boardIdIndex, boardId = mvBoardIdGet();

	boardIdIndex = mvBoardIdIndexGet(boardId);

	/* Frequency mode */
	defaultValue = boardSatrDefault[MV_SATR_CPU_DDR_L2_FREQ].defauleValueForBoard[boardIdIndex];
	if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_CPU_DDR_L2_FREQ)))
		mvCtrlSatRWrite(MV_SATR_WRITE_CPU_FREQ, MV_SATR_CPU_DDR_L2_FREQ, defaultValue);

	/* Core Clock */
	defaultValue = boardSatrDefault[MV_SATR_CORE_CLK_SELECT].defauleValueForBoard[boardIdIndex];
	if (defaultValue == _200MHz)
		defaultValue = 0x1; /* 0x1: 200MHz */
	else
		defaultValue = 0x0; /* 0x0: 166MHz */

	if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_CORE_CLK_SELECT)))
		mvCtrlSatRWrite(MV_SATR_WRITE_CORE_CLK_SELECT, MV_SATR_CORE_CLK_SELECT, defaultValue);

	/* Single / Dual CPU */
	defaultValue = boardSatrDefault[MV_SATR_CPU1_ENABLE].defauleValueForBoard[boardIdIndex];
	if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_CPU1_ENABLE)))
		mvCtrlSatRWrite(MV_SATR_WRITE_CPU1_ENABLE, MV_SATR_CPU1_ENABLE, defaultValue);

	/* SSCG */
	defaultValue = boardSatrDefault[MV_SATR_SSCG_DISABLE].defauleValueForBoard[boardIdIndex];
	if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_SSCG_DISABLE)))
		mvCtrlSatRWrite(MV_SATR_WRITE_SSCG_DISABLE, MV_SATR_SSCG_DISABLE, defaultValue);

	/* MV_SATR_DDR_BUS_WIDTH */
	defaultValue = boardSatrDefault[MV_SATR_DDR_BUS_WIDTH].defauleValueForBoard[boardIdIndex];
	if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_SSCG_DISABLE)))
		mvCtrlSatRWrite(MV_SATR_WRITE_DDR_BUS_WIDTH, MV_SATR_DDR_BUS_WIDTH, defaultValue);

	printf("\nSample at Reset values were restored to default.\n");
	return 0;
}
static int do_sar_list(int argc, char *const argv[])
{
	const char *cmd;
	MV_FREQ_MODE pFreqModes[] = MV_USER_SAR_FREQ_MODES;
	int i, maxFreqModes = mvBoardFreqModesNumGet();

	if (argc < 1)
		goto usage;
	cmd = argv[0];

	if (strcmp(cmd, "freq") == 0) {
		printf("freq options - Determines the frequency of CPU/DDR/L2:\n");
		printf("\n| ID  | CPU Freq (Mhz) | DDR Freq (Mhz) | L2 Freq (Mhz) |\n");
		printf("---------------------------------------------------------\n");
		for (i = 0; i < maxFreqModes; i++) {
			printf("|  %2d |      %4d      |      %d       |      %d      |\n",
				pFreqModes[i].id,
				pFreqModes[i].cpuFreq,
				pFreqModes[i].ddrFreq,
				pFreqModes[i].l2Freq);
		}
		printf("---------------------------------------------------------\n");
	} else if (strcmp(cmd, "coreclock") == 0) {
		printf("Determines the frequency of Core Clock:\n");
		printf("\t0x0 = 166Mhz\n");
		printf("\t0x1 = 200Mhz\n");
	} else if (strcmp(cmd, "cpusnum") == 0) {
		printf("cpusnum options - Determines the number of CPU cores:\n");
		printf("\t0x0 = Single CPU\n");
		printf("\t0x1 = Dual CPU\n");
	} else if (strcmp(cmd, "sscg") == 0) {
		printf("Determines the SSCG  mode:\n");
		printf("\t0x0 = SSCG Enabled\n");
		printf("\t0x1 = SSCG Disabled\n");
	} else if (strcmp(cmd, "ddr_buswidth") == 0) {
		printf("Determines the DDR bus width mode:\n");
		printf("\t0x0 = 32 Bit\n");
		printf("\t0x1 = 16 Bit\n");
	} else if (strcmp(cmd, "mac1") == 0) {
		printf("Determines the MAC1 connectivity setup:\n");
		printf("\t0x0 = Internal GE-PHY\n");
		printf("\t0x1 = SGMII (via ETH SerDes)\n");
	} else {
		goto usage;
	}
	return 0;
usage:
	printf("Usage: sar list [options] (see help)\n");
	return 1;
}

static MV_STATUS GetAndVerifySatr(MV_SATR_TYPE_ID satrField, MV_U32* result)
{
	MV_U32 temp = mvCtrlSatRRead(satrField);

	if (temp == MV_ERROR) {
		printf("%s: Requested SatR field is not relevant for current board\n", __func__);
		return MV_ERROR;
	}

	*result = temp;
	return MV_OK;
}

static int do_sar_read(int argc, char *const argv[])
{
	const char *cmd;
	MV_BOOL dump_all = MV_FALSE;
	MV_U32 temp;
	MV_FREQ_MODE freqMode;
	char* bootSrcArr[8];

	bootSrcArr[0] = "NOR flash";
	bootSrcArr[1] = "NAND flash v2";
	bootSrcArr[2] = "UART";
	bootSrcArr[3] = "SPI0 (CS0)";
	bootSrcArr[4] = "PEX";
	bootSrcArr[5] = "NAND legacy flash";
	bootSrcArr[6] = "PROMPT";
	bootSrcArr[7] = "SPI1 (CS1)";

	if (argc < 1)
		dump_all=MV_TRUE;
	cmd = argv[0];

	if ((strcmp(cmd, "freq") == 0) && (MV_ERROR != mvCtrlCpuDdrL2FreqGet(&freqMode)))
	{
		printf("\nCurrent freq configuration:\n");
		printf("| ID  | CPU Freq (Mhz) | DDR Freq (Mhz) | L2 Freq (Mhz) |\n");
		printf("|  %2d |      %4d      |      %d       |      %d      | \n",
				freqMode.id,
				freqMode.cpuFreq,
				freqMode.ddrFreq,
				freqMode.l2Freq);
		printf("---------------------------------------------------------\n");
	}

	else if (strcmp(cmd, "coreclock") == 0) {
		if (GetAndVerifySatr(MV_SATR_CORE_CLK_SELECT, &temp) == MV_OK)
			printf("\ncoreclock = %d  ==> %sMhz\n",temp, (temp == 0x0) ? "166" : "200");
	}
	else if (strcmp(cmd, "cpusnum") == 0) {
		if (GetAndVerifySatr(MV_SATR_CPU1_ENABLE, &temp) == MV_OK)
			printf("\ncpusnum = %d  ==> %s CPU \n", temp, (temp == 0) ? "Single" : "Dual");
	}
	else if (strcmp(cmd, "sscg") == 0) {
		if (GetAndVerifySatr(MV_SATR_SSCG_DISABLE, &temp) == MV_OK)
			printf("\nsscg = %d  ==> %s\n",temp, (temp == 0) ? "Disabled" : "Enabled");
	}
	else if (strcmp(cmd, "ddr_buswidth") == 0) {
		if (GetAndVerifySatr(MV_SATR_DDR_BUS_WIDTH, &temp) == MV_OK)
			printf("\nDDR Bus width = %d  ==> %s Bit\n",temp, (temp == 0) ? "32" : "16");
	}
	else if (strcmp(cmd, "mac1") == 0) {
		if (GetAndVerifySatr(MV_SATR_MAC1, &temp) == MV_OK)
			printf("\nMAC1 = %d  ==> %s \n",temp, (temp == 0) ? "Internal GE-PHY" : "SGMII (via ETH SerDes)");
	}
	else if (strcmp(cmd, "i2c0") == 0) {
		if (GetAndVerifySatr(MV_SATR_I2C0_SERIAL_ROM, &temp) == MV_OK)
		printf("\ni2c0 = %d  ==> %s\n",temp, (temp == 0) ? "Disabled" : "Enabled");
	}
	else if (strcmp(cmd, "cpureset") == 0) {
		if (GetAndVerifySatr(MV_SATR_EXTERNAL_CORE_RESET, &temp) == MV_OK)
			printf("\ncpureset = %d  ==> %s\n",temp, (temp == 0) ? "Disabled" : "Enabled");
	}
	else if (strcmp(cmd, "corereset") == 0) {
		if (GetAndVerifySatr(MV_SATR_EXTERNAL_CPU_RESET, &temp) == MV_OK)
			printf("\ncorereset = %d  ==> %s\n",temp, (temp == 0) ? "Disabled" : "Enabled");
	}
	else if (strcmp(cmd, "bootsrc") == 0) {
		if (GetAndVerifySatr(MV_SATR_BOOT_DEVICE, &temp) == MV_OK)
			printf("\nbootsrc = %d  ==> %s\n", temp, bootSrcArr[mvBoardBootDeviceGet()]);
	}
	else if (strcmp(cmd, "cpubypass") == 0) {
		if (GetAndVerifySatr(MV_SATR_CPU_PLL_XTAL_BYPASS, &temp) == MV_OK)
		printf("\ncpubypass = %d  ==> %s Bypass\n",temp, (temp == 0) ? "PLL" : "XTAL");
	}
	else if (strcmp(cmd, "cpuendi") == 0) {
		if (GetAndVerifySatr(MV_SATR_CPU0_ENDIANESS, &temp) == MV_OK)
			printf("\ncpuendi = %d  ==> %s Endianess\n", temp, (temp == 0) ? "Little" : "Big");
	}
	else if (strcmp(cmd, "cpunmfi") == 0) {
		if (GetAndVerifySatr(MV_SATR_CPU0_NMFI, &temp) == MV_OK)
			printf("\ncpunmfi = %d  ==> FIQ mask %s \n", temp, (temp == 0) ? "Enabled" : "Disabled");
	}
	else if (strcmp(cmd, "cputhumb") == 0) {
		if (GetAndVerifySatr(MV_SATR_CPU0_THUMB, &temp) == MV_OK)
			printf("\ncputhumb = %d  ==> %s mode \n", temp, (temp == 0) ? "ARM" : "Thumb");
	} else if (strcmp(cmd, "pcimode0") == 0) {
		if (GetAndVerifySatr(MV_SATR_PEX0_CLOCK, &temp) == MV_OK)
		printf("\npcimode0 = %d  ==> %s mode\n", temp, (temp == 0) ? "input" : "output");
	}
	else if (strcmp(cmd, "refclk") == 0) {
		if (GetAndVerifySatr(MV_SATR_REF_CLOCK_ENABLE, &temp) == MV_OK)
			printf("\nrefclk = %d  ==> %s\n",temp, (temp == 0) ? "Disabled" : "Enabled");
	}
	else if (strcmp(cmd, "tester") == 0) {
		if (GetAndVerifySatr(MV_SATR_TESTER_OPTIONS, &temp) == MV_OK)
			printf("\ntester = %d \n",temp);
	}
	else if (dump_all == MV_TRUE)
	{
		printf ("\n\t\t  S@R configuration:\n\t\t----------------------\n");
		if  (MV_ERROR != mvCtrlCpuDdrL2FreqGet(&freqMode))
		{
			printf("\n| ID  | CPU Freq (Mhz) | DDR Freq (Mhz) | L2 Freq (Mhz) |\n");
		printf("---------------------------------------------------------\n");
			printf("|  %2d |      %4d      |      %d       |      %d      | \n",
				freqMode.id,
				freqMode.cpuFreq,
				freqMode.ddrFreq,
				freqMode.l2Freq);
		printf("---------------------------------------------------------\n\n");
		}
		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_CORE_CLK_SELECT)))
			printf("coreclock \t= %3d  ==>   %sMhz\n",temp, (temp == 0x0) ? "166" : "200");
		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_CPU1_ENABLE)))
		printf("cpusnum \t= %3d  ==>   %s CPU \n", temp, (temp == 0) ? "Single" : "Dual");
		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_SSCG_DISABLE)))
			printf("sscg \t\t= %3d  ==>   %s\n",temp, (temp == 0) ? "Disabled" : "Enabled");
		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_DDR_BUS_WIDTH)))
			printf("DDR Bus width \t= %3d  ==>   %s Bit\n",temp, (temp == 0) ? "32" : "16");
		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_MAC1)))
			printf("MAC1 \t\t= %3d  ==>   %s Bit\n",temp, (temp == 0) ? "Internal GE-PHY" : "SGMII (via ETH SerDes)");
		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_I2C0_SERIAL_ROM)))
			printf("i2c0 \t\t= %3d  ==>   %s\n",temp, (temp == 0) ? "Disabled" : "Enabled");
		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_EXTERNAL_CORE_RESET)))
			printf("cpureset \t= %3d  ==>   %s\n",temp, (temp == 0) ? "Disabled" : "Enabled");
		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_EXTERNAL_CPU_RESET)))
			printf("corereset \t= %3d  ==>   %s\n",temp, (temp == 0) ? "Disabled" : "Enabled");
		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_BOOT_DEVICE)))
			printf("bootsrc \t= %3d  ==>   %s\n", temp, bootSrcArr[mvBoardBootDeviceGet()]);
		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_CPU_PLL_XTAL_BYPASS)))
			printf("sscg \t\t= %3d  ==>   %s Bypass\n",temp, (temp == 0) ? "PLL" : "XTAL");
		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_CPU0_ENDIANESS)))
			printf("cpuendi \t= %3d  ==>   %s Endianess\n", temp, (temp == 0) ? "Little" : "Big");
		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_CPU0_NMFI)))
			printf("cpunmfi \t= %3d  ==>   FIQ mask %s \n", temp, (temp == 0) ? "Enabled" : "Disabled");
		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_CPU0_THUMB)))
			printf("cputhumb \t= %3d  ==>   %s mode \n", temp, (temp == 0) ? "ARM" : "Thumb");
		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_PEX0_CLOCK)))
			printf("pcimode0 \t= %3d  ==>   PEX0 clock %s enable\n",temp, (temp == 0) ? "Input" : "Output");
		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_REF_CLOCK_ENABLE)))
			printf("refclk \t\t= %3d  ==>   %s\n",temp, (temp == 0) ? "Disabled" : "Enabled");
		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_TESTER_OPTIONS)))
			printf("tester \t\t= %3d \n",temp);
	}
	else goto usage;
	return 0;
usage:
	printf("Usage: SatR read [options] (see help) \n");
	return 1;
}

static int do_sar_write(int argc, char *const argv[])
{
	const char *cmd = argv[0];
	MV_U32 temp;
	MV_BOOL flag;
	MV_U8 writeVal = simple_strtoul(argv[1], NULL, 10);
	MV_FREQ_MODE cpuFreqMode;

	if (argc < 2)
		goto usage;

	if (strcmp(cmd, "freq") == 0) {
		if (mvCtrlFreqModeGet(writeVal, &cpuFreqMode) != MV_OK)
			goto input_error;
		else if (GetAndVerifySatr(MV_SATR_CPU_DDR_L2_FREQ, &temp) == MV_OK )
			flag = mvCtrlSatRWrite(MV_SATR_WRITE_CPU_FREQ, MV_SATR_CPU_DDR_L2_FREQ, writeVal);
	}
	else if (strcmp(cmd, "coreclock") == 0) {
		if (writeVal != 0 && writeVal != 1)
			goto input_error;
		else if (GetAndVerifySatr(MV_SATR_CORE_CLK_SELECT, &temp) == MV_OK )
			flag = mvCtrlSatRWrite(MV_SATR_WRITE_CORE_CLK_SELECT,MV_SATR_CORE_CLK_SELECT, writeVal);
	}
	else if (strcmp(cmd, "cpusnum") == 0) {
		if (writeVal != 0 && writeVal != 1)
			goto input_error;
		else if (GetAndVerifySatr(MV_SATR_CPU1_ENABLE, &temp) == MV_OK )
			flag = mvCtrlSatRWrite(MV_SATR_WRITE_CPU1_ENABLE,MV_SATR_CPU1_ENABLE, writeVal);
	}
	else if (strcmp(cmd, "sscg") == 0) {
		if (writeVal != 0 && writeVal != 1)
			goto input_error;
		else if (GetAndVerifySatr(MV_SATR_SSCG_DISABLE, &temp) == MV_OK )
			flag = mvCtrlSatRWrite(MV_SATR_WRITE_SSCG_DISABLE,MV_SATR_SSCG_DISABLE, writeVal);
	}
	else if (strcmp(cmd, "ddr_buswidth") == 0) {
		if (writeVal != 0 && writeVal != 1)
			goto input_error;
		else if (GetAndVerifySatr(MV_SATR_DDR_BUS_WIDTH, &temp) == MV_OK )
			flag = mvCtrlSatRWrite(MV_SATR_WRITE_DDR_BUS_WIDTH,MV_SATR_DDR_BUS_WIDTH, writeVal);
	}
	else if (strcmp(cmd, "mac1") == 0) {
		if (writeVal != 0 && writeVal != 1)
			goto input_error;
		else if (GetAndVerifySatr(MV_SATR_MAC1, &temp) == MV_OK )
			flag = mvCtrlSatRWrite(MV_SATR_WRITE_MAC1, MV_SATR_MAC1, writeVal);
	}

/* the first 4 S@R fields are writeable using S@R commands - rest  values are edited using Jumpers/DIP switch/DPR (resistors) */
	else goto usage;

	return 0;

	if (MV_ERROR == flag)
		printf("Write S@R failed!\n");
	return 1;

input_error:
	printf("\nError: value is not valid for \"%s\" (%d)\n\n",cmd , writeVal);
	do_sar_list(1, argv);
	return 1;

usage:
	printf("Usage: SatR write [options] (see help) \n");
	return 1;
}

int do_sar(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	const char *cmd, *cmd2 = NULL;

	/* need at least two arguments */
	if (argc < 2)
		goto usage;

	cmd = argv[1];

	if (argc > 2)
		cmd2 = argv[2];

	if (strcmp(cmd, "list") == 0)
		return do_sar_list(argc - 2, argv + 2);
	else if ((strcmp(cmd, "write") == 0) && (strcmp(cmd2, "default") == 0)) {
		if (do_sar_default() == 0) {
			do_sar_read(argc - 3, argv + 3);
			printf("\nChanges will be applied after reset.\n\n");
			return 0;
		}
		else
			return 1;
	}
	else if (strcmp(cmd, "write") == 0) {
		if (do_sar_write(argc - 2, argv + 2) == 0) {
			do_sar_read(argc - 2, argv + 2);
			if (strcmp(cmd2, "freq") == 0 && !mvCtrlIsValidSatR())
				printf("\n*** Selected Unsupported DDR/CPU/L2 Clock configuration ***\n\n");
			printf("\nChanges will be applied after reset.\n");
			return 0;
		}
		else
			return 1;

	} else if (strcmp(cmd, "read") == 0)
		return do_sar_read(argc - 2, argv + 2);

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
"coreclock                  - Core Clock frequency\n"
"freq                       - CPU frequency\n"
"cpusnum                    - number of CPU cores\n"
"sscg                       - SSCG modes\n"
"i2c0                       - i2c0 mode (read only)\n"
"cpureset                   - CPU reset mode (read only)\n"
"corereset                  - Core reset mode (read only)\n"
"bootsrc                    - boot source (read only)\n"
"cpubypass                  - CPU Bypass mode (read only)\n"
"cpuendi                    - CPU Endianess (read only)\n"
"cpunmfi                    - CPU FIQ mask mode (read only)\n"
"cputhumb                   - CPU Thumb mode (ARM/ Thumb) (read only)\n"
"pcimode0                   - pci0 clock mode (input/output) (read only)\n"
"refclk                     - ref clock mode (read only)\n"
"tester                     - tester mode (read only)\n\n"
"\tSW SatR fields\n"
"\t--------------\n"
"ddr_buswidth               - DDR bus width\n"
"mac1                       - MAC1 modes\n"
);
#endif /*defined(CONFIG_CMD_SAR)*/

