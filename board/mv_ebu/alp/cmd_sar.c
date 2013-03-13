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



static int do_sar_list(int argc, char *const argv[])
{
	const char *cmd;
	int i;

	if (argc < 1)
		goto usage;
	cmd = argv[0];
/* 1. Bios Device (MUX with internal E²PROM)  change by uBoot command satr and reset.
 * 	Following S@R will be changed by this option: */
	if (strcmp(cmd, "cpufreq") == 0) {
		MV_FREQ_MODE pFreqModes[] = MV_SAR_FREQ_MODES;
		printf("Determines the frequency of CPU/DDR/L2:\n");
		printf("\n\n val | CPU Freq (Mhz) | DDR Freq (Mhz) | L2 Freq (Mhz) |\n");
		for (i=0; i < FREQ_MODES_NUM; i++) {
			printf(" %d | %s | %s | %s | \n", i,
				   pFreqModes[i].cpuFreq,
				   pFreqModes[i].ddrFreq,
				   pFreqModes[i].l2Freq);
		}
	} else if (strcmp(cmd, "coreclock") == 0) {
		printf("Determines the frequency of Core Clock:\n");
		printf("\t0x0 = 166Mhz\n");
		printf("\t0x1 = 200Mhz\n");
#if defined (DB_6660_ID) || defined (RD_6660_ID)	/* omriii : remove defines and detect according to runtime boardID */
	} else if (strcmp(cmd, "cpusnum") == 0) {

		printf("Determines the number of CPU cores:\n");
		printf("\t0x0 = Single CPU\n");
		printf("\t0x1 = Dual CPU\n");
		printf("\t0x2 = Reserved\n");
#endif

	} else if (strcmp(cmd, "sscg") == 0) {
		printf("Determines the SSCG  mode:\n");
		printf("\t0x0 = SSCG Enabled\n");
		printf("\t0x1 = SSCG Disabled\n");
	}
	return 0;
usage:
	printf("Usage: sar list [options] (see help) \n");
	return 1;
}

static int do_sar_read(int argc, char *const argv[])
{
	const char *cmd;
	MV_BOOL dump_all=MV_FALSE;
	MV_U8 temp;
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

	if ((strcmp(cmd, "cpufreq") == 0) && (MV_ERROR != mvCtrlCpuDdrL2FreqGet(&freqMode)))
	{
		printf("\n\n val | CPU Freq (Mhz) | DDR Freq (Mhz) | L2 Freq (Mhz) |\n");
			printf(" %d | %s | %s | %s | \n", 
				freqMode.id,
				freqMode.cpuFreq,
 				freqMode.ddrFreq,
				freqMode.l2Freq);
	}

	else if ((strcmp(cmd, "coreclock") == 0) && (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_CORE_CLK_SELECT))))
		printf("coreclock = %d \n", temp);

	else if ((strcmp(cmd, "cpusnum") == 0) && (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_CPU1_ENABLE))))
		printf("cpusnum = %d \n", temp);

	else if ((strcmp(cmd, "sscg") == 0) && (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_SSCG_DISABLE))))
		printf("sscg = %d ==> %s\n",temp,  ( (temp == 0) ? "Disabled" : "Enabled") );

	else if ((strcmp(cmd, "i2c0") == 0) && (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_I2C0_SERIAL_ROM))))
		printf("i2c0 = %d ==> %s\n",temp,  ( (temp == 0) ? "Disabled" : "Enabled") );

	else if ((strcmp(cmd, "cpureset") == 0) &&(MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_EXTERNAL_CORE_RESET))))
		printf("cpureset = %d ==> %s\n",temp,  ( (temp == 0) ? "Disabled" : "Enabled") );

	else if ((strcmp(cmd, "corereset") == 0) &&(MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_EXTERNAL_CPU_RESET))))
			printf("corereset = %d ==> %s\n",temp,  ( (temp == 0) ? "Disabled" : "Enabled") );

	else if ((strcmp(cmd, "bootsrc") == 0) && (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_BOOT_DEVICE))))
			printf("bootsrc = %d ==> %s\n", temp, bootSrcArr[mvBoardBootDeviceGet(temp)]);

	else if ((strcmp(cmd, "cpubypass") == 0) && (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_CPU_PLL_XTAL_BYPASS))))
		printf("sscg = %d ==> %s Bypass\n",temp,  ( (temp == 0) ? "PLL" : "XTAL") );

	else if ((strcmp(cmd, "cpuendi") == 0) && (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_CPU0_ENDIANESS))))
			printf("cpuendi = %d ==> %s Endianess\n", temp, ( (temp == 0) ? "Little" : "Big"));

	else if ((strcmp(cmd, "cpunmfi") == 0) && (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_CPU0_NMFI))))
			printf("cpunmfi = %d ==> FIQ mask %s \n", temp, ( (temp == 0) ? "Enabled" : "Disabled"));

	else if ((strcmp(cmd, "cputhumb") == 0) && (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_CPU0_THUMB))))
			printf("cputhumb = %d ==> %s mode \n", temp, ( (temp == 0) ? "ARM" : "Thumb"));

	else if ((strcmp(cmd, "pcimode0") == 0) && (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_PEX0_CLOCK))))
		printf("pcimode0 = %d ==> %s mode\n",temp,  ( (temp == 0) ? "Root Complex" : "Clock") );

	else if ((strcmp(cmd, "refclk") == 0) &&(MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_REF_CLOCK_ENABLE))))
			printf("refclk = %d ==> %s\n",temp,  ( (temp == 0) ? "Disabled" : "Enabled") );

	else if ((strcmp(cmd, "tester") == 0) &&(MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_TESTER_OPTIONS))))
			printf("tester = %d \n",temp);

	else if (dump_all==MV_TRUE)
	{
		printf ("\t S@R configuration:\n\t  --------------------\n");
		if  (MV_ERROR != mvCtrlCpuDdrL2FreqGet(&freqMode))
		{
			printf("\n\n val | CPU Freq (Mhz) | DDR Freq (Mhz) | L2 Freq (Mhz) |\n");
			printf(" %d | %s | %s | %s | \n", 
				freqMode.id,
				freqMode.cpuFreq,
 				freqMode.ddrFreq,
				freqMode.l2Freq);
		}
	
		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_CORE_CLK_SELECT)))
			printf("coreclock \t= %d \n", temp);

		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_CPU1_ENABLE)))
			printf("cpusnum \t\t= %d \n", temp);

		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_SSCG_DISABLE)))
			printf("sscg \t\t= %d\t\t==> %s\n",temp,  ( (temp == 0) ? "Disabled" : "Enabled") );

		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_I2C0_SERIAL_ROM)))
			printf("i2c0 \t\t= %d\t\t==> %s\n",temp,  ( (temp == 0) ? "Disabled" : "Enabled") );

		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_EXTERNAL_CORE_RESET)))
			printf("cpureset \t\t= %d\t\t==> %s\n",temp,  ( (temp == 0) ? "Disabled" : "Enabled") );

		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_EXTERNAL_CPU_RESET)))
			printf("corereset \t= %d\t\t==> %s\n",temp,  ( (temp == 0) ? "Disabled" : "Enabled") );

		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_BOOT_DEVICE)))
			printf("bootsrc = %d ==> %s\n", temp, bootSrcArr[mvBoardBootDeviceGet(temp)]);

		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_CPU_PLL_XTAL_BYPASS)))
			printf("sscg \t\t= %d\t\t==> %s Bypass\n",temp,  ( (temp == 0) ? "PLL" : "XTAL") );

		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_CPU0_ENDIANESS)))
			printf("cpuendi = %d ==> %s Endianess\n", temp, ( (temp == 0) ? "Little" : "Big"));

		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_CPU0_NMFI)))
			printf("cpunmfi = %d ==> FIQ mask %s \n", temp, ( (temp == 0) ? "Enabled" : "Disabled"));

		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_CPU0_THUMB)))
			printf("cputhumb = %d ==> %s mode \n", temp, ( (temp == 0) ? "ARM" : "Thumb"));

		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_PEX0_CLOCK)))
			printf("pcimode0 \t\t= %d\t\t==> PEX0 clock %s enable\n",temp,  ( (temp == 0) ? "Input" : "Output") );

		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_PEX1_CLOCK)))
			printf("pcimode1 \t\t= %d\t\t==> PEX1 clock %s enable\n",temp,  ( (temp == 0) ? "Input" : "Output") );

		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_REF_CLOCK_ENABLE)))
			printf("refclk \t\t= %d\t\t==> %s\n",temp,  ( (temp == 0) ? "Disabled" : "Enabled") );

		if (MV_ERROR != (temp=mvCtrlSatRRead(MV_SATR_TESTER_OPTIONS)))
			printf("tester \t\t= %d \n",temp);
	}
	else goto usage;
	return 0;
usage:
	printf("Usage: SatR read [options] (see help) \n");
	return 1;
}

static int do_sar_write(int argc, char *const argv[])
{
	const char *cmd;
	MV_BOOL flag;

	if (argc < 2)
		goto usage;

	cmd = argv[0];
	MV_U8 tempVal = simple_strtoul(argv[1], NULL, 10);

	if ((strcmp(cmd, "cpufreq") == 0) && (MV_ERROR != mvCtrlSatRRead(MV_SATR_CPU_FREQ)))
		flag=mvCtrlSatRWrite(MV_SATR_WRITE_CPU_FREQ,MV_SATR_CPU_FREQ, tempVal);

	else if ((strcmp(cmd, "coreclock") == 0) && (MV_ERROR != mvCtrlSatRRead(MV_SATR_CORE_CLK_SELECT)))
		flag=mvCtrlSatRWrite(MV_SATR_WRITE_CORE_CLK_SELECT,MV_SATR_CORE_CLK_SELECT, tempVal);

	else if ((strcmp(cmd, "cpusnum") == 0) && (MV_ERROR != mvCtrlSatRRead(MV_SATR_CPU1_ENABLE)))
		flag=mvCtrlSatRWrite(MV_SATR_WRITE_CPU1_ENABLE,MV_SATR_CPU1_ENABLE, tempVal);

	else if ((strcmp(cmd, "sscg") == 0) && (MV_ERROR != mvCtrlSatRRead(MV_SATR_SSCG_DISABLE)))
		flag=mvCtrlSatRWrite(MV_SATR_WRITE_SSCG_DISABLE,MV_SATR_SSCG_DISABLE, tempVal);

/* the first 4 S@R fields are writeable using S@R commands - rest  values are edited using Jumpers/DIP switch/DPR (resistors) */
	else goto usage;

	return 0;

if (MV_ERROR==flag)
	printf("Write S@R failed!\n");
		return 1;

usage:
	printf("Usage: SatR write [options] (see help) \n");
	return 1;
}

int do_sar(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	const char *cmd;

	/* need at least two arguments */
	if (argc < 2)
		goto usage;

	cmd = argv[1];

	if (strcmp(cmd, "list") == 0)
		return do_sar_list(argc - 2, argv + 2);
	else if (strcmp(cmd, "write") == 0) {
		if (do_sar_write(argc - 2, argv + 2) == 0) {
			do_sar_read(argc - 2, argv + 2);
		}
		return 0;

	} else if (strcmp(cmd, "read") == 0)
		return do_sar_read(argc - 2, argv + 2);

usage:
	cmd_usage(cmdtp);
	return 1;
}

U_BOOT_CMD(SatR, 6, 1, do_sar,
	"Sample At Reset sub-system\n",

	"SatR list cpufreq	- prints the S@R modes list\n"
	"SatR list coreclock	- prints the S@R modes list\n"
	"SatR list cpusnum	- prints the S@R modes list\n"
	"SatR list sscg		- prints the S@R modes list\n\n"

	"SatR read cpufreq	- read and print the CPU frequency S@R value\n"
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
	"SatR read pcimode1	- read and print the pci0 clock mode (input/output) from S@R value (reading the I2C device)\n"
	"SatR read refclk      	- read and print the ref clock mode S@R value \n"
	"SatR read tester	- read and print the tester mode S@R value\n\n"

	"SatR write cpufreq <val>	- write the S@R with CPU frequency value\n"
	"SatR write coreclock <val>	- write the S@R with Core Clock frequency value\n"
	"SatR write cpusnum <val>	- write the S@R with number of CPU cores value\n"
	"SatR write sscg <val>		- write the S@R with sscg mode value\n"	/* omriii : explain the field */
);
#endif /*defined(CONFIG_CMD_SAR)*/

