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
#include <command.h>
#include "mvOs.h"
#include "mv_amp.h"
#include "mvCpuIfRegs.h"
#include "uart/mvUart.h"
#ifdef CONFIG_SYS_HUSH_PARSER
#include <hush.h>
#endif

#ifdef CONFIG_AMP_SUPPORT

#ifdef CONFIG_AMP_TEST_CPU1
#define TEST_CPU1_WAKE_UP
#endif

int  image_load_addr;   /* used to pass load address to micro loader */
int  image_param_addr;  /* used to pass parameters address to micro loader */
int  machine_id;        /* used to pass machine id for linux micro loader */
int  amp_group_id;      /* Stores the current amp group id */
int  amp_sync_boot;     /* All images exit U-BOOT together using barrier*/
int  amp_hw_lock_id = 0;   /* Spinlock for barrier */
int  amp_enable = 0;
volatile int  amp_barrier;       /* Barrier for AMP launch */

void linuxMicroLoader(void);
void vxWorksMicroLoader(void);
void lock_hw_mutex(int hw_lock_id);
void unlock_hw_mutex(int hw_lock_id);
int amp_verify_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);

DECLARE_GLOBAL_DATA_PTR;

void amp_init(void)
{
	char *env;
	int group_count = 0;

	env = getenv("amp_enable");
	if((env) && (((strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0) ))) {
		amp_enable = 1;
	}

	if(amp_enable)
	{
		env = getenv("amp_groups");
		if(env){
			group_count = simple_strtoul(env, NULL, 0);
		}

		if(group_count < 2)
			amp_sync_boot = 0;
		else
			amp_sync_boot = 1;

		amp_barrier = group_count;
	}
}

static void amp_duplicate_env(char *env_name, int group_id, int override)
{
	char *env_val, *env;
	char new_env_name[256];

	env_val = getenv(env_name);
	if(!env_val){
		printf("Failed to duplicate undefined env %s\n", env_name);
		return;
	}

	sprintf(new_env_name,"%s_g%d", env_name, group_id);

	env = getenv(new_env_name);
	if((!env) || (override)) {
		setenv(new_env_name, env_val);
	}

}

static void amp_set_env(char *env_name, char *val, int group_id, int override)
{
	char *env;
	char new_env_name[256];

	sprintf(new_env_name,"%s_g%d", env_name, group_id);

	env = getenv(new_env_name);
	if((!env) || (override)) {
		setenv(new_env_name,val);
	}
}

static char* amp_getenv(char *env_name, int group_id)
{
	char *env;
	char amp_name[256];

	sprintf(amp_name,"%s_g%d", env_name, group_id);

	env = getenv(amp_name);

	return env;
}

static void amp_set_linux_bootargs(int group_id)
{
	char *env, args[256], tmp[256];

	args[0] = '\0';

	sprintf(tmp,"amp_cpu_count_g%d", group_id);
	env = getenv(tmp);
	if(env){
		sprintf(args, "mv_cpu_count=%s",env);
	}

	sprintf(tmp,"amp_resources_g%d", group_id);
	env = getenv(tmp);
	if(env){
		sprintf(args, "%s mv_rsrc=%s",args, env);
	}

	env = getenv("amp_shared_mem");
	if(env){
		sprintf(args, "%s mv_sh_mem=%s", args, env);
	}

	/* Each group gets the master cpu of the other group */
	sprintf(tmp,"amp_master_cpu_g%d", (!group_id));
	env = getenv(tmp);
	if(env){
	        sprintf(args, "%s mv_ipc=%s",args, env);
	}

	sprintf(tmp,"amp_bootargs_g%d", group_id);
	setenv(tmp, args);
}

static void amp_set_vxworks_bootargs(int group_id)
{
	char args[256], tmp[256];

	args[0] = '\0';

	sprintf(args, "neta(0,0)");

	sprintf(tmp,"amp_bootargs_g%d", group_id);
	setenv(tmp, args);
}

static void amp_create_group(int group_id, char *os, char *cpu_count, char *master_cpu,
							 char *mem_base, char *mem_size, char *resrouces, int override)
{
	char boot_cmd[1024], *env;
	char cmd_name[256];
	int img_load_addr;
	int isLinux;
	int isVxWorks;
	int isFreeRTOS;

	isLinux   = ((strcmp(os, "linux") == 0));
	isVxWorks = ((strcmp(os, "vxworks") == 0));
	isFreeRTOS = ((strcmp(os, "freertos") == 0));

	if(!isVxWorks && ! isLinux && !isFreeRTOS)
	{
		printf("Error: Specified unsupported OS %s\n", os);
		return;
	}

	// Create amp envs
	amp_set_env("amp_os", os, group_id, override);
	amp_set_env("amp_cpu_count", cpu_count, group_id, override);
	amp_set_env("amp_master_cpu", master_cpu, group_id, override);
	amp_set_env("amp_mem_base", mem_base, group_id, override);
	amp_set_env("amp_mem_size", mem_size, group_id, override);
	amp_set_env("amp_resources", resrouces, group_id, override);

	/* Duplicate boot variables for group != 0*/
	/* They can be changed later using console */
	if(group_id > 0){
		amp_duplicate_env("image_name", group_id, 0);
		amp_duplicate_env("console", group_id, 0);
#ifdef CONFIG_MTD_DEVICE
		amp_duplicate_env("mtdparts", group_id, 0);
#endif
		amp_duplicate_env("serverip", group_id, 0);
		amp_duplicate_env("bootargs_root", group_id, 0);
		amp_duplicate_env("rootpath", group_id, 0);
		amp_duplicate_env("mvNetConfig", group_id, 0);
		amp_set_env("ipaddr", "192.168.1.1", group_id, 0);

		if(isLinux || isFreeRTOS)
			amp_duplicate_env("bootargs_end", group_id, 0);
	}

	/* Create a boot command */
	img_load_addr = 0x2000000 + (0x1000000 * group_id);

	sprintf(cmd_name, "bootcmd_g%d", group_id);

	if(isLinux || isFreeRTOS)
	{
		amp_set_linux_bootargs(group_id);

	if(group_id == 0){
			sprintf(boot_cmd, "tftpboot %#x $image_name; setenv bootargs "
							  "$console $mtdparts $bootargs_root "
							  "nfsroot=$serverip:$rootpath ip=$ipaddr:$serverip$bootargs_end "
							  "$mvNetConfig $amp_bootargs_g%d video=dovefb:lcd0:$lcd0_params "
							  "clcd.lcd0_enable=$lcd0_enable clcd.lcd_panel=$lcd_panel;  bootm %#x; ",
							  img_load_addr, group_id, img_load_addr);
		}
	else{
			sprintf(boot_cmd, "tftpboot %#x $image_name_g%d; setenv bootargs "
							  "$console_g%d $mtdparts_g%d $bootargs_root_g%d  "
						      "nfsroot=$serverip_g%d:$rootpath_g%d "
							  "ip=$ipaddr_g%d:$serverip_g%d$bootargs_end_g%d "
							  "$mvNetConfig_g%d $amp_bootargs_g%d video=dovefb:lcd0:$lcd0_params "
							  "clcd.lcd0_enable=$lcd0_enable clcd.lcd_panel=$lcd_panel;  bootm %#x; ",
							  img_load_addr, group_id, group_id, group_id, group_id,
							  group_id, group_id, group_id, group_id, group_id,
							  group_id, group_id, img_load_addr);
		}
	}
	else if (isVxWorks)
	{
		amp_set_vxworks_bootargs(group_id);
		if(group_id == 0){
			setenv("bootargs_end", "ffffff00 u=anonymous pw=target");
			sprintf(boot_cmd, "tftpboot %#x $image_name; setenv bootargs "
							  "$amp_bootargs_g%d host:$image_name h=$serverip "
							  "e=$ipaddr:$bootargs_end "
							  "o=$amp_resources_g%d:cpus=$amp_cpu_count_g%d"
							  ":mem=$amp_mem_size_g%d; bootvx %#x;",
							  img_load_addr, group_id, group_id, group_id, group_id, img_load_addr);
		}
		else{
			amp_set_env("bootargs_end", "ffffff00 u=anonymous pw=target", group_id, override);
			sprintf(boot_cmd, "tftpboot %#x $image_name_g%d; setenv bootargs "
							  "$amp_bootargs_g%d host:$image_name_g%d h=$serverip_g%d "
							  "e=$ipaddr_g%d:$bootargs_end_g%d "
							  "o=$amp_resources_g%d:cpus=$amp_cpu_count_g%d"
							  ":mem=$amp_mem_size_g%d; bootvx %#x;",
							  img_load_addr, group_id, group_id, group_id, group_id
							  , group_id, group_id, group_id, group_id, group_id, img_load_addr);
		}
	}

	env = getenv(cmd_name);
	if((!env) || (override))
		setenv(cmd_name, boot_cmd);
}

void amp_wait_to_boot(void)
{
	if(amp_sync_boot == 0)
		return;

	// Notify "im here" by reducing counter
	lock_hw_mutex(amp_hw_lock_id);

	amp_barrier--;

	unlock_hw_mutex(amp_hw_lock_id);

	// Now wait untill all cores
	while(amp_barrier != 0);
}

static void amp_get_boot_cmd(char *boot_cmd_name, int group_id)
{
	char tmp[16];
	char *env;

	sprintf(boot_cmd_name,"bootcmd_g%d", group_id);

	/* Reset the amp boot args to consider latest changes*/
	sprintf(tmp,"amp_os_g%d", group_id);
	env = getenv(tmp);

	if((strcmp(env, "linux") == 0))
		amp_set_linux_bootargs(group_id);
}

/* The mv_amp_group_setup() function tries to allocate requested size of physical
 * memory for each of the AMP groups. It is called from do_bootm_linux() and have
 * to overwrite the dram_hw_info array before call to setup_memory_tags().
 * It will adjust overlapping memory blocks, cut out shared memory and split the
 * window going through the 4GB boundary.
 * After this function is called, the info about memory banks (dram_hw_info) will
 * be corrupted and it will be impossible to run amp_boot again. */
int mv_amp_group_setup(int group_id, int load_addr)
{
	char *env;
	int i;
	unsigned long long mem_base = -1;
	unsigned long long mem_size = 0;
	unsigned long long mem_total = 0;
	unsigned long shared_base = -1;
	unsigned long shared_size = 0;

	unsigned long long next_mem_base;
	unsigned long g_mem_base;
	unsigned long long g_mem_size;
	unsigned long last_mem_base = 0;
	unsigned long long last_mem_size = 0;

	static unsigned int last_group_id;
	static unsigned long long last_mem_end = 0;
	static unsigned long long phys_mem_end = 0;

	env = getenv("amp_enable");
	if( !env || ( ((strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) )))
		return 0;

	env = amp_getenv("amp_mem_base", group_id);
	if(env)
		mem_base = simple_strtoul(env, NULL, 16);
	else {
		printf("Error: please set variable amp_mem_base_g%d\n", group_id);
		return 1;
	}

	env = amp_getenv("amp_mem_size", group_id);
	if(env)
		mem_size = simple_strtoull(env, NULL, 16);
	else {
		printf("Error: please set variable amp_mem_size_g%d\n", group_id);
		return 1;
	}

	env = getenv("amp_shared_mem");
	if(env) {
		shared_base = simple_strtoul(env, &env, 16);
		if (*env == ':')
			shared_size = simple_strtoul(++env, NULL, 16);
	}

	/* Count the total size of contiguous physical memory */
	if(phys_mem_end == 0) {
		phys_mem_end = gd->dram_hw_info[0].start;
		for(i = 0; i < CONFIG_NR_DRAM_BANKS; i++)
			if(phys_mem_end == gd->dram_hw_info[i].start)
				phys_mem_end += gd->dram_hw_info[i].size;
	}

	mem_total = mem_size;
	/* Check whether the memory region does not interfere with other groups
	 * Find end of memory block occupied by the group which have the highest base */
	for(i = 0; i < MAX_AMP_GROUPS; i++) {
		env = amp_getenv("amp_mem_base", i);
		if(env == NULL)
			continue;
		g_mem_base = simple_strtoul(env, NULL, 16);
		env = amp_getenv("amp_mem_size", i);
		if(env == NULL)
			continue;
		g_mem_size = simple_strtoull(env, NULL, 16);

		if((last_mem_end == 0) && (g_mem_base >= last_mem_base)) {
			last_mem_base = g_mem_base;
			last_mem_size = g_mem_size;
			last_group_id = i;
		}

		if((mem_base < g_mem_base) &&
				(mem_base + mem_size > g_mem_base))
			mem_size = g_mem_base - mem_base;
	}

	/* Verify load address matches physical memory region */
	if(((unsigned int)load_addr < mem_base) || ((unsigned int)load_addr >= mem_base + mem_size)){
		printf("\nAMP Error: Load address (%#08x) not within allocated memory "
		       "[%#08x:%#08x]\n\n", load_addr, (unsigned int)mem_base,
		       (unsigned int)(mem_base + mem_size));
		return 1;
	}

	if(last_mem_end == 0) {
		if(last_mem_size == 0) {
			printf("\nAMP Error: Groups memory not configured properly.\n");
			return 1;
		} else
			last_mem_end = last_mem_base + last_mem_size;
	}

	gd->bd->bi_boot_params = mem_base + (0x100); // == param_phys in Makefile.boot

	/* Overwrite dram_hw_info array to change available memory blocks passed
	 * to the OS from setup_memory_tags() */
	for(i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
		next_mem_base = 0;

		if(mem_base >= phys_mem_end) {
			mem_base = 0;
			mem_size = 0;
		} else if(mem_base + mem_size > phys_mem_end) {
			mem_size = phys_mem_end - mem_base;
			next_mem_base = phys_mem_end;
		}
		if((mem_base < 0x100000000ll) &&
				(mem_base + mem_size > 0x100000000ll)) {
			mem_size = 0x100000000ll - mem_base;
			next_mem_base = 0x100000000ll;
		}
		if((shared_size > 0) && (mem_base <= shared_base) &&
				(mem_base + mem_size > shared_base)) {
			mem_size = shared_base - mem_base;
			next_mem_base = shared_base + shared_size;
			if (mem_total > shared_size)
				mem_total -= shared_size;
			else
				mem_total = 0;
		}
		if(next_mem_base > last_mem_end)
			last_mem_end = next_mem_base;
		if(mem_base + mem_size > last_mem_end)
			last_mem_end = mem_base + mem_size;
		if((group_id != last_group_id) && (next_mem_base < last_mem_end))
			next_mem_base = last_mem_end;

		gd->dram_hw_info[i].start = mem_base;
		gd->dram_hw_info[i].size  = mem_size;

		mem_total -= mem_size;
		mem_size = mem_total;
		if(mem_size > 0)
			mem_base = next_mem_base;
		else
			mem_base = 0;
	}

	return 0;
}
#ifdef TEST_CPU1_WAKE_UP
void cpu1IsAlive(void)
{
    ulong cpu1Stack = 0x20000;
    int i;
    asm ("mov sp, %0" : "=r" (cpu1Stack) : );

    for (i=0; i<20; i++) {
	printf("I'm CPU 1 - test cpu1 wake up (defined TEST_CPU1_WAKE_UP)\n");
    }
    hang();
}
#endif
static void mvWakeCpuToFunc(int cpu_id, void *func_addr)
{
	MV_U32 *pOpcode;
	/* Enable a window of size 0x7FF for bootrom code */

        MV_REG_WRITE(0x200b8,0x00000991);//config window to Tunit SRAM
        MV_REG_WRITE(0x200bc ,0xffff0000);
	pOpcode = (MV_U32 *)0xffff0000;
	*pOpcode++= 0xe59f0004;  //set op-codes for boot
	*pOpcode++= 0xe5900000;
	*pOpcode++= 0xe12fff30;
	*pOpcode++= 0xf10182d4;

	/* Write function pointer to boot register. Bootrom will
	 * read from theres_more and jump to that adress */
#ifdef TEST_CPU1_WAKE_UP
	if (cpu_id)
	    func_addr  = cpu1IsAlive;
#endif
        // set address in register
	MV_REG_WRITE(CPU_RESUME_ADDR_REG(cpu_id), (MV_U32)func_addr);

	/* Release hanging core by writing to reset register */
	MV_REG_WRITE(CPU_SOFT_RESET_REG(cpu_id), 0);
}

#define AXP_CPU_DIVCLK_CTRL0			0x18700
#define AXP_CPU_DIVCLK_CTRL2_RATIO_FULL0	0x18708
#define AXP_CPU_DIVCLK_CTRL2_RATIO_FULL1	0x1870C

static void mvSetSecondaryClock(int cpu_id)
{
	MV_U32 val = 0;
	MV_U32 divider;

	/* Scale up CPU clock to max */
	divider = MV_REG_READ(AXP_CPU_DIVCLK_CTRL2_RATIO_FULL1);
	divider = (divider & 0x3F);
	if (cpu_id == 1) {
		val = MV_REG_READ(AXP_CPU_DIVCLK_CTRL2_RATIO_FULL1);
		val &= ~(0x0000FF00); 	/* cpu1 clkdiv ratio; cpu0 based on SAR */
		val |= divider << 8;
		MV_REG_WRITE(AXP_CPU_DIVCLK_CTRL2_RATIO_FULL1, val);
	}
	else if (cpu_id == 2) {
		val = MV_REG_READ(AXP_CPU_DIVCLK_CTRL2_RATIO_FULL1);
                val &= ~(0x00FF0000);   /* cpu1 clkdiv ratio; cpu0 based on SAR */
                val |= divider << 16;
                MV_REG_WRITE(AXP_CPU_DIVCLK_CTRL2_RATIO_FULL1, val);
	}
	else if (cpu_id == 3) {
		val = MV_REG_READ(AXP_CPU_DIVCLK_CTRL2_RATIO_FULL1);
		val &= ~0xFF000000;	/* cpus 3 clkdiv ratios */
		val |= divider << 24;
		MV_REG_WRITE(AXP_CPU_DIVCLK_CTRL2_RATIO_FULL1, val);
	}

	/* Set clock devider reload smooth bit mask */
	val = MV_REG_READ(AXP_CPU_DIVCLK_CTRL0);
	val |= (0x1 << (cpu_id +  20));
	MV_REG_WRITE(AXP_CPU_DIVCLK_CTRL0, val);

	/* Request clock devider reload */
	val = MV_REG_READ(AXP_CPU_DIVCLK_CTRL0);
	val |= 1 << 24;
	MV_REG_WRITE(AXP_CPU_DIVCLK_CTRL0, val);

	/* Wait for clocks to settle down then release reload request */
	udelay(100);
	val &= ~(0xf << 21);
	MV_REG_WRITE(AXP_CPU_DIVCLK_CTRL0, val);
	udelay(100);
}

/*static void mvPrepareSecondaryLoad(int mach_id, int load_addr, int param_addr, int uart_port, int cpu_id)
{

}*/

static void amp_linux_boot(int mach_id, int load_addr, int param_addr, int group_id)
{
	int clock_divisor = (CONFIG_SYS_TCLK / 16)/115200;
	char *env, master_id_env[256], os_env[256];
	int master_core_id=0;

	/* Get master core id */
	sprintf(master_id_env,"amp_master_cpu_g%d", group_id);
	env = getenv(master_id_env);
	if(env) {
		master_core_id = strtol(env, NULL,16);
	}
	else{
		printf("AMP Error: please set variable %s\n", master_id_env);
	}

	// Get OS type
	sprintf(os_env,"amp_os_g%d", group_id);
	env = getenv(os_env);
	if(!env)
		printf("AMP Error: please set variable %s\n", os_env);

	printf ("\nBooting AMP group %d on CPU %d ...\n\n", group_id, master_core_id);

	// Initialize UART for secondary group
	mvUartInit(group_id, clock_divisor, mvUartBase(group_id));
	// Initialize clock for secondary CPU
	mvSetSecondaryClock(master_core_id);

	// Set globals to be accessed by micro loader
	machine_id 	 = mach_id;
	image_load_addr  = load_addr;
	image_param_addr = param_addr;

	if((strcmp(env, "linux") == 0) || (strcmp(env, "freertos") == 0)) {
	/* Wake up CPU to to micro loader function */
	mvWakeCpuToFunc(master_core_id, &linuxMicroLoader);
}
	else if (strcmp(env, "vxworks") == 0){
		mvWakeCpuToFunc(master_core_id, &vxWorksMicroLoader);
	}
	else {
		printf("AMP Error: Unsupported OS type %s\n", env);
	}
}

int amp_boot(int machid, int load_addr, int param_addr)
{
	/* Groups 1 an above are loaded on core > 1. so wake them up and return.
	 * Group 0 waits for other groups and proceeds to regular launch */
	if(amp_group_id > 0){
		amp_linux_boot(machid, load_addr, param_addr, amp_group_id);
		return 1;
	}
	else{
		return 0;
	}
}

static void group_printenv(char *env_name, int group_id, int amp_only_env)
{
	char command[256];

	if((amp_only_env == 0) && (group_id == 0)){
		sprintf(command,"printenv %s",env_name);
	}
	else{
		sprintf(command,"printenv %s_g%d",env_name, group_id);
	}

	parse_string_outer(command, 0);
}

int amp_printenv_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int group_id = -1, group_count, id;
	char *env, *os;
	int isLinux, isFreeRTOS;

	env = getenv("amp_enable");
	if(!env || ( ((strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) ))){
		printf ("Error: Command available only when amp_enable=yes\n");
		return 1;
	}

	if(argc > 1){
		group_id = simple_strtoul(argv[1], NULL, 0);
	}

	/* Ensures the loop prints the required group */
	id 	    = group_id;
	group_count = group_id + 1;

	/* Make the loop print all groups */
	if(group_id == -1)
	{
		id = 0;

		env = getenv("amp_groups");
		if(!env){
			printf ("Warning: amp_groups undefined, assuming 0 groups\n");
			group_count = 0;
		}
		else{
			group_count = simple_strtoul(env, 0, 10);
		}
	}

	/* Print shared AMP envs*/
	printf("\n**** AMP shared ****\n\n");
	parse_string_outer("printenv amp_groups",0);
	parse_string_outer("printenv amp_shared_mem",0);

	/* Print group sepcific envs */
	for(; id < group_count; id++)
	{
		os = amp_getenv("amp_os", id);
		isLinux   = ((strcmp(os, "linux") == 0));
		isFreeRTOS  = ((strcmp(os, "freertos") == 0));

		printf("\n**** AMP Group %d ****\n\n", id);
		group_printenv("amp_os", id, 1);
		group_printenv("amp_master_cpu", id, 1);
		group_printenv("amp_cpu_count", id, 1);
		group_printenv("amp_mem_base", id, 1);
		group_printenv("amp_mem_size", id, 1);
		group_printenv("amp_resources", id, 1);
		group_printenv("amp_bootargs", id, 1);

		group_printenv("image_name", id, 0);
		group_printenv("ipaddr", id, 0);
		group_printenv("serverip", id, 0);
		group_printenv("bootargs_end", id, 0);

		if(isLinux || isFreeRTOS){
			group_printenv("console", id, 0);
#ifdef CONFIG_MTD_DEVICE
			group_printenv("mtdparts", id, 0);
#endif
		group_printenv("bootargs_root", id, 0);
		group_printenv("rootpath", id, 0);
		group_printenv("mvNetConfig", id, 0);
	}
		group_printenv("bootcmd", id, 1);
	}

	return 1;
}

U_BOOT_CMD(
        amp_printenv,      2,     1,      amp_printenv_cmd,
        "amp_printenv - print only AMP env variables\n",
        "<group_id> \n"
        "\t<group_id> - which AMP group env to print. default = all. \n"
);


static int amp_boot_group(int group_id)
{
	char bootcmd_name[256];
	int rcode = 0;

	// Change current AMP group ID
	amp_group_id = group_id;

	// Fetch the bootcmd name. usualy bootcmd_g${X}. X = group ID
	amp_get_boot_cmd(bootcmd_name, group_id);

	// run the boot command
#ifndef CONFIG_SYS_HUSH_PARSER
	if (run_command (getenv (bootcmd_name), 0) < 0)
		rcode = 1;
#else
	if (parse_string_outer (getenv (bootcmd_name),
			FLAG_PARSE_SEMICOLON | FLAG_EXIT_FROM_LOOP) != 0)
		rcode = 1;
#endif

	return rcode;
}

int amp_boot_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int group_id = -1, group_count;
	char *env;
	int rcode = 0;

	env = getenv("amp_enable");
	if(!env || ( ((strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) ))){
		printf ("Error: Command available only when amp_enable=yes\n");
		return 1;
	}
	/* Verify boot params prior to boot */
	env = getenv("amp_verify_boot");
	if((env) && ((strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0))) {
		if(amp_verify_cmd(0,0,0, argv))
			return 1;
	}
	if(argc > 1){
		group_id = simple_strtoul(argv[1], NULL, 0);

		if(argc > 2){
			amp_sync_boot = simple_strtoul(argv[2], NULL, 0);
		}

		/*amp_group_id = group_id;

		do_bootd(cmdtp, flag, 1, NULL);*/

		rcode = amp_boot_group(group_id);
	}
	else{
		/* If no group id specified, boot all groups*/
		env = getenv("amp_groups");

		if(!env){
			printf("Error: please define amp_groups\n");
			return 1;
		}
		else{
			group_count = simple_strtoul(env, NULL, 0);

			if(group_count < 1){
				printf("Error: please define amp_groups > 0\n");
				return 1;
			}
		}

		/* Start booting groups. keep group 0 last */
		group_id = group_count;
		while(group_id--)
		{
			/*amp_group_id = group_id;
			do_bootd(cmdtp, flag, 1, NULL);*/
			rcode |= amp_boot_group(group_id);
		}
	}

	return rcode;
}

U_BOOT_CMD(
        amp_boot,      3,     1,      amp_boot_cmd,
        "amp_boot - boot an AMP group\n",
        "<group_id> \n"
	"<sync_load> \n"
        "\t<group_id> - which AMP group to boot. default = all. \n"
	"\t<sync_load> - All image exit U-BOOT together. \n"
);


int amp_config_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int group_id, group_count;
	char *env, groups[5];

	env = getenv("amp_enable");
	if(!env || ( ((strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) ))){
		printf ("Error: Command available only when amp_enable=yes\n");
		return 1;
	}

	// Config cpu count
	if (argc != 8){
		printf ("Please specify all AMP parameters\n");
		return 1;
	}

	group_id    = simple_strtoul(argv[1], NULL, 0);
	group_count = simple_strtoul(getenv("amp_groups"), NULL, 0);

	if((group_id < 0) || (group_id > (MAX_AMP_GROUPS -1))) {
		printf ("Bad group id %d specified. valid range [%d, %d]\n", group_id, 0, (MAX_AMP_GROUPS -1));
		return 1;
	}

	if ((strcmp(argv[2], "linux") != 0) && (strcmp(argv[2], "vxworks") != 0) &&
	    (strcmp(argv[2], "freertos") != 0))
	{
		printf("Error: Specified unsupported OS %s\n", argv[2]);
		return 1;
	}

	/* Add group to group count */
	if(group_id >= group_count)
	{
		sprintf(groups, "%d", group_count + 1);
		setenv("amp_groups", groups);
	}

	/* Create all env variables for group.*/
	amp_create_group(group_id, argv[2], argv[3], argv[4], argv[5], argv[6], argv[7],1);

	return 0;
}

U_BOOT_CMD(
        amp_config,     8,     1,      amp_config_cmd,
        "Fully config an AMP group\n",
        "<group_id> <os_type> <cpu_count>  <master_cpu> <mem_base> <mem_size> <resources> \n"
        "\t<group_id>   - which AMP group to config\n"
        "\t<os_type>    - Os type to load. Accepts: linux and vxworks\n"
	"\t<cpu_count>  - how many cpus to use in the group. \n"
	"\t<master_cpu> - cpu id of first cpu in the group. \n"
	"\t<mem_base>   - base physical address of group's image \n"
	"\t<mem_size>   - requested memory size of group's image; for LPAE may overlap with other groups \n"
	"\t<resources>  - resource string \n"
);


int amp_verify_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int group_count, id, test_id, i;
	int cpu_count, master_cpu, mem_base, shared_base, shared_size = 0;
	int mem_blk_start[MAX_AMP_GROUPS];
	int base_cpu_id;
	int mstr_rsrc = 0;
	char *env;
        char *src, *test, *token;
	char tmp_rsrc[256];


	env = getenv("amp_enable");
	if(!env || ( ((strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) ))){
		printf ("Set amp_enable=yes\n");
		goto failed;
	}

	/* Verify group count */
	env = getenv("amp_groups");
	if((!env)){
		printf ("Set amp_groups\n");
		goto failed;
	}
	else{
		group_count = simple_strtoul(env, NULL, 0);

		if(group_count < 2){
			printf ("Invalid amp_groups = %d. Set amp_groups > 1 to enable AMP\n", group_count);
			goto failed;
		}
		if(group_count > MAX_AMP_GROUPS){
			printf ("amp_groups %d too large. Supporting %d groups\n",group_count,  MAX_AMP_GROUPS);
			goto failed;
		}
	}

	base_cpu_id = 0;

	for(id = 0; id < group_count; id++)
	{
		/* Verify CPU mapping */
		cpu_count  = simple_strtoul(amp_getenv("amp_cpu_count", id), NULL, 0);
		master_cpu = simple_strtoul(amp_getenv("amp_master_cpu", id), NULL, 0);

		if((master_cpu + cpu_count) > 4){ //TODO- take define here
			printf ("Too many CPUs to group %d\n", id);
			goto failed;
		}

		if((id != 0) && (master_cpu == 0)){
			printf ("CPU 0 can only be part of group 0\n");
			goto failed;
		}

		if(master_cpu < base_cpu_id){
			printf ("Group %d CPUs overlap with group %d\n", id, id - 1);
			goto failed;
		}

		base_cpu_id = master_cpu + cpu_count;

		mem_base  = simple_strtoul(amp_getenv("amp_mem_base", id), NULL, 16);

		/* Verify Memory allocation - groups cannot have the same base addresses.
		 * Overlapping blocks will be automatically adjusted. */
		for(i = 0; i < id; i++)
		{
			if(mem_base == mem_blk_start[i]) {
				printf ("group %d memory (%#010x) overlaps with group %d (%#010x)\n",
						id, mem_base, i, mem_blk_start[i]);
				goto failed;
			}
		}

		/* VxWorks restrictions */
		env = amp_getenv("amp_os", id);
		if(strcmp(env, "vxworks") == 0)
		{
			/* VxWorks SMP must be loaded from CPU 0 due to OS restrictions */
			if(master_cpu != 0)
			{
				printf ("Group %d has unsupported setup. OS = VxWorks && master_cpu != 0\n", id);
 				goto failed;
			}

			/* VxWorks must have physical address space starting from 0x0 */
			if(mem_base > 0)
			{
				printf ("Group %d has unsupported setup. OS = VxWorks && mem_base > 0x0\n", id);
 				goto failed;
			}
		}

		mem_blk_start[id] = mem_base;
	}

	/* Verify shared memory location. Memory blocks overlapping on shared block will be adjusted. */
	env = getenv("amp_shared_mem");
	if(env) {
		shared_base = simple_strtoul(env, &env, 16);
		if (*env == ':')
			shared_size = simple_strtoul(++env, NULL, 16);
		if (shared_size == 0) {
			printf ("Shared memory size is not configured properly.\n");
			goto failed;
		}

		for(id = 0; id < group_count; id++)
			if ((shared_base <= mem_blk_start[id]) &&
					(shared_base + shared_size > mem_blk_start[id])) {
				printf ("Shared memory overlaps with group %d memory.\n", id);
				goto failed;
			}
	} else {
		printf ("Shared memory base is not configured properly.\n");
		goto failed;
	}

	/* Check that all resources are exclusive */
	for(id = 0; id < group_count; id++)
	{
		src  = amp_getenv("amp_resources", id);

		/*Verify at least one group has MSTR set */
		if(strstr(src, "mstr") != 0)
			mstr_rsrc++;

		for(test_id = id + 1; test_id < group_count; test_id++)
		{
			test = amp_getenv("amp_resources", test_id);
			strncpy(tmp_rsrc, test, 256);

			token = strtok (tmp_rsrc,":");
			while (token != NULL)
			{
				if(strstr(src, token) != 0)
				{
					printf("Resource %s exists in group %d and %d\n", token, id, test_id);
					goto failed;
				}
				token = strtok (NULL, ":");
			}
		}
	}

	if(mstr_rsrc == 0)
	{
		printf("No group has the master boot resource (mstr)\n");
		goto failed;
	}

	printf("AMP Configuration test - PASSED !\n");
	return 0;

failed:
	printf("AMP Configuration test - FAILED !\n");
	return 1;
}

U_BOOT_CMD(
        amp_verify,     1,     1,      amp_verify_cmd,
        "Verfiy AMP configuration\n"," "
);

#endif /* CONFIG_AMP_SUPPORT */
