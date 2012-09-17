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
#include "net.h"
#include "mvTypes.h"
#include "gpp/mvGpp.h"
#include "gpp/mvGppRegs.h"
#include "boardEnv/mvBoardEnvLib.h"
static u32 rcvrflag = 0;

void recoveryDetection(void)
{
	MV_32 stateButtonBit = mvBoarGpioPinNumGet(BOARD_GPP_WPS_BUTTON,0);
	MV_32 buttonHigh = 0;
	char* env;

	/* Check if auto recovery is enabled */
	env = getenv("enaAutoRecovery");
	if(!env || ( (strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0) ) )
		setenv("enaAutoRecovery","yes");
	else
	{
		setenv("enaAutoRecovery","no");
		rcvrflag = 0;
		return;
	}

	if (stateButtonBit == MV_ERROR)
	{
		rcvrflag = 0;
		return;
	}

	if (stateButtonBit > 31)
	{
		stateButtonBit = stateButtonBit % 32;
		buttonHigh = 1;
	}

	/* Set state input indication pin as input */
	MV_REG_BIT_SET(GPP_DATA_OUT_EN_REG(buttonHigh),(1<<stateButtonBit));

	/* check if recovery triggered - button is pressed */
	if (!(MV_REG_READ(GPP_DATA_IN_REG(buttonHigh)) & (1 << stateButtonBit)))
	{
		rcvrflag = 1;
	}
}

//extern int do_bootm (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
void recoveryHandle(void)
{
	char cmd[256];
	char img[10];
	char * argv[3];
	char *env;
	MV_32 imagAddr, imagSize = 0, netflag = 1;
	char ip[16]= {"dhcp"};
	char* usbload[5];
	unsigned int netwait = 3000;
	int upgrade = -1;

	/* get the loadaddr env var */
	if (!getenv("loadaddr")) {
		printf("Missing loadaddr environment variable assuming default (%s)!\n", RCVR_LOAD_ADDR);
		setenv("loadaddr",RCVR_LOAD_ADDR);
		saveenv();
	}

#ifdef CONFIG_USB_STORAGE
	/* First try to perform recovery from USB DOK*/
	/* try to recognize storage devices immediately */
	if (usb_init() >= 0)
	{
		if (usb_stor_scan(1) >= 0)
		{
			netflag = 0;
			usbload[0] = "usbload";
			usbload[1] = "usb";
			usbload[2] = "0:1";
			usbload[3] = getenv("loadaddr");
			usbload[4] = "/multiware.img";

			printf("Trying to load image from USB flash drive using FAT FS\n");
			if (do_fat_fsload(0, 0, 5, usbload) == 1)
			{
				printf("Trying to load image from USB flash drive using ext2 FS partition 0\n");
				usbload[2] = "0:0";
				if(do_ext2load(0, 0, 5, usbload) == 1)
				{
					printf("Trying to load image from USB flash drive using ext2 FS partition 1\n");
					usbload[2] = "0:1";
					if(do_ext2load(0, 0, 5, usbload) == 1)
					{
						printf("Couldn't load recovery image from USB flash drive, Trying network interface\n");
						netflag = 1;
					}
					else
					{
						env = getenv("filesize");
						usbload[3] = getenv("loadaddr");
						imagSize = simple_strtoul(env, NULL, 16); /* get the filesize env var */

						/* Trying to check if we forced to upgrade by placing upgrade.me */
						usbload[4] = "/upgrade.me";
						usbload[3] = "0x000000000";
						env=getenv("loadaddr");
						imagAddr=simple_strtoul(env, NULL, 16);
						sprintf(usbload[3], "0x%x", imagAddr + imagSize + 1);
						printf("dummyaddr:%s\n", usbload[3]);
						upgrade=do_ext2load(0, 0, 5, usbload);
					}
				}
				else
				{
					env = getenv("filesize");
					imagSize = simple_strtoul(env, NULL, 16); /* get the filesize env var */

					/* Trying to check if we forced to upgrade by placing upgrade.me */
					usbload[4] = "/upgrade.me";
					usbload[3] = "0x000000000";
					env=getenv("loadaddr");
					imagAddr=simple_strtoul(env, NULL, 16);
					sprintf(usbload[3], "0x%x", imagAddr + imagSize + 1);
					printf("dummyaddr:%s\n", usbload[3]);
					upgrade=do_ext2load(0, 0, 5, usbload);
				}
			}
			else
			{
				env = getenv("filesize");
				imagSize = simple_strtoul(env, NULL, 16); /* get the filesize env var */

				/* Trying to check if we forced to upgrade by placing upgrade.me */
				usbload[4] = "/upgrade.me";
				usbload[3] = "0x000000000";
				env=getenv("loadaddr");
				imagAddr=simple_strtoul(env, NULL, 16);
				sprintf(usbload[3], "0x%x", imagAddr + imagSize + 1);
				printf("dummyaddr:%s\n", usbload[3]);
				upgrade=do_fat_fsload(0, 0, 5, usbload);
			}

			// Decide on upgrade/init mode
			if (upgrade == 0)
				sprintf(ip, "usb_upgrade");
			else
				sprintf(ip, "usb");
			printf("USB mode:%s\n", ip);
		}
	}
#endif

	if(netflag == 1)
	{
		/* if recovery ip set same as our ip then skip dhcp */
		while ( strcmp(getenv("rcvrip"), getenv("ipaddr")) && (netwait > 0) )
		{
			/* Perform the DHCP */
			printf("Aquiring an IP address using DHCP after delay...\n");
			mvOsDelay(netwait);
			netwait-=1000;

			if (NetLoop(DHCP) != -1)
			{
				/* We'vev got DHCP progressing with recovery */
				printf("DHCP should be ready for Recovery.\n");
				break;
			}
		}

		/* No DHCP after timeout or forced */
		if ( !strcmp(getenv("rcvrip"), getenv("ipaddr")) || (netwait <= 0) )
		{
			ulong tmpip;
			printf("No DHCP after timeout or manual IP address configured, switching to static IP address (%s)!\n", getenv("rcvrip"));
			tmpip = getenv_IPaddr ("rcvrip");
			NetCopyIP(&NetOurIP, &tmpip);
			sprintf(ip, "static");
		}

		/* Perform the recovery */
		printf("Starting retrieval of recovery image over network...\n");
		if ((imagSize = NetLoop(RCVR)) == -1)
		{
			printf("Failed\n");
			return;
		}
	}

#ifdef CONFIG_CMD_SF

#endif
	setenv("bootcmd","setenv bootargs $console ubi.mtd=3 root=ubi0:rootfs rootfstype=ubifs $mvNetConfig; nand read.e $loadaddr 0x200000 0x300000; bootm $loadaddr;");
	setenv("console","console=ttyS0,115200");
	setenv("mtdparts", "mtdparts=armada-nand:2m(boot),4m(linux),-(rootfs)");
	saveenv();

	printf("\nPermanent bootcmd: %s\n", getenv("bootcmd"));
	printf("\nPermanent console: %s\n", getenv("console"));

	/* This assignment to cmd should execute prior to the RD setenv and saveenv below*/
	sprintf(cmd,"setenv bootargs $console mtdparts=armada-nand:2m(boot),4m(linux),-(rootfs) root=/dev/ram0 $mvNetConfig recovery=%s rcvrip=%s:%s%s  ethact=$ethact ethaddr=%s eth1addr=%s; bootm $loadaddr;", ip, getenv("rcvrip"), getenv("serverip"), getenv("bootargs_end"), getenv("ethaddr"), getenv("eth1addr"));
	setenv("bootcmd", cmd);
	printf("\nRecovery bootcmd: %s\n", cmd);

	printf("Booting recovery image at: [%s]...\n", getenv("loadaddr"));
	sprintf(cmd, "boot");
	sprintf(img, "%s", getenv("loadaddr"));
	argv[0] = cmd;
	argv[1] = img;

	do_bootd(NULL, 0, 2, argv);
}

void recoveryCheck(void)
{
	/* Start the recovery process if indicated by user */
	if (rcvrflag)
		recoveryHandle();
}

int do_rcvr (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	recoveryHandle();
	return 1;
}

U_BOOT_CMD(
	rcvr,	3,	1,	do_rcvr,
	"rcvr\t- Satrt recovery process (Distress Beacon with TFTP server)\n",
	"\n"
);
#endif
