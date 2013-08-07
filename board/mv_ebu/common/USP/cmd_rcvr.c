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
#if defined(CONFIG_CMD_RCVR)
#include "boardEnv/mvBoardEnvLib.h"

void recoveryHandle(cmd_tbl_t *cmdtp)
{
	char cmd[256];
	char img[10];
	char * args_to_func[3];

	args_to_func[0]="tftp";
	args_to_func[1]=getenv("loadaddr");;
	args_to_func[2]=getenv("rcvr_image");
	do_tftpb(cmdtp, 1, 3,args_to_func);
	mvOsDelay(100);

	setenv("bootcmd","setenv bootargs ${console} ubi.mtd=2 root=ubi0:rootfsU rootfstype=ubifs ${mvNetConfig}; nand read.e ${loadaddr} 0x200000 0x400000; bootm ${loadaddr};");
	setenv("console","console=ttyS0,115200");
	saveenv();

	printf("\nPermanent bootcmd: %s\n", getenv("bootcmd"));
	printf("\nPermanent console: %s\n", getenv("console"));

	sprintf(cmd,"setenv bootargs ${console} root=/dev/ram0 ${mvNetConfig} recovery=static rcvrip=%s:%s%s  ethact=${ethact} ethaddr=%s eth1addr=%s; bootm ${loadaddr};", getenv("ipaddr"), getenv("serverip"), getenv("bootargs_end"), getenv("ethaddr"), getenv("eth1addr"));
	setenv("bootcmd", cmd);
	printf("\nRecovery bootcmd: %s\n", cmd);

	printf("Booting recovery image at: [%s]...\n", getenv("loadaddr"));
	sprintf(cmd, "boot");
	sprintf(img, "%s", getenv("loadaddr"));
	args_to_func[0] = cmd;
	args_to_func[1] = img;

	do_bootd(NULL, 0, 2, args_to_func);
}

int do_rcvr (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	recoveryHandle(cmdtp);
	return 1;
}

U_BOOT_CMD(
	rcvr,	3,	1,	do_rcvr,
	"rcvr\t- Start recovery process (with TFTP server)\n",
	"\n"
);
#endif
