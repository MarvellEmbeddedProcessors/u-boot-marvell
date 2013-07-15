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

#if defined(CONFIG_CMD_SYS_RESTORE)

#include "mvTypes.h"
#include "usb.h"
#include <malloc.h>

extern int do_stage_boot(cmd_tbl_t * cmdtb, int flag, int argc, char * const argv[]);

int do_sys_restore (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char *args_to_func[2], *dev_part, *script_name, *env;

	/* Make a copy of device_partition and script_name variables. */
	dev_part = NULL;
	script_name = NULL;
	env = getenv("device_partition");
	if (env) {
		dev_part = (char *)malloc((strlen(env) + 1) * sizeof(char));
		if (dev_part)
			strcpy(dev_part, env);
	}
	env = getenv("script_name");
	if (env) {
		script_name = (char *)malloc((strlen(env) + 1) * sizeof(char));
		if (script_name)
			strcpy(script_name, env);
	}

	/* Use stage_boot to find and execute install.scr script on USB DOK */
	setenv("device_partition", "0:1");
	setenv("script_name", "install.scr");

	args_to_func[0] = "stage_boot";
	args_to_func[1] = "usb_scr";
	do_stage_boot(cmdtp, 1, 2, args_to_func);

	/* Revert variables in case of an error */
	if (dev_part) {
		setenv("device_partition", dev_part);
		free(dev_part);
	}
	if (script_name) {
		setenv("script_name", script_name);
		free(script_name);
	}

	return 1;
}

U_BOOT_CMD(
	sys_restore,	1,	1,	do_sys_restore,
	"sys_restore\t- Search for install script on USB DOK and start installation of linux kernel and rootfs.\n",
	"\n"
);
#endif
