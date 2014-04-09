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

extern int do_sar(cmd_tbl_t * cmdtb, int flag, int argc, char * const argv[]);

typedef struct _boardSatrDefault {
	MV_SATR_TYPE_ID satrId;
	MV_U32 defauleValueForBoard[MV_MAX_BOARD_ID];
} MV_BOARD_SATR_DEFAULT;

MV_BOARD_SATR_DEFAULT boardSatrDefault[] = {
/* 	defauleValueForBoard[] = { RD_6650,	DB_6650,	RD_6660,	DB_6660 }*/
{ MV_SATR_WRITE_CPU_FREQ,	  {6,		21,		6,		21} 	  },
{ MV_SATR_WRITE_CORE_CLK_SELECT,  {_200MHz,	_200MHz,	_200MHz,	_200MHz}  },
{ MV_SATR_WRITE_CPU1_ENABLE,	  {MV_FALSE,	MV_FALSE,	MV_TRUE,	MV_TRUE} },
{ MV_SATR_WRITE_SSCG_DISABLE,	  {MV_FALSE,	MV_FALSE,	MV_FALSE,	MV_FALSE} },
{ MV_SATR_WRITE_PEX0_CLOCK,	  {MV_FALSE,	MV_TRUE,	MV_FALSE,	MV_TRUE} },
{ MV_SATR_WRITE_PEX1_CLOCK,	  {MV_FALSE,	MV_TRUE,	MV_FALSE,	MV_TRUE} },
};

static int do_sar_default(void)
{
	MV_U32 temp, defaultValue, boardId = mvBoardIdGet();

	if (boardId != DB_6660_ID && boardId != DB_6650_ID) {
		printf("\nError: S@R fields are readable only for current board\n");
		return 1;
	}

	defaultValue = boardSatrDefault[MV_SATR_CPU_DDR_L2_FREQ].defauleValueForBoard[boardId];
	if (mvCtrlSatRRead(MV_SATR_CPU_DDR_L2_FREQ, &temp) == MV_OK )
			mvCtrlSatRWrite(MV_SATR_CPU_DDR_L2_FREQ, defaultValue, MV_TRUE);

	defaultValue = boardSatrDefault[MV_SATR_CORE_CLK_SELECT].defauleValueForBoard[boardId];
	if (defaultValue == _200MHz)
		defaultValue = 0x1;
	else
		defaultValue = 0x0;
	if (mvCtrlSatRRead(MV_SATR_CORE_CLK_SELECT, &temp) == MV_OK )
			mvCtrlSatRWrite(MV_SATR_CORE_CLK_SELECT, defaultValue, MV_TRUE);

	defaultValue = boardSatrDefault[MV_SATR_CPU1_ENABLE].defauleValueForBoard[boardId];
	if (defaultValue == MV_TRUE)
		defaultValue = 0x1;
	else
		defaultValue = 0x0;
	if (mvCtrlSatRRead(MV_SATR_CPU1_ENABLE, &temp) == MV_OK )
			mvCtrlSatRWrite(MV_SATR_CPU1_ENABLE, defaultValue, MV_TRUE);

	defaultValue = boardSatrDefault[MV_SATR_SSCG_DISABLE].defauleValueForBoard[boardId];
	if (defaultValue == MV_TRUE)
		defaultValue = 0x1;
	else
		defaultValue = 0x0;
	if (mvCtrlSatRRead(MV_SATR_SSCG_DISABLE, &temp) == MV_OK )
			mvCtrlSatRWrite(MV_SATR_SSCG_DISABLE, defaultValue, MV_TRUE);

	defaultValue = boardSatrDefault[MV_SATR_PEX0_CLOCK].defauleValueForBoard[boardId];
	if (defaultValue == MV_TRUE)
		defaultValue = 0x1;
	else
		defaultValue = 0x0;
	if (mvCtrlSatRRead(MV_SATR_PEX0_CLOCK, &temp) == MV_OK )
			mvCtrlSatRWrite(MV_SATR_PEX0_CLOCK, defaultValue, MV_TRUE);

	defaultValue = boardSatrDefault[MV_SATR_PEX1_CLOCK].defauleValueForBoard[boardId];
	if (defaultValue == MV_TRUE)
		defaultValue = 0x1;
	else
		defaultValue = 0x0;
	if (mvCtrlSatRRead(MV_SATR_PEX1_CLOCK, &temp) == MV_OK )
			mvCtrlSatRWrite(MV_SATR_PEX1_CLOCK, defaultValue, MV_TRUE);

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
		for (i=0; i < maxFreqModes; i++) {
			printf("|  %2d |      %4d      |      %d       |      %d      | \n",
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
	} else if (strcmp(cmd, "pcimode") == 0) {
		printf("Determines the PEX0/1 clock input/output mode:\n");
		printf("\t0x0 = PEX clock input enabled\n");
		printf("\t0x1 = PEX clock output enabled\n");
	}
	else goto usage;
	return 0;
usage:
	printf("Usage: sar list [options] (see help) \n");
	return 1;
}

static MV_STATUS GetAndVerifySatr(MV_SATR_TYPE_ID satrField, MV_U32* result)
{
	MV_U32 temp;

	if (mvCtrlSatRRead(satrField, &temp) == MV_ERROR) {
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
			printf("\nsscg = %d  ==> %s\n",temp, (temp == 0) ? "Enabled" : "Disabled");
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
	} else if (strcmp(cmd, "pcimode1") == 0) {
		if (GetAndVerifySatr(MV_SATR_PEX1_CLOCK, &temp) == MV_OK)
		printf("\npcimode1 = %d  ==> %s mode\n", temp, (temp == 0) ? "input" : "output");
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
		if (mvCtrlSatRRead(MV_SATR_CORE_CLK_SELECT, &temp) == MV_OK)
			printf("coreclock \t= %3d  ==>   %sMhz\n",temp, (temp == 0x0) ? "166" : "200");
		if (mvCtrlSatRRead(MV_SATR_CPU1_ENABLE, &temp) == MV_OK)
		printf("cpusnum \t= %3d  ==>   %s CPU \n", temp, (temp == 0) ? "Single" : "Dual");
		if (mvCtrlSatRRead(MV_SATR_SSCG_DISABLE, &temp) == MV_OK)
			printf("sscg \t\t= %3d  ==>   %s\n",temp, (temp == 0) ? "Enabled" : "Disabled");
		if (mvCtrlSatRRead(MV_SATR_I2C0_SERIAL_ROM, &temp) == MV_OK)
			printf("i2c0 \t\t= %3d  ==>   %s\n",temp, (temp == 0) ? "Disabled" : "Enabled");
		if (mvCtrlSatRRead(MV_SATR_EXTERNAL_CORE_RESET, &temp) == MV_OK)
			printf("cpureset \t= %3d  ==>   %s\n",temp, (temp == 0) ? "Disabled" : "Enabled");
		if (mvCtrlSatRRead(MV_SATR_EXTERNAL_CPU_RESET, &temp) == MV_OK)
			printf("corereset \t= %3d  ==>   %s\n",temp, (temp == 0) ? "Disabled" : "Enabled");
		if (mvCtrlSatRRead(MV_SATR_BOOT_DEVICE, &temp) == MV_OK)
			printf("bootsrc \t= %3d  ==>   %s\n", temp, bootSrcArr[mvBoardBootDeviceGet()]);
		if (mvCtrlSatRRead(MV_SATR_CPU_PLL_XTAL_BYPASS, &temp) == MV_OK)
			printf("cpubypass \t= %3d  ==>   %s Bypass\n",temp, (temp == 0) ? "PLL" : "XTAL");
		if (mvCtrlSatRRead(MV_SATR_CPU0_ENDIANESS, &temp) == MV_OK)
			printf("cpuendi \t= %3d  ==>   %s Endianess\n", temp, (temp == 0) ? "Little" : "Big");
		if (mvCtrlSatRRead(MV_SATR_CPU0_NMFI, &temp) == MV_OK)
			printf("cpunmfi \t= %3d  ==>   FIQ mask %s \n", temp, (temp == 0) ? "Enabled" : "Disabled");
		if (mvCtrlSatRRead(MV_SATR_CPU0_THUMB, &temp) == MV_OK)
			printf("cputhumb \t= %3d  ==>   %s mode \n", temp, (temp == 0) ? "ARM" : "Thumb");
		if (mvCtrlSatRRead(MV_SATR_PEX0_CLOCK, &temp) == MV_OK)
			printf("pcimode0 \t= %3d  ==>   PEX0 clock %s enable\n", temp, (temp == 0) ? "Input" : "Output");
		if (mvCtrlSatRRead(MV_SATR_PEX1_CLOCK, &temp) == MV_OK)
			printf("pcimode1 \t= %3d  ==>   PEX1 clock %s enable\n", temp, (temp == 0) ? "Input" : "Output");
		if (mvCtrlSatRRead(MV_SATR_REF_CLOCK_ENABLE, &temp) == MV_OK)
			printf("refclk \t\t= %3d  ==>   %s\n",temp, (temp == 0) ? "Disabled" : "Enabled");
		if (mvCtrlSatRRead(MV_SATR_TESTER_OPTIONS, &temp) == MV_OK)
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
	MV_U32 temp, boardId = mvBoardIdGet();
	MV_BOOL flag = MV_ERROR;
	MV_U8 writeVal = simple_strtoul(argv[1], NULL, 10);
	MV_FREQ_MODE cpuFreqMode;

	if (boardId != DB_6660_ID && boardId != DB_6650_ID) {
		printf("\nError: S@R fields are readable only for current board\n");
		return 1;
	}

	if (argc < 2)
		goto usage;

	if (strcmp(cmd, "freq") == 0) {
		if (mvCtrlFreqModeGet(writeVal, &cpuFreqMode) != MV_OK)
			goto input_error;
		else if (GetAndVerifySatr(MV_SATR_CPU_DDR_L2_FREQ, &temp) == MV_OK )
			flag = mvCtrlSatRWrite(MV_SATR_CPU_DDR_L2_FREQ, writeVal, MV_FALSE);
	}
	else if (strcmp(cmd, "coreclock") == 0) {
		if (writeVal != 0 && writeVal != 1)
			goto input_error;
		else if (GetAndVerifySatr(MV_SATR_CORE_CLK_SELECT, &temp) == MV_OK )
			flag = mvCtrlSatRWrite(MV_SATR_CORE_CLK_SELECT, writeVal, MV_FALSE);
	}
	else if (strcmp(cmd, "cpusnum") == 0) {
		if (writeVal != 0 && writeVal != 1)
			goto input_error;
		else if (GetAndVerifySatr(MV_SATR_CPU1_ENABLE, &temp) == MV_OK )
			flag = mvCtrlSatRWrite(MV_SATR_CPU1_ENABLE, writeVal, MV_FALSE);
	}
	else if (strcmp(cmd, "sscg") == 0) {
		if (writeVal != 0 && writeVal != 1)
			goto input_error;
		else if (GetAndVerifySatr(MV_SATR_SSCG_DISABLE, &temp) == MV_OK )
			flag = mvCtrlSatRWrite(MV_SATR_SSCG_DISABLE, writeVal, MV_FALSE);
	} else if (strcmp(cmd, "pcimode0") == 0) {
		if (writeVal != 0 && writeVal != 1)
			goto input_error;
		else if (GetAndVerifySatr(MV_SATR_PEX0_CLOCK, &temp) == MV_OK)
			flag = mvCtrlSatRWrite(MV_SATR_PEX0_CLOCK, writeVal, MV_FALSE);
	} else if (strcmp(cmd, "pcimode1") == 0) {
		if (writeVal != 0 && writeVal != 1)
			goto input_error;
		else if (GetAndVerifySatr(MV_SATR_PEX1_CLOCK, &temp) == MV_OK)
			flag = mvCtrlSatRWrite(MV_SATR_PEX1_CLOCK, writeVal, MV_FALSE);
	}
	else
		goto usage;

	if (flag == MV_ERROR) {
		printf("Write S@R failed!\n");
		return 1;
	}

	return 0;

input_error:
	printf("\nError: value is not valid for \"%s\" (%d)\n\n", cmd , writeVal);
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
	"Sample At Reset sub-system\n",

	"list freq		- prints the S@R modes list\n"
	"SatR list coreclock	- prints the S@R modes list\n"
	"SatR list cpusnum	- prints the S@R modes list\n"
	"SatR list sscg		- prints the S@R modes list\n"
	"SatR list pcimode	- prints the S@R modes list\n\n"

	"SatR read		- read and print all active S@R values\n"
	"SatR read freq		- read and print the CPU frequency S@R value\n"
	"SatR read coreclock	- read and print the Core Clock frequency S@R value\n"
	"SatR read cpusnum	- read and print the number of CPU cores S@R value\n"
	"SatR read sscg		- read and print the SSCG S@R value (reading the I2C device)\n"	 
	"SatR read i2c0		- read and print the i2c0 S@R value (reading the I2C device)\n"
	"SatR read cpureset	- read and print the CPU reset mode S@R value (reading the I2C device)\n"
	"SatR read corereset	- read and print the Core reset mode S@R value (reading the I2C device)\n"
	"SatR read bootsrc	- read and print the boot source from S@R value (reading the I2C device)\n"
	"SatR read cpubypass	- read and print the CPU Bypass mode from S@R value (reading the I2C device)\n"
	"SatR read cpuendi	- read and print the CPU Endianess (Little/Big) S@R value (reading the I2C device)\n"
	"SatR read cpunmfi	- read and print the CPU FIQ mask mode (Little/Big) S@R value (reading the I2C device)\n"
	"SatR read cputhumb	- read and print the CPU Thumb mode (ARM/ Thumb) S@R value (reading the I2C device)\n"
	"SatR read pcimode0	- read and print the pci0 clock mode (input/output) from S@R value (reading the I2C device)\n"
	"SatR read pcimode1	- read and print the pci1 clock mode (input/output) from S@R value (reading the I2C device)\n"
	"SatR read refclk	- read and print the ref clock mode S@R value \n"
	"SatR read tester	- read and print the tester mode S@R value\n\n"

	"SatR write freq <val>		- write the S@R with CPU frequency value\n"
	"SatR write coreclock <val>	- write the S@R with Core Clock frequency value\n"
	"SatR write cpusnum <val>	- write the S@R with number of CPU cores value\n"
	"SatR write sscg <val>		- write the S@R with sscg mode value\n"
	"SatR write pcimode0 <val>	- write the S@R with pci0 clock mode (0:input/1:output)\n"
	"SatR write pcimode1 <val>	- write the S@R with pci1 clock mode (0:input/1:output)\n"

	"SatR write default		- restore writeable S@R values to their default value\n"
);
#endif /*defined(CONFIG_CMD_SAR)*/

