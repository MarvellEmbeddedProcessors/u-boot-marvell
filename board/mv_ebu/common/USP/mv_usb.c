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
/*******************************************************************************
* printUsbError  - This function used to print error message when USB error dedected.
*			 called from "usb_lowlevel_init" and "usb_lowlevel_stop" functions.
*
* INPUT:
*	MV_BOOL value - MV_TRUE if usb connected on current board ,MV_FALSE otherwise.
*
* OUTPUT:
*	prints USB error messages.
*
* RETURN:
*	-1
*
*******************************************************************************/
static int printUsbError(MV_BOOL mvBoardIsUsbPortConnected ,int port)
{
        int usbType = simple_strtoul(getenv("usbType"), NULL, 10);
        int usb3HostNum = mvCtrlUsb3HostMaxGet();
        int usb2HostNum = mvCtrlUsbMaxGet();

	if(mvBoardIsUsbPortConnected == MV_FALSE)
		mvOsPrintf("\nError: requested 'usbActive' (%d) is not connected on current board\n",port);
	else {
		mvOsPrintf("Error: requested 'usbType' (Type %d) is not supported.", usbType);
		if ((usbType == 2 && usb2HostNum < 1) || (usbType ==3 && usb3HostNum < 1))
		mvOsPrintf(" (no available USB%d ports on current Soc).\n",usbType);
	}
	if ((hc_ehci.interface_supported == MV_TRUE && usb2HostNum > 0) ||
			(hc_xhci.interface_supported == MV_TRUE && usb3HostNum > 0))
		mvOsPrintf("\n\n\t Supported Units:\n");
	if (hc_ehci.interface_supported == MV_TRUE && usb2HostNum > 0)
		mvOsPrintf("\n\tUSB2.0: %d ports: set usbType = 2 --> EHCI Stack will be used\n", usb2HostNum);
	if (hc_xhci.interface_supported == MV_TRUE && usb3HostNum > 0)
		mvOsPrintf("\tUSB3.0: %d ports: set usbType = 3 --> xHCI Stack will be used\n", usb3HostNum);
	return -1;

}


/* usb_lowlevel_init:
*	this routine navigate between currently selected stack (eHCI/xHCI)
*	- Read 'usbType' env variable to select requested interface:
*	  usbType = 2 -- > eHCI
*	  usbType = 3 -- > xHCI
*	- Call usb_lowlevel_init from selected stack
*/

/* mark detected usb port, to ensure proper 'stop'
 * process before next detection request */
int currentUsbActive = -1;
int usb_lowlevel_init(int index, void **controller)
{
	int usb2HostNum, usb3HostNum, usbType, usbActive = 0;
	usbActive = simple_strtoul(getenv("usbActive"), NULL, 10);
	usbType = simple_strtoul(getenv("usbType"), NULL, 10);
	usb3HostNum = mvCtrlUsb3HostMaxGet();
	usb2HostNum = mvCtrlUsbMaxGet();

	switch (usbType) {
	case 2:
		if (hc_ehci.interface_supported == MV_TRUE && usb2HostNum > 0) {
			usbActive = mvCtrlUsbMapGet(USB_UNIT_ID, usbActive);
			if (mvBoardIsUsbPortConnected(USB_UNIT_ID, usbActive) == MV_FALSE)
				return printUsbError(MV_FALSE, usbActive);
			hc = &hc_ehci; /* set Host Controller struct for function pointers  */
		} else
			 return printUsbError(MV_TRUE, usbActive);
		break;
	case 3:
		if (hc_xhci.interface_supported == MV_TRUE && usb3HostNum > 0) {
			/* mvCtrlUsbMapGet used only for legacy devices ALP/A375 */
			usbActive = mvCtrlUsbMapGet(USB3_UNIT_ID, usbActive);
			if (mvBoardIsUsbPortConnected(USB3_UNIT_ID, usbActive) == MV_FALSE)
				return printUsbError(MV_FALSE, usbActive);
			if (mvCtrlIsUsbSerDesConnected(usbActive) == MV_FALSE) {
				mvOsPrintf("\n'usbActive' warning (%d): Invalid USB3.0 port (no valid SerDes)..", usbActive);
				mvOsPrintf("Trying USB2.0 Host via USB3.0\n");
			}
#ifdef MV_USB_VBUS_CYCLE
			/* VBUS signal is lowered prior to SerDes initialization sequence,
			 * before initializing and detecting device, set VBUS enabled */
			mvBoardUsbVbusSet(usbActive);
#endif
			hc = &hc_xhci; /* set Host Controller struct for function pointers  */
		} else
			return printUsbError(MV_TRUE, usbActive);
		break;
	default:
		return printUsbError(MV_TRUE, usbActive);
	}
	mvOsPrintf("Port (usbActive) : %d\tInterface (usbType = %d) : ", usbActive,
                ((usbType == 3) ? 3 : 2));

	/* Make sure that usbActive never exeeds the configured max controllers count
	   The CONFIG_USB_MAX_CONTROLLER_COUNT can be changed for different boards */
	if (usbActive >= CONFIG_USB_MAX_CONTROLLER_HOST_COUNT) {
		mvOsPrintf("usbActive=%d is out of supported range\n",usbActive);
		return -1;
	}

	/* Marvell USB code supports only one active controller (USB0), while the actual host
	   device is selected by usbActive environment variable */
	if (index > 0) {
		mvOsPrintf("\nOnly one active controller supported! Skipping USB%d initialization.\n", index);
		return -1;
	}

	currentUsbActive = usbActive;
	return hc->hc_usb_lowlevel_init(usbActive, controller);
}

int usb_lowlevel_stop(int index)
{
	int usbActive = currentUsbActive, ret;

	if (!hc) {
		mvOsPrintf("%s: Error: run 'usb reset' to set host controller interface.\n", __func__);
		return -1;
	}

	/* Mark that there is no current usbActive to stop */
	currentUsbActive = -1;
	ret = hc->hc_usb_lowlevel_stop(usbActive);
	hc = NULL;
	return ret;
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
