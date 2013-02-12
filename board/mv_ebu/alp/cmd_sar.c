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

#ifdef MV88F78X60_Z1
#define FREQ_MODES_NUM		7
#else
#define FREQ_MODES_NUM		20
#endif
typedef struct {
	char *name;
	MV_U8 cpuFreq;
#ifdef MV88F78X60_Z1
	MV_U8 cpuFreqMode;
#endif
	MV_U8 fabricFreq;
#ifdef MV88F78X60_Z1
	MV_U8 fabricFreqMode;
#else
	MV_U8 AltfabricFreq;
#endif
} MV_FREQ_MODE;
#ifdef MV88F78X60_Z1
MV_FREQ_MODE freq_modes[FREQ_MODES_NUM] = {
/*	Freq Conf		CPU Freq	CPUFreqMode	Fabric Freq		FabricFreqMode	*/
/*				0x4d/[4:2]	0x4e[0]		0x4e/[4:1]		0x4f[0]		*/
	{"800 / 400 Mhz",	0x2,		0x1,		0x1,			0x0},
	{"1066 / 533 Mhz",	0x1,		0x0,		0x1,			0x0},
	{"1200 / 600 Mhz",	0x2,		0x0,		0x5,			0x0},
	{"1333 / 667 Mhz",	0x3,		0x0,		0x5,			0x0},
	{"667 / 667 Mhz",	0x1,		0x1,		0x3,			0x1},
	{"1200 / 300 Mhz",	0x2,		0x0,		0xC,			0x0},
	{"1333 / 333 Mhz",	0x3,		0x0,		0xC,			0x0},
};
#else
MV_FREQ_MODE freq_modes[FREQ_MODES_NUM] = {
                /*     Freq Conf             				CPU Freq	Fabric Freq	 AltFabric`    */
                /*       									0x4d/[4:2]	0x4e/[4:1]	              */
                {" 667 / 667 / 222 / 444 Mhz             ",		0x9,	0x2,		0x2},	/*  0 */ 
                {" 667 / 667 / 333 / 667 Mhz             ",  	0x9,    0x13,	   	0x13},  /*  1 */ 
				{" 800 / 400 / 200 / 400 Mhz             ",  	0xA,    0x5,        0x5},   /*  2 */ 
                {"1000 / 500 / 250 / 500 Mhz             ", 	0x0,    0x5,        0x5},   /*  3 */ 
                {"1066 / 533 / 266 / 533 Mhz             ", 	0x1,    0x5,        0x5},   /*  4 */ 
                {"1200 / 600 / 300 / 300 Mhz             ", 	0x2,    0xC,        0xc},   /*  5 */ 
                {"1200 / 600 / 200 / 400 Mhz             ",  	0x2,    0x9,        0x9},   /*  6 */ 
                {"1200 / 600 / 300 / 600 Mhz             ",  	0x2,    0x5,        0x5},   /*  7 */ 
                {"1333 / 667 / 266 / 533 Mhz             ",  	0x3,    0xA,        0xA},   /*  8 */ 
                {"1333 / 667 / 333 / 667 Mhz             ",  	0x3,    0x5,        0x5},   /*  9 */ 
				{" 667 / 667 / 222 / 444 Mhz (Fabric DFS)",		0x9,   0x1a,   	    0x2},  /*  10 */ 
                {" 667 / 667 / 333 / 667 Mhz (Fabric DFS)",  	0x9,   0x1a,   	    0x13}, /*  11 */ 
				{" 800 / 400 / 200 / 400 Mhz (Fabric DFS)",  	0xA,   0x1a,   	    0x5},  /*  12 */ 
                {"1000 / 500 / 250 / 500 Mhz (Fabric DFS)", 	0x0,   0x1a,   	    0x5},  /*  13 */ 
                {"1066 / 533 / 266 / 533 Mhz (Fabric DFS)",  	0x1,   0x1a,   	    0x5},  /*  14 */ 
                {"1200 / 600 / 300 / 300 Mhz (Fabric DFS)",  	0x2,   0x1a,   	    0xc},  /*  15 */ 
                {"1200 / 600 / 200 / 400 Mhz (Fabric DFS)",  	0x2,   0x1a,   	    0x9},  /*  16 */ 
                {"1200 / 600 / 300 / 600 Mhz (Fabric DFS)",  	0x2,   0x1a,   	    0x5},  /*  17 */ 
                {"1333 / 667 / 266 / 533 Mhz (Fabric DFS)",  	0x3,   0x1a,   	    0xA},  /*  18 */ 
                {"1333 / 667 / 333 / 667 Mhz (Fabric DFS)",  	0x3,   0x1a,   	    0x5}   /*  19 */ 
};
MV_FREQ_MODE freq_modes_B0[FREQ_MODES_NUM] = {
                /*     Freq Conf             	  CPU Freq		Fabric Freq	 AltFabric    			*/
                /* 		       					  0x4d/[4:2]	0x4e/[4:1]	           			*/
				{" 800 / 400 / 200 / 400 Mhz",    0xA,          0x5,         0x5},          /*   0 */ 
				{"1066 / 533 / 266 / 533 Mhz",    0x1,          0x5,         0x5},          /*   1 */ 
				{"1200 / 600 / 300 / 600 Mhz",    0x2,          0x5,         0x5},          /*   2 */ 
				{"1200 / 600 / 200 / 400 Mhz",    0x2,          0x9,         0x9},          /*   3 */ 
				{"1333 / 667 / 333 / 667 Mhz",    0x3,          0x5,         0x5},          /*   4 */ 
				{"1500 / 750 / 375 / 750 Mhz",    0x4,          0x5,         0x5},          /*   5 */ 
				{"1500 / 750 / 250 / 500 Mhz",    0x4,          0x9,         0x9},          /*   6 */ 
				{"1600 / 800 / 266 / 533 Mhz",    0xb,          0x9,         0x9},          /*   7 */ 
				{"1600 / 800 / 400 / 800 Mhz",    0xb,          0x5,         0x5},          /*   9 */ 
				{NULL,0,0,0},                                                               /*   9 */ 
				{NULL,0,0,0},                                                               /*  10 */ 
				{NULL,0,0,0},                                                               /*  11 */
				{NULL,0,0,0},                                                               /*  12 */
				{NULL,0,0,0},                                                               /*  13 */
				{NULL,0,0,0},                                                               /*  14 */
				{NULL,0,0,0},                                                               /*  15 */
				{NULL,0,0,0},                                                               /*  16 */
				{NULL,0,0,0},                                                               /*  17 */
				{NULL,0,0,0},                                                               /*  18 */
				{NULL,0,0,0}                                                                /*  19 */
};


#endif
static int do_sar_list(int argc, char *const argv[])
{
	const char *cmd;
	int i;

	if (argc < 1)
		goto usage;
	cmd = argv[0];

	if (strcmp(cmd, "cpufreq") == 0) {

		printf("Determines the frequency of CPU:\n");
		printf("\t0x0 = 1000Mhz\n");
		printf("\t0x1 = 1066Mhz\n");
		printf("\t0x2 = 1200Mhz\n");
		printf("\t0x3 = 1333Mhz\n");
		printf("\t0x4 = 1500Mhz\n");
		printf("\t0x9 =  667Mhz\n");
		printf("\t0xA =  800Mhz\n");
		printf("\t0xB = 1600Mhz\n");

	} else if (strcmp(cmd, "fabfreq") == 0) {

		printf("Determines the ratios between PCLK0, XPCLK, HCLK and DRAM clock.\n");
		printf("For full deatails about the various options please refer to the clocking section in the HW spec.\n");

	} else if (strcmp(cmd, "l2size") == 0) {

		printf("Determines the amount of L2 cache:\n");
		printf("\t0x0 = 0.5MB\n");
		printf("\t0x1 = 1MB\n");
		printf("\t0x3 = 2MB\n");

	} else if (strcmp(cmd, "bootsrc") == 0) {

		printf("Determines the Boot source device:\n");
		printf("\t0x0 = NOR\n");
		printf("\t0x1 = NAND\n");
		printf("\t0x2 = UART\n");
		printf("\t0x3 = SPI\n");
		printf("\t0x4 = PCI-E\n");
		printf("\t0x5 = SATA\n");
	/*	printf("\t0x6 = NAND (legacy)\n");
		printf("\t0x7 = Debug prompt\n");*/

	} else if (strcmp(cmd, "bootwidth") == 0) {

		printf("Determines the Boot device width for boot via NOR/NAND flash:\n");
		printf("\t0x0 = 8bit\n");
		printf("\t0x1 = 16bit\n");
		if (DB_784MP_GP_ID != mvBoardIdGet()){
			printf("\t0x2 = 32bit\n");
			printf("\t0x3 = Reserved\n");
		}
		printf("Determines the Boot device width for boot via SPI flash:\n");
		printf("\t0x0 = 32bit\n");
		printf("\t0x1 = 24bit\n");
		printf("\t0x2-0x3 = Reserved\n");
#ifdef MV88F78X60_Z1
	} else if (strcmp(cmd, "cpu0core") == 0) {
#else
	} else if (strcmp(cmd, "cpu0Endianess") == 0) {
#endif

		printf("Determines the CPU core mode:\n");
		if (DB_784MP_GP_ID != mvBoardIdGet()){
			printf("\t0x0 = ARMv6 UP\n");
			printf("\t0x1 = ARMv7 UP\n");
			printf("\t0x2 = ARMv6 MP\n");
		}
#ifndef MV88F78X60_Z1
		printf("\t0x3 = ARMv7 MP\n");
#endif
	} else if (strcmp(cmd, "cpusnum") == 0) {

		printf("Determines the number of CPU cores:\n");
		printf("\t0x0 = Single CPU\n");
		printf("\t0x1 = Dual CPU\n");
		printf("\t0x2 = Reserved\n");
		printf("\t0x3 = Quad CPU\n");

	} else if (strcmp(cmd, "freq") == 0) {
		MV_FREQ_MODE *pFreqModes;
		if (mvCtrlRevGet() == 2) {
			pFreqModes = freq_modes_B0;
			printf("\n\nval| Frequency Configuration   | CPU Freq| Fabric Freq| \n");
		}
		else {
			pFreqModes = freq_modes;
			printf("\n\nval| Clock Frequency Configuration Options  | CPU Freq| Fabric Freq| AltFabric Freq|\n");
		}
		for (i=0; i < FREQ_MODES_NUM; i++) {
			if (pFreqModes->name == NULL)
				break;
			printf(" %02d| %s|     0x%02x|        0x%02x|", i,
				   pFreqModes->name,
				   pFreqModes->cpuFreq,
				   pFreqModes->fabricFreq);
			if (mvCtrlRevGet() == 2)
				printf("\n");
			else
				printf("           0x%02x|\n", pFreqModes->AltfabricFreq);
			pFreqModes++;
		}
	} else if (strcmp(cmd, "pex") == 0) {

		printf("Determines the pex capability mode:\n");
		printf("\t0x0 = GEN 1\n");
		printf("\t0x1 = GEN 2\n");

	} else if (strcmp(cmd, "dramecc") == 0) {

		printf("Determines the Dram ECC mode: (Valid only in 1200/300 frequency setup)\n");
		printf("\t0x0 = Dram ECC is disabled\n");
		printf("\t0x1 = Dram ECC is enabled\n");

	} else if (strcmp(cmd, "drambuswidth") == 0) {

		printf("Determines the Dram bus width: (Valid only in 1066/533 and 800/400 modes)\n");
		printf("\t0x0 = Bus width - 64bit\n");
		printf("\t0x1 = Bus width - 32bit\n");

	} else if (strcmp(cmd, "dump") == 0) {

		printf("Read the SAR register directly\n");

	}
	else if (strcmp(cmd, "pexmode") == 0) {

		printf("sets the mode of PEX0 and PEX1\n");
		printf("\t0x0 = PEX0 is x1 and PEX1 is x1\n");
		printf("\t0x1 = PEX0 is x4 and PEX1 is x1\n");
		printf("\t0x2 = PEX0 is x1 and PEX1 is x4\n");
		printf("\t0x3 = PEX0 is x4 and PEX1 is x4\n");

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
	MV_U8 l2size;
	MV_U8 bootsrc;
	MV_U8 bootwidth;
	MV_U8 cpu0core;
	MV_U8 cpusnum;
	MV_U8 AltfabricFreq;
	MV_U8 mvCtrlRev = mvCtrlRevGet();

	char* cpuFreqArr[12];
	char* bootSrcArr[8];
	char* bootWidthArr[2];

	cpuFreqArr[0] = "1000 MHz";
	cpuFreqArr[1] = "1066 MHz";
	cpuFreqArr[2] = "1200 MHz";
	cpuFreqArr[3] = "1333 MHz";
	cpuFreqArr[4] = "1500 MHz";
	cpuFreqArr[5] = "RSVD";
	cpuFreqArr[6] = "RSVD";
	cpuFreqArr[7] = "RSVD";
	cpuFreqArr[8] = "RSVD";
	cpuFreqArr[9] = "667 MHz";
	cpuFreqArr[10] = "800 MHz";
	cpuFreqArr[11] = "1600 MHz";

	bootSrcArr[0] = "BootROM enabled, Boot from Device (NOR) flash";
	bootSrcArr[1] = "BootROM enabled, Boot from NAND flash";
	bootSrcArr[2] = "BootROM enabled, Boot from UART";
	bootSrcArr[3] = " BootROM enabled, Boot from SPI0 (CS0)";
	bootSrcArr[4] = "BootROM enabled, Boot from PCIe Port 0.0";
	bootSrcArr[5] = "BootROM enabled, Boot from SATA (see Nand Flash Initialization Sequence / SERDES Selection SAR for more details)";
	bootSrcArr[6] = "RSVD";
	bootSrcArr[7] = "BootROM enabled, UART debug prompt mode";

	bootWidthArr[0] = "NOR/NAND: 8bits / SPI: 32bits";
	bootWidthArr[1] = "NOR/NAND: 16bits / SPI: 24bits/16bits";

	int flag = 0;

	if (argc < 1)
		goto usage;
	cmd = argv[0];

	if (strcmp(cmd, "cpufreq") == 0) {

		printf("cpufreq = %d\n", mvBoardCpuFreqGet());

	} else if (strcmp(cmd, "fabfreq") == 0) {

		printf("fabfreq = %d\n", mvBoardFabFreqGet());

	} else if (strcmp(cmd, "l2size") == 0) {

		printf("l2size = %d\n", mvBoardL2SizeGet());

	} else if (strcmp(cmd, "bootsrc") == 0) {

		printf("bootsrc = %d\n", mvBoardBootDevGet());

	} else if (strcmp(cmd, "bootwidth") == 0) {

		printf("bootwidth = %d\n", mvBoardBootDevWidthGet());
#ifdef MV88F78X60_Z1
	} else if (strcmp(cmd, "cpu0core") == 0) {
		printf("cpu0core = %d\n", mvBoardCpu0CoreModeGet());
#else
	} else if (strcmp(cmd, "cpu0Endianess") == 0) {
		printf("cpu0Endianess = %d\n", mvBoardCpu0EndianessGet());
#endif
	} else if (strcmp(cmd, "cpusnum") == 0) {

		printf("cpusnum = %d\n", mvBoardCpuCoresNumGet());

	} else if (strcmp(cmd, "pex") == 0) {

		printf("pex = %d\n", mvBoardPexCapabilityGet());

	} else if (strcmp(cmd, "dramecc") == 0) {

		printf("Dram ECC = %d\n", mvBoardDramEccGet());

	} else if (strcmp(cmd, "drambuswidth") == 0) {

		if (mvBoardDramBusWidthGet())
			printf("Dram Bus Width = 32bit\n");
		else
			printf("Dram Bus Width = 64bit\n");

	} else if (strcmp(cmd, "dump") == 0) {

			cpuFreq = (MV_REG_READ(MPP_SAMPLE_AT_RESET(0)) & SAR0_CPU_FREQ_MASK) >> SAR0_CPU_FREQ_OFFSET;
			fabricFreq = (MV_REG_READ(MPP_SAMPLE_AT_RESET(0)) & SAR0_FABRIC_FREQ_MASK) >> SAR0_FABRIC_FREQ_OFFSET;
			l2size = (MV_REG_READ(MPP_SAMPLE_AT_RESET(0)) & SAR0_L2_SIZE_MASK) >> SAR0_L2_SIZE_OFFSET;
			bootsrc = (MV_REG_READ(MPP_SAMPLE_AT_RESET(0)) & SAR0_BOOTSRC_MASK) >> SAR0_BOOTSRC_OFFSET;
			bootwidth = (MV_REG_READ(MPP_SAMPLE_AT_RESET(0)) & SAR0_BOOTWIDTH_MASK) >> SAR0_BOOTWIDTH_OFFSET;
			cpu0core = ((MV_REG_READ(MPP_SAMPLE_AT_RESET(0)) & SAR0_CPU0CORE_MASK) >> SAR0_CPU0CORE_OFFSET) |		   ((MV_REG_READ(MPP_SAMPLE_AT_RESET(1)) & SAR1_CPU0CORE_MASK) << (SAR1_CPU0CORE_OFFSET + 1));
			cpusnum = (MV_REG_READ(MPP_SAMPLE_AT_RESET(1)) & SAR1_CPU_CORE_MASK) >> SAR1_CPU_CORE_OFFSET;

			printf("cpufreq = %d ==> %s\n", cpuFreq, cpuFreqArr[cpuFreq]);
			printf("fabricFreq = %d\n", fabricFreq);
			printf("l2size = %d\n", l2size);
			printf("bootsrc = %d ==> %s\n", bootsrc, bootSrcArr[bootsrc]);
			printf("bootwidth = %d ==> %s\n", bootwidth, bootWidthArr[bootwidth]);
			printf("cpu0core = %d\n", cpu0core);
			printf("cpusnum = %d\n", cpusnum);

	} else if (strcmp(cmd, "freq") == 0) {
		MV_FREQ_MODE *pFreqModes;
		cpuFreq = mvBoardCpuFreqGet();
		if (mvCtrlRev == 2)
			pFreqModes = freq_modes_B0;
		else {
			pFreqModes = freq_modes;
		AltfabricFreq = mvBoardAltFabFreqGet();
		}

		fabricFreq = mvBoardFabFreqGet();
		for (i=0; i<FREQ_MODES_NUM; i++) {
			if (pFreqModes->name == NULL)
				continue;
			if ((pFreqModes->cpuFreq == cpuFreq) &&
				(pFreqModes->fabricFreq == fabricFreq)) {
				if (mvCtrlRev ==2) {
					printf("Mode is: %s\n", pFreqModes->name);
					flag = 1;
					break;
				}
				if (pFreqModes->AltfabricFreq == AltfabricFreq) {
					printf("Mode is: %s\n", pFreqModes->name);
				flag = 1;
					break;
				}
			}
			pFreqModes++;
		}
		if (flag == 0)
			printf("Current freq mode is invalid!\n");
	} else if (strcmp(cmd, "pexmode") == 0) {
		if ((mvBoardPexModeGet()) == 0)
			printf("\t0x0 = PEX0 is x1 and PEX1 is x1\n");
		else if ((mvBoardPexModeGet()) == 1)
			printf("\t0x1 = PEX0 is x4 and PEX1 is x1\n");
		else if ((mvBoardPexModeGet()) == 2)
			printf("\t0x2 = PEX0 is x1 and PEX1 is x4\n");
		else if ((mvBoardPexModeGet()) == 3)
			printf("\t0x3 = PEX0 is x4 and PEX1 is x4\n");
		else
			printf("PEX mode is not valid!\n");
	}

	return 0;
usage:
	printf("Usage: SatR read [options] (see help) \n");
	return 1;
}

static int do_sar_write(int argc, char *const argv[])
{
	const char *cmd;
	MV_FREQ_MODE *pFreqModes;
	MV_U8 mvCtrlRev = mvCtrlRevGet();
	MV_U32 tmp;

	if (argc < 2)
		goto usage;

	cmd = argv[0];

	if (strcmp(cmd, "cpufreq") == 0) {

		MV_U8 freq = simple_strtoul(argv[1], NULL, 10);
		if (mvBoardCpuFreqSet(freq) != MV_OK)
			goto write_fail;
		if (mvCtrlRevGet() == 2) {
			/* update AVS based on new cpu frequency */
			/* since AVS enble 0x20868[9] is not afected by reset */
			tmp = MV_REG_READ(AVS_CONTROL2_REG);
			if ((0x4 == freq) || (0xB == freq)){
				MV_U32 tmp2;
				tmp2 = MV_REG_READ(AVS_LOW_VDD_LIMIT);
				tmp2 |= 0x0f0;
				MV_REG_WRITE(AVS_LOW_VDD_LIMIT , tmp2);
				tmp |= BIT9; 	/* Enable AVS  */
			}
			else
				tmp &= ~BIT9; /* disable AVS  */
			MV_REG_WRITE(AVS_CONTROL2_REG , tmp);
		}




	} else if (strcmp(cmd, "fabfreq") == 0) {

		MV_U8 freq = simple_strtoul(argv[1], NULL, 10);

		if (mvBoardFabFreqSet(freq) != MV_OK)
			goto write_fail;

	} else if (strcmp(cmd, "l2size") == 0) {

		MV_U8 l2c = simple_strtoul(argv[1], NULL, 10);
		if (mvBoardL2SizeSet(l2c) != MV_OK)
			goto write_fail;

	} else if (strcmp(cmd, "bootsrc") == 0) {

		MV_U8 boot = simple_strtoul(argv[1], NULL, 10);
		if (mvBoardBootDevSet(boot) != MV_OK)
			goto write_fail;

	} else if (strcmp(cmd, "bootwidth") == 0) {

		MV_U8 width = simple_strtoul(argv[1], NULL, 10);
		if (mvBoardBootDevWidthSet(width) != MV_OK)
			goto write_fail;
#ifdef MV88F78X60_Z1
	} else if (strcmp(cmd, "cpu0core") == 0) {
#else
	} else if (strcmp(cmd, "cpu0Endianess") == 0) {
#endif

		MV_U8 mode = simple_strtoul(argv[1], NULL, 10);
#ifdef MV88F78X60_Z1
		if (mvBoardCpu0CoreModeSet(mode) != MV_OK)
#else
		if (mvBoardCpu0EndianessSet(mode) != MV_OK)
#endif
			goto write_fail;

	} else if (strcmp(cmd, "cpusnum") == 0) {

		MV_U8 cores = simple_strtoul(argv[1], NULL, 10);
		if (mvBoardCpuCoresNumSet(cores) != MV_OK)
			goto write_fail;

	} else if (strcmp(cmd, "pex") == 0) {

		MV_U8 pexMode = simple_strtoul(argv[1], NULL, 10);
		if (mvBoardPexCapabilitySet(pexMode) != MV_OK)
			goto write_fail;

	} else if (strcmp(cmd, "dramecc") == 0) {

		MV_U8 eccMode = simple_strtoul(argv[1], NULL, 10);
		if (mvBoardDramEccSet(eccMode) != MV_OK)
			goto write_fail;

	} else if (strcmp(cmd, "drambuswidth") == 0) {

		MV_U8 dramBusWidth = simple_strtoul(argv[1], NULL, 10);
		if (mvBoardDramBusWidthSet(dramBusWidth) != MV_OK)
			goto write_fail;

	} else if (strcmp(cmd, "freq") == 0) {
		MV_U8 mode = simple_strtoul(argv[1], NULL, 10);

		if ((mode<0) || (mode >=FREQ_MODES_NUM)) {
			printf("invalid mode %d\n", mode);
			goto write_fail;
		}
		if (mvCtrlRev == 2)
			pFreqModes = &freq_modes_B0[mode];
		else {
			pFreqModes = &freq_modes[mode];
			if (mvBoardAltFabFreqSet(pFreqModes->AltfabricFreq) != MV_OK)
			goto write_fail;
		}
		if (mvBoardCpuFreqSet(pFreqModes->cpuFreq) != MV_OK)
				goto write_fail;
		if (mvCtrlRevGet() == 2) {
			/* update AVS based on new cpu frequency */
			/* since AVS enble 0x20868[9] is not afected by reset */
			tmp = MV_REG_READ(AVS_CONTROL2_REG);
			if ( (0x4 == pFreqModes->cpuFreq) || (0xB == pFreqModes->cpuFreq)) {
				MV_U32 tmp2;
				tmp2 = MV_REG_READ(AVS_LOW_VDD_LIMIT);
				tmp2 |= 0x0f0;
				MV_REG_WRITE(AVS_LOW_VDD_LIMIT , tmp2);
				tmp |= BIT9; 	/* Enable AVS  */
			}
			else
				tmp &= ~BIT9; /* disable AVS  */
			MV_REG_WRITE(AVS_CONTROL2_REG , tmp);
		}

		if (mvBoardFabFreqSet(pFreqModes->fabricFreq) != MV_OK)
				goto write_fail;

	} else if (strcmp(cmd, "pexmode") == 0) {

		MV_U8 pexMode= simple_strtoul(argv[1], NULL, 10);
		if (mvBoardPexModeSet(pexMode) != MV_OK)
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

	"list cpufreq  - prints the S@R modes list\n"
	"SatR list fabfreq	- prints the S@R modes list\n"
	"SatR list l2size	- prints the S@R modes list\n"
	"SatR list bootsrc	- prints the S@R modes list\n"
	"SatR list bootwidth	- prints the S@R modes list\n"
#ifdef MV88F78X60_Z1
	"SatR list cpu0core     - prints the S@R modes list\n"
#else
	"SatR list cpu0Endianess- prints the S@R modes list\n"
#endif
	"SatR list cpusnum	- prints the S@R modes list\n"
	"SatR list freq		- prints the S@R modes list\n"
	"SatR list pex		- prints the S@R modes list\n"
	"SatR list pexmode	- prints the S@R modes list\n"
	"SatR list dramecc	- prints the S@R modes list\n"
	"SatR list drambuswidth	- prints the S@R modes list\n"

	"SatR read cpufreq	- read and print the CPU frequency S@R value\n"
	"SatR read fabfreq	- read and print the Fabric frequency S@R value\n"
	"SatR read l2size	- read and print the L2 cache size S@R value\n"
	"SatR read bootsrc	- read and print the Boot source S@R value\n"
	"SatR read bootwidth	- read and print the Boot device width S@R value\n"
#ifdef MV88F78X60_Z1
	"SatR read cpu0core     - read and print the CPU0 core mode S@R value\n"
#else
	"SatR read cpu0Endianess- read and print the CPU0 core mode S@R value\n"
#endif
	"SatR read cpusnum	- read and print the number of CPU cores S@R value (reading the I2C device)\n"
	"SatR read freq		- read and print the mode of cpu/ddr freq S@R value (reading the I2C device)\n"
	"SatR read pex		- read and print the pex capability mode from S@R value (reading the I2C device)\n"
	"SatR read dramecc	- read and print the Dram ECC mode from S@R value (reading the I2C device)\n"
	"SatR read drambuswidth <val>	- read and print the S@R with the Dram bus width value (reading the I2C device)\n"
	"SatR read pex		- read and print the pex mode(x1 or x4) from S@R value (reading the I2C device)\n"
	"SatR read dump      	- read and print the SAR register \n\n"

	"SatR write cpufreq <val>	- write the S@R with CPU frequency value\n"
	"SatR write fabfreq <val>	- write the S@R with Fabric frequency value\n"
	"SatR write l2size <val>	- write the S@R with L2 cache size value\n"
	"SatR write bootsrc <val>	- write the S@R with Boot source value\n"
	"SatR write bootwidth <val>	- write the S@R with Boot device width value\n"
#ifdef MV88F78X60_Z1
	"SatR write cpu0core <val>      - write the S@R with CPU0 core mode value\n"
#else
	"SatR write cpu0Endianess <val>	- write the S@R with CPU0 cpu0Endianess value\n"
#endif
	"SatR write cpusnum <val>	- write the S@R with the number of CPU cores\n"
	"SatR write freq <val>		- write the S@R with the cpu/ddr freq mode\n"
	"SatR write pex <val>		- write the S@R with the pex capability mode\n"
	"SatR write dramecc <val>	- write the S@R with the Dram ECC mode\n"
	"SatR write drambuswidth <val>	- write the S@R with the Dram bus width value\n"
	"SatR write pexmode <val> 	- write the S@R with the pex mode (x1 or x4)\n"
);
#endif /*defined(CONFIG_CMD_SAR)*/
