/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
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
#include "mvTypes.h"
#include "mvBoardEnvLib.h"
#include "mvCpuIf.h"
#include "mvCtrlEnvLib.h"
#include "mv_mon_init.h"
#include "mvDebug.h"
#include "device/mvDevice.h"
#include "twsi/mvTwsi.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#if defined(MV_ETH_LEGACY)
#include "eth/mvEth.h"
#include "mv_eth_legacy.h"
#else
#include "neta/gbe/mvNeta.h"
#endif /* MV_ETH_LEGACY or MV_ETH_NETA */
#include "pex/mvPex.h"
#include "gpp/mvGpp.h"
#include "gpp/mvGppRegs.h"
#include "mvSysHwConfig.h"
#include "mv_phy.h"
#include "ddr2_3/mvDramIfRegs.h"
#include "mv_egiga_neta.h"
#ifdef MV_INCLUDE_RTC
#include "rtc/integ_rtc/mvRtc.h"
#include "rtc.h"
#elif CONFIG_RTC_DS1338_DS1339
#include "rtc/ext_rtc/mvDS133x.h"
#endif

#if defined(MV_INCLUDE_XOR)
#include "xor/mvXor.h"
#include "mvSysXorApi.h"
#endif
#if defined(MV_INCLUDE_IDMA)
#include "sys/mvSysIdma.h"
#include "idma/mvIdma.h"
#endif
#if defined(MV_INCLUDE_USB)
#include "usb/mvUsb.h"
#include "mvSysUsbApi.h"
#endif

#include "cpu/mvCpu.h"
#include "nand.h"
#include "spi_flash.h"
#ifdef CONFIG_PCI
	#include <pci.h>
#endif
//#include "pci/mvPciRegs.h"

#include <asm/arch-armv7/vfpinstr.h>
#include <asm/arch-armv7/vfp.h>
//#include <asm/arch/vfpinstr.h>
//#include <asm/arch/vfp.h>

#include <net.h>
#include <netdev.h>
#include <command.h>
#include "mvCommon.h"
#include "uart/mvUart.h"

/* #define MV_DEBUG */
#ifdef MV_DEBUG
#define DB(x) x
#else
#define DB(x)
#endif

/* CPU address decode table. */
MV_CPU_DEC_WIN mvCpuAddrWinMap[] = MV_CPU_IF_ADDR_WIN_MAP_TBL;
/*extern MV_U32 dummyFlavour;*/
#if 0
static void mvHddPowerCtrl(void);
#endif
#if defined(CONFIG_CMD_RCVR)
extern void recoveryDetection(void);
#endif
void mv_cpu_init(void);
#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
int mv_set_power_scheme(void);
#endif



#if defined(MV_INCLUDE_UNM_ETH) || defined(MV_INCLUDE_GIG_ETH)
//extern MV_VOID mvBoardEgigaPhySwitchInit(void);
#endif



extern nand_info_t nand_info[];       /* info for NAND chips */

extern struct spi_flash *flash;
extern const char version_string[];
#ifdef CONFIG_MRVL_MMC
int mrvl_mmc_initialize(bd_t *bis);
#endif

#ifdef MV_NAND_BOOT
extern MV_U32 nandEnvBase;
#endif

/* Define for SDK 2.0 */
//int raise(void) {return 0;}

DECLARE_GLOBAL_DATA_PTR;

void mv_print_map(void)
{
	int add;
#ifdef DB_78X60_PCAC
		return 0;
#endif
	printf("\nMap:   Code:\t\t0x%08x:0x%08x\n", (unsigned int)gd->reloc_off, (unsigned int)(gd->reloc_off + _bss_start_ofs));
	printf("       BSS:\t\t0x%08x\n", (unsigned int)(gd->reloc_off + _bss_end_ofs));
	printf("       Stack:\t\t0x%08x\n", (unsigned int)gd->start_addr_sp); 
#if defined(MV_INCLUDE_MONT_EXT)
	if(!enaMonExt()) {
		add = MV_PT_BASE(whoAmI());
		printf("       PageTable:\t0x%08x\n", add);
	}
#endif
	printf("       Heap:\t\t0x%08x:0x%08x\n\n",(unsigned int)(gd->relocaddr - TOTAL_MALLOC_LEN), (unsigned int)gd->relocaddr);
}

void print_mvBanner(void)
{
#ifdef CONFIG_SILENT_CONSOLE
	DECLARE_GLOBAL_DATA_PTR;
	gd->flags |= GD_FLG_SILENT;
#endif
	printf("\n");
	printf(" __   __                      _ _\n");
	printf("|  \\/  | __ _ _ ____   _____| | |\n");
	printf("| |\\/| |/ _` | '__\\ \\ / / _ \\ | |\n");
	printf("| |  | | (_| | |   \\ V /  __/ | |\n");
	printf("|_|  |_|\\__,_|_|    \\_/ \\___|_|_|\n");
	printf("         _   _     ____              _\n");
	printf("        | | | |   | __ )  ___   ___ | |_ \n");
	printf("        | | | |___|  _ \\ / _ \\ / _ \\| __| \n");
	printf("        | |_| |___| |_) | (_) | (_) | |_ \n");
	printf("         \\___/    |____/ \\___/ \\___/ \\__| \n");
//#if !defined(MV_NAND_BOOT)
#if defined(MV_INCLUDE_MONT_EXT)
    //mvMPPConfigToSPI();
	if(!enaMonExt())
		printf(" ** LOADER **\n");
	else
		printf(" ** MONITOR **\n");
    //mvMPPConfigToDefault();
#else

	printf(" ** LOADER **\n");
#endif /* MV_INCLUDE_MONT_EXT */
//#endif
	return;
}

void maskAllInt(void) {
	int i;
	/* for all interrupts (0-115) reset bit 0:3 and 8:11 to disable IRQ and FIQ */
	for (i=0; i < MV_IRQ_NR; i++)
		MV_REG_WRITE(CPU_INT_SOURCE_CONTROL_REG(i), MV_REG_READ(CPU_INT_SOURCE_CONTROL_REG(i)) & ~(0xF0F));

}

/*******************************************************************************
* enable_caches - Platform hook for enabling caches
*
* DESCRIPTION:	This function is called by main u-boot init function
* 				If caches are required they can be enabled here. Currently
* 				this is a stub doing nothing
* INPUT:
*		None
* OUTPUT:
*       None*
* RETURN:
* 		None
*******************************************************************************/
void enable_caches(void)
{
	return;
}
/* init for the Master*/
void misc_init_r_dec_win(void)
{
	char *env, envname[10];
	MV_U32 bitMaskConfig = 0;

#if defined(MV_INCLUDE_USB)
	{
		int  i;
		for (i = 0; i < mvCtrlUsbMaxGet(); i++) {

			sprintf(envname, "usb%dMode", i);
			env = getenv(envname);
			if((!env) || (strcmp(env,"device") == 0) || (strcmp(env,"Device") == 0) )
			{
				printf("USB %d: Device Mode\n", i);
				mvSysUsbInit(i, MV_FALSE);
			}
			else
			{
				printf("USB %d: Host Mode\n", i);
				mvSysUsbInit(i, MV_TRUE);
			}
		}
	}
#endif/* #if defined(MV_INCLUDE_USB) */

#if defined(MV_INCLUDE_SATA)
	{
		env = getenv("enaExtDisk");
		if((!env) || (strcmp(env,"no") == 0))
			bitMaskConfig &= ~(1 << 0);
		else
			bitMaskConfig |= (1 << 0);
			
		mvBoardBitMaskConfigSet(bitMaskConfig);
	}
#endif /*#if defined(MV_INCLUDE_SATA)*/

#if defined(MV_INCLUDE_XOR)
	mvSysXorInit();
#endif

#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
	mv_set_power_scheme();
#endif

    return;
}


/*
 * Miscellaneous platform dependent initialisations
 */

extern	MV_STATUS mvEthPhyRegRead(MV_U32 phyAddr, MV_U32 regOffs, MV_U16 *data);
extern	MV_STATUS mvEthPhyRegWrite(MV_U32 phyAddr, MV_U32 regOffs, MV_U16 data);

/* golabal mac address for yukon EC */
unsigned char yuk_enetaddr[6];

extern int timer_init(void );
extern void i2c_init(int speed, int slaveaddr);

int board_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;
	MV_U32 boardId;
	int clock_divisor;
	
	mvCtrlModelSet();
	boardId = mvBoardIdGet();
	
/* TODO : Should disable this code after we finish debugging - we added this code to enable printing before console init */
#if 1
	clock_divisor = (CONFIG_SYS_TCLK / 16)/115200;

	/* muti-core support, initiate each Uart to each cpu */
	mvUartInit(whoAmI(), clock_divisor, mvUartBase(whoAmI()));
#endif
	if (whoAmI() != 0)
		return 0;
	/*omri*/
	//mvCtrlUpdatePexId();
	/*omri*/

#if defined(MV_INCLUDE_TWSI)
	MV_TWSI_ADDR slave;
#endif
	MV_GPP_HAL_DATA gppHalData;

	unsigned int i;

	maskAllInt();

	/* must initialize the int in order for udelay to work */
	/* interrupt_init(); - no interrupt handling in u-boot */
	timer_init();

	/* Init the Board environment module (device bank params init) */
	mvBoardEnvInit();

#if defined(MV_INCLUDE_TWSI)
	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(0, CONFIG_SYS_I2C_SPEED, CONFIG_SYS_TCLK, &slave, 0);
#endif

	/* Init the Controlloer environment module (MPP init) */
	mvCtrlEnvInit();

	mvBoardDebugLed(2);

	/* Init the Controller CPU interface */
	mvCpuIfDramInit();
	mvCpuIfInit(mvCpuAddrWinMap);
		/*omri*/
	#if defined(MV_NOR_BOOT)
	env_init();
	#endif
		/*omri*/
	/* Init the GPIO sub-system */
	gppHalData.ctrlRev = mvCtrlRevGet();
	mvGppInit(&gppHalData);

	/* arch number of Integrator Board */
	switch (boardId) {
		case DB_88F6710_BP_ID:
			gd->bd->bi_arch_number = 3038;
			break;
		default:
			gd->bd->bi_arch_number = 3038;
			break;
	}

	/* adress of boot parameters */
	gd->bd->bi_boot_params = 0x00000100;

	/* relocate the exception vectors */
	/* U-Boot is running from DRAM at this stage */
	for(i = 0; i < 0x100; i+=4) {
		*(unsigned int *)(0x0 + i) = *(unsigned int*)(CONFIG_SYS_TEXT_BASE + i);
	}

	mvBoardDebugLed(4);

		/*omri*/
	mv_print_map();
		/*omri*/
	return 0;
}

void misc_init_r_env(void){
	char *env;
	char tmp_buf[10];
	unsigned int malloc_len;
	char buff[256];

	env = getenv("console");
	if(!env) {
		setenv("console","console=ttyS0,115200");
	}

	env = getenv("mtdids");
	if(!env) {
#if defined(MV_NAND)
		setenv("mtdids","nand0=armada-nand");
#endif
	}
	env = getenv("mtdparts");
	if(!env) {
#if defined(MV_NAND)
		setenv("mtdparts", "mtdparts=armada-nand:4m(boot),-(rootfs)");
#endif
	}

	/* Monitor extension */
#ifdef MV_INCLUDE_MONT_EXT
	env = getenv("enaMonExt");
	if(/* !env || */ ( (strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0) ) )
		setenv("enaMonExt","yes");
	else
#endif
	setenv("enaMonExt","no");

#if defined (MV_INC_BOARD_NOR_FLASH)
	env = getenv("enaFlashBuf");
	if( ( (strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) ) )
		setenv("enaFlashBuf","no");
	else
		setenv("enaFlashBuf","yes");
#endif

	if (enaMonExt()){
		/* Write allocation */
		env = getenv("enaWrAllo");
		if( !env || ( ((strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) )))
			setenv("enaWrAllo","no");
		else
			setenv("enaWrAllo","yes");
		
		env = getenv("disL2Cache");
		if(!env || ( (strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0) ) )
			setenv("disL2Cache","yes");
		else
			setenv("disL2Cache","no");
		
		/* Make address 0x80000000-0x8fffffff shared (set 'S' in pgd) */
		env = getenv("cacheShare");
		if( !env || ( ((strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) )))
			setenv("cacheShare","no");
		else
			setenv("cacheShare","yes");

	}

	/* Pex mode */
	env = getenv("pexMode");
	if( env && ( ((strcmp(env,"EP") == 0) || (strcmp(env,"ep") == 0) )))
		setenv("pexMode","EP");
	else
		setenv("pexMode","RC");

	env = getenv("setL2CacheWT");
	if(!env || ( (strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) ) )
		setenv("setL2CacheWT","no");
	else
		setenv("setL2CacheWT","yes");

	env = getenv("sata_dma_mode");
	if( env && ((strcmp(env,"No") == 0) || (strcmp(env,"no") == 0) ) )
		setenv("sata_dma_mode","no");
	else
		setenv("sata_dma_mode","yes");
	
	env = getenv("sata_delay_reset");
	if (!env)
		setenv("sata_delay_reset","0");

	env = getenv("enaExtDisk");
	if(!env)
		setenv("enaExtDisk","no");

	/* Malloc length */
	env = getenv("MALLOC_len");
	malloc_len =  simple_strtoul(env, NULL, 10) << 20;
	if(malloc_len == 0){
		sprintf(tmp_buf,"%d",CONFIG_SYS_MALLOC_LEN>>20);
		setenv("MALLOC_len",tmp_buf);
	}

	/* primary network interface */
	env = getenv("ethprime");
	if(!env) {
//	if(mvBoardIdGet() == RD_88F6281A_ID)
//		setenv("ethprime","egiga1");
//	else
		setenv("ethprime",ENV_ETH_PRIME);
	}

	/* netbsd boot arguments */
	env = getenv("netbsd_en");
	if( !env || ( ((strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) ))) {
		setenv("netbsd_en","no");
	} else {
		setenv("netbsd_en","yes");
		env = getenv("netbsd_gw");
		if(!env)
			setenv("netbsd_gw","192.168.0.254");
		env = getenv("netbsd_mask");
		if(!env)
			setenv("netbsd_mask","255.255.255.0");

		env = getenv("netbsd_fs");
		if(!env)
			setenv("netbsd_fs","nfs");

		env = getenv("netbsd_server");
		if(!env)
			setenv("netbsd_server","192.168.0.1");

		env = getenv("netbsd_ip");
		if(!env) {
			env = getenv("ipaddr");
			setenv("netbsd_ip",env);
		}

		env = getenv("netbsd_rootdev");
		if(!env)
			setenv("netbsd_rootdev","mgi0");

		env = getenv("netbsd_add");
		if(!env)
			setenv("netbsd_add","0x800000");

		env = getenv("netbsd_get");
	    if(!env)
			setenv("netbsd_get","tftpboot $netbsd_add $image_name");

#if defined(MV_INC_BOARD_QD_SWITCH)
		env = getenv("netbsd_netconfig");
		if(!env)
			setenv("netbsd_netconfig","mv_net_config=<((mgi0,00:00:11:22:33:44,0)(mgi1,00:00:11:22:33:55,1:2:3:4)),mtu=1500>");
#endif
		env = getenv("netbsd_set_args");
		if(!env)
			setenv("netbsd_set_args","setenv bootargs nfsroot=$netbsd_server:$rootpath fs=$netbsd_fs \
                    ip=$netbsd_ip serverip=$netbsd_server mask=$netbsd_mask gw=$netbsd_gw rootdev=$netbsd_rootdev \
                    ethaddr=$ethaddr eth1addr=$eth1addr ethmtu=$ethmtu eth1mtu=$eth1mtu $netbsd_netconfig");

		env = getenv("netbsd_boot");
		if(!env)
			setenv("netbsd_boot","bootm $netbsd_add $bootargs");

		env = getenv("netbsd_bootcmd");
		if(!env)
			setenv("netbsd_bootcmd","run netbsd_get ; run netbsd_set_args ; run netbsd_boot");
	}

	/* vxWorks boot arguments */
	env = getenv("vxworks_en");
	if( !env || ( ((strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) )))
		setenv("vxworks_en","no");
	else {
		char* buff = (char *)0x1100;
		setenv("vxworks_en","yes");

		sprintf(buff,"mgi(0,0) host:vxWorks.st");
		env = getenv("serverip");
		strcat(buff, " h=");
		strcat(buff,env);
		env = getenv("ipaddr");
		strcat(buff, " e=");
		strcat(buff,env);
		strcat(buff, ":ffff0000 u=anonymous pw=target ");

		setenv("vxWorks_bootargs",buff);
	}

	/* linux boot arguments */
	env = getenv("bootargs_root");
	if(!env)
		setenv("bootargs_root","root=/dev/nfs rw");

	/* For open Linux we set boot args differently */
	env = getenv("mainlineLinux");
	if(env && ((strcmp(env,"yes") == 0) ||  (strcmp(env,"Yes") == 0))) {
		env = getenv("bootargs_end");
		if(!env)
			setenv("bootargs_end",":::orion:eth0:none");
	} else {
		env = getenv("bootargs_end");
		if(!env)
#if defined(MV_INC_BOARD_QD_SWITCH)
			setenv("bootargs_end",MV_BOOTARGS_END_SWITCH);
#else
			setenv("bootargs_end",MV_BOOTARGS_END);
#endif
	}

	env = getenv("image_name");
	if(!env)
		setenv("image_name","uImage");


/* omri  -  no real device tree - empty setenv*/
#if CONFIG_OF_LIBFDT
	env = getenv("fdtaddr");
		if(!env)
			setenv("fdtaddr","0x1000000");

	env = getenv("fdtfile");
		if(!env)
			setenv("fdtfile","armada_xp_db.dtb");
#endif
/* omri  -  no real device tree - empty setenv*/
	env = getenv("load_addr");
	if(!env)
		setenv("load_addr",RCVR_LOAD_ADDR);

#if (CONFIG_BOOTDELAY >= 0)
	env = getenv("bootcmd");
	if(!env)
	/* omri  -  no real device tree - empty setenv*/
	#if defined(CONFIG_OF_LIBFDT)
		setenv("bootcmd","tftpboot 0x2000000 $image_name;tftpboot $fdtaddr $fdtfile;\
setenv bootargs $console $mtdparts $bootargs_root nfsroot=$serverip:$rootpath \
ip=$ipaddr:$serverip$bootargs_end $mvNetConfig video=dovefb:lcd0:$lcd0_params clcd.lcd0_enable=$lcd0_enable clcd.lcd_panel=$lcd_panel;  bootm 0x2000000 - 0x1000000;");
	
	/* omri  -  no real device tree - empty setenv*/
#elif defined(MV_INCLUDE_TDM) && defined(MV_INC_BOARD_QD_SWITCH)
		setenv("bootcmd","tftpboot 0x2000000 $image_name;\
setenv bootargs $console $mtdparts $bootargs_root nfsroot=$serverip:$rootpath \
ip=$ipaddr:$serverip$bootargs_end $mvNetConfig video=dovefb:lcd0:$(lcd0_params) clcd.lcd0_enable=$(lcd0_enable) clcd.lcd_panel=$(lcd_panel);  bootm $load_addr; ");
#elif defined(MV_INC_BOARD_QD_SWITCH)
		setenv("bootcmd","tftpboot 0x2000000 $image_name;\
setenv bootargs $console  $mtdparts $bootargs_root nfsroot=$serverip:$rootpath \
ip=$ipaddr:$serverip$bootargs_end $mvNetConfig  video=dovefb:lcd0:$(lcd0_params) clcd.lcd0_enable=$(lcd0_enable) clcd.lcd_panel=$(lcd_panel);  bootm $load_addr; ");
#elif defined(MV_INCLUDE_TDM)
		setenv("bootcmd","tftpboot 0x2000000 $image_name;\
setenv bootargs $console $mtdparts $bootargs_root nfsroot=$serverip:$rootpath \
ip=$ipaddr:$serverip$bootargs_end $mvNetConfig video=dovefb:lcd0:$(lcd0_params) clcd.lcd0_enable=$(lcd0_enable) clcd.lcd_panel=$(lcd_panel);  bootm $load_addr; ");
#else

		setenv("bootcmd","tftpboot 0x2000000 $image_name;\
setenv bootargs $console $mtdparts $bootargs_root nfsroot=$serverip:$rootpath \
ip=$ipaddr:$serverip$bootargs_end  video=dovefb:lcd0:$(lcd0_params) clcd.lcd0_enable=$(lcd0_enable) clcd.lcd_panel=$(lcd_panel);  bootm $load_addr; ");
#endif
#endif /* (CONFIG_BOOTDELAY >= 0) */

	env = getenv("standalone");
	if(!env)
#if defined(MV_INCLUDE_TDM) && defined(MV_INC_BOARD_QD_SWITCH)
		setenv("standalone","fsload 0x2000000 $image_name;setenv bootargs $console $mtdparts root=/dev/mtdblock0 rw \
ip=$ipaddr:$serverip$bootargs_end $mvNetConfig; bootm 0x2000000;");
#elif defined(MV_INC_BOARD_QD_SWITCH)
	setenv("standalone","fsload 0x2000000 $image_name;setenv bootargs $console $mtdparts root=/dev/mtdblock0 rw \
ip=$ipaddr:$serverip$bootargs_end $mvNetConfig; bootm 0x2000000;");
#elif defined(MV_INCLUDE_TDM)
	setenv("standalone","fsload 0x2000000 $image_name;setenv bootargs $console $mtdparts root=/dev/mtdblock0 rw \
ip=$ipaddr:$serverip$bootargs_end; bootm 0x2000000;");
#else
	setenv("standalone","fsload 0x2000000 $image_name;setenv bootargs $console $mtdparts root=/dev/mtdblock0 rw \
ip=$ipaddr:$serverip$bootargs_end; bootm 0x2000000;");
#endif

	/* Set boodelay to 3 sec, if Monitor extension are disabled */
	if(!enaMonExt()) {
		setenv("disaMvPnp","no");
	}

	/* Disable PNP config of Marvell memory controller devices. */
	env = getenv("disaMvPnp");
	if(!env)
		setenv("disaMvPnp","no");

	env = getenv("bootdelay");
	if(!env)
		setenv("bootdelay","3");

#if (defined(MV_INCLUDE_GIG_ETH) || defined(MV_INCLUDE_UNM_ETH))
	/* Generate random ip and mac address */
	/* Read RTC to create pseudo-random data for enc */
	struct rtc_time tm;
	unsigned int xi, xj, xk, xl;
	char ethaddr_0[30];
	char ethaddr_1[30];
	char pon_addr[30];

	rtc_get(&tm);
	xi = ((tm.tm_yday + tm.tm_sec)% 254);
	/* No valid ip with one of the fileds has the value 0 */
	if (xi == 0)
		xi+=2;

	xj = ((tm.tm_yday + tm.tm_min)%254);
	/* No valid ip with one of the fileds has the value 0 */
	if (xj == 0)
		xj+=2;

	/* Check if the ip address is the same as the server ip */
	if ((xj == 1) && (xi == 11))
		xi+=2;

	xk = (tm.tm_min * tm.tm_sec)%254;
	xl = (tm.tm_hour * tm.tm_sec)%254;

	sprintf(ethaddr_0,"00:50:43:%02x:%02x:%02x",xk,xi,xj);
	sprintf(ethaddr_1,"00:50:43:%02x:%02x:%02x",xl,xi,xj);
	sprintf(pon_addr,"00:50:43:%02x:%02x:%02x",xj,xk,xl);

	/* MAC addresses */
	env = getenv("ethaddr");
	if(!env)
		setenv("ethaddr",ethaddr_0);

	env = getenv("eth1addr");
	if(!env)
		setenv("eth1addr",ethaddr_1);

	env = getenv("ethmtu");
	if(!env)
		setenv("ethmtu","1500");

	env = getenv("eth1mtu");
	if(!env)
		setenv("eth1mtu","1500");

	/* Set mvNetConfig env parameter */
	env = getenv("mvNetConfig");
	if(!env ) {

				setenv("mvNetConfig","mv_net_config1=1,(00:50:43:11:11:11,0:1:2:3:4),mtu=1500");
			
	}
#endif /*  (MV_INCLUDE_GIG_ETH) || defined(MV_INCLUDE_UNM_ETH) */

#if defined(MV_INCLUDE_USB)
	/* USB Host */
	env = getenv("usb0Mode");
	if(!env)
		setenv("usb0Mode",ENV_USB0_MODE);
	env = getenv("usb1Mode");
	if(!env)
		setenv("usb1Mode",ENV_USB1_MODE);
		/* omri */
	env = getenv("usb2Mode");
	if(!env)
		setenv("usb2Mode",ENV_USB2_MODE);
		/* omri */
	env = getenv("usbActive");
	if(!env)
		setenv("usbActive",ENV_USB_ACTIVE);

#endif  /* (MV_INCLUDE_USB) */


#if defined(YUK_ETHADDR)
	env = getenv("yuk_ethaddr");
	if(!env)
		setenv("yuk_ethaddr",YUK_ETHADDR);

	{
		int i;
		char *tmp = getenv ("yuk_ethaddr");
		char *end;

		for (i=0; i<6; i++) {
			yuk_enetaddr[i] = tmp ? simple_strtoul(tmp, &end, 16) : 0;
			if (tmp)
				tmp = (*end) ? end+1 : end;
		}
	}
#endif /* defined(YUK_ETHADDR) */

#if defined(MV_NAND)
	env = getenv("nandEcc");
	if(!env) {
		setenv("nandEcc", "1bit");
	}
#endif
#if 0
#if defined(RD_88F6281A) || defined(RD_88F6192A) || defined(RD_88F6190A)
	mvHddPowerCtrl();
#endif
#endif
#if defined(CONFIG_CMD_RCVR)
	env = getenv("netretry");
	if (!env)
		setenv("netretry","no");

	env = getenv("rcvrip");
	if (!env)
		setenv("rcvrip",RCVR_IP_ADDR);

	env = getenv("loadaddr");
	if (!env)
		setenv("loadaddr",RCVR_LOAD_ADDR);

	env = getenv("autoload");
	if (!env)
		setenv("autoload","no");

	/* Check the recovery trigger */
	recoveryDetection();
#endif
	env = getenv("eeeEnable");
	if (!env)
		setenv("eeeEnable","no");

	return;
}
#define MV_CPU_SW_RESET_CONTROL(cpu)	(0x20800 + ((cpu) * 0x8))


#ifdef BOARD_LATE_INIT
int board_late_init (void)
{
	/* Check if to use the LED's for debug or to use single led for init and Linux heartbeat */

	mvBoardDebugLed(0);
	return 0;
}
#endif

void pcie_tune(void)
{
	MV_REG_WRITE(0xF1041AB0, 0x100);
	MV_REG_WRITE(0xF1041A20, 0x78000801);
	MV_REG_WRITE(0xF1041A00, 0x4014022F);
	MV_REG_WRITE(0xF1040070, 0x18110008);

	return;
}

int board_eth_init(bd_t *bis)
{
#ifdef  CONFIG_MARVELL
#if defined(MV_INCLUDE_GIG_ETH) || defined(MV_INCLUDE_UNM_ETH)
	/* move to the begining so in case we have a PCI NIC it will
	read the env mac addresses correctlly. */
	mv_eth_initialize(bis);
#endif
#endif
#if defined(CONFIG_SK98)
	skge_initialize(bis);
#endif
#if defined(CONFIG_E1000)
	e1000_initialize(bis);
#endif
	return 0;
}

#ifdef CONFIG_MMC
	int board_mmc_init(bd_t *bis)
	{
#ifdef CONFIG_MRVL_MMC
		mrvl_mmc_initialize(bis);
#endif
		return 0;
	}
#endif
			
int print_cpuinfo (void)
{
	char name[50];

	
	mvBoardNameGet(name);
	printf("Board: %s\n",  name);
	mvCtrlModelRevNameGet(name);
	printf("SoC:   %s\n", name);
#if 0
	if (!mvCtrlIsValidSatR())
		printf("       Custom configuration\n");
#endif
	mvCpuNameGet(name);
	printf("CPU:   %s",  name);
#ifdef MV_CPU_LE
	printf(" LE\n");
#else
	printf(" BE\n");
#endif
printf("       CPU    @ %d [MHz]\n",  mvCpuPclkGet()/1000000);
	printf("       L2     @ %d [MHz]\n", mvCpuL2ClkGet()/1000000);
	printf("       TClock @ %d [MHz]\n", mvTclkGet()/1000000);
	printf("       DDR    @ %d [MHz]\n", CONFIG_SYS_BUS_CLK/1000000);
	printf("       DDR %dBit Width, %s Memory Access\n", mvCtrlDDRBudWidth(), mvCtrlDDRThruXbar()?"XBAR":"FastPath");


	return 0;
}
int misc_init_r (void)
{
	char *env;

	mvBoardDebugLed(5);

	/* init special env variables */
	misc_init_r_env();

	mv_cpu_init();

#if defined(MV_INCLUDE_MONT_EXT)
	if(enaMonExt()) {
		printf("Marvell monitor extension:\n");
		mon_extension_after_relloc();
	}
#endif /* MV_INCLUDE_MONT_EXT */

	/* init the units decode windows */
	misc_init_r_dec_win();

	/* Clear old kernel images which remained stored in memory */
	memset (CONFIG_SYS_LOAD_ADDR, 0, CONFIG_SYS_MIN_HDR_DEL_SIZE);
#if 0
#ifdef CONFIG_PCI
#if !defined(MV_MEM_OVER_PCI_WA) && !defined(MV_MEM_OVER_PEX_WA)
	pci_init();
#endif
#endif
#endif
	mvBoardDebugLed(6);
	
	/* Prints the modules detected */
	mvBoardMppModuleTypePrint();
	mvBoardOtherModuleTypePrint();
	
	mvBoardDebugLed(7);

	/* pcie fine tunning */
	env = getenv("pcieTune");
	if(env && ((strcmp(env,"yes") == 0) || (strcmp(env,"yes") == 0)))
		pcie_tune();
	else
		setenv("pcieTune","no");

#if defined(MV_INCLUDE_UNM_ETH) || defined(MV_INCLUDE_GIG_ETH)
	/* Init the PHY or Switch of the board */
	mvBoardEgigaPhyInit();
#endif /* #if defined(MV_INCLUDE_UNM_ETH) || defined(MV_INCLUDE_GIG_ETH) */

	return 0;
}

MV_U32 mvTclkGet(void)
{
	DECLARE_GLOBAL_DATA_PTR;
	/* get it only on first time */
	if(gd->tclk == 0)
		gd->tclk = mvBoardTclkGet();

	return gd->tclk;
}

MV_U32 mvSysClkGet(void)
{
	DECLARE_GLOBAL_DATA_PTR;
	/* get it only on first time */
	if(gd->bus_clk == 0)
		gd->bus_clk = mvBoardSysClkGet();

	return gd->bus_clk;
}

/* exported for EEMBC */
MV_U32 mvGetRtcSec(void)
{
	MV_RTC_TIME time;
#ifdef MV_INCLUDE_RTC
	mvRtcTimeGet(&time);
#elif CONFIG_RTC_DS1338_DS1339
	mvRtcDS133xTimeGet(&time);
#endif
	return (time.minutes * 60) + time.seconds;
}

void reset_cpu (ulong addr)
{
	mvBoardReset();
}


void mv_cpu_init(void)
{
	char *env;
	volatile unsigned int temp;

	/* enable access to CP10 and CP11 */
	temp = 0x00f00000;
	__asm__ __volatile__("mcr p15, 0, %0, c1, c0, 2" :: "r" (temp));

	env = getenv("enaFPU");
	if((strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0)){
		/* init and Enable FPU to Run Fast Mode */
		printf("FPU initialized to Run Fast Mode.\n");
		/* Enable */
		temp = FPEXC_ENABLE;
		fmxr(FPEXC, temp);
		/* Run Fast Mode */
		temp = fmrx(FPSCR);
		temp |= (FPSCR_DEFAULT_NAN | FPSCR_FLUSHTOZERO);
		fmxr(FPSCR, temp);
	}else{
		printf("FPU not initialized\n");
		/* Disable */
		temp = fmrx(FPEXC);
		temp &= ~FPEXC_ENABLE;
		fmxr(FPEXC, temp);
	}

	__asm__ __volatile__("mrc p15, 1, %0, c15, c1, 1" : "=r" (temp));
	temp |= BIT16; /* Disable reac clean intv */
	__asm__ __volatile__("mcr p15, 1, %0, c15, c1, 1\n" \
			"mcr p15, 0, %0, c7, c5, 4": :"r" (temp)); /*imb*/

	__asm__ __volatile__("mrc p15, 1, %0, c15, c1, 2" : "=r" (temp));
	temp |= (BIT25 | BIT27 | BIT29 | BIT30);
	/* removed BIT23 in order to enable fast LDR bypass */
	__asm__ __volatile__("mcr p15, 1, %0, c15, c1, 2\n" \
			"mcr p15, 0, %0, c7, c5, 4": :"r" (temp)); /*imb*/

	/* Enable speculative read miss from L1 to "line fill" L1 */
	__asm__ __volatile__("mrc p15, 1, %0, c15, c2, 0" : "=r" (temp));

	env = getenv("L1SpeculativeEn");
	if( (strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) )
		temp |= BIT7;
	else{
		temp &= ~BIT7;
	}

	__asm__ __volatile__("mcr p15, 1, %0, c15, c2, 0\n" \
			"mcr p15, 0, %0, c7, c5, 4": :"r" (temp)); /*imb*/

#if 1
	/* Set L2C WT mode */
	temp = MV_REG_READ(CPU_L2_AUX_CTRL_REG) & ~CL2ACR_WB_WT_ATTR_MASK;
	env = getenv("setL2CacheWT");
	if(!env || ((strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0))) {
		temp |= CL2ACR_WB_WT_ATTR_WT;
	}
	MV_REG_WRITE(CPU_L2_AUX_CTRL_REG, temp);
#endif

	/* enable L2C */
	temp = MV_REG_READ(CPU_L2_CTRL_REG);

	env = getenv("disL2Cache");
	if(((strcmp(env,"no") == 0) || (strcmp(env,"No") == 0)) && enaMonExt())
		temp |= CL2CR_L2_EN_MASK;
	else
		temp &= ~CL2CR_L2_EN_MASK;

	MV_REG_WRITE(CPU_L2_CTRL_REG, temp);

	/* Configure L2 options if L2 exists */
	if (MV_REG_READ(CPU_L2_CTRL_REG) & CL2CR_L2_EN_MASK) {

		/* Read L2 Auxilary control register */
		temp = MV_REG_READ(CPU_L2_AUX_CTRL_REG);
		/* Clear fields */
		temp &= ~(CL2ACR_WB_WT_ATTR_MASK | CL2ACR_FORCE_WA_MASK);

		/* Set "Force write policy" field */
		env = getenv("L2forceWrPolicy");
		if( (strcmp(env,"WB") == 0) || (strcmp(env,"wb") == 0) )
			temp |= CL2ACR_WB_WT_ATTR_WB;
		else if( (strcmp(env,"WT") == 0) || (strcmp(env,"wt") == 0) )
			temp |= CL2ACR_WB_WT_ATTR_WT;
		else
			temp |= CL2ACR_WB_WT_ATTR_PAGE;

		/* Set "Force Write Allocate" field */
		env = getenv("L2forceWrAlloc");
		if( (strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) )
			temp |= CL2ACR_FORCE_NO_WA;
		else if( (strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0) )
			temp |= CL2ACR_FORCE_WA;
		else
			temp |= CL2ACR_FORCE_WA_DISABLE;

		/* Set "ECC" */
		env = getenv("L2EccEnable");
		if(!env || ( (strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) ) )
			temp &= ~CL2ACR_ECC_EN;
		else
			temp |= CL2ACR_ECC_EN;

		/* Set other L2 configurations */
		temp |= (CL2ACR_PARITY_EN | CL2ACR_INVAL_UCE_EN);

		/* Set L2 algorithm to semi_pLRU */
		temp &= ~CL2ACR_REP_STRGY_MASK;
		temp |= CL2ACR_REP_STRGY_PLRU_MASK;
		
		/* Write to L2 Auxilary control register */
		MV_REG_WRITE(CPU_L2_AUX_CTRL_REG, temp);

		env = getenv("L2SpeculativeRdEn");
		if((strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) )
			MV_REG_BIT_SET(0x20228, ((0x1 << 5)));
		else
			MV_REG_BIT_RESET(0x20228, ((0x1 << 5)));

	}

	/* Enable i cache */
	asm ("mrc p15, 0, %0, c1, c0, 0":"=r" (temp));
	temp |= BIT12;
	/* Change reset vector to address 0x0 */
	temp &= ~BIT13;
	asm ("mcr p15, 0, %0, c1, c0, 0\n" \
		"mcr p15, 0, %0, c7, c5, 4": :"r" (temp)); /* imb */

	/* Disable MBUS Err Prop - inorder to avoid data aborts */
	MV_REG_BIT_RESET(SOC_COHERENCY_FABRIC_CTRL_REG, BIT8);
	
	/* Enable IOCC */
	env = getenv("cacheShare");
	if(((strcmp(env,"yes") == 0) || (strcmp(env,"Yes") == 0)) && enaMonExt()) {

		__asm__ __volatile__("mrc p15, 1, %0, c15, c1, 1" : "=r" (temp));
		temp |= BIT7; /* @ v7 IO coherency support (Single core) */
		__asm__ __volatile__("mcr p15, 1, %0, c15, c1, 1\n" \
				"mcr p15, 0, %0, c7, c5, 4": :"r" (temp)); /*imb*/

		MV_REG_BIT_SET(SOC_COHERENCY_FABRIC_CTRL_REG, BIT24);
	}

}

/* Set unit in power off mode acording to the detection of MPP/SERDES */
#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
int mv_set_power_scheme(void)
{
	MV_U32 boardId = mvBoardIdGet();
	MV_U32 mppGrp1 = mvBoardMppModulesCfgGet(1);
	MV_U32 mppGrp2 = mvBoardMppModulesCfgGet(2);
	MV_U32 srdsCfg = mvBoardSerdesModeGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mv_set_power_scheme: Board unknown.\n");
		return -1;
	}

	mvOsOutput("Shutting down unused interfaces:\n");

	/* PCI-E */
	if (!(srdsCfg & SRDS_MOD_PCIE0_LANE0)) {
		mvCtrlPwrClckSet(PEX_UNIT_ID, 0, MV_FALSE);
		mvOsOutput("       PEX0\n");
	}
	if (!(srdsCfg & SRDS_MOD_PCIE1_LANE1)) {
		mvCtrlPwrClckSet(PEX_UNIT_ID, 1, MV_FALSE);
		mvOsOutput("       PEX1\n");
	}

	/* SATA */
	/* Disable SATA 0 only if SATA 1 is not required as well */
	if (!(srdsCfg & SRDS_MOD_SATA1_LANE3) || (mvCtrlSataMaxPortGet() < 2)) {
		mvCtrlPwrClckSet(SATA_UNIT_ID, 1, MV_FALSE);
		if (mvCtrlSataMaxPortGet() == 2)
			mvOsOutput("       SATA1\n");

		if (!(srdsCfg & (SRDS_MOD_SATA0_LANE0 | SRDS_MOD_SATA0_LANE2))) {
			mvCtrlPwrClckSet(SATA_UNIT_ID, 0, MV_FALSE);
			mvOsOutput("       SATA0\n");
		}
	}

	/* GBE - SMI Bus is connected to EGIGA0 so we only shut down EGIGA1 if needed */
	if (!(srdsCfg & SRDS_MOD_SGMII1_LANE3)) {
		if (!(mppGrp1 & (MV_BOARD_GMII0 | MV_BOARD_RGMII1))) {
			mvCtrlPwrClckSet(ETH_GIG_UNIT_ID, 1, MV_FALSE);
			mvOsOutput("       GBE1\n");
		}
	}

	/* SDIO */
	if (!(mppGrp1 & MV_BOARD_SDIO)) {
		mvCtrlPwrClckSet(SDIO_UNIT_ID, 0, MV_FALSE);
		mvOsOutput("       SDIO\n");
	}

	/* Audio */
	if (!((mppGrp1 & MV_BOARD_I2S) || (mppGrp2 & MV_BOARD_I2S))) {
		mvCtrlPwrClckSet(AUDIO_UNIT_ID, 0, MV_FALSE);
		if (mvCtrlAudioSupport())
			mvOsOutput("       AUDIO\n");
	}
	
	/* TDM */
	if(!((mppGrp1 & MV_BOARD_TDM) || (mppGrp2 & MV_BOARD_TDM)) || !(mvCtrlTdmSupport())) {
		mvCtrlPwrClckSet(TDM_2CH_UNIT_ID, 0, MV_FALSE);
		if (mvCtrlTdmSupport())
			mvOsOutput("       TDM\n");
	}
}
#endif /* defined(MV_INCLUDE_CLK_PWR_CNTRL) */

#if 0
/*******************************************************************************
* mvHddPowerCtrl -
*
* DESCRIPTION:
*       This function set HDD power on/off acording to env or wait for button push
* INPUT:
*	None
* OUTPUT:
*	None
* RETURN:
*       None
*
*******************************************************************************/
static void mvHddPowerCtrl(void)
{
	MV_32 hddPowerBit;
	MV_32 fanPowerBit;
	MV_32 hddHigh = 0;
	MV_32 fanHigh = 0;
	char* env;

	if(RD_88F6281A_ID == mvBoardIdGet())
	{
		hddPowerBit = mvBoarGpioPinNumGet(BOARD_GPP_HDD_POWER, 0);
		fanPowerBit = mvBoarGpioPinNumGet(BOARD_GPP_FAN_POWER, 0);
		if (hddPowerBit > 31)
		{
			hddPowerBit = hddPowerBit % 32;
			hddHigh = 1;
		}
		if (fanPowerBit > 31)
		{
			fanPowerBit = fanPowerBit % 32;
			fanHigh = 1;
		}
	}

	if ((RD_88F6281A_ID == mvBoardIdGet()) || (RD_88F6192A_ID == mvBoardIdGet()) ||
        (RD_88F6190A_ID == mvBoardIdGet()))
	{
		env = getenv("hddPowerCtrl");
		if(!env || ( (strcmp(env,"no") == 0) || (strcmp(env,"No") == 0) ) )
			setenv("hddPowerCtrl","no");
		else
			setenv("hddPowerCtrl","yes");

		if(RD_88F6281A_ID == mvBoardIdGet())
		{
			mvBoardFanPowerControl(MV_TRUE);
			mvBoardHDDPowerControl(MV_TRUE);
		}
		else
		{
			/* FAN power on */
			MV_REG_BIT_SET(GPP_DATA_OUT_REG(fanHigh),(1<<fanPowerBit));
			MV_REG_BIT_RESET(GPP_DATA_OUT_EN_REG(fanHigh),(1<<fanPowerBit));
			/* HDD power on */
			MV_REG_BIT_SET(GPP_DATA_OUT_REG(hddHigh),(1<<hddPowerBit));
			MV_REG_BIT_RESET(GPP_DATA_OUT_EN_REG(hddHigh),(1<<hddPowerBit));
		}
	}
}

#endif


