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
#include <net.h>
#include <usb.h>
#include <mmc.h>
#include <ide.h>
#if defined(CONFIG_CMD_STAGE_BOOT)

#ifdef CONFIG_SYS_HUSH_PARSER
#include <hush.h>
#endif

#define LOAD_ADDR getenv("script_addr_r")
#define SCRIPT_PATH getenv("ide_path")
#define INTERFACE_HD "ide"
#define INTERFACE_MMC "mmc"
#define INTERFACE_USB "usb"
#define FLAG_EXIT_FROM_LOOP 1
#define FLAG_PARSE_SEMICOLON (1 << 1)	  /* symbol ';' is special for parser */
#define DEFAULT_DEVICE_NUMBER 0
#define DEFAULT_PART_NUMBER 1
#define MAX_MMC_DEV 1

char enviroment[10][22];

extern int marvell_recursive_parse;
extern int parse_string_outer(const char *s, int flag);
extern int do_pxe_boot(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
extern int do_pxe_get(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
extern int do_dhcp (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
extern int do_ext2load (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
extern int do_fat_fsload (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
extern int do_mmcops(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
extern int do_usb(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
extern void * malloc(__kernel_size_t);
extern int free(void *);
void save_env (void);
void restore_env(void);
int fsload (cmd_tbl_t *cmdtp, int flag, int argc, char * argv[]);

int do_stage_boot(cmd_tbl_t * cmdtb, int flag, int argc, char * const argv[])
{
	char *path_to_image, *path_to_scr, *path_to_initrd;
        ulong addr;
	int j,i=1,step=0,device,initrd=0,devnum,partnum,dev_count,max_dev_count;
	char * kernel_addr, *bootargs;
	char dev_part[6], *dev_part_ptr;
        char * args_to_func[5];
	char boot_source_device[8];
	block_dev_desc_t * dev_desc;

	if(argc < 2){
		printf("No args, try help stage_boot\n");
		return 0;
	}
	path_to_initrd = (char *)malloc((strlen(SCRIPT_PATH)+strlen(getenv("initrd_name"))+1)*sizeof(char));
	strcpy(path_to_initrd,SCRIPT_PATH);
	strcat(path_to_initrd,getenv("initrd_name"));
	path_to_image = (char *)malloc((strlen(SCRIPT_PATH)+strlen(getenv("image_name"))+1)*sizeof(char));
	strcpy(path_to_image,SCRIPT_PATH);
	strcat(path_to_image,getenv("image_name"));
	path_to_scr = (char *)malloc((strlen(SCRIPT_PATH)+strlen(getenv("script_name"))+1)*sizeof(char));
	strcpy(path_to_scr,SCRIPT_PATH);
	strcat(path_to_scr,getenv("script_name"));

	kernel_addr = getenv("kernel_addr_r");

	/* initialize interfaces */
	ide_init();
	args_to_func[0]="usb";
	args_to_func[1]="start";
	do_usb(cmdtb, 1, 2, args_to_func);
	args_to_func[0]="mmc";
	args_to_func[1]="rescan";
	do_mmcops(cmdtb, 1, 2, args_to_func);

	/* get device and partition number from 'device_partition' variable */
	dev_part_ptr = getenv("device_partition");
	if (dev_part_ptr == NULL)
	{
		devnum = DEFAULT_DEVICE_NUMBER;
		printf("'device_partition' variable not set"
		    ", using default device number "
		    "%d\n", devnum);
		partnum = DEFAULT_PART_NUMBER;
		printf("'device_partition' variable not set"
		    ", using default partition number "
		    "%d\n", partnum);
	}
	else
	{
		devnum = simple_strtoul(dev_part_ptr, NULL, 0);
		dev_part_ptr = strchr(dev_part_ptr, ':');
		if (dev_part_ptr == NULL)
		{
			devnum = DEFAULT_DEVICE_NUMBER;
			printf("'device_partition' variable set "
			    "incorrectly, using default device number "
			    "%d\n", devnum);
		}
		dev_part_ptr++;
		partnum = simple_strtoul(dev_part_ptr, NULL, 0);
		if (partnum == 0)
		{
			partnum = DEFAULT_PART_NUMBER;
			printf("'device_partition' variable set "
			    "incorrectly, using default partition number "
			    "%d\n", partnum);
		}
	}

	for(device=1;device<argc;device++)
	{
		/* step 1 load script from storage device */
		if( i == 1 && (strcmp(argv[device],"hd_scr")==0 ||
		    strcmp(argv[device],"mmc_scr")==0 ||
		    strcmp(argv[device],"usb_scr")==0))
		{
			step=1;
			dev_count=0;
			args_to_func[3]=LOAD_ADDR;
			args_to_func[4]=path_to_scr;
			if (strcmp(argv[device],"hd_scr") == 0)
			{
				args_to_func[1] = INTERFACE_HD;
				max_dev_count = CONFIG_SYS_IDE_MAXDEVICE;
			}
			else if (strcmp(argv[device],"mmc_scr") == 0)
			{
				args_to_func[1] = INTERFACE_MMC;
				max_dev_count = MAX_MMC_DEV;
			}
			else
			{
				args_to_func[1] = INTERFACE_USB;
				max_dev_count = USB_MAX_STOR_DEV;
			}
			printf("\nscanning %s devices beginning from: %d:%d\n",
			    args_to_func[1], devnum, partnum);
			for ( j = devnum; i==1 && j < max_dev_count; j++ )
			{
				dev_desc = get_dev(args_to_func[1], j);
				if ((dev_desc == NULL) || (dev_desc->type==DEV_TYPE_UNKNOWN))
					continue;
				dev_count++;
				sprintf(dev_part, "%d:%d", j, partnum);
				args_to_func[2] = dev_part;
				printf("\ntry to load script from %s %s\n",
				    args_to_func[1], args_to_func[2]);
				i = fsload(cmdtb, 1, 5 , args_to_func);
			}
			if (dev_count == 0)
				printf("No %s devices found\n", args_to_func[1]);
			addr = simple_strtoul(args_to_func[3], NULL, 16);
		}
		/* finish step 1 */
		/* step 2 boot PXE */
		if (i== 1 && strcmp(argv[device],"pxe") == 0)
		{
			step = 2;
			save_env();
			setenv("boot_from_pxe","1");
			setenv("autoload","no");
			setenv("pxefile_addr_r",LOAD_ADDR);
			args_to_func[0]="dhcp";
			args_to_func[1]=getenv("pxefile_addr_r");
			i = do_dhcp(cmdtb, 1, 1, args_to_func);
			if(i==0)
				i = do_pxe_get(cmdtb, 1 , 1, args_to_func);
			setenv("boot_from_pxe","0");
			if(i==1)
				restore_env();
		}
		/* finish step 2 */
		/* step 3 load linux image from storage device */
		if( i == 1 && (strcmp(argv[device],"hd_img")==0 ||
		    strcmp(argv[device],"mmc_img")==0 ||
		    strcmp(argv[device],"usb_img")==0))
		{
			step = 3;
			dev_count=0;
			args_to_func[3]=getenv("kernel_addr_r");
			args_to_func[4]=path_to_image;
			if (strcmp(argv[device],"hd_img") == 0)
			{
				args_to_func[1] = INTERFACE_HD;
				max_dev_count = CONFIG_SYS_IDE_MAXDEVICE;
			}
			else if (strcmp(argv[device],"mmc_img") == 0)
			{
				args_to_func[1] = INTERFACE_MMC;
				max_dev_count = MAX_MMC_DEV;
			}
			else
			{
				args_to_func[1] = INTERFACE_USB;
				max_dev_count = USB_MAX_STOR_DEV;
			}
			printf("\nscanning %s devices beginning from: %d:%d\n",
			    args_to_func[1], devnum, partnum);
			for ( j = devnum; i==1 && j < max_dev_count; j++ )
			{
				dev_desc = get_dev(args_to_func[1], j);
				if ((dev_desc == NULL) || (dev_desc->type==DEV_TYPE_UNKNOWN))
					continue;
				dev_count++;
				sprintf(dev_part, "%d:%d", j, partnum);
				args_to_func[2] = dev_part;
				printf("\ntry to load image from %s %s\n",
				    args_to_func[1], args_to_func[2]);
				i = fsload(cmdtb, 1, 5 , args_to_func);
			}
			if (dev_count == 0)
				printf("No %s devices found\n", args_to_func[1]);
			addr = simple_strtoul(args_to_func[3], NULL, 16);
			if( i==0 ){
				int temp;
				args_to_func[3]=getenv("ramdisk_addr_r");
				args_to_func[4]=path_to_initrd;
				temp = fsload(cmdtb, 1, 5 , args_to_func);
				if(temp == 0)
				{
					initrd = 1;
					args_to_func[1] = getenv("kernel_addr_r");
					args_to_func[2] = getenv("ramdisk_addr_r");
				}
			}
		}
		/* finish step 3 */
		/*step 4 load script from tftp */
		if( i == 1 && strcmp(argv[device],"net_scr")==0 )
		{
			printf("\ntry to load script from tftp\n");
			step = 4;
			args_to_func[0]="tftp";
			args_to_func[1]=LOAD_ADDR;
			args_to_func[2]=getenv("script_name");
		        i = do_tftpb(cmdtb, 1, 3,args_to_func);
			addr = simple_strtoul(args_to_func[1], NULL, 16);
		}
		/* finish step 4 */
		/*step 5 load linux image from tftp */
		if( i == 1 && strcmp(argv[device],"net_img")==0  )
		{
			printf("\ntry to load image from tftp\n");
			step = 5;
			args_to_func[0]="tftp";
			args_to_func[1]=getenv("kernel_addr_r");
			args_to_func[2]=getenv("image_name");
		        i = do_tftpb(cmdtb, 1, 3,args_to_func);
			addr = simple_strtoul(args_to_func[1], NULL, 16);
		}
		/* finish step 5 */
	}
	free(path_to_image);
	free(path_to_scr);
	free(path_to_initrd);
	path_to_image = getenv("bootargs_dflt");
	if(!path_to_image) {
		printf("missing environment variable: bootargs_dflt\n");
		return 0;
	}
	if (step == 3 && initrd ==1){
		setenv("bootargs_dflt","console=ttyS0,115200 earlyprintk=ttyS0 root=/dev/sda2 ro pm_disable");
	}
	bootargs = (char *)malloc(sizeof(char)*(strlen(getenv("bootargs_dflt"))+17));
	strcpy(bootargs,"setenv bootargs ");
	#ifndef CONFIG_SYS_HUSH_PARSER
		if (run_command(strcat(bootargs,getenv("bootargs_dflt")), flag) < 0) {
			printf("missing environment variable: bootargs_dflt\n");
			return 0;
		}
	#else
		marvell_recursive_parse = 1;
		if (parse_string_outer(strcat(bootargs,getenv("bootargs_dflt")),
			FLAG_PARSE_SEMICOLON | FLAG_EXIT_FROM_LOOP) != 0){
		printf("missing environment variable: bootargs_dflt\n");
		marvell_recursive_parse = 0;
		return 0;
		}
		marvell_recursive_parse = 0;
	#endif
	if(i==0)
	{
		if (step == 1)
		{
			sprintf(boot_source_device, "%s %s", args_to_func[1], args_to_func[2]);
			setenv("boot_source_device", boot_source_device);
			source(addr,NULL);
		}
		else if (step == 4)
			source(addr,NULL);
		else if ((step == 3 && initrd ==0 )|| step == 5)
			do_bootm(cmdtb, 1,1,(char * const*)kernel_addr);
		else if (step ==2)
			do_pxe_boot(cmdtb, 2, 1,(char * const *)NULL);
		else if (step == 3 && initrd ==1){
			do_bootm(cmdtb, 1,2,args_to_func);
		}

	}
	else {
		printf("Unable to load image/script\n");
	}
        return 1;


}
U_BOOT_CMD(stage_boot, 10, 1,do_stage_boot, "command to load script/image from different devices\n\
	example: stage_boot hd_img pxe",
	"Usage:\n \
	 stage_boot hd_img - load and boot image from system installed on ide.\n \
	 stage_boot hd_scr - load and run script from system installed on ide.\n \
	 stage_boot pxe - boot from network (PXE-boot).\n \
	 stage_boot net_img - load and boot image from tftp server.\n \
	 stage_boot net_scr - load and run script from tftp server.\n \
	 stage_boot usb_img - load and boot image from system installed on usb.\n \
	 stage_boot usb_scr - load and run script from system installed on usb.\n \
	 stage_boot mmc_img - load and boot image from system installed on mmc.\n \
	 stage_boot mmc_scr - load and run script from system installed on mmc.\n \
");

void save_env (void)
{
	strcpy(enviroment[0],getenv("serverip"));
	strcpy(enviroment[1],getenv("ipaddr"));
	strcpy(enviroment[2],getenv("netmask"));
	strcpy(enviroment[3],getenv("rootpath"));
}

void restore_env(void)
{
	strcpy(getenv("serverip"),enviroment[0]);
	strcpy(getenv("ipaddr"),enviroment[1]);
	strcpy(getenv("netmask"),enviroment[2]);
	strcpy(getenv("rootpath"),enviroment[3]);
}

int fsload (cmd_tbl_t *cmdtp, int flag, int argc, char * argv[])
{
	int result;

	argv[0] = "ext2load";
	result = do_ext2load (cmdtp, flag, argc, argv);
	if (result) {
		printf("Failed to mount ext filesystem, trying fat...\n");
		argv[0] = "fatload";
		result = do_fat_fsload (cmdtp, flag, argc, argv);
	}

	return result;
}

#endif
