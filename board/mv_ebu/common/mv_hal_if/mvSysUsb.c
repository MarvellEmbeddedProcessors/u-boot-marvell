/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

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
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer.

    *   Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.

    *   Neither the name of Marvell nor the names of its contributors may be
	used to endorse or promote products derived from this software without
	specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include "mvCommon.h"
#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "usb/mvUsb.h"
#include "ctrlEnv/mvCtrlEnvAddrDec.h"
#include "usb/mvUsbRegs.h"

/*******************************************************************************
* mvSysUsbHalInit - Initialize the USB subsystem
*
* DESCRIPTION:
*
* INPUT:
*       None
* OUTPUT:
*		None
* RETURN:
*       None
*
*******************************************************************************/
MV_STATUS mvSysUsbInit(MV_VOID)
{
	MV_USB_HAL_DATA halData;
	MV_STATUS status = MV_OK;
	MV_U32 dev;
	MV_UNIT_WIN_INFO addrWinMap[MAX_TARGETS + 1];

	halData.ctrlModel = mvCtrlModelGet();
	halData.ctrlRev = mvCtrlRevGet();
	halData.ctrlFamily = mvCtrlDevFamilyIdGet(halData.ctrlModel);


	status = mvCtrlAddrWinMapBuild(addrWinMap, MAX_TARGETS + 1);

#ifdef CONFIG_USB_EHCI
		MV_BOOL isHost;
		char envname[10], *env;
		int maxUsbPorts = mvCtrlUsbMaxGet();
		/* for ALP/A375: if using single usb2 port, use Virtual MAC ID since
		 MAC ID0 (usbActive =0) is connected to Physical MAC ID1 */
		int id, mac_id[2] = {1, 0};

		for (id = 0; id < mvCtrlUsbMaxGet(); id++) {
			if (maxUsbPorts == 1 && (halData.ctrlFamily == MV_88F67X0 ||
					(halData.ctrlRev == MV_88F66XX_A0_ID && halData.ctrlFamily == MV_88F66X0)))
				dev = mac_id[id];
			else
				dev = id;

			sprintf(envname, "usb%dMode", dev);
			env = getenv(envname);
			if ((!env) || (strcmp(env, "device") == 0) || (strcmp(env, "Device") == 0))
				isHost = MV_FALSE;
			else
				isHost = MV_TRUE;

			if (status == MV_OK)	/* Map DDR windows to EHCI */
#ifdef CONFIG_USB_XHCI_HCD
/* CONFIG_USB_XHCI_HCD indicate that both xHCI and eHCI are compiled:
 * Last Boolean argument is used to indicate the HAL layer which unit is currently initiated */
				status = mvUsbWinInit(dev, addrWinMap, MV_FALSE);
#else
				status = mvUsbWinInit(dev, addrWinMap);
#endif
			if (status == MV_OK)
				status = mvUsbHalInit(dev, isHost, &halData);
			if (status == MV_OK)
				printf("USB2.0 %d: %s Mode\n", dev, (isHost == MV_TRUE ? "Host" : "Device"));
			else
				mvOsPrintf("%s: Error: USB2.0 initialization failed (port %d).\n", __func__, dev);
		}
#endif
#ifdef CONFIG_USB_XHCI
		MV_U32 reg;
		for (dev = 0; dev < mvCtrlUsb3MaxGet(); dev++) {
			status = mvUsbUtmiPhyInit(dev, &halData);
			if (halData.ctrlFamily == MV_88F66X0 || halData.ctrlFamily == MV_88F67X0) {
				/* ALP/A375: Set UTMI PHY Selector:
				 * - Connect UTMI PHY to USB2 port of USB3 Host
				 * - Powers down the other unit (so USB3.0 unit's registers are accessible) */
				reg = MV_REG_READ(USB_CLUSTER_CONTROL);
				reg = (reg & (~0x1)) | 0x1;
				MV_REG_WRITE(USB_CLUSTER_CONTROL, reg);
			}
			if (status == MV_OK)	/* Map DDR windows to XHCI */
				status = mvUsbWinInit(dev, addrWinMap, MV_TRUE);
			if (status == MV_OK)
				printf("USB3.0 %d: Host Mode\n", dev);
			else
				mvOsPrintf("%s: Error: USB3.0 initialization failed (port %d).\n", __func__, dev);
		}
#endif
	return status;
}
