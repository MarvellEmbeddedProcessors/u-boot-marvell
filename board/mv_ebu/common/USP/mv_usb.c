/*
 * (C) Copyright 2008, Michael Trimarchi <trimarchimichael@yahoo.it>
 *
 * Author: Michael Trimarchi <trimarchimichael@yahoo.it>
 * This code is based on ehci freescale driver
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#if defined(CONFIG_CMD_USB)
#include <usb.h>
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "ctrlEnv/sys/mvCpuIf.h"
#if defined (CONFIG_USB_XHCI)
#include "drivers/usb/host/xhci.h"
#elif defined (CONFIG_USB_EHCI)
#include "drivers/usb/host/ehci.h"
#endif


#if defined (CONFIG_USB_XHCI)

#define USB3_WIN_CTRL(w)	(0x0 + ((w) * 8))
#define USB3_WIN_BASE(w)	(0x4 + ((w) * 8))
#define USB3_MAX_WINDOWS	4
#define USB3_XHCI_REGS_SIZE	_64K

/*******************************************************************************
* mvUsb3WinInit -
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
*       MV_ERROR if register parameters are invalid.
*
*******************************************************************************/
MV_STATUS mvUsb3WinInit(MV_U32 unitId)
{
	MV_U32 win, winCtrlValue;
	MV_TARGET_ATTRIB targetAttrib;
	MV_CPU_DEC_WIN cpuAddrDecWin;
	MV_U64 baseAddr;

	/* Clear all existing windows */
	for (win = 0; win < USB3_MAX_WINDOWS; win++) {
		MV_REG_WRITE(MV_USB3_REGS_OFFSET(unitId) + USB3_WIN_CTRL(win), 0);
		MV_REG_WRITE(MV_USB3_REGS_OFFSET(unitId) + USB3_WIN_BASE(win), 0);
	}

	/* Program each DRAM CS in a seperate window */
	for (win = 0; win < CONFIG_NR_DRAM_BANKS; win++) {
		/* Get CS attribute and target ID */
		if (mvCtrlAttribGet(win, &targetAttrib) != MV_OK) {
			mvOsPrintf("%s: Error - mvCtrlAttribGet() failed.\n", __func__);
			return MV_ERROR;
		}

		/* Get CS base and size */
		if (mvCpuIfTargetWinGet(win, &cpuAddrDecWin) != MV_OK) {
			mvOsPrintf("%s: Error - mvCpuIfTargetWinGet() failed.\n", __func__);
			return MV_ERROR;
		}

		if (cpuAddrDecWin.enable != MV_TRUE)
			continue;

		/* prepare CTRL and BASE values */
		winCtrlValue = ((((MV_U32)cpuAddrDecWin.addrWin.size - 1) & 0xffff0000) |
				(targetAttrib.attrib << 8) | (targetAttrib.targetId << 4) | 1);
		baseAddr = (MV_U64)((((MV_U64)cpuAddrDecWin.addrWin.baseHigh << 32ll)) +
				(MV_U64)cpuAddrDecWin.addrWin.baseLow);

		MV_REG_WRITE(MV_USB3_REGS_OFFSET(0) + USB3_WIN_CTRL(win), winCtrlValue);
		MV_REG_WRITE(MV_USB3_REGS_OFFSET(0) + USB3_WIN_BASE(win), baseAddr);
	}

	return MV_OK;

}


/*
 * initialize USB3 :
 * - Set UTMI PHY Selector
 * - Map DDR address space to xHCI
 * - LFPS FREQ WA
 */
static int mv_xhci_core_init(MV_U32 unitId)
{
	int reg, mask;

	/* Set UTMI PHY Selector:
	 * - Connect UTMI PHY to USB2 port of USB3 Host
	 * - Powers down the other unit (so unit's registers aren't accessible) */
	reg = MV_REG_READ(USB_CLUSTER_CONTROL);
	reg = (reg & (~0x1)) | 0x1;
	MV_REG_WRITE(USB_CLUSTER_CONTROL, reg);

	/* Map the DDR address space to the XHCI */
	if (mvUsb3WinInit(unitId) != MV_OK) {
		mvOsPrintf("%s: Error - mvUsb3WinInit() failed.\n", __func__);
		return MV_ERROR;
	}

	/* LFPS FREQUENCY WA */
	// Add support for A375 Z1 Revision
	if (mvCtrlRevGet() == MV_88F66X0_Z1_ID) {
		/*
		 * All defines below are used for a temporary workaround, and therefore
		 * placed inside the code and not in an include file
		 */
		#define USB3_MAC_REGS_BASE_OFFSET	(0x10000)
		#define USB3_CNTR_PULSE_WIDTH_OFFSET	(0x454)
		#define REF_CLK_100NS_OFFSET		(24)
		#define REF_CLK_100NS_MASK		(0xFF)
		//#define USB3_MAC_REGS_SIZE		(0x500)
		#define USB3_MAC_REGS_BASE (USB3_REGS_PHYS_BASE + USB3_MAC_REGS_BASE_OFFSET)

		/* Modify the LFPS timing to fix clock issues on ALP-Z1 */
		reg = MV_REG_READ(USB3_MAC_REGS_BASE + USB3_CNTR_PULSE_WIDTH_OFFSET);
		mask = ~(REF_CLK_100NS_MASK << REF_CLK_100NS_OFFSET);
		reg = (reg & mask) | (0x10 << REF_CLK_100NS_OFFSET);
		MV_REG_WRITE(USB3_MAC_REGS_BASE + USB3_CNTR_PULSE_WIDTH_OFFSET, reg);
	}

	return 0;
}

int xhci_hcd_init(int index, struct xhci_hccr **hccr, struct xhci_hcor **hcor)
{
	MV_U32 ctrlModel = mvCtrlModelGet();

	/* USB 3.0 is currently supported only for ALP DB-6660 and A375 board */
	if (ctrlModel != MV_6660_DEV_ID && ctrlModel != MV_6720_DEV_ID) {
		printf("%s: Error: USB 3.0 is not supported on current soc\n" ,__func__);
		return -1;
	}

	mv_xhci_core_init(index);

	/* Set operational xHCI register base*/
	*hccr = (struct xhci_hccr *)(USB3_REGS_PHYS_BASE);
	/* Set host controller operation register base */
	*hcor = (struct xhci_hcor *)((uint32_t) (*hccr) + HC_LENGTH(xhci_readl(&(*hccr)->cr_capbase)));

	debug("marvell-xhci: init hccr %x and hcor %x hc_length %d\n",
		(uint32_t)*hccr, (uint32_t)*hcor,
		(uint32_t)HC_LENGTH(xhci_readl(&(*hccr)->cr_capbase)));

	return 0;
}

void xhci_hcd_stop(int index)
{
	return;
}

#elif defined (CONFIG_USB_EHCI)
/*
 * Create the appropriate control structures to manage
 * a new EHCI host controller.
 */
int ehci_hcd_init(int index, struct ehci_hccr **hccr, struct ehci_hcor **hcor)
{
	char *env;
	int usbActive;

	env = getenv("usbActive");
	usbActive = simple_strtoul(env, NULL, 10);
	printf("Active port:\t");
	if (usbActive >= mvCtrlUsbMaxGet()) {
		printf("invalid port number %d, switching to port 0\n", usbActive);
		usbActive=0;
	} else {
		printf("%d\n", usbActive);
	}

	*hccr = (struct ehci_hccr *)(INTER_REGS_BASE + MV_USB_REGS_OFFSET(usbActive) + 0x100);
	*hcor = (struct ehci_hcor *)((uint32_t) (*hccr) + HC_LENGTH(ehci_readl(&(*hccr)->cr_capbase)));

	debug ("Marvell init hccr %x and hcor %x hc_length %d\n",
		(uint32_t)hccr, (uint32_t)hcor,
		(uint32_t)HC_LENGTH(ehci_readl(&(*hccr)->cr_capbase)));
	return 0;
}

/*
 * Destroy the appropriate control structures corresponding
 * the the EHCI host controller.
 */
int ehci_hcd_stop(int index)
{
	return 0;
}
#endif /* CONFIG_USB_EHCI */
#endif /* CONFIG_CMD_USB */
