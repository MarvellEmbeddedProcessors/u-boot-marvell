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
#include "boardEnv/mvBoardEnvLib.h"

#define FREQ_MODES_NUM			9
#define	MAX_FABRIC_MODES_NUM	20
#define	MAX_CPU_MODES_NUM		7
#define MAX_BOOTSRC_OPT			3

typedef struct {
	char *name;
	MV_U8 cpuFreq;
	MV_U8 fabricFreq;
} MV_FREQ_MODE;

MV_FREQ_MODE freq_modes[FREQ_MODES_NUM] = {
/*	Freq Conf					CPU Freq		Fabric Freq			*/
/*								0x4c/[3:0]		0x4c[4],0x4d/[3:0]	*/
	{"667  / 333 / 333 Mhz",		0x2,			0x1			},		/* 1:2:2:2 - 1:1 mode */	
	{"400  / 400 / 400 Mhz",		0x0,			0x1B		},		/* 1:1:1:1 - 1:1 mode */	
	{"800  / 400 / 400 Mhz",		0x3,			0x1			},		/* 1:2:2:2 - 1:1 mode */
	{"1200 / 400 / 400 Mhz",		0x6,			0x7			},		/* 2:3:6:6 - 1:1 mode */
/*	{"1000 / 250 / 500 Mhz",		0x4,			0x5			},	*/	/* 1:2:4:2 - 2:1 mode */ 
	{"800  / 266 / 533 Mhz",		0x3,			0x14		},		/* 2:3:6:3 - 2:1 mode */
	{"1066 / 266 / 533 Mhz",		0x5,			0x5			},		/* 1:2:4:2 - 2:1 mode */
	{"1200 / 300 / 600 Mhz",		0x6,			0x5			},		/* 1:2:4:2 - 2:1 mode */
	{"667  / 333 / 667 Mhz",		0x2,			0x13		},		/* 1:1:2:1 - 2:1 mode */
	{"1000 / 333 / 667 Mhz",		0x4,			0x14		},		/* 2:3:6:3 - 2:1 mode */
};

static int do_sar_list(int argc, char *const argv[])
{
	const char *cmd;
	int i;

	if (argc < 1)
		goto usage;
	cmd = argv[0];

	if (strcmp(cmd, "cpufreq") == 0) {

		printf("Determines the frequency of CPU:\n");
		printf("Don't use this commnad. Use 'freq' instead. \n");
		printf("\t0 = 400Mhz\n");
		printf("\t1 = 533Mhz\n");
		printf("\t2 = 667Mhz\n");
		printf("\t3 = 800Mhz\n");
		printf("\t4 = 1000Mhz\n");
		printf("\t5 = 1067Mhz\n");
		printf("\t6 = 1200Mhz\n");
		printf("\t7 = 1333Mhz\n");

	} else if (strcmp(cmd, "fabfreq") == 0) {

		printf("Determines the ratios between PCLK0, XPCLK, HCLK and DRAM clock.\n");
		printf("For full deatails about the various options please refer to the clocking section in the HW spec.\n");
		printf("Entered number is decimal.\n");
		printf("Don't use this commnad. Use 'freq' instead. \n");

	} else if (strcmp(cmd, "bootsrc") == 0) {

		printf("Determines the Boot source device:\n");
		printf("\t0 = SPI - 32bit\n");
		printf("\t1 = NAND - 8bit, 4bit ECC, 4KB page size\n");
		printf("\t2 = NOR - ALE - 0, Muxed, 16bit \n");
		printf("\t3 'opt' = user manual option (opt) \n");

	} else if (strcmp(cmd, "freq") == 0) {

		printf("val	CPU  / HCLK / DDR		CPU Freq	Fabric Freq\n");
		for (i=0; i<FREQ_MODES_NUM; i++)
			printf("%d\t%s\t\t0x%x\t\t0x%x\n", i, freq_modes[i].name, freq_modes[i].cpuFreq, freq_modes[i].fabricFreq);
	
	} else if (strcmp(cmd, "pex") == 0) {
		
		printf("Determines the pex capability mode:\n");
		printf("\t1 = GEN 1\n");
		printf("\t2 = GEN 2\n");
	}

	return 0;
usage:
	printf("Usage: sar list [options] (see help) \n");
	return 1;
}

static int do_sar_read(int argc, char *const argv[])
{
	int i = 0;
	const char *cmd;
	MV_U8 cpuFreq;
	MV_U8 fabricFreq;
	int flag = 0;

	if (argc < 1)
		goto usage;
	cmd = argv[0];

	if (strcmp(cmd, "cpufreq") == 0) {

		printf("cpufreq = %d\n", mvBoardCpuFreqGet());

	} else if (strcmp(cmd, "fabfreq") == 0) {

		printf("fabfreq = %d\n", mvBoardFabFreqGet());

	} else if (strcmp(cmd, "bootsrc") == 0) {

		printf("bootsrc = %d\n", mvBoardBootDevGet());

	} else if (strcmp(cmd, "freq") == 0) {
		cpuFreq = mvBoardCpuFreqGet();
		fabricFreq = mvBoardFabFreqGet();
		for (i=0; i<FREQ_MODES_NUM; i++) {
			if ((freq_modes[i].cpuFreq == cpuFreq) && (freq_modes[i].fabricFreq == fabricFreq)) {
				printf("Mode is: %s\n", freq_modes[i].name);
				flag = 1;
			}
		}
		if (flag == 0)
			printf("Current freq mode is invalid!\n");

	} else if (strcmp(cmd, "pex") == 0) {

		printf("pex = %d\n", mvBoardPexCapabilityGet());

	}
	return 0;
usage:
	printf("Usage: SatR read [options] (see help) \n");
	return 1;
}

static int do_sar_write(int argc, char *const argv[])
{
	const char *cmd;

	if (argc < 1)
		goto usage;

	cmd = argv[0];

	if (strcmp(cmd, "cpufreq") == 0) {

		MV_U8 freq = simple_strtoul(argv[1], NULL, 10);
		if (freq > MAX_CPU_MODES_NUM) {
			printf("invalid cpufreq %d\n", freq);
			goto write_fail;
		}
		if (mvBoardCpuFreqSet(freq) != MV_OK)
			goto write_fail;

	} else if (strcmp(cmd, "fabfreq") == 0) {

		MV_U8 freq = simple_strtoul(argv[1], NULL, 10);
		if (freq > MAX_FABRIC_MODES_NUM) {
			printf("invalid fabfreq %d\n", freq);
			goto write_fail;
		}
		if (mvBoardFabFreqSet(freq) != MV_OK)
			goto write_fail;
	
	} else if (strcmp(cmd, "freq") == 0) {
		MV_U8 mode = simple_strtoul(argv[1], NULL, 10);

		if ((mode<0) || (mode >=FREQ_MODES_NUM)) {
			printf("invalid mode %d\n", mode);
			goto write_fail;
		}
		if (mvBoardCpuFreqSet(freq_modes[mode].cpuFreq) != MV_OK)
			goto write_fail;
		if (mvBoardFabFreqSet(freq_modes[mode].fabricFreq) != MV_OK)
			goto write_fail;
	} else if (strcmp(cmd, "bootsrc") == 0) {

		MV_U8 boot = simple_strtoul(argv[1], NULL, 10);
		if (boot > MAX_BOOTSRC_OPT) {
			printf("invalid bootsrc %d\n", boot);
			goto write_fail;
		}
		MV_U8 bootCfg;
		switch (boot) {
		case 0:		/* SPI - 32bit */
			bootCfg = 0x14;
			break;
		case 1:		/* NAND - 8bit, 4bit ECC, 4KB page size */
			bootCfg = 0xB;
			break;
		case 2:		/* NOR - ALE - 0, Muxed, 16bit */
			bootCfg = 0x31;
			break;
		case 3:		/* User Option */
			bootCfg = simple_strtoul(argv[2], NULL, 10);
			break;
		}
		if (mvBoardBootDevSet(bootCfg) != MV_OK)
			goto write_fail;

	} else if (strcmp(cmd, "default") == 0) {
		if (mvBoardCpuFreqSet(0x6) != MV_OK)
			goto write_fail;
		if (mvBoardFabFreqSet(0x5) != MV_OK)
			goto write_fail;
		if (mvBoardBootDevSet(0x14) != MV_OK)	/* SPI - 32bit */
			goto write_fail;
		if (mvBoardPexCapabilitySet(0x1) != MV_OK)	/* Pex - Gen 1.0 */
			goto write_fail;

	} else if (strcmp(cmd, "pex") == 0) {

		MV_U8 pexMode = simple_strtoul(argv[1], NULL, 10);
		if (pexMode < 1 || pexMode > 2) {
			printf("invalid pexMode %d\n", pexMode);
			goto write_fail;
		}
		if (mvBoardPexCapabilitySet(pexMode) != MV_OK)
			goto write_fail;
	}

	return 0;

write_fail:
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

		if (do_sar_write(argc - 2, argv + 2) == 0)
			do_sar_read(argc - 2, argv + 2);
		return 0;

	} else if (strcmp(cmd, "read") == 0)
		return do_sar_read(argc - 2, argv + 2);

usage:
	cmd_usage(cmdtp);
	return 1;
}

U_BOOT_CMD(SatR, 6, 1, do_sar,
	"Sample At Reset sub-system\n",

	"list cpufreq  - prints the S@R modes list\n"
	"SatR list fabfreq	- prints the S@R modes list\n"
	"SatR list freq		- prints the S@R modes list\n"
	"SatR list bootsrc	- prints the S@R modes list\n"
	"SatR list pex		- prints the S@R modes list\n"

	"SatR read cpufreq	- read and print the CPU frequency S@R value\n"
	"SatR read fabfreq	- read and print the Fabric frequency S@R value\n"
	"SatR read freq		- read and print the mode of cpu/ddr freq S@R value\n"
	"SatR read bootsrc	- read and print the Boot source S@R value\n"
	"SatR read pex		- read and print the pex capability mode from S@R value\n"
			
	"SatR write cpufreq <val>	- write the S@R with CPU frequency value\n"
	"SatR write fabfreq <val>	- write the S@R with Fabric frequency value\n"
	"SatR write freq <val>		- write the S@R with the cpu/ddr freq mode\n"
	"SatR write bootsrc <val> <optManVal>	- write the S@R with Boot source value\n"
	"SatR write pex <val>		- write the S@R with the pex capability mode\n"
	"SatR write default	- write the S@R with default board values \n"
);
#endif /*defined(CONFIG_CMD_SAR)*/

