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

#include <config.h>
#include <common.h>
#include <command.h>
#include <pci.h>
#include <net.h>
#include <spi_flash.h>
#include <bzlib.h>

#include "mvCommon.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "cpu/mvCpu.h"

#if defined(MV_INC_BOARD_NOR_FLASH)
#include "norflash/mvFlash.h"
#endif

#if defined(MV_INCLUDE_GIG_ETH)
#include "eth-phy/mvEthPhy.h"
#endif

#if defined(MV_INCLUDE_PEX)
#include "pex/mvPex.h"
#endif

#if defined(MV_INCLUDE_PDMA)
#include "pdma/mvPdma.h"
#include "mvSysPdmaApi.h"
#endif

#if defined(MV_INCLUDE_XOR)
#include "xor/mvXorRegs.h"
#include "xor/mvXor.h"
#endif

#if defined(MV_INCLUDE_PMU)
#include "pmu/mvPmuRegs.h"
#endif

#include "cntmr/mvCntmrRegs.h"
#include "switchingServices.h"

/* defines  */
#undef MV_DEBUG
#ifdef MV_DEBUG
#define DB(x) x
#define DB1(x)  x
#else
#define DB(x)
#define DB1(x)
#endif

/* SAR defines when Connected to Bc2*/
#define MV_BOARD_CTRL_I2C_ADDR_BC2	0x0
#define TWSI_CHANNEL_BC2			0
#define TWSI_SPEED_BC2				20000 // wa for bits 1,2 in 0x4c. Mmust lower 100000 -> 20000 . adiy, erez
typedef volatile unsigned long VUL;

static MV_U8 tread_bc2(MV_U8 addr, int reg)
{
		MV_TWSI_SLAVE twsiSlave;
		MV_TWSI_ADDR slave;
		MV_U8 data;

		DB(printf("tread_bc2, DevAddr = 0x%x\n", addr));

		/* TWSI init */
		slave.address = MV_BOARD_CTRL_I2C_ADDR_BC2;
		slave.type = ADDR7_BIT;

		mvTwsiInit (TWSI_CHANNEL_BC2, TWSI_SPEED_BC2, mvBoardTclkGet(), &slave, 0);

		/* read SatR */
		twsiSlave.slaveAddr.type = ADDR7_BIT;
		twsiSlave.slaveAddr.address = addr ;
		twsiSlave.validOffset = MV_TRUE;
		twsiSlave.offset = reg;
		twsiSlave.moreThen256 = MV_FALSE;

		if (MV_OK != mvTwsiRead(TWSI_CHANNEL_BC2, &twsiSlave, &data, 1)) {
				DB(printf("tread_bc2 : twsi read fail\n"));
				return MV_ERROR;
		}
		DB(printf("tread_bc2: twsi read succeeded, data = 0x%x\n", data));

		return data;
}

static MV_STATUS twrite_bc2(MV_U8 addr, int reg, MV_U8 regVal)
{
		MV_TWSI_SLAVE twsiSlave;
		MV_TWSI_ADDR slave;
		MV_U8 data;

		//  printf(">>> in twrite_bc2, addr=0x%x, reg = 0x%x, val=0x%x\n", addr, reg, regVal);
		/* TWSI init */
		slave.address = MV_BOARD_CTRL_I2C_ADDR_BC2;
		slave.type = ADDR7_BIT;

		mvTwsiInit (TWSI_CHANNEL_BC2, TWSI_SPEED_BC2, mvBoardTclkGet(), &slave, 0);

		/* write SatR */
		twsiSlave.slaveAddr.address = addr;
		twsiSlave.slaveAddr.type = ADDR7_BIT;
		twsiSlave.validOffset = MV_TRUE;
		twsiSlave.offset = reg;
		twsiSlave.moreThen256 = MV_FALSE;

		data = regVal;
		if (MV_OK != mvTwsiWrite(TWSI_CHANNEL_BC2, &twsiSlave, &data, 1)) {
				DB(mvOsPrintf("twrite_bc2: twsi write fail\n"));
				return MV_ERROR;
		}
		DB(mvOsPrintf("twrite_bc2: twsi write succeeded\n"));

		return MV_OK;
}

static int do_sar_list_bc2(int argc, char *const argv[])
{
		const char *cmd;
		int all = 0;

		if (argc < 1)
				goto usage;
		cmd = argv[0];

		if (strcmp(cmd, "all") == 0)
				all = 1;

		if ((strcmp(cmd, "corefreq") == 0) || all) {
				printf("corefreq (0x4d 0:3): Determines the CORE frequency:\n");
				printf("\t0x0 = 360MHz\n");
				printf("\t0x1 = 220MHz\n");
				printf("\t0x2 = 250MHz\n");
				printf("\t0x3 = 400MHz\n");
				printf("\t0x4 = 500MHz\n");
				printf("\t0x5 = 520MHz\n");
				printf("\t0x6 = 450MHz\n");
				printf("\n");
		}

		if ((strcmp(cmd, "cpufreq") == 0)  || all) {
				printf("cpufreq (0x4d 3:4 + 0x4e 0:0): Determines the CPU and DDR frequencies:\n");
				printf("\t0x0 = CPU 400MHz, DDR 400MHz\n");
				printf("\t0x2 = CPU 667MHz, DDR 667MHz\n");
				printf("\t0x3 = CPU 800MHz, DDR 800MHz\n");
				printf("\n");
		}

		if ((strcmp(cmd, "tmfreq") == 0)  || all) {
				printf("tmfreq (0x4e 1:3): Determines the boot selection:\n");
				printf("\t0x0 = TM clock is disabled\n");
				printf("\t0x1 = TM runs 400MHZ, DDR3 runs 800MHz\n");
				printf("\t0x2 = TM runs 466MHZ, DDR3 runs 933MHz\n");
				printf("\t0x3 = TM runs 333MHZ, DDR3 runs 667MHz\n");
				printf("\n");
		}

		if ((strcmp(cmd, "bootsel") == 0) || all) {
				printf("bootsel (0x4f 0:2): Determines the TM frequency:\n");
				printf("\t0x0 = BootROM enabled, Boot from Device(NOR) flash\n");
				printf("\t0x1 = BootROM enabled, Boot from NAND flash (new NF controller on DEV_CSn[0])\n");
				printf("\t0x2 = BootROM enabled, boot from UART\n");
				printf("\t0x3 = BootROM enabled, boot from SPI0(CS0)\n");
				printf("\t0x5 = BootROM enabled, Standby slave. Must set PCIs as endpoint (i.e. CPU IDLE)\n");
				printf("\t0x6 = BootROM enabled, UART debug prompt mode\n");
				printf("\n");
		}

		if ((strcmp(cmd, "jtagcpu") == 0) || all) {
				printf("jtagcpu (0x4f 3:3): Determines the JTAG to CPU connection:\n");
				printf("\t0x0 = JTAG is connected to C3M CPU\n");
				printf("\t0x1 = JTAG is connected to MSYS CPU\n");
				printf("\n");
		}

		if ((strcmp(cmd, "ptppll") == 0) || all) {
				printf("ptppll (0x4f 4:4): Determines the ptppll:\n");
				printf("\t0x1 = Must write 0x1. PTP PLL runs @ 1093.75MHz, PTP Clock = 546.875MHz\n");
				printf("\n");
		}

		if ((strcmp(cmd, "pciegen1") == 0) || all) {
				printf("pciegen1 (0x4c.1 4:4): Determines the pciegen1:\n");
				printf("\t0x0 = Do not force BC2 PCIe connection to GEN1\n");
				printf("\t0x1 = Force BC2 PCIe connection to GEN1\n");
				printf("\n");
		}

		return 0;

		usage:
		printf("Usage: Satr - see options\n");
		return 1;
}

static int do_sar_read_bc2(int argc, char *const argv[])
{

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

		const char *cmd;
		volatile uint satr_reg;

		cmd = argv[0];

		if (strcmp(cmd, "dump") == 0) {
				satr_reg =
				((tread_bc2(0x4c, 0)&0x1f) << 0) |	// 5 bits
				((tread_bc2(0x4d, 0)&0x1f) << 5) |
				((tread_bc2(0x4e, 0)&0x1f) << 10) |
				((tread_bc2(0x4f, 0)&0x1f) << 15);
				printf("BC2 S@R raw register = 0x%08x\n", satr_reg);
		}

		else if (strcmp(cmd, "devid") == 0) {
				satr_reg = tread_bc2(0x4c, 0) & 0x1f;
				printf("BC2 S@R devid = 0x%02x\n", satr_reg);
		}

		else if (strcmp(cmd, "corefreq") == 0) {
				satr_reg = tread_bc2(0x4d, 0) & 0x07;
				printf("BC2 S@R corefreq = 0x%x\n", satr_reg);
		}

		else if (strcmp(cmd, "cpufreq") == 0) {
				satr_reg = ((tread_bc2(0x4d, 0) & 0x18) >> 3) | ((tread_bc2(0x4e, 0) & 0x01) << 2);
				printf("BC2 S@R cpufreq = 0x%x\n", satr_reg);
		}

		else if (strcmp(cmd, "tmfreq") == 0) {
				satr_reg = (tread_bc2(0x4e, 0) & 0x0e) >> 1;
				printf("BC2 S@R tmfreq = 0x%x\n", satr_reg);
		}

		else if (strcmp(cmd, "bootsel") == 0) {
				satr_reg = tread_bc2(0x4f, 0) & 0x07;
				printf("BC2 S@R bootsel = 0x%x\n", satr_reg);
		}

		else if (strcmp(cmd, "jtagcpu") == 0) {
				satr_reg = (tread_bc2(0x4f, 0) & 0x08) >> 3;
				printf("BC2 S@R jtagcpu = 0x%x\n", satr_reg);
		}

		else if (strcmp(cmd, "ptppll") == 0) {
				satr_reg = (tread_bc2(0x4f, 0) & 0x10) >> 4;
				printf("BC2 S@R ptppll = 0x%x\n", satr_reg);
		}

		else if (strcmp(cmd, "pciegen1") == 0) {
				satr_reg = (tread_bc2(0x4c, 1) & 0x10) >> 4;
				printf("BC2 S@R pciegen1 = 0x%x\n", satr_reg);
		}

		return 0;
}

static int do_sar_write_bc2(int argc, char *const argv[])
{
		const char *cmd;
		volatile uint satr_reg;
		volatile int bit_mask;

		cmd = argv[0];

		if (strcmp(cmd, "devid") == 0) {
				bit_mask = (simple_strtoul(argv[1], NULL, 16) & 0x1f);
				twrite_bc2(0x4c, 0, (MV_U8)bit_mask);
		}

		else if (strcmp(cmd, "corefreq") == 0) {
				bit_mask = (simple_strtoul(argv[1], NULL, 16) & 0x07);
				satr_reg = tread_bc2(0x4d, 0) & 0x1f;
				satr_reg = (satr_reg & 0x18) | bit_mask;
				twrite_bc2(0x4d, 0, (MV_U8)satr_reg);
		}

		else if (strcmp(cmd, "cpufreq") == 0) {
				bit_mask = (simple_strtoul(argv[1], NULL, 16) & 0x07);
				satr_reg = tread_bc2(0x4d, 0) & 0x1f;
				satr_reg = (satr_reg & 0x07) | ((bit_mask & 0x03) << 3);
				twrite_bc2(0x4d, 0, (MV_U8)satr_reg);

				satr_reg = tread_bc2(0x4e, 0) & 0x1f;
				satr_reg = (satr_reg & 0x1e) | ((bit_mask & 0x04) >> 2);
				twrite_bc2(0x4e, 0, (MV_U8)satr_reg);
		}

		else if (strcmp(cmd, "tmfreq") == 0) {
				bit_mask = (simple_strtoul(argv[1], NULL, 16) & 0x07);
				satr_reg = tread_bc2(0x4e, 0) & 0x1f;
				satr_reg = (satr_reg & 0x11) | (bit_mask  << 1);
				twrite_bc2(0x4e, 0, (MV_U8)satr_reg);
		}

		else if (strcmp(cmd, "bootsel") == 0) {
				bit_mask = (simple_strtoul(argv[1], NULL, 16) & 0x07);
				satr_reg = tread_bc2(0x4f, 0) & 0x1f;
				satr_reg = (satr_reg & 0x18) | bit_mask;
				twrite_bc2(0x4f, 0, (MV_U8)satr_reg);
		}

		else if (strcmp(cmd, "jtagcpu") == 0) {
				bit_mask = (simple_strtoul(argv[1], NULL, 16) & 0x01);
				satr_reg = tread_bc2(0x4f, 0) & 0x1f;
				satr_reg = (satr_reg & 0x17) | (bit_mask << 3);
				twrite_bc2(0x4f, 0, (MV_U8)satr_reg);
		}

		else if (strcmp(cmd, "ptppll") == 0) {
				bit_mask = (simple_strtoul(argv[1], NULL, 16) & 0x01);
				satr_reg = tread_bc2(0x4f, 0) & 0x1f;
				satr_reg = (satr_reg & 0x0f) | (bit_mask << 4);
				twrite_bc2(0x4f, 0, (MV_U8)satr_reg);
		}

		else if (strcmp(cmd, "pciegen1") == 0) {
			bit_mask = (simple_strtoul(argv[1], NULL, 16) & 0x01);
			satr_reg = tread_bc2(0x4c, 1) & 0x1f;
			satr_reg = (satr_reg & 0x0f) | (bit_mask << 4);
			twrite_bc2(0x4c, 1, (MV_U8)satr_reg);
		}

		return 0;
}



MV_STATUS check_twsi_bc2(void)
{
		MV_U8 reg = tread_bc2(0x4c, 0);
		DB(printf("\ncheck_twsi_bc2: read_BC2= 0x%x\n", reg));
		if (reg == 0xff)
				return MV_ERROR;
		else
				return MV_OK;
}

int do_sar_bc2(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{

		const char *cmd;

		/* need at least two arguments */
		if (argc < 2)
				goto usage;

		cmd = argv[1];

		if (check_twsi_bc2() == MV_ERROR) {
				printf("BC2 twsi channel not connected\n");
				return 1;
		}


		if (strcmp(cmd, "list") == 0)
				return do_sar_list_bc2(argc - 2, argv + 2);

		else if (strcmp(cmd, "read") == 0)
				return do_sar_read_bc2(argc - 2, argv + 2);


		else if (strcmp(cmd, "write") == 0) {
				if (do_sar_write_bc2(argc - 2, argv + 2) == 0) {
						do_sar_read_bc2(argc - 2, argv + 2);
				}
				return 0;

		}

		usage:
		printf("\n");
		printf("BC2 Sample At Reset sub-system\n");
		printf("SatR list corefreq  - print corefreq list\n");
		printf("SatR list cpufreq   - print cpufreq list\n");
		printf("SatR list tmfreq    - print tmfreq list\n");
		printf("SatR list bootsel   - print boolsel list\n");
		printf("SatR list jtagcpu   - print jtagcpu list\n");
		printf("SatR list ptppll    - print ptppll list\n");
		printf("SatR list pciegen1  - print pciegen1 list\n");

		printf("\n");

		printf("SatR read devid     - read device id\n");
		printf("SatR read corefreq  - read CORE frequency\n");
		printf("SatR read cpufreq   - read CPU frequency\n");
		printf("SatR read tmfreq    - read TM frequency\n");
		printf("SatR read bootsel   - read Boot select\n");
		printf("SatR read jtagcpu   - read JTAG CPU selection\n");
		printf("SatR read ptppll    - read PTP PLL setting\n");
		printf("SatR read pciegen1  - read Force PCIe GEN1 setting\n");
		printf("SatR read dump      - read all values\n");
		printf("\n");

		printf("SatR write devid    <val> - write device id\n");
		printf("SatR write corefreq <val> - write CORE frequency\n");
		printf("SatR write cpufreq  <val> - write CPU  frequency\n");
		printf("SatR write tmfreq   <val> - write TM frequency\n");
		printf("SatR write bootsel  <val> - write Boot select\n");
		printf("SatR write jtagcpu  <val> - write JTAG CPU selection\n");
		printf("SatR write ptppll   <val> - write PTP PLL setting\n");
		printf("SatR write pciegen1 <val> - write Force PCIe GEN1 setting\n");

		return 1;
}

#ifndef MV_MSYS
/* This function used for AMC + remote MSYS case. The MSYS standalone implementation differs */
static int do_qsgmii_sel(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

		int bit_mask;

		if (argc < 2)
				goto usage;

		if (check_twsi_bc2() == MV_ERROR) {
				printf("BC2 twsi channel not connected\n");
				return 1;
		}

		bit_mask = (simple_strtoul(argv[1], NULL, 16) & 0x0000ffff);

		twrite_bc2(0x20, 2, (MV_U8)((bit_mask >> 0) & 0xff));
		twrite_bc2(0x20, 3, (MV_U8)((bit_mask >> 8) & 0xff));
		twrite_bc2(0x20, 6, 0x0);
		twrite_bc2(0x20, 7, 0x0);
		return 1;

		usage:
		cmd_usage(cmdtp);
		return 1;
}


U_BOOT_CMD(
		  qsgmii_sel,      2,     1,      do_qsgmii_sel,
		  " Select SFP or QSGMII modes on bc2.\n",
		  " apply 16 bit array to select SFP or QSGMII modes"
		  );

#endif


