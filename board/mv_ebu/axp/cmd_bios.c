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



static int do_bios_list(void)
{
	int i = 0;
	MV_BIOS_MODE * pBbiosModes;
	MV_U8 confId = mvBoardConfIdGet();


	printf("|DB Conf| Conf ID| L2 size| CPU Freq| Fabric| Freq CPU1/2/3 Enable|CPU Endianess| DramBusWidth|\n");

	if (confId != 0x11) {
		if (mvCtrlRevGet() == 2)
			pBbiosModes = bios_modes_b0;
		else
			pBbiosModes = bios_modes;
		for (i=0; i < BIOS_MODES_NUM-1; i++) {
			printf("| %s |   0x%02x |     0x%x|      0x%x|   0x%02x|                  0x%x|          0x%x|          0x%x|\n",
				   pBbiosModes->name, 
				   pBbiosModes->confId, 
				   pBbiosModes->l2size, 
				   pBbiosModes->cpuFreq, 
				   pBbiosModes->fabricFreq, 
				   pBbiosModes->cpuEna, 
				   pBbiosModes->cpuEndianess, 
				   pBbiosModes->dramBusWidth); 
		pBbiosModes++;
		}
	} else {
		if (mvCtrlRevGet() == 2)
			pBbiosModes = &bios_modes_b0[1];
		else
			pBbiosModes = &bios_modes[1];
		printf("%s\t\t0x%x\t\t0x%x\t\t0x%x\t\t0x%x\t\t0x%x\t\t0x%x\t\t0x%x\n", pBbiosModes->name, pBbiosModes->confId, pBbiosModes->l2size, pBbiosModes->cpuFreq, pBbiosModes->fabricFreq, pBbiosModes->cpuEna, pBbiosModes->cpuEndianess, pBbiosModes->dramBusWidth);
	}

	return 0;
}
static int do_bios_read(void)
{
	int i = 0;
	int flag = 0;
	MV_BIOS_MODE * pBbiosModes;

	MV_U8 l2size = mvBoardL2SizeGet();
	MV_U8 cpuEna = mvBoardCpuCoresNumGet();
	MV_U8 confId = mvBoardConfIdGet();
	MV_U8 dramBusWidth = mvBoardDramBusWidthGet();
	if (mvCtrlRevGet() == 2)
		pBbiosModes = bios_modes_b0;
	else
		pBbiosModes = bios_modes;
/*	printf("ConfID=0x%x | L2Size=0x%x | CPUEna=0x%x |  dramBusWidth=0x%x \n",
		   confId , l2size , cpuEna , dramBusWidth ); */

	for (i=0; i < BIOS_MODES_NUM; i++) {
/*		printf("TABLE= ConfID=0x%x | L2Size=0x%x | CPUEna=0x%x |   dramBusWidth=0x%x \n",
			   pBbiosModes->confId, pBbiosModes->l2size, pBbiosModes->cpuEna, pBbiosModes->dramBusWidth); */
		if ((pBbiosModes->l2size == l2size) &&
			/* (pBbiosModes->cpuFreq == cpuFreq) &&*/
			/* (pBbiosModes->fabricFreq == fabricFreq) &&*/
			/* (pBbiosModes->cpuEndianess == cpuEndianess) && */
			(pBbiosModes->cpuEna == cpuEna) &&
			(pBbiosModes->confId == confId) &&
			(pBbiosModes->dramBusWidth == dramBusWidth)) 
		{

			printf("Current BIOS mode is: %s\n", pBbiosModes->name);
			flag = 1;
		}
		pBbiosModes++;
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
	MV_BIOS_MODE * pBbiosModes;

	if (argc < 1)
		goto usage;
	cmd = argv[0];

	if (mvCtrlRevGet() == 2)
		pBbiosModes = bios_modes_b0;
	else
		pBbiosModes = bios_modes;

	for (i=0; i < BIOS_MODES_NUM; i++) {
		if (strcmp(cmd, pBbiosModes->name) == 0) {
			
			printf("Setting BIOS mode : %s\n", pBbiosModes->name);
			mvBoardL2SizeSet(pBbiosModes->l2size);
			mvBoardCpuFreqSet(pBbiosModes->cpuFreq);
			/*mvBoardCpuFreqModeSet(pBbiosModes->cpuFreqMode);*/
			mvBoardFabFreqSet(pBbiosModes->fabricFreq);
			mvBoardAltFabFreqSet(pBbiosModes->AltfabricFreq);
			/*mvBoardFabFreqModeSet(pBbiosModes->fabricFreqMode);*/
			mvBoardCpuCoresNumSet(pBbiosModes->cpuEna);
//			mvBoardCpu0EndianessSet(pBbiosModes->cpuEndianess);
			mvBoardConfIdSet(pBbiosModes->confId);
			mvBoardDramBusWidthSet(pBbiosModes->dramBusWidth);
			mvBoardBootDevSet(pBbiosModes->bootSource);
			mvBoardBootDevWidthSet(pBbiosModes->bootWidth);
			mvBoardPexCapabilitySet(1); /*set pex to be GEN2 by default */
			flag = 1;
		}
		pBbiosModes++;
	}
	if (flag == 0)
		printf("BIOS mode is invalid!, please use bios list to see all modes\n");

	return 0;
usage:
	printf("Usage: bios write <mode> (see help) \n");
	return 1;
}
int do_bios(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv[])
{
	const char *cmd;

	/* need at least two arguments */
	if (argc < 2)
		goto usage;

	cmd = argv[1];

	if (strcmp(cmd, "list") == 0)
		return do_bios_list();
	if (strcmp(cmd, "write") == 0) {
		if (do_bios_write(argc - 2, (argv + 2)) == 0)
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
