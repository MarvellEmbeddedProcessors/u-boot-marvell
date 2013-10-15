/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */
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

/* PCI.c - PCI functions */
#include "mvCommon.h"
#include <common.h>
#include <config.h>
#include <command.h>
#if defined(CONFIG_CMD_PCI)

#include <pci.h>
#include "mvSysPexApi.h"
#include "pex/mvPexRegs.h"
#include "gpp/mvGpp.h"
//#include "pci-if/mvPciIf.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "ctrlEnv/sys/mvCpuIf.h"
#include "pci-if/pci_util/mvPciUtils.h"

#undef DEBUG
#ifdef DEBUG
#define DB(x) x
#else
#define DB(x)
#endif /* DEBUG */

/* global definetion */
#define REG_NUM_MASK    (0x3FF << 2)

/* global indicate wether we are in the scan process */
unsigned int scan_in_progress = 0;
extern unsigned int whoAmI(void);

#if defined(CONFIG_CMD_BSP)
/******************************************************************************
 * Category     - PCI0
 * Functionality- Scans PCI0 for devices and prints relevant information
 * Need modifications (Yes/No) - No
 *****************************************************************************/
MV_BOOL scanPci(MV_U32 host)
{
	MV_U32 index, numOfElements = 4 * 8, barIndex;
	MV_PCI_DEVICE pciDevices[4 * 8];    //3 slots and us,Max 8 functions per slot

	memset(&pciDevices, 0, 12 * sizeof(MV_PCI_DEVICE));

	if (mvPciScan(host, pciDevices, &numOfElements) != MV_OK) {
		DB(printf("scanPci:mvPciScan failed for host %d \n", host));
		return MV_FALSE;
	}

	for (index = 0; index < numOfElements; index++) {
		printf("\nBus: %x Device: %x Func: %x Vendor ID: %x Device ID: %x\n",
		       pciDevices[index].busNumber,
		       pciDevices[index].deviceNum,
		       pciDevices[index].function,
		       pciDevices[index].venID,
		       pciDevices[index].deviceID);

		printf("-------------------------------------------------------------------\n");

		printf("Class: %s\n", pciDevices[index].type);

		/* check if we are bridge*/
		if ((pciDevices[index].baseClassCode == PCI_BRIDGE_CLASS) &&
		    (pciDevices[index].subClassCode == P2P_BRIDGE_SUB_CLASS_CODE)) {
			printf("Primary Bus:0x%x \tSecondary Bus:0x%x \tSubordinate Bus:0x%x\n",
			       pciDevices[index].p2pPrimBusNum,
			       pciDevices[index].p2pSecBusNum,
			       pciDevices[index].p2pSubBusNum);

			printf("IO Base:0x%x \t\tIO Limit:0x%x", pciDevices[index].p2pIObase,
			       pciDevices[index].p2pIOLimit);

			(pciDevices[index].bIO32) ? (printf(" (32Bit IO)\n")) :
			(printf(" (16Bit IO)\n"));

			printf("Memory Base:0x%x \tMemory Limit:0x%x\n", pciDevices[index].p2pMemBase,
			       pciDevices[index].p2pMemLimit);

			printf("Pref Memory Base:0x%x \tPref Memory Limit:0x%x",
			       pciDevices[index].p2pPrefMemBase,
			       pciDevices[index].p2pPrefMemLimit);

			(pciDevices[index].bPrefMem64) ? (printf(" (64Bit PrefMem)\n")) :
			(printf(" (32Bit PrefMem)\n"));
			if (pciDevices[index].bPrefMem64) {
				printf("Pref Base Upper 32bit:0x%x \tPref Limit Base Upper32 bit:0x%x\n",
				       pciDevices[index].p2pPrefBaseUpper32Bits,
				       pciDevices[index].p2pPrefLimitUpper32Bits);
			}
		}

		for (barIndex = 0; barIndex < pciDevices[index].barsNum; barIndex++) {
			if (pciDevices[index].pciBar[barIndex].barType == PCI_64BIT_BAR) {
				printf("PCI_BAR%d (%s-%s) base: %x%08x%s", barIndex,
				       (pciDevices[index].pciBar[barIndex].barMapping == PCI_MEMORY_BAR) ? "Mem" : "I/O",
				       "64bit",
				       pciDevices[index].pciBar[barIndex].barBaseHigh,
				       pciDevices[index].pciBar[barIndex].barBaseLow,
				       (pciDevices[index].pciBar[barIndex].barBaseLow == 0) ? "\t\t" : "\t");
			}else if (pciDevices[index].pciBar[barIndex].barType == PCI_32BIT_BAR) {
				printf("PCI_BAR%d (%s-%s) base: %x%s", barIndex,
				       (pciDevices[index].pciBar[barIndex].barMapping == PCI_MEMORY_BAR) ? "Mem" : "I/O",
				       "32bit",
				       pciDevices[index].pciBar[barIndex].barBaseLow,
				       (pciDevices[index].pciBar[barIndex].barBaseLow == 0) ? "\t\t\t" : "\t\t");
			}

			if (pciDevices[index].pciBar[barIndex].barSizeHigh != 0)
				printf("size: %d%08d bytes\n", pciDevices[index].pciBar[barIndex].barSizeHigh,
				       pciDevices[index].pciBar[barIndex].barSizeLow);
			else
				printf("size: %dMb\n", pciDevices[index].pciBar[barIndex].barSizeLow / 1024 / 1024);
		}
	}
	return MV_TRUE;
}

int sp_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	MV_U32 host = 0;
#if defined(MV88F68XX)
	MV_BOARD_PEX_INFO 	*boardPexInfo = mvBoardPexInfoGet();
#endif
	MV_U32 pexHWInf = 0;

	if (argc > 1)
		host = simple_strtoul(argv[1], NULL, 10);

	if (host >= mvCtrlPexMaxIfGet()) {
		printf("PCI %d doesn't exist\n", host);
		return 1;
	}

#if defined(MV88F68XX)
	pexHWInf = boardPexInfo->pexMapping[host];
#else
	pexHWInf = host;
#endif

	printf("scanning pex number: %d\n", pexHWInf);
	if ( scanPci(pexHWInf) == MV_FALSE)
		printf("PCI %d Scan - FAILED!!.\n", host);
	return 1;
}

U_BOOT_CMD(
	sp,      2,     1,      sp_cmd,
	"sp	- Scan PCI Interface [bus].\n",
	"\tScan and detect all devices on mvPCI Interface \n"
	);

int me_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	MV_U32 host = 0;

	if (argc > 1)
		host = simple_strtoul(argv[1], NULL, 10);

	if (host >= mvCtrlPexMaxIfGet()) {
		printf("Master %d doesn't exist\n", host);
		return 1;
	}

	if (mvPexMasterEnable(host, MV_TRUE)  == MV_OK)
		printf("PCI %d Master enabled.\n", host);
	else
		printf("PCI %d Master enabled -FAILED!!\n", host);

	return 1;
}

U_BOOT_CMD(
	me,      2,      1,      me_cmd,
	"me	- PCI master enable\n",
	" [0/1] \n"
	"\tEnable the MV device as Master on PCI 0/1. \n"
	);

int se_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	MV_U32 host = 0, dev = 0, bus = 0;
#if defined(MV88F68XX)
	MV_BOARD_PEX_INFO 	*boardPexInfo = mvBoardPexInfoGet();
#endif
	MV_U32 pexHWInf = 0;

	if (argc != 4) {
		printf("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	host = simple_strtoul(argv[1], NULL, 10);
	bus = simple_strtoul(argv[2], NULL, 16);
	dev = simple_strtoul(argv[3], NULL, 16);

	if (host >= mvCtrlPexMaxIfGet()) {
		printf("PCI %d doesn't exist\n", host);
		return 1;
	}

#if defined(MV88F68XX)
	pexHWInf = boardPexInfo->pexMapping[host];
#else
	pexHWInf = host;
#endif

	if (mvPexSlaveEnable(pexHWInf, bus, dev, MV_TRUE) == MV_OK)
		printf("PCI %d Bus %d Slave 0x%x enabled.\n", host, bus, dev);
	else
		printf("PCI %d Bus %d Slave 0x%x enabled - FAILED!!.\n", host, bus, dev);
	return 1;
}

U_BOOT_CMD(
	se,      4,     1,      se_cmd,
	"se	- PCI Slave enable\n",
	" [0/1] bus dev \n"
	"\tEnable the PCI device as Slave on PCI 0/1. \n"
	);

/******************************************************************************
 * Functionality- The commands changes the pci remap register and displays the
 *                address to be used in order to access PCI 0.
 *****************************************************************************/
int mapPci_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	MV_ADDR_WIN pciWin;
	MV_TARGET target = 0;
	MV_U32 host = 0, effectiveBaseAddress = 0;
#if defined(MV88F68XX)
	MV_BOARD_PEX_INFO 	*boardPexInfo = mvBoardPexInfoGet();
#endif
	MV_U32 pexHWInf = 0;

	pciWin.baseLow = 0;
	pciWin.baseHigh = 0;

	if (argc > 1)
		host = simple_strtoul(argv[1], NULL, 10);

	if (argc > 2)
		pciWin.baseLow = simple_strtoul(argv[2], NULL, 16);

	if (host >= mvCtrlPexMaxIfGet()) {
		printf("PCI %d doesn't exist\n", host);
		return 1;
	}

#if defined(MV88F68XX)
	pexHWInf = boardPexInfo->pexMapping[host];
#else
	pexHWInf = host;
#endif

	target = PCI0_MEM0 + (2 * pexHWInf);

	printf("mapping pci %x to address 0x%x\n", host, pciWin.baseLow);

#if defined(MV_INCLUDE_PEX) || defined(MV_INCLUDE_PCI)
	effectiveBaseAddress = mvCpuIfPexRemap(target, &pciWin);
#endif

	if ( effectiveBaseAddress == 0xffffffff) {
		printf("Error remapping\n");
		return 1;
	}

	printf("PCI %x Access base address : %x\n", host, effectiveBaseAddress);
	return 1;
}

U_BOOT_CMD(
	mp,      3,     1,      mapPci_cmd,
	"mp	- map PCI BAR\n",
	" [0/1] address \n"
	"\tChange the remap of PCI 0/1 window 0 to address 'addrress'.\n"
	"\tIt also displays the new access address, since the remap is not always\n"
	"\tthe same as requested. \n"
	);

#endif

MV_U32 mv_mem_ctrl_dev(MV_U32 pexIf, MV_U32 bus, MV_U32 dev)
{
	MV_U32 ven, class;

	ven =    mvPexConfigRead(pexIf, bus, dev, 0, PCI_VENDOR_ID) & 0xffff;
	class = (mvPexConfigRead(pexIf, bus, dev, 0, PCI_REVISION_ID) >> 16 ) & 0xffff;
	/* if we got any other Marvell PCI cards ignore it. */
	if (((ven == 0x11ab) && (class == PCI_CLASS_MEMORY_OTHER)) ||
	    ((ven == 0x11ab) && (class == PCI_CLASS_BRIDGE_HOST)))
		return 1;
	return 0;
}

static int mv_read_config_dword(struct pci_controller *hose,
				pci_dev_t dev,
				int offset, u32* value)
{
	MV_U32 bus, func, regOff, dev_no;
	char *env;

	bus = PCI_BUS(dev);
	dev_no = PCI_DEV(dev);

	func = (MV_U32)PCI_FUNC(dev);
	regOff = (MV_U32)offset & REG_NUM_MASK;

	/*  We will scan only ourselves and the PCI slots that exist on the
	        board, because we may have a case that we have one slot that has
	        a Cardbus connector, and because CardBus answers all IDsels we want
	        to scan only this slot and ourseleves.
	 */

	if ( scan_in_progress == 1) {
		env = getenv("disaMvPnp");
		if (env && ( (strcmp(env, "yes") == 0) || (strcmp(env, "Yes") == 0) ) ) {
			if ( mv_mem_ctrl_dev((MV_U32)hose->cfg_addr, bus, dev_no) ) {
				*value = 0xffffffff;
				return 0;
			}
		}
	}

	*value = (u32)mvPexConfigRead((MV_U32)hose->cfg_addr, bus, dev_no,
				      func, regOff);

	DB(printf("PEX%d CFG READ bus=%d dev=%d func=%d reg=%x val=%x\n",
		  hose->cfg_addr, bus, dev_no, func, regOff, *value));

	return 0;
}

static int mv_write_config_dword(struct pci_controller *hose,
				 pci_dev_t dev,
				 int offset, u32 value)
{
	MV_U32 bus, func, regOff, dev_no;

	bus = PCI_BUS(dev);
	dev_no = PCI_DEV(dev);
	func = (MV_U32)PCI_FUNC(dev);
	regOff = offset & REG_NUM_MASK;
	mvPexConfigWrite((MV_U32)hose->cfg_addr, bus, dev_no, func, regOff, value);

	DB(printf("PEX%d CFG WRITE bus=%d dev=%d func=%d reg=%x val=%x\n",
		  hose->cfg_addr, bus, dev_no, func, regOff, value));

	return 0;
}

static void mv_setup_ide(struct pci_controller *hose,
			 pci_dev_t dev, struct pci_config_table *entry)
{
	static const int ide_bar[] = { 8, 4, 8, 4, 16, 1024 };
	u32 bar_response, bar_value;
	int bar;

	for (bar = 0; bar < 6; bar++) {
		/*ronen different function for 3rd bank.*/
		unsigned int offset = (bar < 2) ? bar * 8 : 0x100 + (bar - 2) * 8;

		pci_write_config_dword(dev, PCI_BASE_ADDRESS_0 + offset, 0x0);
		pci_read_config_dword(dev, PCI_BASE_ADDRESS_0 + offset, &bar_response);

		pciauto_region_allocate(bar_response & PCI_BASE_ADDRESS_SPACE_IO ?
					hose->pci_io : hose->pci_mem, ide_bar[bar], &bar_value);

		pci_write_config_dword(dev, PCI_BASE_ADDRESS_0 + bar * 4, bar_value);
	}
}

static void mv_setup_host(struct pci_controller *hose,
			  pci_dev_t dev, struct pci_config_table *entry)
{
	//skip our host
	DB(printf("skipping :bus=%x dev=%x fun=%x\n",
		  (unsigned int)PCI_BUS(dev),
		  (unsigned int)PCI_DEV(dev),
		  (unsigned int)PCI_FUNC(dev)));
	return;
}

static void mv_pci_bus_mode_display(MV_U32 host)
{
#if defined(MV_INCLUDE_PEX)

	MV_PEX_MODE pexMode;
	MV_U32 pexHWInf;
	MV_32 linkDelayCount;
#if defined(MV88F68XX)
	MV_BOARD_PEX_INFO 	*boardPexInfo = mvBoardPexInfoGet();
	if (boardPexInfo == NULL) {
		printf("mv_pci_bus_mode_display: mvBoardPexInfoGet failed\n");
		return;
	}
	pexHWInf = boardPexInfo->pexMapping[host];
#else
	pexHWInf = host;
#endif

	if (mvPexModeGet(pexHWInf, &pexMode) != MV_OK)
		printf("mv_pci_bus_mode_display: mvPexModeGet failed\n");

	printf("PEX %d: ", host);

	switch (pexMode.pexType) {
	case MV_PEX_ROOT_COMPLEX:
		printf("Root Complex Interface");
		break;
	case MV_PEX_END_POINT:
		printf("End Point Interface");
		break;
	}
	linkDelayCount = 2000;
	while (!(pexMode.pexLinkUp) && (linkDelayCount)) {
		mvOsDelay(1);
		linkDelayCount--;
		if (mvPexModeGet(pexHWInf, &pexMode) != MV_OK) {
			printf("mv_pci_bus_mode_display: mvPexModeGet failed\n");
			break;
		}
	}

	/* Check if we have link */
	if (!(pexMode.pexLinkUp))
		printf(", no Link.\n");
	else{
		if (MV_PEX_WITDH_X1 ==  pexMode.pexWidth)
			printf(", Detected Link X1");
		else if (MV_PEX_WITDH_X4 ==  pexMode.pexWidth)
			printf(", Detected Link X4");
		if (MV_PEX_GEN2_0 ==  pexMode.pexGen)
			printf(", GEN 2.0\n");
		else if (MV_PEX_GEN1_1 ==  pexMode.pexGen)
			printf(", GEN 1.1\n");

	}

	return;

	#endif /* MV_INCLUDE_PEX */
}

struct pci_config_table mv_config_table[] = {
	{ PCI_ANY_ID, PCI_ANY_ID, PCI_CLASS_STORAGE_IDE,
	  PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID, mv_setup_ide },

	{ PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID,
	  PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID, mv_setup_host }, //PCI host

	{}

};

/* Defines for more modularity of the pci_init_board function */

struct pci_controller pci_hose[MV_PEX_MAX_IF];

#define PCI_IF_MEM(pexIf)       ((pexIf == 0) ? PCI_IF0_MEM0 : PCI_IF1_MEM0)
#define PCI_IF_REMAPED_MEM_BASE(pexIf) ((pexIf == 0) ? PCI_IF0_REMAPED_MEM_BASE : PCI_IF1_REMAPED_MEM_BASE)
#define PCI_IF_MEM_BASE(pexIf)  ((pexIf == 0) ? PCI_IF0_MEM0_BASE : PCI_IF1_MEM0_BASE)
#define PCI_IF_MEM_SIZE(pexIf)  ((pexIf == 0) ? PCI_IF0_MEM0_SIZE : PCI_IF1_MEM0_SIZE)
#define PCI_IF_IO_BASE(pexIf)   ((pexIf == 0) ? PCI_IF0_IO_BASE : PCI_IF1_IO_BASE)
#define PCI_IF_IO_SIZE(pexIf)   ((pexIf == 0) ? PCI_IF0_IO_SIZE : PCI_IF1_IO_SIZE)

/* because of CIV team needs we are gonna do a remap to PCI memory */
#define PCI_IF0_REMAPED_MEM_BASE        0x40000000
#define PCI_IF1_REMAPED_MEM_BASE        0x40000000

int pci_skip_dev(struct pci_controller *hose, pci_dev_t dev)
{
	return 0;
}

void pci_init_board(void)
{
	/* Pex scan skipped by secondary CPUs*/
	if (whoAmI() != 0)
		return;

	MV_U32 activePexCount;
	MV_ADDR_WIN rempWin;
	MV_CPU_DEC_WIN cpuAddrDecWin;
	PCI_IF_MODE pexIfMode = PCI_IF_MODE_HOST;
	MV_U32 pexHWInf, pexIf;
	struct pci_controller *pci;
	char *env;
	int status;
#ifdef MV88F68XX
	MV_BOARD_PEX_INFO *boardPexInfo = mvBoardPexInfoGet();
#endif

	if (mvCtrlPexMaxIfGet() == 0)
		return;

	env = getenv("disaMvPnp");

	if (env && ( (strcmp(env, "yes") == 0) || (strcmp(env, "Yes") == 0) ) )
		printf("Warning: skip configuration of Marvell devices!!!\n");

	activePexCount = mvCtrlSocUnitInfoNumGet(PEX_UNIT_ID);
	DB(printf("Start scan of %d PEX interfaces\n", activePexCount));

	/* Initialize and scan all PEX interfaces */
	for (pexIf = 0; pexIf < activePexCount; pexIf++) {
		pci = &pci_hose[pexIf];
#if defined(MV88F68XX)
		pexHWInf = boardPexInfo->pexMapping[pexIf];
#else
		pexHWInf = pexIf;
#endif
		DB(printf("Starting scan of PEX%d\n", pexHWInf));

		/* Set bus numbers in U-BOOT stack */
		if (pexIf == 0) {
			pci->first_busno = 0;
			pci->last_busno = 0;
		}else  {
			pci->first_busno = pci_hose[pexIf - 1].last_busno + 1;
			pci->last_busno = pci->first_busno;
		}

		pci->config_table = mv_config_table;

		/* Check if PEX IF is powered */
		if (MV_FALSE == mvCtrlPwrClckGet(PEX_UNIT_ID, pexHWInf))
			continue;

		/* Set device or host mode */
#if defined(MV_INCLUDE_PEX)
		/* Set pex mode incase S@R not exist */
		env = getenv("pexMode");
		if (env && (((strcmp(env, "EP") == 0) || (strcmp(env, "ep") == 0) )))
			pexIfMode = MV_PEX_END_POINT;
		else
			pexIfMode = MV_PEX_ROOT_COMPLEX;
#endif

#if defined(DB_78X60_PCAC) || defined(DB_78X60_PCAC_REV2) || defined(DB_88F6710_PCAC)
		pexIfMode = MV_PEX_END_POINT;
#endif

#if defined(DB_78X60_AMC)
		pexIfMode = MV_PEX_ROOT_COMPLEX;
#endif

		DB(printf("Initializing HAL\n"));

		status = mvSysPexInit(pexHWInf, pexIfMode);
		if (status == MV_ERROR)
			printf("pci_init_board:Error calling mvPexIfInit for PEX%d.%d(%d)\n", pexHWInf / 4, pexHWInf % 4, pexIf);
		else {
			if (status == MV_OK) {
				/* Link detected. Set U-BOOT scan parameters */
				pci->current_busno = pci->first_busno;
				pci->last_busno = 0xff;

				/*
				 * Set device number to 1 to enable detecting
				 * Devices that answer only when device is 0
				 */
				mvPexLocalDevNumSet(pexHWInf, 1);

				/* Set bus No based on previous scan results */
				if (mvPexLocalBusNumSet(pexHWInf, pci->first_busno) != MV_OK)
					printf("pci_init_board:Error calling mvPexLocalBusNumSet for pexIf %d\n", pexIf);
			}else  {
				/* Interface with no link */
				printf("PEX %d: Detected No Link.\n", pexIf);
				continue;
			}
		}

		/* Print PEX mode, Lane count and GEN*/
		mv_pci_bus_mode_display(pexIf);

		/* Skip scan if link is down */
		if (status == MV_NO_SUCH) {
			pci->last_busno = pci->first_busno;
			continue;
		}

		/* Get the address decode windows */
		DB(printf("Setting memory regions\n"));
		if (MV_OK != mvCpuIfTargetWinGet(PCI_MEM(pexHWInf, 0), &cpuAddrDecWin)) {
			printf("%s: ERR. mvCpuIfTargetWinGet failed\n", __FUNCTION__);
			return;
		}

		rempWin.baseLow = ((cpuAddrDecWin.addrWin.baseLow & 0x0fffffff) | PCI_IF_REMAPED_MEM_BASE(pexHWInf));
		rempWin.baseHigh = 0;

		/* Perform a remap for the PEX0 interface */
		if (0xffffffff == mvCpuIfPexRemap(PCI_MEM(pexHWInf, 0), &rempWin)) {
			printf("%s:mvCpuIfPexRemap failed, %d\n", __FUNCTION__, pexHWInf);
			return;
		}

		/* PCI memory space */
		pci_set_region(pci->regions + 0,
			       rempWin.baseLow,  /* bus address */
			       cpuAddrDecWin.addrWin.baseLow,
			       cpuAddrDecWin.addrWin.size,
			       PCI_REGION_MEM);

		if (MV_OK != mvCpuIfTargetWinGet(PCI_IO(pexHWInf), &cpuAddrDecWin))
			/* No I/O space */
			pci->region_count = 1;
		else {
			/* PCI I/O space */
			pci_set_region(pci->regions + 1,
				       cpuAddrDecWin.addrWin.baseLow,
				       cpuAddrDecWin.addrWin.baseLow,
				       cpuAddrDecWin.addrWin.size,
				       PCI_REGION_IO);
			pci->region_count = 2;
		}

		/* Connect to U-BOOT PCI stack */
		pci_set_ops(pci,
			    pci_hose_read_config_byte_via_dword,
			    pci_hose_read_config_word_via_dword,
			    mv_read_config_dword,
			    pci_hose_write_config_byte_via_dword,
			    pci_hose_write_config_word_via_dword,
			    mv_write_config_dword);

		pci->cfg_addr = (unsigned int*)pexHWInf;

		pci->config_table[1].bus = mvPexLocalBusNumGet(pexHWInf);
		pci->config_table[1].dev = mvPexLocalDevNumGet(pexHWInf);

		pci_register_hose(pci);

		if (pexIfMode == PCI_IF_MODE_HOST) {
			/* Perform the PCI bus scan */
			DB(printf("Starting bus scan\n"));
			scan_in_progress = 1;
			pci->last_busno = pci_hose_scan(pci);
			scan_in_progress = 0;

			/*pci_fixup_bridge(pci, pexHWInf, pexIf);*/

		}else
			pci->last_busno = pci->first_busno;
	}

	DB(printf("Completed PEX scan\n"));
#ifdef DB_FPGA
	MV_REG_BIT_RESET(PCI_BASE_ADDR_ENABLE_REG(0), BIT10);
#endif
}

#endif /* CONFIG_PCI */
