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

/* SAR defines when Connected to MSYS (Only Bc2 & BOBK) */
#define MV_BOARD_CTRL_I2C_ADDR_MSYS	0x0
#define TWSI_CHANNEL_MSYS			0
#define TWSI_SPEED_MSYS		20000 /* wa for bits 1,2 in 0x4c. Mmust lower
					 100000 -> 20000 . adiy, erez*/
typedef volatile unsigned long VUL;
const char *chipStr[] = {"XCAT2", "NP5", "BC2", "BOBK", "AC3", "OTHER"};

static int do_sar_read_msys(int silt, uint *value, int argc, char *const argv[]);


static MV_U8 tread_msys(MV_U8 addr, int reg, MV_BOOL moreThen256)
{
		MV_TWSI_SLAVE twsiSlave;
		MV_TWSI_ADDR slave;
		MV_U8 data;

		DB(printf("tread_msys, DevAddr = 0x%x\n", addr));

		/* TWSI init */
		slave.address = MV_BOARD_CTRL_I2C_ADDR_MSYS;
		slave.type = ADDR7_BIT;

		mvTwsiInit(TWSI_CHANNEL_MSYS, TWSI_SPEED_MSYS, mvBoardTclkGet(), &slave, 0);

		/* read SatR */
		twsiSlave.slaveAddr.type = ADDR7_BIT;
		twsiSlave.slaveAddr.address = addr ;
		twsiSlave.validOffset = MV_TRUE;
		twsiSlave.offset = reg;
		twsiSlave.moreThen256 = moreThen256;

		if (MV_OK != mvTwsiRead(TWSI_CHANNEL_MSYS, &twsiSlave, &data, 1)) {
				DB(printf("tread_msys : twsi read fail\n"));
				return MV_ERROR;
		}
		DB(printf("tread_msys: twsi read succeeded, data = 0x%x\n", data));

		return data;
}

static MV_STATUS twrite_msys(MV_U8 addr, int reg, MV_U8 regVal, MV_BOOL moreThen256)
{
		MV_TWSI_SLAVE twsiSlave;
		MV_TWSI_ADDR slave;
		MV_U8 data;

		/* printf(">>> in twrite_msys, addr=0x%x, reg = 0x%x, val=0x%x\n", addr, reg, regVal);*/
		/* TWSI init */
		slave.address = MV_BOARD_CTRL_I2C_ADDR_MSYS;
		slave.type = ADDR7_BIT;

		mvTwsiInit(TWSI_CHANNEL_MSYS, TWSI_SPEED_MSYS, mvBoardTclkGet(), &slave, 0);

		/* write SatR */
		twsiSlave.slaveAddr.address = addr;
		twsiSlave.slaveAddr.type = ADDR7_BIT;
		twsiSlave.validOffset = MV_TRUE;
		twsiSlave.offset = reg;
		twsiSlave.moreThen256 = moreThen256;

		data = regVal;
		if (MV_OK != mvTwsiWrite(TWSI_CHANNEL_MSYS, &twsiSlave, &data, 1)) {
				DB(mvOsPrintf("twrite_msys: twsi write fail\n"));
				return MV_ERROR;
		}
		DB(mvOsPrintf("twrite_msys: twsi write succeeded\n"));

		return MV_OK;
}

static int do_sar_list_msys(int silt, int argc, char *const argv[])
{
		const char *cmd;
		int all = 0;
		uint boardId = 0;
		char *const boardIdArgv[1] = {"boardid"};

		if (argc < 1)
				goto usage;
		cmd = argv[0];

		if (strcmp(cmd, "all") == 0)
				all = 1;

		if ((strcmp(cmd, "coreclock") == 0) || all) {
				if (silt == SILT_BC2) {
						printf("coreclock (0x4d 0:2): Determines the CORE frequency:\n");
						printf("\t0x0 = 360MHz\n");
						printf("\t0x1 = 220MHz\n");
						printf("\t0x2 = 250MHz\n");
						printf("\t0x3 = 400MHz\n");
						printf("\t0x4 = 500MHz\n");
						printf("\t0x5 = 520MHz\n");
						printf("\t0x6 = 450MHz\n");
						printf("\n");
				}
				else if (silt == SILT_BOBK) {
						if (do_sar_read_msys(SILT_BOBK, &boardId, 1, boardIdArgv) || (boardId >= BOBK_BOARDID_MAX)) {
								printf("Read BoardId failed, unknown Board\n");
								printf("\n");
								return 1;
						}
						else {
								printf("coreclock (0x4d 0:2): Determines the CORE frequency:\n");
								printf("\t0x0 = 365MHz\n");
								printf("\t0x1 = 220MHz\n");
								printf("\t0x2 = 250MHz\n");
								printf("\t0x3 = 200MHz\n");
								printf("\t0x4 = 167MHz\n");
								if (boardId == BOBK_DB_98DX4203_12XG_BOARDID)
									printf("\t0x5 = 133MHz\n");
								printf("\n");
						}
				}
		}

		if ((strcmp(cmd, "freq") == 0)  || all) {
				if (silt == SILT_BC2) {
						printf("freq (0x4d 3:4 + 0x4e 0:0): Determines the CPU and DDR frequencies:\n");
						printf("\t0x0 = CPU 400MHz, DDR 400MHz\n");
						printf("\t0x2 = CPU 667MHz, DDR 667MHz\n");
						printf("\t0x3 = CPU 800MHz, DDR 800MHz\n");
						printf("\n");
				}
				else if (silt == SILT_BOBK) {
						if (do_sar_read_msys(SILT_BOBK, &boardId, 1, boardIdArgv) || (boardId >= BOBK_BOARDID_MAX)) {
								printf("Read BoardId failed, unknown Board\n");
								printf("\n");
								return 1;
						}
						else {
								printf("freq (0x4c 3:4 + 0x4f 0:0): Determines the CPU and DDR frequencies:\n");
								printf("\t0x0 = CPU 400MHz, DDR 400MHz\n");
								if (boardId == BOBK_DB_98DX4235_12XG_BOARDID)
										printf("\t0x1 = CPU 1000MHZ, DDR 667MHz\n");
								printf("\t0x2 = CPU 667MHz, DDR 667MHz\n");
								printf("\t0x3 = CPU 800MHz, DDR 800MHz\n");
								if (boardId == BOBK_DB_98DX4235_12XG_BOARDID)
										printf("\t0x4 = CPU 1200MHz, DDR 800MHz\n");
								printf("\t0x5 = CPU 800MHz, DDR 400MHz\n");
								printf("\n");
						}
				}
		}

		if ((strcmp(cmd, "tmfreq") == 0)  || all) {
				if (silt == SILT_BC2)
					printf("tmfreq (0x4e 1:3): Determines the TM frequency:\n");
				else if (silt == SILT_BOBK)
					printf("tmfreq (0x4c 0:2): Determines the TM frequency:\n");

				printf("\t0x0 = TM clock is disabled\n");
				printf("\t0x1 = TM runs 400MHZ, DDR3 runs 800MHz\n");
				printf("\t0x2 = TM runs 466MHZ, DDR3 runs 933MHz\n");
				printf("\t0x3 = TM runs 333MHZ, DDR3 runs 667MHz\n");
				printf("\n");
		}

		if ((strcmp(cmd, "bootsrc") == 0) || all) {
				if (silt == SILT_BC2)
					printf("bootsrc (0x4f 0:2): Determines the boot selection:\n");
				else if (silt == SILT_BOBK)
					printf("bootsrc (0x4f 1:3): Determines the boot selection:\n");

				printf("\t0x0 = BootROM enabled, Boot from Device(NOR) flash\n");
				printf("\t0x1 = BootROM enabled, Boot from NAND flash (new NF controller on DEV_CSn[0])\n");
				printf("\t0x2 = BootROM enabled, boot from UART\n");
				printf("\t0x3 = BootROM enabled, boot from SPI0(CS0)\n");
				printf("\t0x5 = BootROM enabled, Standby slave. Must set PCIs as endpoint (i.e. CPU IDLE)\n");
				printf("\t0x6 = BootROM enabled, UART debug prompt mode\n");
				printf("\n");
		}

		if ((strcmp(cmd, "jtagcpu") == 0) || all) {
				if (silt == SILT_BC2)
					printf("jtagcpu (0x4f 3:3): Determines the JTAG to CPU connection:\n");
				else if (silt == SILT_BOBK)
					printf("jtagcpu (0x4d 3:3): Determines the JTAG to CPU connection:\n");

				printf("\t0x0 = JTAG is connected to CM3 CPU\n");
				printf("\t0x1 = JTAG is connected to MSYS CPU\n");
				printf("\n");
		}

		if ((strcmp(cmd, "ptppll") == 0) || all) {
				if (silt == SILT_BC2) {
					printf("ptppll (0x4f 4:4): Determines the ptppll:\n");
					printf("\t0x1 = Must write 0x1. PTP PLL runs @ 1093.75MHz, PTP Clock = 546.875MHz\n");
					printf("\n");
				}
		}

		if ((strcmp(cmd, "boardid") == 0) || all) {
				if (silt == SILT_BC2) {
					printf("boardid (0x50.7 0:2): Determines the board ID (0-7)\n");
					printf("\t0x0 = Board is DB-DXBC2-MM\n");
					printf("\t0x1 = Board is RD-DXBC2-48G-12XG2XLG\n");
					printf("\t0x2 = Board is RD-MTL-BC2-48G-12XG2XLG\n");
					printf("\n");
				}
				if (silt == SILT_BOBK) {
					printf("boardid (0x50.7 0:2): Determines the board ID (0-7)\n");
					printf("\t0x0 = Board is DB-98DX4235-12XG (Cetus)\n");
					printf("\t0x1 = Board is DB-98DX4203-12XG (Caelum)\n");
					printf("\n");
				}
				if (silt == SILT_AC3) {
					printf("boardid (0x50.7 0:2): Determines the board ID (0-7)\n");
					printf("\t0x0 = Board is DB-XC3-24G4XG\n");
					printf("\t0x1 = Board is RD-XC3-48G4XG-A\n");
					printf("\t0x2 = Board is RD-XC3-48G2XG2XXG-A\n");
					printf("\t0x3 = Board is DB-XC3-24G-4G\n");
					printf("\t0x4 = Board is RD-XC3-24G-4SFP\n");
					printf("\n");
				}
		}

		if ((strcmp(cmd, "forcegen1") == 0) || all) {
				if (silt == SILT_BC2 || silt == SILT_BOBK) {
					printf("forcegen1 (0x57.3 2:2): Determines if whether to force PCI-e");
					printf(" connection to GEN1 or not:\n");
					printf("\t0x0 = Do not force BC2 PCI-e connection to GEN1\n");
					printf("\t0x1 = Force BC2 PCI-e connection to GEN1\n");
					printf("\n");
				}
		}

		return 0;

		usage:
		printf("Usage: Satr - see options\n");
		return 1;
}

static int do_sar_read_msys(int silt, uint *value, int argc, char *const argv[])
{

/*
   bc2 sample and reset register

#     4f       #     4e       #     4d       #      4c      #
#              #              #              #              #
#--|--|--|--|--#--|--|--|--|--#--|--|--|--|--#--|--|--|--|--#
#  |  |  |  |  #  |  |  |  |  #  |  |  |  |  #  |  |  |  |  #
#--|--|--|--|--#--|--|--|--|--#--|--|--|--|--#--|--|--|--|--#
#  |           #  |        |  #     |        #              #
#-P|-R|bootsrc-#-R|-TM-f---|-CPU-f--|-CORE-f-#-----devid----#


*/

		const char *cmd;
		volatile uint satr_reg;
		volatile int bit_mask = 0;
		uint twsi_Addr = 0;
		uint twsi_Reg = 0;
		uint field_Offs = 0;
		MV_BOOL moreThen256 = MV_FALSE;

		cmd = argv[0];

		if (strcmp(cmd, "dump") == 0) {
				satr_reg =
				((tread_msys(0x4c, 0, moreThen256)&0x1f) << 0) |	/* 5 bits */
				((tread_msys(0x4d, 0, moreThen256)&0x1f) << 5) |
				((tread_msys(0x4e, 0, moreThen256)&0x1f) << 10) |
				((tread_msys(0x4f, 0, moreThen256)&0x1f) << 15);
				printf("MSYS S@R raw register = 0x%08x\n", satr_reg);

				*value = satr_reg;
				return 0;
		}

		else if (strcmp(cmd, "devid") == 0) {
				twsi_Addr = (silt == SILT_BC2) ? 0x4c : 0x4e;
				field_Offs = 0;
				bit_mask = 0x1f;
		}

		else if (strcmp(cmd, "coreclock") == 0) {
				twsi_Addr = 0x4d;
				field_Offs = 0;
				bit_mask = 0x7;
		}

		else if (strcmp(cmd, "freq") == 0) {
				twsi_Addr = (silt == SILT_BC2) ? 0x4d : 0x4c;
				field_Offs = 3;
				bit_mask = 0x3;

				satr_reg = (tread_msys(twsi_Addr, twsi_Reg, moreThen256) >> field_Offs) & bit_mask;

				twsi_Addr = (silt == SILT_BC2) ? 0x4e : 0x4f;
				field_Offs = 0;
				bit_mask = 0x1;

				satr_reg = satr_reg |
					(((tread_msys(twsi_Addr, twsi_Reg, moreThen256) >> field_Offs) & bit_mask) << 2);

				printf("MSYS S@R freq = 0x%x\n", satr_reg);

				*value = satr_reg;
				return 0;
		}

		else if (strcmp(cmd, "tmfreq") == 0) {
				twsi_Addr = (silt == SILT_BC2) ? 0x4e : 0x4c;
				field_Offs = (silt == SILT_BC2) ? 1 : 0;
				bit_mask = 0x7;
		}

		else if (strcmp(cmd, "bootsrc") == 0) {
				twsi_Addr = 0x4f;
				field_Offs = (silt == SILT_BC2) ? 0 : 1;
				bit_mask = 0x7;
		}

		else if (strcmp(cmd, "jtagcpu") == 0) {
				twsi_Addr = (silt == SILT_BC2) ? 0x4f : 0x4d;
				field_Offs = 3;
				bit_mask = 0x1;
		}

		else if (strcmp(cmd, "ptppll") == 0 && silt == SILT_BC2) {
				twsi_Addr = 0x4f;
				field_Offs = 4;
				bit_mask = 0x1;
		}

		else if (strcmp(cmd, "boardid") == 0) {
						twsi_Addr = 0x50;
						field_Offs = 0;
						bit_mask = 0x7;
						twsi_Reg = 7;
						moreThen256 = MV_TRUE;
		}

		else if (strcmp(cmd, "forcegen1") == 0 &&
				(silt == SILT_BC2 || silt == SILT_BOBK)) {
				twsi_Addr = 0x57;
				field_Offs = 2;
				bit_mask = 0x1;
				twsi_Reg = 3;
				moreThen256 = MV_TRUE;
		}

		else {
				printf("'%s' is not supported by %s board SAR\n",
					cmd, chipStr[silt]);
				return -1;
		}

		satr_reg = (tread_msys(twsi_Addr, twsi_Reg, moreThen256) >> field_Offs) & bit_mask;
		printf("MSYS S@R %s = 0x%02x\n", cmd, satr_reg);

		*value = satr_reg;
		return 0;
}

static int do_sar_write_msys(int silt, int argc, char *const argv[])
{
		const char *cmd;
		volatile uint satr_reg;
		volatile int bit_mask;
		uint data;
		uint twsi_Addr = 0;
		uint twsi_Reg = 0;
		uint field_Offs = 0;
		MV_BOOL moreThen256 = MV_FALSE;
		MV_U8 write_Mask = 0x1f;

		cmd = argv[0];
		data = simple_strtoul(argv[1], NULL, 16);

		if (strcmp(cmd, "devid") == 0) {
				twsi_Addr = (silt == SILT_BC2) ? 0x4c : 0x4e;
				field_Offs = 0;
				bit_mask = 0x1f;
		}

		else if (strcmp(cmd, "coreclock") == 0) {
				twsi_Addr = 0x4d;
				field_Offs = 0;
				bit_mask = 0x7;
		}

		else if (strcmp(cmd, "freq") == 0) {
				twsi_Addr = (silt == SILT_BC2) ? 0x4d : 0x4c;
				field_Offs = 3;
				bit_mask = 0x3;

				satr_reg = tread_msys(twsi_Addr, 0, moreThen256) & 0x1f;
				satr_reg = (satr_reg & (~(bit_mask << field_Offs))) | ((data & bit_mask) << field_Offs);
				twrite_msys(twsi_Addr, twsi_Reg, (MV_U8)satr_reg, moreThen256);

				data = data >> 2;
				twsi_Addr = (silt == SILT_BC2) ? 0x4e : 0x4f;
				field_Offs = 0;
				bit_mask = 0x1;
		}

		else if (strcmp(cmd, "tmfreq") == 0) {
				twsi_Addr = (silt == SILT_BC2) ? 0x4e : 0x4c;
				field_Offs = (silt == SILT_BC2) ? 1 : 0;
				bit_mask = 0x7;
		}

		else if (strcmp(cmd, "bootsrc") == 0) {
				twsi_Addr = 0x4f;
				field_Offs = (silt == SILT_BC2) ? 0 : 1;
				bit_mask = 0x7;
		}

		else if (strcmp(cmd, "jtagcpu") == 0) {
				twsi_Addr = (silt == SILT_BC2) ? 0x4f : 0x4d;
				field_Offs = 3;
				bit_mask = 0x1;
		}

		else if (strcmp(cmd, "ptppll") == 0 && silt == SILT_BC2) {
				twsi_Addr = 0x4f;
				field_Offs = 4;
				bit_mask = 0x1;
		}

		else if (strcmp(cmd, "boardid") == 0) {
				twsi_Addr = 0x50;
				bit_mask = 0x7;
				twsi_Reg = 7;
				moreThen256 = MV_TRUE;
				write_Mask = 0xff;
		}

		else if (strcmp(cmd, "forcegen1") == 0 &&
				(silt == SILT_BC2 || silt == SILT_BOBK)) {
				twsi_Addr = 0x57;
				field_Offs = 2;
				bit_mask = 0x1;
				twsi_Reg = 3;
				moreThen256 = MV_TRUE;
				write_Mask = 0xff;
		}

		else {
				printf("'%s' is not supported by %s board SAR\n",
					cmd, chipStr[silt]);
				return -1;
		}

		satr_reg = tread_msys(twsi_Addr, 0, moreThen256) & write_Mask;
		satr_reg = (satr_reg & (~(bit_mask << field_Offs))) | ((data & bit_mask) << field_Offs);
		twrite_msys(twsi_Addr, twsi_Reg, (MV_U8)satr_reg, moreThen256);

		return 0;
}



MV_STATUS check_twsi_msys(void)
{
		MV_U8 reg = tread_msys(0x4c, 0, MV_FALSE);
		DB(printf("\ncheck_twsi_msys: read_MSYS= 0x%x\n", reg));
		if (reg == 0xff)
				return MV_ERROR;
		else
				return MV_OK;
}

/* Now only support BC2 and BOBK, AC3 only support "boardid" field */
int do_sar_msys(cmd_tbl_t * cmdtp, int flag, int silt, int argc, char * const argv[])
{

		const char *cmd;
		uint value;

		/* need at least two arguments */
		if (argc < 2)
				goto usage;

		cmd = argv[1];

		if (mvBoardEnableExternalI2C() == MV_ERROR) {
				printf("MSYS twsi channel not enabled\n");
				return 1;
		}

		if (check_twsi_msys() == MV_ERROR) {
				printf("MSYS twsi channel not connected\n");
				return 1;
		}

		/* Only support "boardid" field on AC3 */
		if (silt == SILT_AC3 && (strcmp(*(argv + 2), "boardid") != 0)) {
				printf("Only support \"boardid\" filed on AC3 boards\n");
				goto usage;
		}

		if (strcmp(cmd, "list") == 0)
				return do_sar_list_msys(silt, argc - 2, argv + 2);

		else if (strcmp(cmd, "read") == 0)
				return do_sar_read_msys(silt, &value, argc - 2, argv + 2);


		else if (strcmp(cmd, "write") == 0) {
				if (do_sar_write_msys(silt, argc - 2, argv + 2) == 0)
						return do_sar_read_msys(silt, &value, argc - 2, argv + 2);

				return -1;

		}

		usage:
		printf("\n");
		if (silt == SILT_AC3)
				printf("AC3 DB Sample At Reset sub-system\n");
		else if (silt == SILT_BC2)
				printf("BC2 DB Sample At Reset sub-system\n");
		else if (silt == SILT_BOBK)
				printf("BOBK DB Sample At Reset sub-system\n");

		if (silt == SILT_BC2 || silt == SILT_BOBK) {
				printf("SatR list coreclock  - print coreclock list\n");
				printf("SatR list freq   - print freq list\n");
				printf("SatR list tmfreq    - print tmfreq list\n");
				printf("SatR list bootsrc   - print boolsel list\n");
				printf("SatR list jtagcpu   - print jtagcpu list\n");
				printf("SatR list forcegen1  - print forcegen1 list\n");
		}
		if (silt == SILT_BC2)
				printf("SatR list ptppll    - print ptppll list\n");

		printf("SatR list boardid  - print boardid list\n");
		printf("\n");

		if (silt == SILT_BC2 || silt == SILT_BOBK) {
				printf("SatR read devid     - read device id\n");
				printf("SatR read coreclock  - read CORE frequency\n");
				printf("SatR read freq   - read CPU frequency\n");
				printf("SatR read tmfreq    - read TM frequency\n");
				printf("SatR read bootsrc   - read Boot select\n");
				printf("SatR read jtagcpu   - read JTAG CPU selection\n");
		}
		if (silt == SILT_BC2) {
				printf("SatR read ptppll    - read PTP PLL setting\n");
				printf("SatR read forcegen1  - read Force PCIe GEN1 setting\n");
		}
		printf("SatR read boardid      - read Board ID\n");
		if (silt == SILT_BC2 || silt == SILT_BOBK)
				printf("SatR read dump      - read all values\n");
		printf("\n");

		if (silt == SILT_BC2 || silt == SILT_BOBK) {
				printf("SatR write devid    <val> - write device id\n");
				printf("SatR write coreclock <val> - write CORE frequency\n");
				printf("SatR write freq  <val> - write CPU  frequency\n");
				printf("SatR write tmfreq   <val> - write TM frequency\n");
				printf("SatR write bootsrc  <val> - write Boot select\n");
				printf("SatR write jtagcpu  <val> - write JTAG CPU selection\n");
		}
		if (silt == SILT_BC2) {
				printf("SatR write ptppll   <val> - write PTP PLL setting\n");
				printf("SatR write forcegen1 <val> - write Force PCIe GEN1 setting\n");
		}

		printf("SatR write boardid   <val> - write Board ID\n");
		return 1;
}

#ifndef MV_MSYS
/* This function used for AMC + remote MSYS case. The MSYS standalone implementation differs */
static int do_qsgmii_sel(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

		int bit_mask;

		if (argc < 2)
				goto usage;

		if (mvBoardEnableExternalI2C() == MV_ERROR) {
				printf("MSYS twsi channel not enabled\n");
				return 1;
		}

		if (check_twsi_msys() == MV_ERROR) {
				printf("MSYS twsi channel not connected\n");
				return 1;
		}

		bit_mask = (simple_strtoul(argv[1], NULL, 16) & 0x0000ffff);

		twrite_msys(0x20, 2, (MV_U8)((bit_mask >> 0) & 0xff), MV_FALSE);
		twrite_msys(0x20, 3, (MV_U8)((bit_mask >> 8) & 0xff), MV_FALSE);
		twrite_msys(0x20, 6, 0x0, MV_FALSE);
		twrite_msys(0x20, 7, 0x0, MV_FALSE);
		return 1;

		usage:
		cmd_usage(cmdtp);
		return 1;
}


U_BOOT_CMD(
		  qsgmii_sel,      2,     1,      do_qsgmii_sel,
		  " Select SFP or QSGMII modes on bc2/bobk.\n",
		  " apply 16 bit array to select SFP or QSGMII modes"
		  );

#endif


