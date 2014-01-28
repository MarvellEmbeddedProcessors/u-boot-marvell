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
#include "mvBoardEnvSpec.h"
#include "mvCpuIf.h"
#include "mvCtrlEnvLib.h"
#include "mvDebug.h"
#include "device/mvDevice.h"
#include "twsi/mvTwsi.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#if defined(MV_ETH_LEGACY)
#include "eth/mvEth.h"
#include "mv_eth_legacy.h"
#elif defined(MV_ETH_NETA)
#include "neta/gbe/mvNeta.h"
#else
#include "pp2/gbe/mvPp2Gbe.h"
#endif /* MV_ETH_LEGACY or MV_ETH_NETA or MV_ETH_PP2 */
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
#ifdef CONFIG_AMP_SUPPORT
#include "mv_amp.h"
#endif

#include "cpu/mvCpu.h"
#include "nand.h"
#include "spi_flash.h"
#ifdef CONFIG_PCI
	#include <pci.h>
#endif

#ifdef CONFIG_SCSI_AHCI
	#include <ahci.h>
	#include <scsi.h>
	#include "mvSysSata3Api.h"
#endif
#include <asm/arch-armv7/vfpinstr.h>
#include <asm/arch-armv7/vfp.h>

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

extern int display_dram_config(int print);
int late_print_cpuinfo(void);

/* CPU address decode table. */
MV_CPU_DEC_WIN mvCpuAddrWinMap[] = MV_CPU_IF_ADDR_WIN_MAP_TBL;

void mv_cpu_init(void);
#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
void mv_set_power_scheme(void);
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

DECLARE_GLOBAL_DATA_PTR;

void mv_print_map(void)
{
#ifdef DB_78X60_PCAC
	return 0;
#endif

	printf("\nMap:   Code:\t\t0x%08x:0x%08x\n", (unsigned int)gd->reloc_off, (unsigned int)(gd->reloc_off + _bss_start_ofs));
	printf("       BSS:\t\t0x%08x\n", (unsigned int)(gd->reloc_off + _bss_end_ofs));
	printf("       Stack:\t\t0x%08x\n", (unsigned int)gd->start_addr_sp);
#if defined(MV_INCLUDE_MONT_EXT)
	int add;
	if (!enaMonExt()) {
		add = MV_PT_BASE(whoAmI());
		printf("       PageTable:\t0x%08x\n", add);
	}
#endif
	printf("       Heap:\t\t0x%08x:0x%08x\n\n", (unsigned int)(gd->relocaddr - TOTAL_MALLOC_LEN), (unsigned int)gd->relocaddr);
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

#if defined(MV_INCLUDE_MONT_EXT)
	if (!enaMonExt())
		printf(" ** LOADER **\n");
	else
		printf(" ** MONITOR **\n");
#else
	printf(" ** LOADER **\n");
#endif  /* MV_INCLUDE_MONT_EXT */
	return;
}

void maskAllInt(void)
{
	int i;

	/* for all interrupts (0-115) reset bit 0:3 and 8:11 to disable IRQ and FIQ */
	for (i = 0; i < MV_IRQ_NR; i++)
		MV_REG_WRITE(CPU_INT_SOURCE_CONTROL_REG(i), MV_REG_READ(CPU_INT_SOURCE_CONTROL_REG(i)) & ~(0xF0F));
}

/*******************************************************************************
* enable_caches - Platform hook for enabling caches
*
* DESCRIPTION:	This function is called by main u-boot init function
*               If caches are required they can be enabled here. Currently
*               this is a stub doing nothing
* INPUT:
*       None
* OUTPUT:
*       None
* RETURN:
*       None
*******************************************************************************/
void enable_caches(void)
{
	/* Nothing */
}

void misc_init_r_dec_win(void)
{
	char *env;

#if defined(DB_78X60_PCAC) || defined(DB_78X60_PCAC_REV2)
	/* TODO: no support for usb on PCAC board for now. */
#else
#if defined(MV_INCLUDE_USB)
	{
#ifdef CONFIG_USB_EHCI
		char envname[10];
		int i;

		for (i = 0; i < mvCtrlUsbMaxGet(); i++) {
			sprintf(envname, "usb%dMode", i);
			env = getenv(envname);
			if ((!env) || (strcmp(env, "device") == 0) || (strcmp(env, "Device") == 0) ) {
				printf("USB %d: Device Mode\n", i);
				mvSysUsbInit(i, MV_FALSE, MV_FALSE);
			}else  {
				printf("USB %d: Host Mode\n", i);
				mvSysUsbInit(i, MV_TRUE, MV_FALSE);
			}
		}
#endif
	}
#endif  /* MV_INCLUDE_USB */
#endif

#if defined(MV_INCLUDE_XOR)
	mvSysXorInit();
#endif

#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
	env = getenv("enaClockGating");
	if ((strcmp(env, "yes") == 0) || (strcmp(env, "Yes") == 0))
		mv_set_power_scheme();
#endif
}

/*
 * Miscellaneous platform dependent initializations
 */

extern MV_STATUS mvEthPhyRegRead(MV_U32 phyAddr, MV_U32 regOffs, MV_U16 *data);
extern MV_STATUS mvEthPhyRegWrite(MV_U32 phyAddr, MV_U32 regOffs, MV_U16 data);

/* golabal mac address for yukon EC */
unsigned char yuk_enetaddr[6];

extern int timer_init(void);
extern void i2c_init(int speed, int slaveaddr);

int board_init(void)
{
	DECLARE_GLOBAL_DATA_PTR;

	if (whoAmI() != 0)
		return 0;

#if defined(MV_INCLUDE_TWSI)
	MV_TWSI_ADDR slave;
#endif

	unsigned int i;

	maskAllInt();
#if defined(MV_INCLUDE_TWSI)
	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(0, CONFIG_SYS_I2C_SPEED, CONFIG_SYS_TCLK, &slave, 0);
#endif
	/* Init the Board environment module (device bank params init) */
	mvBoardEnvInit();

	/* Init the Controlloer environment module (MPP init) */
	mvCtrlEnvInit();

#if defined(CONFIG_DISPLAY_CPUINFO)
	late_print_cpuinfo();          /* display cpu info (and speed) */
#endif

	mvBoardDebugLed(2);

	mvCpuIfInit(mvCpuAddrWinMap);

#ifdef MV_NOR_BOOT
	env_init();
#endif

	/* Init the GPIO sub-system */
	MV_GPP_HAL_DATA gppHalData;
	gppHalData.ctrlRev = mvCtrlRevGet();
	mvGppInit(&gppHalData);

	gd->bd->bi_arch_number = 528;
	gd->bd->bi_boot_params = 0x00000100;

	/* relocate the exception vectors */
	/* U-Boot is running from DRAM at this stage */
	for (i = 0; i < 0x100; i += 4)
		*(unsigned int*)(0x0 + i) =
			*(unsigned int*)(CONFIG_SYS_TEXT_BASE + i);

	mvBoardDebugLed(4);
	mv_print_map();
	return 0;
}

void misc_init_r_env(void)
{
	char *env;
	char tmp_buf[10];
	unsigned int malloc_len;

	env = getenv("console");
	if (!env)
		setenv("console", "console=ttyS0,115200");

	env = getenv("mtdids");
	if (!env) {
#if defined(MV_NAND)
		setenv("mtdids", "nand0=armada-nand");
#endif
	}
	env = getenv("mtdparts");
	if (!env) {
#if defined(MV_NAND)
		setenv("mtdparts", "mtdparts=armada-nand:8m(boot)ro,8m@4m(kernel),-(rootfs)");
#endif
	}

	/* update the CASset env parameter */
	env = getenv("CASset");
	if (!env) {
#ifdef MV_MIN_CAL
		setenv("CASset", "min");
#else
		setenv("CASset", "max");
#endif
	}
	/* Monitor extension */
#ifdef MV_INCLUDE_MONT_EXT
	env = getenv("enaMonExt");
	if (/* !env || */ ( (strcmp(env, "yes") == 0) || (strcmp(env, "Yes") == 0) ) )
		setenv("enaMonExt", "yes");
	else
#endif
	setenv("enaMonExt", "no");

	/* CPU streaming */
	env = getenv("enaCpuStream");
	if (!env || ( (strcmp(env, "no") == 0) || (strcmp(env, "No") == 0) ) )
		setenv("enaCpuStream", "no");
	else
		setenv("enaCpuStream", "yes");

	/* Clock Gating */
#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
	env = getenv("enaClockGating");
	if (!env || ( (strcmp(env, "no") == 0) || (strcmp(env, "No") == 0) ) )
		setenv("enaClockGating", "no");
	else
		setenv("enaClockGating", "yes");
#endif

	/* Write allocation */
	env = getenv("enaWrAllo");
	if ( !env || ( ((strcmp(env, "no") == 0) || (strcmp(env, "No") == 0) )))
		setenv("enaWrAllo", "no");
	else
		setenv("enaWrAllo", "yes");

	/* Pex mode */
	env = getenv("pexMode");
	if ( env && ( ((strcmp(env, "EP") == 0) || (strcmp(env, "ep") == 0) )))
		setenv("pexMode", "EP");
	else
		setenv("pexMode", "RC");

	env = getenv("disL2Cache");
	if (!env || ( (strcmp(env, "yes") == 0) || (strcmp(env, "Yes") == 0) ) )
		setenv("disL2Cache", "yes");
	else
		setenv("disL2Cache", "no");

	env = getenv("MPmode");
	if (!env || ( (strcmp(env, "smp") == 0) || (strcmp(env, "SMP") == 0) ) )
		setenv("MPmode", "SMP");
	else
		setenv("MPmode", "AMP");

	/* Make address 0x80000000-0x8fffffff shared (set 'S' in pgd) */
	env = getenv("cacheShare");
	if ( !env || ( ((strcmp(env, "no") == 0) || (strcmp(env, "No") == 0) )))
		setenv("cacheShare", "no");
	else
		setenv("cacheShare", "yes");

	env = getenv("setL2CacheWT");
	if (!env || ( (strcmp(env, "no") == 0) || (strcmp(env, "No") == 0) ) )
		setenv("setL2CacheWT", "no");
	else
		setenv("setL2CacheWT", "yes");

	env = getenv("disL2Prefetch");
	if (!env || ( (strcmp(env, "yes") == 0) || (strcmp(env, "Yes") == 0) ) ) {
		setenv("disL2Prefetch", "yes");

		/* ICache Prefetch */
		env = getenv("enaICPref");
		if ( env && ( ((strcmp(env, "no") == 0) || (strcmp(env, "No") == 0) )))
			setenv("enaICPref", "no");
		else
			setenv("enaICPref", "yes");

		/* DCache Prefetch */
		env = getenv("enaDCPref");
		if ( env && ( ((strcmp(env, "no") == 0) || (strcmp(env, "No") == 0) )))
			setenv("enaDCPref", "no");
		else
			setenv("enaDCPref", "yes");
	}else  {
		setenv("disL2Prefetch", "no");
		setenv("enaICPref", "no");
		setenv("enaDCPref", "no");
	}

	env = getenv("enaFPU");
	if (env && ((strcmp(env, "no") == 0) || (strcmp(env, "no") == 0)))
		setenv("enaFPU", "no");
	else
		setenv("enaFPU", "yes");

	env = getenv("sata_dma_mode");
	if ( env && ((strcmp(env, "No") == 0) || (strcmp(env, "no") == 0) ) )
		setenv("sata_dma_mode", "no");
	else
		setenv("sata_dma_mode", "yes");

	env = getenv("sata_delay_reset");
	if (!env)
		setenv("sata_delay_reset", "0");

	/* Malloc length */
	env = getenv("MALLOC_len");
	if (env)
		malloc_len =  simple_strtoul(env, NULL, 10) << 20;
	else
		malloc_len      = 0;
	if (malloc_len == 0) {
		sprintf(tmp_buf, "%d", CONFIG_SYS_MALLOC_LEN >> 20);
		setenv("MALLOC_len", tmp_buf);
	}

	/* primary network interface */
	env = getenv("ethprime");
	if (!env)
		setenv("ethprime", ENV_ETH_PRIME);

	/* image/script addr */
#if defined (CONFIG_CMD_STAGE_BOOT)
	env = getenv("fdt_addr");
	if (!env)
		setenv("fdt_addr", "2040000");
	env = getenv("kernel_addr_r");
	if (!env)
		setenv("kernel_addr_r", "2080000");
	env = getenv("ramdisk_addr_r");
	if (!env)
		setenv("ramdisk_addr_r", "2880000");
	env = getenv("device_partition");
	if (!env)
		setenv("device_partition", "0:1");
	env = getenv("boot_order");
	if (!env)
		setenv("boot_order", "hd_scr usb_scr mmc_scr hd_img usb_img mmc_img pxe net_img net_scr");
	env = getenv("script_name");
	if (!env)
		setenv("script_name", "boot.scr");
	env = getenv("ide_path");
	if (!env)
		setenv("ide_path", "/");
	env = getenv("script_addr_r");
	if (!env)
		setenv("script_addr_r", "3000000");
	env = getenv("bootargs_dflt");
	if (!env)
		setenv("bootargs_dflt", "$console $mtdparts $bootargs_root nfsroot=$serverip:$rootpath \
ip=$ipaddr:$serverip$bootargs_end $mvNetConfig video=dovefb:lcd0:$lcd0_params \
clcd.lcd0_enable=$lcd0_enable clcd.lcd_panel=$lcd_panel");
	env = getenv("bootcmd_auto");
	if (!env)
		setenv("bootcmd_auto", "stage_boot $boot_order");
	env = getenv("bootcmd_lgcy");
	if (!env)
		setenv("bootcmd_lgcy", "tftpboot 0x2000000 $image_name; setenv bootargs $bootargs_dflt; bootm 0x2000000; ");
#endif
	env = getenv("pxe_files_load");
	if (!env)
		setenv("pxe_files_load", ":default.arm-armadaxp-db:default.arm-armadaxp:default.arm");
	env = getenv("pxefile_addr_r");
	if (!env)
		setenv("pxefile_addr_r", "3100000");
	env = getenv("initrd_name");
	if (!env)
		setenv("initrd_name", "uInitrd");

	/* netbsd boot arguments */
	env = getenv("netbsd_en");
	if ( !env || ( ((strcmp(env, "no") == 0) || (strcmp(env, "No") == 0) )))
		setenv("netbsd_en", "no");
	else {
		setenv("netbsd_en", "yes");
		env = getenv("netbsd_gw");
		if (!env)
			setenv("netbsd_gw", "192.168.0.254");
		env = getenv("netbsd_mask");
		if (!env)
			setenv("netbsd_mask", "255.255.255.0");

		env = getenv("netbsd_fs");
		if (!env)
			setenv("netbsd_fs", "nfs");

		env = getenv("netbsd_server");
		if (!env)
			setenv("netbsd_server", "192.168.0.1");

		env = getenv("netbsd_ip");
		if (!env) {
			env = getenv("ipaddr");
			setenv("netbsd_ip", env);
		}

		env = getenv("netbsd_rootdev");
		if (!env)
			setenv("netbsd_rootdev", "mgi0");

		env = getenv("netbsd_add");
		if (!env)
			setenv("netbsd_add", "0x800000");

		env = getenv("netbsd_get");
		if (!env)
			setenv("netbsd_get", "tftpboot $netbsd_add $image_name");

#if defined(MV_INC_BOARD_QD_SWITCH)
		env = getenv("netbsd_netconfig");
		if (!env)
			setenv("netbsd_netconfig", "mv_net_config=<((mgi0,00:00:11:22:33:44,0)(mgi1,00:00:11:22:33:55,1:2:3:4)),mtu=1500>");
#endif
		env = getenv("netbsd_set_args");
		if (!env)
			setenv("netbsd_set_args", "setenv bootargs nfsroot=$netbsd_server:$rootpath fs=$netbsd_fs \
                    ip=$netbsd_ip serverip=$netbsd_server mask=$netbsd_mask gw=$netbsd_gw rootdev=$netbsd_rootdev \
                    ethaddr=$ethaddr eth1addr=$eth1addr ethmtu=$ethmtu eth1mtu=$eth1mtu $netbsd_netconfig");

		env = getenv("netbsd_boot");
		if (!env)
			setenv("netbsd_boot", "bootm $netbsd_add $bootargs");

		env = getenv("netbsd_bootcmd");
		if (!env)
			setenv("netbsd_bootcmd", "run netbsd_get ; run netbsd_set_args ; run netbsd_boot");
	}

	/* vxWorks boot arguments */
	env = getenv("vxworks_en");
	if ( !env || ( ((strcmp(env, "no") == 0) || (strcmp(env, "No") == 0) )))
		setenv("vxworks_en", "no");
	else {
		char* buff = (char*)0x1100;
		setenv("vxworks_en", "yes");

		sprintf(buff, "mgi(0,0) host:vxWorks.st");
		env = getenv("serverip");
		strcat(buff, " h=");
		strcat(buff, env);
		env = getenv("ipaddr");
		strcat(buff, " e=");
		strcat(buff, env);
		strcat(buff, ":ffff0000 u=anonymous pw=target ");

		setenv("vxWorks_bootargs", buff);
		setenv("bootaddr", "0x1100");
	}

	/* linux boot arguments */
	env = getenv("bootargs_root");
	if (!env)
		setenv("bootargs_root", "root=/dev/nfs rw");

	/* For open Linux we set boot args differently */
	env = getenv("mainlineLinux");
	if (env && ((strcmp(env, "yes") == 0) ||  (strcmp(env, "Yes") == 0))) {
		env = getenv("bootargs_end");
		if (!env)
			setenv("bootargs_end", ":::orion:eth0:none");
	} else {
		env = getenv("bootargs_end");
		if (!env)
#if defined(MV_INC_BOARD_QD_SWITCH)
			setenv("bootargs_end", MV_BOOTARGS_END_SWITCH);
#else
			setenv("bootargs_end", MV_BOOTARGS_END);
#endif
	}

	env = getenv("image_name");
	if (!env)
		setenv("image_name", "uImage");

#if CONFIG_AMP_SUPPORT
	env = getenv("amp_enable");
	if (!env || ( ((strcmp(env, "no") == 0) || (strcmp(env, "No") == 0) )))
		setenv("amp_enable", "no");
	else{
		env = getenv("amp_groups");
		if (!env)
			setenv("amp_groups", "0");

		env = getenv("amp_shared_mem");
		if (!env)
			setenv("amp_shared_mem", "0x80000000:0x100000");

		setenv("bootcmd", "amp_boot");

		env = getenv("amp_verify_boot");
		if (!env)
			setenv("amp_verify_boot", "yes");

	}
#endif

#ifdef CONFIG_ARM_LPAE
	/* LPAE support */
	env = getenv("enaLPAE");
	if (!env)
		setenv("enaLPAE", "no");
#endif

#if CONFIG_OF_LIBFDT
	char bootcmd_fdt[] = "tftpboot 0x2000000 $image_name;tftpboot $fdtaddr $fdtfile;\
setenv bootargs $console $mtdparts $bootargs_root nfsroot=$serverip:$rootpath ip=$ipaddr: \
$serverip$bootargs_end $mvNetConfig video=dovefb:lcd0:$lcd0_params \
clcd.lcd0_enable=$lcd0_enable clcd.lcd_panel=$lcd_panel;  bootz 0x2000000 - $fdtaddr;";
	env = getenv("fdtaddr");
	if (!env)
		setenv("fdtaddr", "0x1000000");

	env = getenv("fdtfile");
	if (!env)
		setenv("fdtfile", "armada-385-db.dtb");
	env = getenv("bootcmd_fdt");
	if (!env)
		setenv("bootcmd_fdt",bootcmd_fdt);
#endif

#if (CONFIG_BOOTDELAY >= 0)
	env = getenv("bootcmd");
	if (!env)
#if defined(CONFIG_OF_LIBFDT) && defined (CONFIG_OF_LIBFDT_IS_DEFAULT)
		setenv("bootcmd",bootcmd_fdt);
#elif defined(CONFIG_CMD_STAGE_BOOT)
//		setenv("bootcmd","stage_boot $boot_order");
// Temporary workaround till stage_boot gets stable.
		setenv("bootcmd","tftpboot 0x2000000 $image_name;\
setenv bootargs $console $mtdparts $bootargs_root nfsroot=$serverip:$rootpath \
ip=$ipaddr:$serverip$bootargs_end  video=dovefb:lcd0:$lcd0_params clcd.lcd0_enable=$lcd0_enable clcd.lcd_panel=$lcd_panel;  bootm $loadaddr; ");

#elif defined(MV_INCLUDE_TDM) || defined(MV_INC_BOARD_QD_SWITCH)
		setenv("bootcmd","tftpboot 0x2000000 $image_name;\
setenv bootargs $console $mtdparts $bootargs_root nfsroot=$serverip:$rootpath \
ip=$ipaddr:$serverip$bootargs_end $mvNetConfig video=dovefb:lcd0:$lcd0_params clcd.lcd0_enable=$(lcd0_enable) clcd.lcd_panel=$lcd_panel;  bootm $loadaddr; ");
#else
		setenv("bootcmd","tftpboot 0x2000000 $image_name;\
setenv bootargs $console $mtdparts $bootargs_root nfsroot=$serverip:$rootpath \
ip=$ipaddr:$serverip$bootargs_end  video=dovefb:lcd0:$lcd0_params clcd.lcd0_enable=$lcd0_enable clcd.lcd_panel=$lcd_panel;  bootm $loadaddr; ");
#endif
#endif  /* (CONFIG_BOOTDELAY >= 0) */

	env = getenv("standalone");
	if (!env)
#if defined(MV_INCLUDE_TDM) && defined(MV_INC_BOARD_QD_SWITCH)
		setenv("standalone", "fsload 0x2000000 $image_name;setenv bootargs $console $mtdparts root=/dev/mtdblock0 rw \
ip=$ipaddr:$serverip$bootargs_end $mvNetConfig; bootm 0x2000000;");
#elif defined(MV_INC_BOARD_QD_SWITCH)
		setenv("standalone", "fsload 0x2000000 $image_name;setenv bootargs $console $mtdparts root=/dev/mtdblock0 rw \
ip=$ipaddr:$serverip$bootargs_end $mvNetConfig; bootm 0x2000000;");
#elif defined(MV_INCLUDE_TDM)
		setenv("standalone", "fsload 0x2000000 $image_name;setenv bootargs $console $mtdparts root=/dev/mtdblock0 rw \
ip=$ipaddr:$serverip$bootargs_end; bootm 0x2000000;");
#else
		setenv("standalone", "fsload 0x2000000 $image_name;setenv bootargs $console $mtdparts root=/dev/mtdblock0 rw \
ip=$ipaddr:$serverip$bootargs_end; bootm 0x2000000;");
#endif

	/* Set boodelay to 3 sec, if Monitor extension are disabled */
	if (!enaMonExt())
		setenv("disaMvPnp", "no");

	/* Disable PNP config of Marvell memory controller devices. */
	env = getenv("disaMvPnp");
	if (!env)
		setenv("disaMvPnp", "no");

	env = getenv("bootdelay");
	if (!env)
		setenv("bootdelay", "3");

#if (defined(MV_INCLUDE_GIG_ETH) || defined(MV_INCLUDE_UNM_ETH))
	/* Generate random ip and mac address */
	unsigned int xi = 0x1, xj = 0x2, xk = 0x3, xl = 0x4;
	char ethaddr_0[30];
	char ethaddr_1[30];
	char ethaddr_2[30];
	char ethaddr_3[30];
	char pon_addr[30];

#if defined(MV_INCLUDE_RTC)
	/* Read RTC to create pseudo-random data for enc */
	struct rtc_time tm;

	rtc_get(&tm);
	xi = ((tm.tm_yday + tm.tm_sec) % 254);
	/* No valid ip with one of the fileds has the value 0 */
	if (xi == 0)
		xi += 2;

	xj = ((tm.tm_yday + tm.tm_min) % 254);
	/* No valid ip with one of the fileds has the value 0 */
	if (xj == 0)
		xj += 2;

	/* Check if the ip address is the same as the server ip */
	if ((xj == 1) && (xi == 11))
		xi += 2;

	xk = (tm.tm_min * tm.tm_sec) % 254;
	xl = (tm.tm_hour * tm.tm_sec) % 254;
#endif  /* defined(MV_INCLUDE_RTC) */

	sprintf(ethaddr_0, "00:50:43:%02x:%02x:%02x", xk, xi, xj);
	sprintf(ethaddr_1, "00:50:43:%02x:%02x:%02x", xl, xi, xj);
	sprintf(ethaddr_2, "00:50:43:%02x:%02x:%02x", xl, xk, xj);
	sprintf(ethaddr_3, "00:50:43:%02x:%02x:%02x", xi, xk, xl);
	sprintf(pon_addr, "00:50:43:%02x:%02x:%02x", xj, xk, xl);

	/* MAC addresses */
	env = getenv("ethaddr");
	if (!env)
		setenv("ethaddr", ethaddr_0);

	env = getenv("eth1addr");
	if (!env)
		setenv("eth1addr", ethaddr_1);

	env = getenv("eth2addr");
	if (!env)
		setenv("eth2addr", ethaddr_2);

	env = getenv("eth3addr");
	if (!env)
		setenv("eth3addr", ethaddr_3);

	env = getenv("mv_pon_addr");
	if (!env)
		setenv("mv_pon_addr", pon_addr);

	env = getenv("ethmtu");
	if (!env)
		setenv("ethmtu", "1500");

	env = getenv("eth1mtu");
	if (!env)
		setenv("eth1mtu", "1500");

	env = getenv("eth2mtu");
	if (!env)
		setenv("eth2mtu", "1500");

	env = getenv("eth3mtu");
	if (!env)
		setenv("eth3mtu", "1500");

	/* Set mvNetConfig env parameter */
	env = getenv("mvNetConfig");
	if (!env)
		setenv("mvNetConfig", "mv_net_config=4,(00:50:43:11:11:11,0:1:2:3),mtu=1500");

#endif  /*  (MV_INCLUDE_GIG_ETH) || defined(MV_INCLUDE_UNM_ETH) */

#if defined(MV_INCLUDE_USB)
	/* USB Host */
	env = getenv("usb0Mode");
	if (!env)
		setenv("usb0Mode", ENV_USB0_MODE);

	env = getenv("usb1Mode");
	if (!env)
		setenv("usb1Mode", ENV_USB1_MODE);

	env = getenv("usbActive");
	if (!env)
		setenv("usbActive", ENV_USB_ACTIVE);

#endif  /* (MV_INCLUDE_USB) */
#ifdef CONFIG_SCSI_AHCI
	env = getenv("sataActive");
	if (!env)
		setenv("sataActive", ENV_SATA_ACTIVE);
#endif
#if defined(YUK_ETHADDR)
	env = getenv("yuk_ethaddr");
	if (!env)
		setenv("yuk_ethaddr", YUK_ETHADDR);

	{
		int i;
		char *tmp = getenv("yuk_ethaddr");
		char *end;

		for (i = 0; i < 6; i++) {
			yuk_enetaddr[i] = tmp ? simple_strtoul(tmp, &end, 16) : 0;
			if (tmp)
				tmp = (*end) ? end + 1 : end;
		}
	}
#endif  /* defined(YUK_ETHADDR) */

#if defined(MV_NAND)
	env = getenv("nandEcc");
	if (!env)
		setenv("nandEcc", "1bit");

#endif
#if defined(CONFIG_CMD_RCVR)
	env = getenv("netretry");
	if (!env)
		setenv("netretry", "no");

	env = getenv("loadaddr");
	if (!env)
		setenv("loadaddr", RCVR_LOAD_ADDR);

	env = getenv("autoload");
	if (!env)
		setenv("autoload", "no");

#endif
	env = getenv("eeeEnable");
	if (!env)
		setenv("eeeEnable", "no");

	env = getenv("lcd0_enable");
	if (!env)
		setenv("lcd0_enable", "0");

	env = getenv("lcd0_params");
	if (!env)
		setenv("lcd0_params", "640x480-16@60");

	env = getenv("lcd_panel");
	if (!env)
		setenv("lcd_panel", "0");

	return;
}

#define MV_CPU_SW_RESET_CONTROL(cpu)    (0x20800 + ((cpu) * 0x8))
#define AXP_CPU_DIVCLK_CTRL0                    0x18700
#define AXP_CPU_DIVCLK_CTRL2_RATIO_FULL0        0x18708
#define AXP_CPU_DIVCLK_CTRL2_RATIO_FULL1        0x1870C

void kick_next(void)
{
	int cpuNum;
	unsigned int divider;
	unsigned int val;
	unsigned int reg;

	if (whoAmI() == 0) {
		MV_REG_WRITE(0x20988, 0x0);
		MV_REG_WRITE(0x22224, 0x600000);
		MV_REG_WRITE(0x22324, 0x600000);
		MV_REG_WRITE(0x22424, 0x600000);
		MV_REG_WRITE(0x22424, 0x600000);
		MV_REG_WRITE(0x200b8, 0x07ff1d11);

		/* set the scondary CPUs ratio to MAX freq*/
		divider = (MV_REG_READ(AXP_CPU_DIVCLK_CTRL2_RATIO_FULL1) & 0x3F);

		val = MV_REG_READ(AXP_CPU_DIVCLK_CTRL2_RATIO_FULL1);
		val &= ~(0xFFFFFF00);   /* clear cpu1-3 ratio */
		val |= ((divider << 8) | (divider << 16) | (divider << 24));
		MV_REG_WRITE(AXP_CPU_DIVCLK_CTRL2_RATIO_FULL1, val);

		val = MV_REG_READ(AXP_CPU_DIVCLK_CTRL0);
		val |= (0x7 << 21);
		MV_REG_WRITE(AXP_CPU_DIVCLK_CTRL0, val);

		/* Request clock devider reload */
		val = MV_REG_READ(AXP_CPU_DIVCLK_CTRL0);
		val |= 1 << 24;
		MV_REG_WRITE(AXP_CPU_DIVCLK_CTRL0, val);

		/* Wait for clocks to settle down then release reload request */
		udelay(1000);
		val &= ~(0xf << 21);
		MV_REG_WRITE(AXP_CPU_DIVCLK_CTRL0, val);
		udelay(1000);
	}

	cpuNum = mvBoardCpuCoresNumGet() == 2 ? (mvBoardCpuCoresNumGet() - 1) : (mvBoardCpuCoresNumGet());
	if (whoAmI() < cpuNum) {
		reg = MV_REG_READ((MV_CPU_SW_RESET_CONTROL(whoAmI() + 1)));
		reg &= 0xFFFFFFFE;
		MV_REG_WRITE(MV_CPU_SW_RESET_CONTROL(whoAmI() + 1), reg);
		udelay(1000000);

	}
}

#ifdef BOARD_LATE_INIT
int board_late_init(void)
{
	char *env;
	env = getenv("enaMP");
	if (env && ((strcmp(env, "yes") == 0) || (strcmp(env, "Yes") == 0)))
		kick_next();

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
}

int board_eth_init(bd_t *bis)
{
#if defined(MV_INCLUDE_GIG_ETH) || defined(MV_INCLUDE_UNM_ETH)
	/* move to the begining so in case we have a PCI NIC it will
	   read the env mac addresses correctlly. */
	mv_eth_initialize(bis);
#endif

#if defined(CONFIG_SK98)
	skge_initialize(bis);
#endif

#if defined(CONFIG_E1000)
	e1000_initialize(bis);
#endif

#ifdef CONFIG_EEPRO100
	eepro100_initialize(bis);
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

/*
* print_cpuinfo - original U-Boot print function - used before code relocation
*
* DESCRIPTION:
*       This function is called by board_init_f (before code relocation).
*       no actual print is done here, due to global variables limitations (bss).
*       after code relocation, we can start using global variables and print board information.
 */
int print_cpuinfo(void)
{
	return 0;
}

/*
* late_print_cpuinfo - marvell U-Boot print function - used after code relocation
*
* DESCRIPTION:
*       This function is called by board_init_r (after code relocation).
*       all global variables limitations(bss) are off at this state
 */
int late_print_cpuinfo(void)
{
	char name[50];

	mvCtrlUpdatePexId();

	mvBoardNameGet(name,50);
	printf("Board: %s\n",  name);
	mvCtrlModelRevNameGet(name);
	printf("SoC:   %s\n", name);
	printf("       running %d CPUs\n", mvCtrlGetCpuNum() + 1);
	mvCpuNameGet(name);
	printf("CPU:   %s",  name);
#ifdef MV_CPU_LE
	printf(" LE\n");
#else
	printf(" BE\n");
#endif

	printf("       CPU %d\n",  whoAmI());
	printf("       CPU    @ %d [MHz]\n", mvCpuPclkGet()/1000000);
	printf("       L2     @ %d [MHz]\n", mvCpuL2ClkGet()/1000000);
	printf("       TClock @ %d [MHz]\n", mvTclkGet()/1000000);
	printf("       DDR    @ %d [MHz]\n", CONFIG_SYS_BUS_CLK/1000000);
	printf("       DDR %dBit Width, %s Memory Access\n", mvCtrlDDRBudWidth(), mvCtrlDDRThruXbar() ? "XBAR" : "FastPath");

	display_dram_config(1);
	return 0;
}
int board_early_init_f (void)
{
	/* set mpp0,1 to uart and MPP 2,3 to twsi */
        MV_U32 regData = (MV_REG_READ(mvCtrlMppRegGet(0))  & ~(GROUP0_DEFAULT_MPP_TWSI_I2C_MASK));
        regData |= GROUP0_DEFAULT_MPP_TWSI_I2C;
        MV_REG_WRITE(mvCtrlMppRegGet(0), regData);
	return 0;
}
int misc_init_r(void)
{
	mvBoardDebugLed(5);

	/* init special env variables */
	misc_init_r_env();

#ifdef CONFIG_AMP_SUPPORT
	amp_init();
#endif

	mv_cpu_init();

#if defined(MV_INCLUDE_MONT_EXT)
	if (enaMonExt()) {
		printf("Marvell monitor extension:\n");
		mon_extension_after_relloc();
	}
#endif
	/* init the units decode windows */
	misc_init_r_dec_win();
	memset((void*)CONFIG_SYS_LOAD_ADDR, 0, CONFIG_SYS_MIN_HDR_DEL_SIZE);
	mvBoardDebugLed(6);

	mvBoardMppModuleTypePrint();
	mvBoardOtherModuleTypePrint();

	mvBoardDebugLed(7);

#ifdef CONFIG_SCSI_AHCI
	{
		int sataPort = 0;
		mvSysSata3WinInit();
		if(strcmp(getenv("sataActive"), "1") == 0)
			sataPort = 1;
		printf("SCSI: active SATA unit %d, offset 0x%x\n",
		       sataPort, (INTER_REGS_BASE | MV_SATA3_REGS_OFFSET(sataPort)));
		if (0 == ahci_init(INTER_REGS_BASE | MV_SATA3_REGS_OFFSET(sataPort)))
			scsi_init();
		else
			printf("AHCI init failed!\n");
	}
#endif
	char *env;
	/* pcie fine tunning */
	env = getenv("pcieTune");
	if (env && ((strcmp(env, "yes") == 0) || (strcmp(env, "yes") == 0)))
		pcie_tune();
	else

	setenv("pcieTune", "no");

#if defined(MV_INCLUDE_UNM_ETH) || defined(MV_INCLUDE_GIG_ETH)
	mvBoardEgigaPhyInit();
#endif

	return 0;
}

MV_U32 mvTclkGet(void)
{
	DECLARE_GLOBAL_DATA_PTR;
	/* get it only on first time */
	if (gd->tclk == 0)
		gd->tclk = mvBoardTclkGet();

	return gd->tclk;
}

MV_U32 mvSysClkGet(void)
{
	DECLARE_GLOBAL_DATA_PTR;
	/* get it only on first time */
	if (gd->bus_clk == 0)
		gd->bus_clk = mvBoardSysClkGet();

	return gd->bus_clk;
}

void reset_cpu(ulong addr)
{
	mvBoardReset();
}

void mv_cpu_init(void)
{
#if 0
	char *env;
	volatile unsigned int temp;

	/* enable access to CP10 and CP11 */
	temp = 0x00f00000;
	__asm__ __volatile__ ("mcr p15, 0, %0, c1, c0, 2" :: "r" (temp));

	env = getenv("enaFPU");
	if (env && ((strcmp(env, "yes") == 0) || (strcmp(env, "Yes") == 0))) {
		/* init and Enable FPU to Run Fast Mode */
		printf("FPU initialized to Run Fast Mode.\n");
		/* Enable */
		temp = FPEXC_ENABLE;
		fmxr(FPEXC, temp);
		/* Run Fast Mode */
		temp = fmrx(FPSCR);
		temp |= (FPSCR_DEFAULT_NAN | FPSCR_FLUSHTOZERO);
		fmxr(FPSCR, temp);
	} else {
		printf("FPU not initialized\n");
		/* Disable */
		temp = fmrx(FPEXC);
		temp &= ~FPEXC_ENABLE;
		fmxr(FPEXC, temp);
	}

	__asm__ __volatile__ ("mrc p15, 1, %0, c15, c1, 2" : "=r" (temp));
	temp |= (BIT25 | BIT27 | BIT29 | BIT30);
	/* removed BIT23 in order to enable fast LDR bypass */
	__asm__ __volatile__ ("mcr p15, 1, %0, c15, c1, 2\n" \
			      "mcr p15, 0, %0, c7, c5, 4" : : "r" (temp)); /*imb*/

	/* Multi-CPU managment */
	env = getenv("enaMP");
	if ( env && ((strcmp(env, "yes") == 0) || (strcmp(env, "Yes") == 0)) ) {
		env = getenv("MPmode");
		if ( env && ((strcmp(env, "AMP") == 0) || (strcmp(env, "amp") == 0)) ) {
			/* Set AMP in Auxilary control register */
			__asm__ __volatile__ ("mrc p15, 0, %0, c1, c0, 1" : "=r" (temp));
			temp &= ~(0x1 << 5);
			__asm__ __volatile__ ("mcr p15, 0, %0, c1, c0, 1\n" \
					      "mcr p15, 0, %0, c7, c5, 4" : : "r" (temp)); /* imb */

			/* Set AMP in Auxiliary Funcional Modes Control register */
			__asm__ __volatile__ ("mrc p15, 1, %0, c15, c2, 0" : "=r" (temp));
			temp &= ~(0x1 << 1);
			__asm__ __volatile__ ("mcr p15, 1, %0, c15, c2, 0\n" \
					      "mcr p15, 0, %0, c7, c5, 4" : : "r" (temp)); /* imb */
		}else  {
			/* Set SMP in Auxilary control register */
			__asm__ __volatile__ ("mrc p15, 0, %0, c1, c0, 1" : "=r" (temp));
			temp |= (0x1 << 5);
			__asm__ __volatile__ ("mcr p15, 0, %0, c1, c0, 1\n" \
					      "mcr p15, 0, %0, c7, c5, 4" : : "r" (temp)); /* imb */

			/* Set SMP in Auxiliary Funcional Modes Control register */
			__asm__ __volatile__ ("mrc p15, 1, %0, c15, c2, 0" : "=r" (temp));
			temp |= (0x1 << 1);
			__asm__ __volatile__ ("mcr p15, 1, %0, c15, c2, 0\n" \
					      "mcr p15, 0, %0, c7, c5, 4" : : "r" (temp)); /* imb */

			/* Enable CPU respond to coherency fabric requests */
			/* Assaf: Note must be enabled for IO coherency as well */
			MV_REG_BIT_SET(SOC_COHERENCY_FABRIC_CTRL_REG, (0x1 << (24 + whoAmI())));

			/* Configure all Cores to be in SMP Group0 */
			MV_REG_BIT_SET(SOC_COHERENCY_FABRIC_CFG_REG, (0x1 << (24 + whoAmI())));

			/* In loader mode, set fabric regs for both CPUs.*/

			env = getenv("enaMonExt");
			if ( env && ((strcmp(env, "no") == 0) || (strcmp(env, "No") == 0)) ) {
				/* Configure Core1 to be in SMP Group0 */
				MV_REG_BIT_SET(SOC_COHERENCY_FABRIC_CFG_REG, (0x1 << 25));
			}

			/* Set number of CPUs=2 (for Linux) */
			MV_REG_BIT_SET(SOC_COHERENCY_FABRIC_CFG_REG, 0x1);
		}
	}

	/* Set L2C WT mode */
	temp = MV_REG_READ(CPU_L2_AUX_CTRL_REG) & ~CL2ACR_WB_WT_ATTR_MASK;
	env = getenv("setL2CacheWT");
	if (!env || ((strcmp(env, "yes") == 0) || (strcmp(env, "Yes") == 0)))
		temp |= CL2ACR_WB_WT_ATTR_WT;
	 /* Set L2 algorithm to semi_pLRU */
	temp &= ~CL2ACR_REP_STRGY_MASK;
	if (mvCtrlRevGet() == 1)
		temp |= CL2ACR_REP_STRGY_semiPLRU_MASK;
	else{
		temp |= CL2ACR_REP_STRGY_semiPLRU_WA_MASK;
		temp |= CL2_DUAL_EVICTION;
		temp |= CL2_PARITY_ENABLE;
		temp |= CL2_InvalEvicLineUCErr;
	}

	MV_REG_WRITE(CPU_L2_AUX_CTRL_REG, temp);

	/* enable L2C */
	temp = MV_REG_READ(CPU_L2_CTRL_REG);

	env = getenv("disL2Cache");
	if (!env || ((strcmp(env, "no") == 0) || (strcmp(env, "No") == 0)))
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
		if ( env && ((strcmp(env, "WB") == 0) || (strcmp(env, "wb") == 0)) )
			temp |= CL2ACR_WB_WT_ATTR_WB;
		else if ( env && ((strcmp(env, "WT") == 0) || (strcmp(env, "wt") == 0)) )
			temp |= CL2ACR_WB_WT_ATTR_WT;
		else
			temp |= CL2ACR_WB_WT_ATTR_PAGE;

		/* Set "Force Write Allocate" field */
		env = getenv("L2forceWrAlloc");
		if ( env && ((strcmp(env, "no") == 0) || (strcmp(env, "No") == 0)) )
			temp |= CL2ACR_FORCE_NO_WA;
		else if ( env && ((strcmp(env, "yes") == 0) || (strcmp(env, "Yes") == 0)) )
			temp |= CL2ACR_FORCE_WA;
		else
			temp |= CL2ACR_FORCE_WA_DISABLE;

		/* Set "ECC" */
		env = getenv("L2EccEnable");
		if (!env || ( (strcmp(env, "no") == 0) || (strcmp(env, "No") == 0) ) )
			temp &= ~CL2ACR_ECC_EN;
		else
			temp |= CL2ACR_ECC_EN;

		/* Set other L2 configurations */
		temp |= (CL2ACR_PARITY_EN | CL2ACR_INVAL_UCE_EN);

		/* Set L2 algorithm to semi_pLRU */
		temp &= ~CL2ACR_REP_STRGY_MASK;

		temp |= CL2ACR_REP_STRGY_semiPLRU_MASK;

		/* Write to L2 Auxilary control register */
		MV_REG_WRITE(CPU_L2_AUX_CTRL_REG, temp);

		env = getenv("L2SpeculativeRdEn");
		if (env && ((strcmp(env, "no") == 0) || (strcmp(env, "No") == 0)) )
			MV_REG_BIT_SET(0x20228, ((0x1 << 5)));
		else
			MV_REG_BIT_RESET(0x20228, ((0x1 << 5)));

	}

	/* Enable i cache */
	asm ("mrc p15, 0, %0, c1, c0, 0" : "=r" (temp));
	temp |= BIT12;
	/* Change reset vector to address 0x0 */
	temp &= ~BIT13;
	asm ("mcr p15, 0, %0, c1, c0, 0\n" \
	     "mcr p15, 0, %0, c7, c5, 4" : : "r" (temp));  /* imb */
#endif

}

/* Set unit in power off mode acording to the detection of MPP/SERDES */
#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
void mv_set_power_scheme(void)
{
	return;
}

#endif
