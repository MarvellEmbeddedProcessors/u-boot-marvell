
#include <common.h>
#include <command.h>
#include <net.h>
#if defined(CONFIG_CMD_STAGE_BOOT)

#define LOAD_ADDR getenv("script_addr_r")
#define SCRIPT_PATH getenv("ide_path")
#define INTERFACE_HD "ide"
	
char enviroment[10][22];

extern int do_pxe_boot(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
extern int do_pxe_get(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
extern int do_dhcp (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
extern int do_ext2load (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
extern void * malloc(__kernel_size_t);
extern int free(void *);
void save_env (void);
void restore_env(void);

int do_stage_boot(cmd_tbl_t * cmdtb, int flag, int argc, char *argv[])
{
	char *path_to_image, *path_to_scr, *all_partions;
        ulong addr;
	int j,i=1,step,len,index,device;
	char * kernel_addr;
        char * args_to_func[5];
	char device_prt[4];

	path_to_image = (char *)malloc((strlen(SCRIPT_PATH)+strlen(getenv("image_name")))*sizeof(char));
	strcpy(path_to_image,SCRIPT_PATH);
	strcat(path_to_image,getenv("image_name"));
	path_to_scr = (char *)malloc((strlen(SCRIPT_PATH)+strlen(getenv("script_name")))*sizeof(char));
	strcpy(path_to_scr,SCRIPT_PATH);
	strcat(path_to_scr,getenv("script_name"));


	all_partions = (char *)malloc(strlen(getenv("device_partition"))*sizeof(char));
	strcpy(all_partions,getenv("device_partition"));
	kernel_addr = getenv("kernel_addr_r");

	ide_init();
	for(device=1;device<argc;device++)
	{
		/* step 1 load script from ide */
		len=strlen(all_partions);
		for(index=0;index<len && i==1 && strcmp(argv[device],"hd_scr")==0 ;index++)
		{
			step=1;
			for(j=0;j<3;j++,index++)
				device_prt[j]=all_partions[index];
			device_prt[3]='\0';
			printf("\ntry to load script from ide %s\n",device_prt);
			args_to_func[0]="ext2load";
			args_to_func[1]=INTERFACE_HD;
			args_to_func[2]=device_prt;
			args_to_func[3]=LOAD_ADDR;
			args_to_func[4]=path_to_scr;
			i = do_ext2load(cmdtb, 1, 5 , args_to_func);
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
		/* step 3 load linux image from ide */
		if( i == 1 && strcmp(argv[device],"hd_img")==0 )
		{
			step = 3;
			len=strlen(all_partions);
			for(index=0;index<len && i==1 ;index++)
			{
				for(j=0;j<3;j++,index++)
					device_prt[j]=all_partions[index];
				device_prt[3]='\0';
				printf("\ntry to load image from ide %s\n", device_prt);
				args_to_func[0]="ext2load";
				args_to_func[1]=INTERFACE_HD;
				args_to_func[2]=device_prt;
				args_to_func[3]=getenv("kernel_addr_r");
				args_to_func[4]=path_to_image;
				i = do_ext2load(cmdtb, 1, 5 , args_to_func);
				addr = simple_strtoul(args_to_func[3], NULL, 16);
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
	if(i==0)
	{	
		if(step == 1 || step == 4) 
			source(addr,NULL);
		else if (step == 3 || step == 5)
			do_bootm(cmdtb, 1,1,(char * const*)kernel_addr);
		else if (step ==2)
			 do_pxe_boot(cmdtb, 2, 1,(char * const *)NULL);

	}	        
	else {
		printf("Unable to load image/script\n");
	}
        return 1;


}
U_BOOT_CMD(stage_boot, 6, 1,do_stage_boot, "command to load script/image from different devices\n\
	example: stage_boot hd_img pxe",
	"Usage:\n \
	 stage_boot hd_img - load and boot image from installed system.\n \
	 stage_boot hd_scr - load and run script from installed system.\n \
	 stage_boot pxe - boot from network (PXE-boot).\n \
	 stage_boot net_img - load and boot image from tftp server.\n \
	 stage_boot net_scr - load and run script from tftp server.\n \
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


#endif
