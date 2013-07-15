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
#include <common.h>
#include <command.h>
#include <menu.h>
#include <malloc.h>

#include <ext4fs.h>
#include <ide.h>
#include <ata.h>

#if defined(CONFIG_CMD_BOOT_MENU)

extern int do_stage_boot(cmd_tbl_t * cmdtb, int flag, int argc, char * const argv[]);
extern int do_ext2load (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
extern int do_ext2ls (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
extern int do_usb(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
extern int do_mmcops(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);

#define BM_TIMEOUT		(5)
#define BM_SCRIPTS_LOCATION	"/boot_scripts/"
#define BM_OUT_BUF_SIZE		(1024)
#define BM_MAX_LIST_ITEM	(8)
struct bm_item {
	char key[4];
	char *name;
} bm_item_list[BM_MAX_LIST_ITEM];

void bm_item_print(void *item_data)
{
	struct bm_item *item = (struct bm_item *)item_data;

	printf("    %s : %s\n", item->key, item->name);
}

int do_boot_menu(cmd_tbl_t * cmdtb, int flag, int argc, char * const argv[])
{
	char *out_buffer, *filename, *ifname, *dst;
	char source_device[10];
	char dev_part[10];
	struct bm_item *choice;
	int j, i, device, part;
	block_dev_desc_t * dev_desc;
	struct menu *m = NULL;
	char *args_to_func[2];
	disk_partition_t info;

	out_buffer = (char *)malloc(BM_OUT_BUF_SIZE);
	if (out_buffer == NULL) {
		printf("boot_menu: Failed to allocate memory!");
		return 0;
	}

	/* Read interface name, device number and partition from boot_source_device
	   environment variable. It should be set by stage_boot command. */
	ifname = getenv("boot_source_device");
	if (ifname == NULL) {
		printf("boot_menu: Environment variable 'boot_source_device' must be defined.\n");
		goto error;
	}
	strncpy(source_device, ifname, sizeof(source_device));
	source_device[sizeof(source_device) - 1] = '\0';
	ifname = strchr(source_device, ' ');
	if (ifname == NULL) {
		printf("boot_menu: Incorrect format of 'boot_source_device' variable.\n");
		goto error;
	}
	*ifname = '\0';
	ifname++;

	strncpy(dev_part, ifname, strstr(ifname, ':') - ifname - 1);
	device = simple_strtoul(ifname, NULL, 0);
	ifname = strchr(ifname, ':');
	if (ifname == NULL) {
		printf("boot_menu: Incorrect format of 'boot_source_device' variable.\n");
		goto error;
	}
	ifname++;
	part = simple_strtoul(ifname, NULL, 0);
	ifname = source_device;

	/* Initialize proper interface and check if the device exists. */
	if (strcmp(ifname, "ide") == 0)
		ide_init();
	else if (strcmp(ifname, "usb") == 0) {
		args_to_func[0]=ifname;
		args_to_func[1]="start";
		do_usb(cmdtb, 1, 2, args_to_func);
	} else if (strcmp(ifname, "mmc") == 0) {
		args_to_func[0]=ifname;
		args_to_func[1]="rescan";
		do_mmcops(cmdtb, 1, 2, args_to_func);
	}
	filename = BM_SCRIPTS_LOCATION;

	printf("Search for boot scripts in %s directory, on %s %d:%d [%s] device.\n",
		filename, ifname, device, part, dev_part);

	/* Read list of files in /boot_scripts/ directory. */
	out_buffer[0] = '\0';

	part = get_device_and_partition(args_to_func[0], dev_part, &dev_desc, &info, 1);
	if (part < 0)
		return 1;

	/* set the device as block device */
	ext4fs_set_blk_dev(dev_desc, &info);

	if (ext4fs_mount(info.size))
		ext4fs_ls (filename, out_buffer, BM_OUT_BUF_SIZE);
	ext4fs_close();

	/* Prepare a menu to select one of boot scripts. */
	putc('\n');
	putc('\n');
	m = menu_create("Select boot script:", BM_TIMEOUT * 10,
			0, bm_item_print);
	filename = out_buffer;
	j = 0;
	while (1) {
		filename = strstr(filename, ".scr");
		if (filename == NULL)
			break;
		if ((*(filename + 4) > 32) && (*(filename + 4) < 127)) {
			filename += 4;
			continue;
		}

		i = 4;
		while ((*(filename - 1) != ' ') && (*(filename - 1) != '\n')) {
			filename--;
			i++;
		}

		if (*(filename + i) == '\0')
			i = -1;
		else {
			*(filename + i) = '\0';
			i++;
		}

		sprintf(bm_item_list[j].key, "%d", j + 1);
		bm_item_list[j].name = filename;
		menu_item_add(m, bm_item_list[j].key, &bm_item_list[j]);

		j++;
		if (i < 0)
			break;
		if (j >= BM_MAX_LIST_ITEM)
			break;
		filename += i;
	}
	if (j == 0) {
		printf("boot_menu: No boot scripts found.\n");
		goto error;
	}

	/* Choose default value. If the bootmenu_default variable does not
	   exists or is invalid, the first option will be the default. */
	filename = getenv("bootmenu_default");
	i = j;
	if (filename) {
		for (i = 0; i < j; i++)
			if (strcmp(filename, bm_item_list[i].name) == 0) {
				menu_default_set(m, bm_item_list[i].key);
				break;
			}
	}
	if (i == j)
		menu_default_set(m, bm_item_list[0].key);

	/* Show the menu and save the default option. */
	if (menu_get_choice(m, (void**)&choice) != 1) {
		printf("boot_menu: Incorrect choice.\n");
		goto error;
	}
	menu_destroy(m);

	if ((!filename) || (strcmp(choice->name, filename) != 0)) {
		setenv("bootmenu_default", choice->name);
		saveenv();
	}

	/* Set device_partition and script_name to point to the chosen script and
	   call do_stage_boot. Copy original values and restore them after use. */
	ifname = getenv("device_partition");
	if (ifname) {
		dst = out_buffer + BM_OUT_BUF_SIZE - 1 - strlen(ifname);
		ifname = strcpy(dst, ifname);
	}
	filename = getenv("script_name");
	if (filename) {
		dst = ifname - 1 - strlen(filename);
		filename = strcpy(dst, filename);
	}

	sprintf(out_buffer, "%d:%d", device, part);
	setenv("device_partition", out_buffer);
	sprintf(out_buffer, "%s%s", BM_SCRIPTS_LOCATION, choice->name);
	setenv("script_name", out_buffer);

	args_to_func[0] = "stage_boot";
	if (strcmp(source_device, "ide") == 0)
		args_to_func[1] = "hd_scr";
	else if (strcmp(source_device, "usb") == 0)
		args_to_func[1] = "usb_scr";
	else if (strcmp(source_device, "mmc") == 0)
		args_to_func[1] = "mmc_scr";
	do_stage_boot(cmdtb, 1, 2, args_to_func);

	if (ifname)
		setenv("device_partition", ifname);
	if (filename)
		setenv("script_name", filename);

	free(out_buffer);
	return 1;

error:
	if (m)
		menu_destroy(m);
	free(out_buffer);
	return 0;
}

U_BOOT_CMD(boot_menu, 1, 1, do_boot_menu, "command allows to select boot script from boot device\n\
	example: boot_menu",
	"Usage:\n \
	 boot_menu - search device defined in boot_source_device variable for boot scripts and show menu.\n \
");

#endif
