/* Copyright (C) 2011
 * Corscience GmbH & Co. KG - Simon Schwarz <schwarz@corscience.de>
 *  - Added prep subcommand support
 *  - Reorganized source - modeled after powerpc version
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307	 USA
 *
 */

#include <common.h>
#include <command.h>
#include <image.h>
#include <u-boot/zlib.h>
#include <asm/byteorder.h>
#include <fdt.h>
#include <libfdt.h>
#include <fdt_support.h>
#include <asm/bootm.h>
#include <linux/compiler.h>

DECLARE_GLOBAL_DATA_PTR;

#if defined(CONFIG_SETUP_MEMORY_TAGS) || \
	defined(CONFIG_CMDLINE_TAG) || \
	defined(CONFIG_INITRD_TAG) || \
	defined(CONFIG_SERIAL_TAG) || \
	defined (CONFIG_MARVELL_TAG) ||	\
	defined(CONFIG_REVISION_TAG)
static struct tag *params;
#endif

#if defined (CONFIG_MARVELL_TAG)
extern void mvEgigaStrToMac(char *source, char *dest);
static void setup_marvell_tag(void);
#endif

static ulong get_sp(void)
{
	ulong ret;

	asm ("mov %0, sp" : "=r" (ret) : );
	return ret;
}

#ifdef CONFIG_AMP_SUPPORT
extern int amp_enable;
extern int amp_group_id;
int  amp_boot(int mach_id, int load_addr, int param_addr);
void amp_wait_to_boot(void);
int  mv_amp_group_setup(int group_id, int load_addr);
#endif

void arch_lmb_reserve(struct lmb *lmb)
{
	ulong sp;

	/*
	 * Booting a (Linux) kernel image
	 *
	 * Allocate space for command line and board info - the
	 * address should be as high as possible within the reach of
	 * the kernel (see CONFIG_SYS_BOOTMAPSZ settings), but in unused
	 * memory, which means far enough below the current stack
	 * pointer.
	 */
	sp = get_sp();
	debug("## Current stack ends at 0x%08lx ", sp);

	/* adjust sp by 4K to be safe */
	sp -= 4096;
	lmb_reserve(lmb, sp,
		    gd->bd->bi_dram[0].start + gd->bd->bi_dram[0].size - sp);
}

#ifdef CONFIG_OF_LIBFDT
int fixup_memory_node(void *blob)
{
#ifndef CONFIG_MARVELL
	bd_t	*bd = gd->bd;
#endif
	int bank;
	u64 start[CONFIG_NR_DRAM_BANKS];
	u64 size[CONFIG_NR_DRAM_BANKS];

#ifdef CONFIG_MARVELL
	for (bank = 0; bank < CONFIG_NR_DRAM_BANKS; bank++) {
		start[bank] = gd->dram_hw_info[bank].start;
		size[bank] = gd->dram_hw_info[bank].size;
		if ((start[bank] + size[bank]) == _4G)		/* Save 256M for IO at 4G-256M */
			size[bank] = ((_4G - _256M) - start[bank]);
	}
#else
	for (bank = 0; bank < CONFIG_NR_DRAM_BANKS; bank++) {
		start[bank] = bd->bi_dram[bank].start;
		size[bank] = bd->bi_dram[bank].size;
	}
#endif

	return fdt_fixup_memory_banks(blob, start, size, CONFIG_NR_DRAM_BANKS);
}

#endif

static void announce_and_cleanup(void)
{
	printf("\nStarting kernel ...\n\n");
	bootstage_mark_name(BOOTSTAGE_ID_BOOTM_HANDOFF, "start_kernel");
#ifdef CONFIG_BOOTSTAGE_FDT
	bootstage_fdt_add_report();
#endif
#ifdef CONFIG_BOOTSTAGE_REPORT
	bootstage_report();
#endif

#ifdef CONFIG_USB_DEVICE
	udc_disconnect();
#endif
	cleanup_before_linux();
}

#if defined(CONFIG_SETUP_MEMORY_TAGS) || \
	defined(CONFIG_CMDLINE_TAG) || \
	defined(CONFIG_INITRD_TAG) || \
	defined(CONFIG_SERIAL_TAG) || \
	defined (CONFIG_REVISION_TAG) || \
	defined (CONFIG_MARVELL_TAG)
static void setup_start_tag(bd_t *bd)
{
	params = (struct tag*)bd->bi_boot_params;

	params->hdr.tag = ATAG_CORE;
	params->hdr.size = tag_size(tag_core);

	params->u.core.flags = 0;
	params->u.core.pagesize = 0;
	params->u.core.rootdev = 0;

	params = tag_next(params);
}

#endif

#ifdef CONFIG_SETUP_MEMORY_TAGS
static void setup_memory_tags(bd_t *bd)
{
	int i;
	char *env = getenv("enaLPAE");
	int lpae_en;

	if (!env || (strcmp(env, "no") == 0) || (strcmp(env, "No") == 0))
		lpae_en = 0;
	else
		lpae_en = 1;

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
		if (lpae_en) {
			uint64_t start, size;
			if (gd->dram_hw_info[i].size == 0ll)
				continue;
			params->hdr.tag = ATAG_MEM64;
			params->hdr.size = tag_size(tag_mem64);
			start = gd->dram_hw_info[i].start;
			size = gd->dram_hw_info[i].size;
			if ((start + size) == 0x100000000ll) {
				params->u.mem64.start = start;
				params->u.mem64.size = (0xF0000000ll - start);
				size = 0;
				params = tag_next(params);
				params->hdr.tag = ATAG_MEM64;
				params->hdr.size = tag_size(tag_mem64);
			}
			if (size) {
				params->u.mem64.start = start;
				params->u.mem64.size = size;
				params = tag_next(params);
			}
		} else {
			u32 start, size;
			if (gd->dram_hw_info[i].size == 0x0ll)
				continue;
			params->hdr.tag = ATAG_MEM;
			params->hdr.size = tag_size(tag_mem32);
			start = (u32)gd->dram_hw_info[i].start;
			size = (u32)gd->dram_hw_info[i].size;
			if ((start - 1 + size) == 0xFFFFFFFF) {
				params->u.mem.start = start;
				params->u.mem.size = (0xF0000000 - start);
				size = 0;
				params = tag_next(params);
				params->hdr.tag = ATAG_MEM;
				params->hdr.size = tag_size(tag_mem32);
			}

			if (size) {
				params->u.mem.start = start;
				params->u.mem.size = size;
				params = tag_next(params);
			}
		}

	}
}

#endif

#ifdef CONFIG_CMDLINE_TAG
static void setup_commandline_tag(bd_t *bd, char *commandline)
{
	char *p;

	if (!commandline)
		return;

	/* eat leading white space */
	for (p = commandline; *p == ' '; p++) ;

	/* skip non-existent command lines so the kernel will still
	 * use its default command line.
	 */
	if (*p == '\0')
		return;

	params->hdr.tag = ATAG_CMDLINE;
	params->hdr.size =
		(sizeof(struct tag_header) + strlen(p) + 1 + 4) >> 2;

	strcpy(params->u.cmdline.cmdline, p);

	params = tag_next(params);
}

#endif

#ifdef CONFIG_INITRD_TAG
static void setup_initrd_tag(bd_t *bd, ulong initrd_start, ulong initrd_end)
{
	/* an ATAG_INITRD node tells the kernel where the compressed
	 * ramdisk can be found. ATAG_RDIMG is a better name, actually.
	 */
	params->hdr.tag = ATAG_INITRD2;
	params->hdr.size = tag_size(tag_initrd);

	params->u.initrd.start = initrd_start;
	params->u.initrd.size = initrd_end - initrd_start;

	params = tag_next(params);
}

#endif

#if defined(CONFIG_MARVELL_TAG)

extern unsigned int mvBoardIdGet(void);
extern void mvBoardModuleConfigGet(u32 *modConfig);

static void setup_marvell_tag(void)
{
	char *env;
	char temp[20];
	int i;
	unsigned int boardId;
	u32 modCfg;

	params->hdr.tag = ATAG_MARVELL;
	params->hdr.size = tag_size(tag_mv_uboot);

	params->u.mv_uboot.uboot_version = VER_NUM;
	if (strcmp(getenv("nandEcc"), "4bit") == 0)
		params->u.mv_uboot.nand_ecc = 4;
	else if (strcmp(getenv("nandEcc"), "1bit") == 0)
		params->u.mv_uboot.nand_ecc = 1;

	boardId = mvBoardIdGet();
	params->u.mv_uboot.uboot_version |= boardId;
	params->u.mv_uboot.tclk = CONFIG_SYS_TCLK;
	params->u.mv_uboot.sysclk = CONFIG_SYS_BUS_CLK;

#if defined(MV_INCLUDE_USB)
	extern unsigned int mvCtrlUsbMaxGet(void);

	for (i = 0; i < mvCtrlUsbMaxGet(); i++) {
		sprintf(temp, "usb%dMode", i);
		env = getenv(temp);
		if ((!env) || (strcmp(env, "Host") == 0 ) || (strcmp(env, "host") == 0) )
			params->u.mv_uboot.isUsbHost |= (1 << i);
		else
			params->u.mv_uboot.isUsbHost &= ~(1 << i);
	}
#endif  /*#if defined(MV_INCLUDE_USB)*/
#if defined(MV_INCLUDE_GIG_ETH) || defined(MV_INCLUDE_UNM_ETH) || defined (CONFIG_MACH_AVANTA_LP_FPGA)
	extern int mvMacStrToHex(const char* macStr, unsigned char* macHex);

	for (i = 0; i < 4; i++) {
		memset(params->u.mv_uboot.macAddr[i], 0, sizeof(params->u.mv_uboot.macAddr[i]));
		params->u.mv_uboot.mtu[i] = 0;
	}

	for (i = 0; i < MV_UBOOT_MAX_PORTS; i++) {
		sprintf(temp, (i ? "eth%daddr" : "ethaddr"), i);

		env = getenv(temp);
		if (env)
			mvMacStrToHex(env, (unsigned char*)params->u.mv_uboot.macAddr[i]);

		sprintf(temp, (i ? "eth%dmtu" : "ethmtu"), i);
		env = getenv(temp);
		if (env)
			params->u.mv_uboot.mtu[i] = simple_strtoul(env, NULL, 10);
	}
#endif  /* (MV_INCLUDE_GIG_ETH) || defined(MV_INCLUDE_UNM_ETH) */

	/* Set Board modules configuration */
	modCfg = (u32) - 1;
	params->u.mv_uboot.board_module_config = modCfg;
	params = tag_next(params);
}

#endif

#ifdef CONFIG_SERIAL_TAG
void setup_serial_tag(struct tag **tmp)
{
	struct tag *params = *tmp;
	struct tag_serialnr serialnr;

	void get_board_serial(struct tag_serialnr *serialnr);

	get_board_serial(&serialnr);
	params->hdr.tag = ATAG_SERIAL;
	params->hdr.size = tag_size(tag_serialnr);
	params->u.serialnr.low = serialnr.low;
	params->u.serialnr.high = serialnr.high;
	params = tag_next(params);
	*tmp = params;
}

#endif

#ifdef CONFIG_REVISION_TAG
void setup_revision_tag(struct tag **in_params)
{
	u32 rev = 0;

	u32 get_board_rev(void);

	rev = get_board_rev();
	params->hdr.tag = ATAG_REVISION;
	params->hdr.size = tag_size(tag_revision);
	params->u.revision.rev = rev;
	params = tag_next(params);
}

#endif

#if defined(CONFIG_SETUP_MEMORY_TAGS) || \
	defined(CONFIG_CMDLINE_TAG) || \
	defined(CONFIG_INITRD_TAG) || \
	defined(CONFIG_SERIAL_TAG) || \
	defined(CONFIG_REVISION_TAG)
static void setup_end_tag(bd_t *bd)
{
	params->hdr.tag = ATAG_NONE;
	params->hdr.size = 0;
}

#endif

#ifdef CONFIG_OF_LIBFDT
static int create_fdt(bootm_headers_t *images)
{
	ulong of_size = images->ft_len;
	char **of_flat_tree = &images->ft_addr;
	ulong *initrd_start = &images->initrd_start;
	ulong *initrd_end = &images->initrd_end;
	struct lmb *lmb = &images->lmb;
	ulong rd_len;
	int ret, skip = 1;
	char *env;

	debug("using: FDT\n");

	boot_fdt_add_mem_rsv_regions(lmb, *of_flat_tree);

	rd_len = images->rd_end - images->rd_start;
	ret = boot_ramdisk_high(lmb, images->rd_start, rd_len,
				initrd_start, initrd_end);
	if (ret)
		return ret;

	ret = boot_relocate_fdt(lmb, of_flat_tree, &of_size);
	if (ret)
		return ret;

	fdt_chosen(*of_flat_tree, 1);

	env = getenv("fdt_skip_update");
	if (env && ((strncmp(env, "yes", 3) == 0)))
		printf("\n   Skipping Device Tree update ('fdt_skip_update' = yes)\n");
	else {
		skip = 0;
		printf("\n   Starting Device Tree update ('fdt_skip_update' = no)\n");
		fixup_memory_node(*of_flat_tree);
		fdt_fixup_ethernet(*of_flat_tree);
	}

	fdt_initrd(*of_flat_tree, *initrd_start, *initrd_end, 1);

#ifdef CONFIG_OF_BOARD_SETUP
	if (!skip)
		ft_board_setup(*of_flat_tree, gd->bd);
#endif

	return 0;
}

#endif

__weak void setup_board_tags(struct tag **in_params)
{
}

/* Subcommand: PREP */
static void boot_prep_linux(bootm_headers_t *images)
{
#ifdef CONFIG_CMDLINE_TAG
	char *commandline = getenv("bootargs");
#endif

#ifdef CONFIG_OF_LIBFDT
	if (images->ft_len) {
		debug("using: FDT\n");
		if (create_fdt(images)) {
			printf("FDT creation failed! hanging...");
			hang();
		}
	} else
#endif
	{
#if defined(CONFIG_SETUP_MEMORY_TAGS) || \
		defined(CONFIG_CMDLINE_TAG) || \
		defined(CONFIG_INITRD_TAG) || \
		defined(CONFIG_SERIAL_TAG) || \
		defined(CONFIG_REVISION_TAG)
		debug("using: ATAGS\n");
		setup_start_tag(gd->bd);
#ifdef CONFIG_SERIAL_TAG
		setup_serial_tag(&params);
#endif
#ifdef CONFIG_CMDLINE_TAG
		setup_commandline_tag(gd->bd, commandline);
#endif
#ifdef CONFIG_REVISION_TAG
		setup_revision_tag(&params);
#endif
#ifdef CONFIG_SETUP_MEMORY_TAGS
		setup_memory_tags(gd->bd);
#endif
#ifdef CONFIG_INITRD_TAG
		if (images->rd_start && images->rd_end)
			setup_initrd_tag(gd->bd, images->rd_start,
					 images->rd_end);
#endif
		setup_board_tags(&params);
#if defined (CONFIG_MARVELL_TAG)
		/* Linux open port doesn't support the Marvell TAG */
		char *env = getenv("mainlineLinux");
		if (!env || ((strcmp(env, "no") == 0) ||  (strcmp(env, "No") == 0)))
			setup_marvell_tag();
#endif
		setup_end_tag(gd->bd);
#else           /* all tags */
		printf("FDT and ATAGS support not compiled in - hanging\n");
		hang();
#endif          /* all tags */
	}
}

/* Subcommand: GO */
static void boot_jump_linux(bootm_headers_t *images)
{
	unsigned long machid = gd->bd->bi_arch_number;
	char *s;

	void (*kernel_entry)(int zero, int arch, uint params);
	unsigned long r2;

	kernel_entry = (void (*)(int, int, uint))images->ep;

	s = getenv("machid");
	if (s) {
		strict_strtoul(s, 16, &machid);
		printf("Using machid 0x%lx from environment\n", machid);
	}

	debug("## Transferring control to Linux (at address %08lx)" \
	      "...\n", (ulong)kernel_entry);
	bootstage_mark(BOOTSTAGE_ID_RUN_OS);

#ifdef CONFIG_AMP_SUPPORT
	if (amp_enable) {
		/* Boot AMP group. if boot completed (group_id > 0), return.*/
		if (amp_boot(machid, (int)kernel_entry, gd->bd->bi_boot_params))
			return;
	}
#endif

	announce_and_cleanup();

#ifdef CONFIG_AMP_SUPPORT
	/*
	 * Function amp_wait_to_boot() decreases amp_barrier. If the amp_barrier
	 * reach 0, other cores will start and may change internal register base.
	 * Internal registers (console) cannot be written after this call.
	 */
	if(amp_enable)
		amp_wait_to_boot();
#endif
#ifdef CONFIG_OF_LIBFDT
	if (images->ft_len)
		r2 = (unsigned long)images->ft_addr;
	else
#endif
	r2 = gd->bd->bi_boot_params;

	kernel_entry(0, machid, r2);
}

/* Main Entry point for arm bootm implementation
 *
 * Modeled after the powerpc implementation
 * DIFFERENCE: Instead of calling prep and go at the end
 * they are called if subcommand is equal 0.
 */
int do_bootm_linux(int flag, int argc, char *argv[], bootm_headers_t *images)
{
	/* No need for those on ARM */
	if (flag & BOOTM_STATE_OS_BD_T || flag & BOOTM_STATE_OS_CMDLINE)
		return -1;

#ifdef CONFIG_AMP_SUPPORT
	if (amp_enable)
		if (mv_amp_group_setup(amp_group_id, (int)images->ep))
			return 1; // in case of error stop boot flow

#endif

	if (flag & BOOTM_STATE_OS_PREP) {
		boot_prep_linux(images);
		return 0;
	}

	if (flag & BOOTM_STATE_OS_GO) {
		boot_jump_linux(images);
		return 0;
	}

	boot_prep_linux(images);
	boot_jump_linux(images);
	return 0;
}

#ifdef CONFIG_CMD_BOOTZ

struct zimage_header {
	uint32_t code[9];
	uint32_t zi_magic;
	uint32_t zi_start;
	uint32_t zi_end;
};

#define LINUX_ARM_ZIMAGE_MAGIC  0x016f2818

int bootz_setup(void *image, void **start, void **end)
{
	struct zimage_header *zi = (struct zimage_header*)image;

	if (zi->zi_magic != LINUX_ARM_ZIMAGE_MAGIC) {
		puts("Bad Linux ARM zImage magic!\n");
		return 1;
	}

	*start = (void*)zi->zi_start;
	*end = (void*)zi->zi_end;

	debug("Kernel image @ 0x%08x [ 0x%08x - 0x%08x ]\n",
	      (uint32_t)image, (uint32_t)*start, (uint32_t)*end);

	return 0;
}

#endif  /* CONFIG_CMD_BOOTZ */
