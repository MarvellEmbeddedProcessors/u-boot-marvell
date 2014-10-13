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
#include "boardEnv/mvBoardEnvLib.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "ctrlEnv/sys/mvCpuIf.h"
#if defined(CONFIG_USB_XHCI)
#include "drivers/usb/host/xhci.h"
#endif
#if defined(CONFIG_USB_EHCI)
#include "drivers/usb/host/ehci.h"
#endif
#include "mvSysUsbConfig.h"

/*******************************************************************************
* getUsbActive - read 'usbActive' env variable and set active port
*
* INPUT:
* 	MV_U32 usbUnitId - USB interface indication (USB2.0 / USB3.0)
*
* OUTPUT:
* 	prints selected active port number, and selected interface
*
* RETURN:
*       Num of requested interface USB2/3
*
*******************************************************************************/
MV_STATUS getUsbActive(MV_U32 usbUnitId, MV_U32 maxUsbHostPorts, MV_U32 maxSerDesLanes)
{
	char *env = getenv("usbActive");
	int usbActive = simple_strtoul(env, NULL, 10);


	/*  if requested USB3.0 is not connected via SerDes, but there are enough USB3.0 ports */
	if (usbUnitId == USB3_UNIT_ID && (usbActive >= maxSerDesLanes && maxUsbHostPorts > maxSerDesLanes)) {
		mvOsPrintf("\n'usbActive' warning (%d): Invalid USB3.0 port (no valid SerDes)..", usbActive);
		mvOsPrintf("Trying USB2.0 Host via USB3.0\n");
	}
	else if (usbActive >= maxUsbHostPorts && usbUnitId == USB_UNIT_ID)
		mvOsPrintf("\n'usbActive' warning (%d): Invalid USB2.0 port \n", usbActive);

	mvOsPrintf("Port (usbActive) : %d\tInterface (usbType = %d) : ", usbActive, ((usbUnitId == USB3_UNIT_ID) ? 3 : 2));
	/* Fetch SoC USB mapping:
	   For Some SoCs, when using single USB port, unit 1 is active and not 0 */
	usbActive = mvCtrlUsbMapGet(usbUnitId, usbActive);

	return usbActive;
}

#if defined (CONFIG_USB_XHCI)
/*********************************************************************************/
/**********************      xHCI Stack registration layer  **********************/
/*********************************************************************************/
/*
 * initialize USB3 Core:
 * Set UTMI PHY Selector
 */
static void mv_xhci_core_init(MV_U32 unitId)
{
	/* A shared xHCI MAC with USB2/3 requires UTMI Phy selection */
	mvCtrlUtmiPhySelectorSet(USB3_UNIT_ID);
}

int xhci_hcd_init(int index, struct xhci_hccr **hccr, struct xhci_hcor **hcor)
{
	if (mvCtrlUsb3HostMaxGet() <= 0) {
		mvOsPrintf("\n%s: Error: USB 3.0 is not supported on current device\n", __func__);
		return -1;
	}

	mv_xhci_core_init(index);

	/* Set operational xHCI register base*/
	*hccr = (struct xhci_hccr *)(USB3_REGS_PHYS_BASE(index));
	/* Set host controller operation register base */
	*hcor = (struct xhci_hcor *)((uint32_t) (*hccr) + XHCI_HC_LENGTH(xhci_readl(&(*hccr)->cr_capbase)));

	debug("marvell-xhci: init hccr %x and hcor %x hc_length %d\n",
		(uint32_t)*hccr, (uint32_t)*hcor,
		(uint32_t)XHCI_HC_LENGTH(xhci_readl(&(*hccr)->cr_capbase)));

	return 0;
}

void xhci_hcd_stop(int index)
{
	return;
}

#endif /* CONFIG_USB_XHCI */

#if defined(CONFIG_USB_EHCI)
/*********************************************************************************/
/**********************      eHCI Stack registration layer  **********************/
/*********************************************************************************/
/* ehci_hcd_init:
 *	Create the appropriate control structures to manage new EHCI host controller.
 */
int ehci_hcd_init(int index, struct ehci_hccr **hccr, struct ehci_hcor **hcor)
{
	/* ALP/A375 USB2 port 0 is a shared MAC with USB2/3, and requires UTMI Phy selection */
	if (index == 0)
		mvCtrlUtmiPhySelectorSet(USB_UNIT_ID);

	*hccr = (struct ehci_hccr *)(MV_USB2_CAPLENGTH_OFFSET(index));
	*hcor = (struct ehci_hcor *)((uint32_t) (*hccr) + HC_LENGTH(ehci_readl(&(*hccr)->cr_capbase)));

	debug ("Marvell init hccr %x and hcor %x hc_length %d\n",
		(uint32_t)hccr, (uint32_t)hcor,
		(uint32_t)HC_LENGTH(ehci_readl(&(*hccr)->cr_capbase)));
	return 0;
}

/* ehci_hcd_stop:
 *	Destroy the appropriate control structures corresponding the the EHCI host controller.
 */
int ehci_hcd_stop(int index)
{
	return 0;
}

int ehci_usb_alloc_device(struct usb_device *udev)
{
	return 0;
}
#endif /* CONFIG_USB_EHCI */

/*********************************************************************************/
/******************** Host Controller stack replacement-layer ********************/
/*********************************************************************************/
/* Stack pointers*/
struct hc_interface {
	MV_BOOL interface_supported;
	int	(*hc_usb_lowlevel_init)(int index, void **controller);
	int	(*hc_usb_lowlevel_stop)(int index);
	int	(*hc_submit_int_msg)(struct usb_device *dev, unsigned long pipe, void *buffer,
			int length, int interval);
	int	(*hc_submit_bulk_msg)(struct usb_device *dev, unsigned long pipe, void *buffer,
			int length);
	int	(*hc_submit_control_msg)(struct usb_device *dev, unsigned long pipe, void *buffer,
		   int length, struct devrequest *setup);
	int	(*hc_usb_alloc_device)(struct usb_device *udev);
};

/* hc is the Host Controller struct for xHCI/eHCI routine's pointers */
struct hc_interface *hc;

struct hc_interface hc_ehci = {
#if defined(CONFIG_USB_EHCI)
	.interface_supported = MV_TRUE,
	.hc_usb_lowlevel_init	= ehci_usb_lowlevel_init,
	.hc_usb_lowlevel_stop	= ehci_usb_lowlevel_stop,
	.hc_submit_int_msg	= ehci_submit_int_msg,
	.hc_submit_bulk_msg	= ehci_submit_bulk_msg,
	.hc_submit_control_msg	= ehci_submit_control_msg,
	.hc_usb_alloc_device	= ehci_usb_alloc_device
#else
	.interface_supported = MV_FALSE
#endif
};

struct hc_interface hc_xhci = {
#if defined(CONFIG_USB_XHCI)
	.interface_supported = MV_TRUE,
	.hc_usb_lowlevel_init	= xhci_usb_lowlevel_init,
	.hc_usb_lowlevel_stop	= xhci_usb_lowlevel_stop,
	.hc_submit_int_msg	= xhci_submit_int_msg,
	.hc_submit_bulk_msg	= xhci_submit_bulk_msg,
	.hc_submit_control_msg	= xhci_submit_control_msg,
	.hc_usb_alloc_device	= xhci_usb_alloc_device
#else
	.interface_supported = MV_FALSE
#endif
};

/* usb_lowlevel_init:
*	this routine navigate between currently selected stack (eHCI/xHCI)
*	- Read 'usbType' env variable to select requested interface:
*	  usbType = 2 -- > eHCI
*	  usbType = 3 -- > xHCI
*	- Call usb_lowlevel_init from selected stack
*/
int usb_lowlevel_init(int index, void **controller)
{
	int usb2HostNum, usb3HostNum, usbType, usbActive = 0;

	usbType = simple_strtoul(getenv("usbType"), NULL, 10);
	usb3HostNum = mvCtrlUsb3HostMaxGet();
	usb2HostNum = mvCtrlUsbMaxGet();

	switch (usbType) {
	case 2:
		if (hc_ehci.interface_supported == MV_TRUE && usb2HostNum > 0) {
			usbActive = getUsbActive(USB_UNIT_ID, usb2HostNum, 0); /* read requested active port */
			hc = &hc_ehci; /* set Host Controller struct for function pointers  */
		} else
			goto input_error;
		break;
	case 3:
		if (hc_xhci.interface_supported == MV_TRUE && usb3HostNum > 0) {
			usbActive = getUsbActive(USB3_UNIT_ID, usb3HostNum , mvCtrlUsb3MaxGet()); /* read requested active port */
			hc = &hc_xhci; /* set Host Controller struct for function pointers  */
		} else
			goto input_error;
		break;
	default:
		goto input_error;
	}

	return hc->hc_usb_lowlevel_init(usbActive, controller);

input_error:
	mvOsPrintf("Error: requested 'usbType' (Type %d) is not supported", usbType);
	if ((usbType == 2 && usb2HostNum < 1) || (usbType ==3 && usb3HostNum < 1))
		mvOsPrintf(" (no available USB ports).\n");

	if ((hc_ehci.interface_supported == MV_TRUE && usb2HostNum > 0) ||
		(hc_xhci.interface_supported == MV_TRUE && usb3HostNum > 0))
		mvOsPrintf("\n\n\t Supported Units:\n");
	if (hc_ehci.interface_supported == MV_TRUE && usb2HostNum > 0)
		mvOsPrintf("\n\tUSB2.0: %d ports: set usbType = 2 --> EHCI Stack will be used\n", usb2HostNum);
	if (hc_xhci.interface_supported == MV_TRUE && usb3HostNum > 0)
		mvOsPrintf("\tUSB3.0: %d ports: set usbType = 3 --> xHCI Stack will be used\n", usb3HostNum);
	return -1;
}

int usb_lowlevel_stop(int index)
{
	if (!hc) {
		mvOsPrintf("%s: Error: run 'usb reset' to set host controller interface.\n", __func__);
		return -1;
	}
	return hc->hc_usb_lowlevel_stop(index);
}

int submit_int_msg(struct usb_device *dev, unsigned long pipe, void *buffer,
			int length, int interval)
{
	if (!hc) {
		mvOsPrintf("%s: Error: run 'usb reset' to set host controller interface.\n", __func__);
		return -1;
	}
	return hc->hc_submit_int_msg(dev, pipe, buffer, length, interval);
}

int submit_bulk_msg(struct usb_device *dev, unsigned long pipe, void *buffer, int length)
{
	if (!hc) {
		mvOsPrintf("%s: Error: run 'usb reset' to set host controller interface.\n", __func__);
		return -1;
	}
	return hc->hc_submit_bulk_msg(dev, pipe, buffer, length);
}

int submit_control_msg(struct usb_device *dev, unsigned long pipe, void *buffer,
			int length, struct devrequest *setup)
{
	if (!hc) {
		mvOsPrintf("%s: Error: run 'usb reset' to set host controller interface.\n", __func__);
		return -1;
	}
	return hc->hc_submit_control_msg(dev, pipe, buffer, length, setup);
}

int usb_alloc_device(struct usb_device *udev)
{
	if (!hc) {
		mvOsPrintf("%s: Error: run 'usb reset' to set host controller interface.\n", __func__);
		return -1;
	}
	return hc->hc_usb_alloc_device(udev);
}

#endif /* CONFIG_CMD_USB */
