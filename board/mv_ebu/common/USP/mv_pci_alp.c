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

#include "mvCommon.h"
#include <common.h>
#include <config.h>
#include <command.h>

#if defined(CONFIG_CMD_PCI)

#include <pci.h>
#include "mvSysPexApi.h"
#include "pex/mvPexRegs.h"
#include "gpp/mvGpp.h"
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

static inline mvPciBarType(unsigned int pci_bar_type)
{
	if (pci_bar_type == PCI_MEMORY_BAR)
		return "Mem";
	else if (pci_bar_type == PCI_32BIT_BAR)
		return "I/O";
	else
		return "Err";
}

static void mvPciPrintBars(MV_PCI_DEVICE *pci_dev)
{
	MV_PCI_DEVICE *d = pci_dev;
	PCI_BAR *b;
	MV_U32 i;

	for (i = 0; i < d->barsNum; i++) {
		b = &d->pciBar[i];

		if (b->barType == PCI_64BIT_BAR) {
			printf("PCI_BAR%d (%s-%s) base: %x%08x%s", i,
			       mvPciBarType(b->barMapping),
			       "64bit", b->barBaseHigh, b->barBaseLow,
			       (b->barBaseLow == 0) ? "\t\t" : "\t");

		} else if (b->barType == PCI_32BIT_BAR) {
			printf("PCI_BAR%d (%s-%s) base: %x%s", i,
			       mvPciBarType(b->barMapping),
			       "32bit", b->barBaseLow,
			       (b->barBaseLow == 0) ? "\t\t\t" : "\t\t");
		}

		if (b->barSizeHigh != 0)
			printf("size: %d%08d bytes\n", b->barSizeHigh,
					b->barSizeLow);
		else
			printf("size: %dMb\n", b->barSizeLow / 1024 / 1024);
	}
}

static void mvPciPrintScannedInfo(MV_PCI_DEVICE *pci_devs, MV_U32 n)
{
	MV_PCI_DEVICE *d;
	MV_U32 i;

	for (i = 0; i < n; i++) {
		d = &pci_devs[i];

		printf("\nBus: %x Device: %x Func: %x Vendor ID: %x Device ID: %x\n",
		       d->busNumber, d->deviceNum, d->function, d->venID, d->deviceID);

		printf("-------------------------------------------------------------------\n");
		printf("Class: %s\n", d->type);

		/* check if we are bridge*/
		if (d->baseClassCode == PCI_BRIDGE_CLASS &&
		    d->subClassCode == P2P_BRIDGE_SUB_CLASS_CODE) {
			printf("Primary Bus:0x%x \tSecondary Bus:0x%x \tSubordinate Bus:0x%x\n",
			       d->p2pPrimBusNum, d->p2pSecBusNum, d->p2pSubBusNum);

			printf("IO Base:0x%x \t\tIO Limit:0x%x",
				d->p2pIObase, d->p2pIOLimit);

			if (d->bIO32)
				printf(" (32Bit IO)\n");
			else
				printf(" (16Bit IO)\n");

			printf("Memory Base:0x%x \tMemory Limit:0x%x\n",
				d->p2pMemBase, d->p2pMemLimit);

			printf("Pref Memory Base:0x%x \tPref Memory Limit:0x%x",
			       d->p2pPrefMemBase, d->p2pPrefMemLimit);

			if (d->bPrefMem64)
				printf(" (64Bit PrefMem)\n");
			else
				printf(" (32Bit PrefMem)\n");

			if (d->bPrefMem64) {
				printf("Pref Base Upper 32bit:0x%x \tPref Limit Base Upper32 bit:0x%x\n",
				       d->p2pPrefBaseUpper32Bits, d->p2pPrefLimitUpper32Bits);
			}
		}

		mvPciPrintBars(d);
	}
}

MV_BOOL scanPci(MV_U32 host)
{
	MV_U32 index, numOfElements = 4 * 8;
	/* 3 slots and us,Max 8 functions per slot */
	MV_PCI_DEVICE pciDevices[4 * 8];

	memset(&pciDevices, 0, 12 * sizeof(MV_PCI_DEVICE));

	if (mvPciScan(host, pciDevices, &numOfElements) != MV_OK) {
		DB(printf("scanPci:mvPciScan failed for host %d \n", host));
		return MV_FALSE;
	}

	mvPciPrintScannedInfo(pciDevices, numOfElements);
	return MV_TRUE;
}

int sp_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	MV_U32 host = 0;

	if (argc > 1)
		host = simple_strtoul(argv[1], NULL, 10);

	if (host >= mvCtrlPciMaxIfGet()) {
		printf("PCI %d doesn't exist\n", host);
		return 1;
	}

	if (scanPci(0) == MV_FALSE)
		printf("%s: scanPci(%d) failed!\n", __func__, host);

	return 1;
}

U_BOOT_CMD(
	sp,      2,     1,      sp_cmd,
	"sp	- Scan PCI Interface [bus].\n",
	"\tScan and detect all devices on mvPCI Interface \n"
	"\t(This command can be used only if enaMonExt is set!)\n"
	);

int me_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	MV_U32 host = 0;

	if (argc > 1)
		host = simple_strtoul(argv[1], NULL, 10);

	if (host >= mvCtrlPciMaxIfGet()) {
		printf("Master %d doesn't exist\n", host);
		return 1;
	}

	if (mvPciMasterEnable(host, MV_TRUE) == MV_OK)
		printf("PCI %d Master enabled.\n", host);
	else
		printf("%s: mvPciMasterEnable(%d) failed\n", __func__, host);

	return 1;
}

U_BOOT_CMD(
	me,      2,      1,      me_cmd,
	"me	- PCI master enable\n",
	" [0/1] \n"
	"\tEnable the MV device as Master on PCI 0/1. \n"
	);

MV_U32 mv_mem_ctrl_dev(MV_U32 pexIf, MV_U32 bus, MV_U32 dev)
{
	MV_U32 ven, class;

	ven =    mvPciConfigRead(pexIf, bus, dev, 0, PCI_VENDOR_ID) & 0xffff;
	class = (mvPciConfigRead(pexIf, bus, dev, 0, PCI_REVISION_ID) >> 16) & 0xffff;

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

	bus	= PCI_BUS(dev);
	dev_no	= PCI_DEV(dev);
	func	= (MV_U32)PCI_FUNC(dev);
	regOff	= (MV_U32)offset & REG_NUM_MASK;

	DB(printf("%s: hose->cfg_addr %x\n",	__func__, hose->cfg_addr);)
	DB(printf("%s: bus %x\n",		__func__, bus);)
	DB(printf("%s: dev_no %x\n",		__func__, dev_no);)
	DB(printf("%s: func %x\n",		__func__, func);)
	DB(printf("%s: regOff %x\n",		__func__, regOff);)

	*value = (u32)mvPciConfigRead((MV_U32)hose->cfg_addr, bus, dev_no, func, regOff);

	DB(printf("%s: value %x\n", __func__, *value);)
	return 0;
}

static int mv_write_config_dword(struct pci_controller *hose,
				 pci_dev_t dev,
				 int offset, u32 value)
{
	MV_U32 bus, func, regOff, dev_no;

	bus	= PCI_BUS(dev);
	dev_no	= PCI_DEV(dev);
	func	= (MV_U32)PCI_FUNC(dev);
	regOff	= offset & REG_NUM_MASK;

	mvPciConfigWrite((MV_U32)hose->cfg_addr, bus, dev_no, func, regOff, value);
	return 0;
}

static void mv_setup_ide(struct pci_controller *hose,
			 pci_dev_t dev, struct pci_config_table *entry)
{
	static const int ide_bar[] = { 8, 4, 8, 4, 16, 1024 };
	u32 bar_response, bar_value;
	int bar;

	for (bar = 0; bar < 6; bar++) {
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
	/* skip our host */
	DB(printf("skipping :bus=%x dev=%x fun=%x\n",
		  (unsigned int)PCI_BUS(dev),
		  (unsigned int)PCI_DEV(dev),
		  (unsigned int)PCI_FUNC(dev)));
}

struct pci_config_table mv_config_table[] = {
	{
		.vendor		= PCI_ANY_ID,
		.device		= PCI_ANY_ID,
		.class		= PCI_CLASS_STORAGE_IDE,
		.bus		= PCI_ANY_ID,
		.dev		= PCI_ANY_ID,
		.func		= PCI_ANY_ID,
		.config_device	= mv_setup_ide
	},
	{
		.vendor		= PCI_ANY_ID,
		.device		= PCI_ANY_ID,
		.class		= PCI_ANY_ID,
		.bus		= PCI_ANY_ID,
		.dev		= PCI_ANY_ID,
		.func		= PCI_ANY_ID,
		.config_device	= mv_setup_host
	},
	{
		/* empty */
	},
};

#define PCI_IF_REMAPED_MEM_BASE(pexIf)		0x40000000
struct pci_controller pci_hose[8];

void pci_init_board(void)
{
	struct pci_controller *pc = &pci_hose[0];
	ulong pci_data, base_class_code, sub_class_code;
	pci_addr_t remap_base_low;
	phys_addr_t base_low;
	pci_size_t size;

	pc->config_table = mv_config_table;

	MV_REG_WRITE(0x20000, 0x000fd041);
	MV_REG_WRITE(0x20004, 0xf2000000);
	MV_REG_WRITE(0x20008, 0xf2000000);
	MV_REG_WRITE(0x2000c, 0x00000000);

	MV_REG_WRITE(0x20010, 0x03ffd841);
	MV_REG_WRITE(0x20014, 0xe0000000);
	MV_REG_WRITE(0x20018, 0x40000000);
	MV_REG_WRITE(0x2001c, 0x00000000);

	mvPciInit(0, PCI_IF_MODE_HOST);

	pc->first_busno = 0;
	pc->current_busno = 0;
	pc->last_busno = 0xff;

	if (mvPciLocalBusNumSet(0, 0) != MV_OK) {
		printf("%s: mvPciLocalBusNumSet failed\n", __func__, __LINE__);
		return;
	}

	remap_base_low  = 0x40000000;
	base_low        = 0xe0000000;
	size            = 0x4000000; /* 64M */
	pci_set_region(&pc->regions[0], remap_base_low, base_low, size, PCI_REGION_MEM);

	remap_base_low  = 0xf2000000;
	base_low        = 0xf2000000;
	size            = 0x100000; /* 1M */
	pci_set_region(&pc->regions[1], remap_base_low, base_low, size, PCI_REGION_IO);

	pc->region_count = 2;

	pci_set_ops(pc, pci_hose_read_config_byte_via_dword,
		    pci_hose_read_config_word_via_dword,
		    mv_read_config_dword,
		    pci_hose_write_config_byte_via_dword,
		    pci_hose_write_config_word_via_dword,
		    mv_write_config_dword);

	pc->config_table[1].bus = 0;
	pc->config_table[1].dev = 0;

	pci_register_hose(pc);

	pc->last_busno = pci_hose_scan(pc);

	pci_data = mvPciConfigRead(0, pc->first_busno, 1, 0,
				   PCI_CLASS_CODE_AND_REVISION_ID);
	base_class_code = (pci_data & PCCRIR_BASE_CLASS_MASK)
			  >> PCCRIR_BASE_CLASS_OFFS;
	sub_class_code  = (pci_data & PCCRIR_SUB_CLASS_MASK)
			  >> PCCRIR_SUB_CLASS_OFFS;

	if (base_class_code == PCI_BRIDGE_CLASS &&
	    sub_class_code  == P2P_BRIDGE_SUB_CLASS_CODE) {
		pci_data = mvPciConfigRead(0, pc->first_busno, 1, 0,
					   P2P_IO_BASE_LIMIT_SEC_STATUS);
		pci_data &= PIBLSS_SEC_STATUS_MASK;
		pci_data |= 0xff00;
		mvPciConfigWrite(0, pc->first_busno, 1, 0,
				 P2P_IO_BASE_LIMIT_SEC_STATUS, pci_data);
		pci_data = mvPciConfigRead(0, pc->first_busno, 1, 0,
					   P2P_MEM_BASE_LIMIT);
		pci_data = 0xEFF00000 | (PCI_IF_REMAPED_MEM_BASE(0) >> 16);
		mvPciConfigWrite(0, pc->first_busno, 1, 0,
				 P2P_MEM_BASE_LIMIT, pci_data);
	}
}

#endif /* CONFIG_CMD_PCI */

