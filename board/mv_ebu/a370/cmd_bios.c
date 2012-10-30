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
#include "mvCommon.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "pex/mvPex.h"



static int do_bios_list()
{
	int i = 0;
	MV_U8 confId = mvBoardConfIdGet();
	
	printf("DB Conf | Conf ID | L2 size | CPU Freq | Fabric | Freq | CPU1/2/3 Enable | CPU Mode V6UP/V6MP\n");
#warning TODO: -1 is for not showing the 78480 problematic flavour
	if (confId != 0x11) {
		for (i=0; i < BIOS_MODES_NUM-1; i++) {
		
			printf("%s\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\n", bios_modes[i].name, bios_modes[i].confId, bios_modes[i].l2size, bios_modes[i].cpuFreq, bios_modes[i].fabricFreq, bios_modes[i].cpuEna, bios_modes[i].cpuMode);
		}
	} else {
		printf("%s\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\n", bios_modes[1].name, bios_modes[1].confId, bios_modes[1].l2size, bios_modes[1].cpuFreq, bios_modes[1].fabricFreq, bios_modes[1].cpuEna, bios_modes[1].cpuMode);
	}

	return 0;
}
static int do_bios_read()
{
	int i = 0;
	int flag = 0;
	MV_U8 l2size = mvBoardL2SizeGet();
	MV_U8 cpuFreq = mvBoardCpuFreqGet();
	MV_U8 fabricFreq = mvBoardFabFreqGet();
	MV_U8 cpuEna = mvBoardCpuCoresNumGet();
	MV_U8 cpuMode = mvBoardCpu0CoreModeGet();
	MV_U8 confId = mvBoardConfIdGet();

	for (i=0; i < BIOS_MODES_NUM; i++) {
		if ((bios_modes[i].l2size == l2size) &&
			(bios_modes[i].cpuFreq == cpuFreq) &&
			(bios_modes[i].fabricFreq == fabricFreq) &&
			(bios_modes[i].cpuEna == cpuEna) &&
			(bios_modes[i].cpuMode == cpuMode) &&
			(bios_modes[i].confId == confId)) {

			printf("Current BIOS mode is: %s\n", bios_modes[i].name);
			flag = 1;
		}
	}
	if (flag == 0)
		printf("Current BIOS mode is invalid!\n");

	return 0;
}
static int do_bios_write(int argc, char * const argv[])
{
	int i = 0;
	int flag = 0;
	const char *cmd;

	if (argc < 1)
		goto usage;
	cmd = argv[0];

	for (i=0; i < BIOS_MODES_NUM; i++) {
		if (strcmp(cmd, bios_modes[i].name) == 0) {
			
			printf("Setting BIOS mode : %s\n", bios_modes[i].name);
			mvBoardL2SizeSet(bios_modes[i].l2size);
			mvBoardCpuFreqSet(bios_modes[i].cpuFreq);
			mvBoardCpuFreqModeSet(bios_modes[i].cpuFreqMode);
			mvBoardFabFreqSet(bios_modes[i].fabricFreq);
			mvBoardFabFreqModeSet(bios_modes[i].fabricFreqMode);
			mvBoardCpuCoresNumSet(bios_modes[i].cpuEna);
			mvBoardCpu0CoreModeSet(bios_modes[i].cpuMode);
			mvBoardConfIdSet(bios_modes[i].confId);
			flag = 1;
		}
	}
	if (flag == 0)
		printf("BIOS mode is invalid!, please use bios list to see all modes\n");

	return 0;
usage:
	printf("Usage: bios write <mode> (see help) \n", argv[0]);
	return 1;
}
int do_bios(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	const char *cmd;

	/* need at least two arguments */
	if (argc < 2)
		goto usage;

	cmd = argv[1];

	if (strcmp(cmd, "list") == 0)
		return do_bios_list();
	if (strcmp(cmd, "write") == 0) {
		if (do_bios_write(argc - 2, argv + 2) == 0)
			do_bios_read();
		return 0;
	}
	if (strcmp(cmd, "read") == 0)
		return do_bios_read();

usage:
	cmd_usage(cmdtp);
	return 1;
}

U_BOOT_CMD(bios, 3, 1, do_bios,
	"BIOS command to set the DB configuration.\n",

	"list     - prints the BIOS modes list\n"
	"bios read     - read and print the BIOS value\n"

	"bios write <mode>      - write the BIOS to <mode>, see list for all modes\n"
);
