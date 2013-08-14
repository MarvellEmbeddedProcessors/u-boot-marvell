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
#include "mvCommon.h"
#include <command.h>
#include "boardEnv/mvBoardEnvLib.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"

int do_active_units(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	MV_U32  cpuNum, egigaNum, pexNum;
	MV_U32  i;

	printf("Active: ");

	/*   CPU	*/
	cpuNum = mvCtrlGetCpuNum();
	for (i = 0; i <= cpuNum; i++) {
		if (i)
			printf(", ");
		printf("cpu%d", i);
	}

	/*   PEX	*/
	pexNum = mvCtrlPexActiveUnitNumGet();
	for (i = 0; i < pexNum; i++)
		printf(", pex%d", i);

	/*   EGIGA	*/
	egigaNum = mvCtrlEthMaxPortGet();
	for (i = 0; i < egigaNum; i++) {
		if (mvBoardIsEthConnected(i) == MV_TRUE)
			printf(", egiga%d", i);
	}

#ifdef MV_USB
	/*   USB	*/
	MV_U32 usbNum = mvCtrlUsbMaxGet() + mvCtrlUsb3MaxGet();
	for (i = 0; i < usbNum; i++)
		printf(", usb%d", i);
#endif

	/*   SDIO	*/
#ifdef CONFIG_MRVL_MMC
	if (mvCtrlSdioSupport() == MV_TRUE)
		printf(", mmc0");
#endif

	/*   SATA	*/
#if defined(MV_INCLUDE_INTEG_SATA)
	MV_U32 sataNum = mvCtrlSataMaxPortGet();
	for (i = 0; i < sataNum; i++)
		printf(", sata%d", i);
#endif

	/*   SPI	*/
#if defined(MV_INCLUDE_SPI)
	printf(", spi");
#endif

	/*   NAND	*/
#if defined(MV_NAND)
	printf(", nand");
#endif

	/*   I2C	*/
#if defined(MV_INCLUDE_TWSI)
	printf(", i2c0");
#endif

	/*   TDM	*/
	if (mvBoardSlicUnitTypeGet() != MV_BOARD_SLIC_DISABLED)
		printf(", tdm0");

	printf("\n");
	return 0;
}

U_BOOT_CMD(
	active_units,	1,	1,	do_active_units,
	"print active units on board",
	""
);
