// SPDX-License-Identifier:    GPL-2.0
/*
 * Copyright (C) 2020 Marvell International Ltd.
 *
 * https://spdx.org/licenses
 */

#include <console.h>
#include <dm.h>
#include <log.h>
#include <linux/delay.h>
#include <dm/uclass-internal.h>
#include <malloc.h>
#include <errno.h>
#include <asm/io.h>
#include <linux/compiler.h>
#include <asm/arch/smc.h>
#include <asm/arch/soc.h>
#include <asm/arch/board.h>
#include <dm/util.h>

DECLARE_GLOBAL_DATA_PTR;

#define	SUPER_FW_RAM_ADDR	0x10040000
#define	CM3_FW_RAM_ADDR		0x10100000

/* Switch boot progress codes */
#define SW_BOOT_INIT_IN_PROGRESS	1
#define SW_BOOT_INIT_DONE		2
#define SW_BOOT_INIT_FAILED		3

#define SW_MAGIC_WORD		0x5a5b5c5d
#define SW_MAGWRD_OFFSET	0xBFFFCULL
#define SW_MAGWRD2_OFFSET	0xBFFF8ULL

void *second_magic_word_loc(void *bar2)
{
	return (bar2 + readl(bar2 + SW_MAGWRD2_OFFSET));
}

#define SWITCH_FIRST_MAGIC_WORD_LOC(x)	(void *)((x) + SW_MAGWRD_OFFSET)
#define MAILBOX_POINTER(x)		second_magic_word_loc(x)
#define CONTROL_WORD_POINTER(x)		MAILBOX_POINTER(x) - 256
#define OPCODE_WORD_POINTER(x)		MAILBOX_POINTER(x) - 252
#define COMMAND_DATA_POINTER(x)		MAILBOX_POINTER(x) - 248
#define BOOT_STATUS_POINTER(x)		MAILBOX_POINTER(x) + 4
#define BOOT_ERROR_POINTER(x)		MAILBOX_POINTER(x) + 8
#define GENERAL_ERROR_POINTER(x)	MAILBOX_POINTER(x) + 12

/* Error Codes */
#define INVALID_CFG_ID			0x10
#define CFG_FILE_NOT_FOUND		0x11

struct boot_chan {
	u32 control_word;
	u16 opcode_word;
	u16 data[125];
};

void *sw_bar2;

void copy32(void *source, void *dest, u32 count)
{
	u32 *src = source, *dst = dest;

	count >>= 2;
	while (count--) {
		writel(readl(src++), dst++);
	};
}

int load_switch_images(u64 *cm3_size)
{
	return smc_load_switch_fw(SUPER_FW_RAM_ADDR, CM3_FW_RAM_ADDR,
				  cm3_size);
}

struct udevice *get_switch_dev(void)
{
	struct udevice *bus, *dev;
	struct uclass *uc;

	if (!fdt_get_switch_config()) {
		printf("\nSkip Switch micro-init option is set\n");
		return NULL;
	}
	uclass_id_foreach_dev(UCLASS_PCI, bus, uc) {
		device_foreach_child(dev, bus) {
			struct pci_child_platdata *pplat;

			pplat = dev_get_parent_platdata(dev);
			if (pplat && pplat->vendor == PCI_VENDOR_ID_MARVELL) {
				if (0x21 == ((pplat->device >> 8) & 0xFF))
					return dev;
			}
		}
	}
	return NULL;
}

int create_cmd(struct boot_chan *cmd, u16 opcode, int argc, ...)
{
	va_list valist;
	int i;
	u32 tmp;

	va_start(valist, argc);

	memset(cmd, 0, sizeof(struct boot_chan));
	cmd->opcode_word = opcode;
	for (i = 0; i < argc * 2; i += 2) {
		tmp = va_arg(valist, u32);
		cmd->data[i] = (tmp & 0xFFFF);
		cmd->data[i + 1] = (tmp >> 16) & 0xFFFF;
	}

	va_end(valist);
	return 0;
}

int read_virtual_reg_offset(u32 offset, u32 *reg_val)
{
	*reg_val = 0;

	if (offset > 19)
		return -1;

	*reg_val = readl(MAILBOX_POINTER(sw_bar2) + offset);

	return 0;
}

int send_bootch_cmd(struct boot_chan *cmd, u8 num_bytes)
{
	u32 mailbox_offset;
	u32 reg_val;
	int timeout;

	if (!cmd) {
		debug("%s ERR: CMD NULL\n", __func__);
		return -1;
	}

	/* Check Boot INIT_DONE */
	mailbox_offset = readl(MAILBOX_POINTER(sw_bar2));
	reg_val = readl(BOOT_STATUS_POINTER(sw_bar2));
	if (reg_val != SW_BOOT_INIT_DONE)
		return -1;

	/* Wait for OWN bit to clear in control word */
	timeout = 100;
	reg_val = readl(CONTROL_WORD_POINTER(sw_bar2));
	while (reg_val & (1 << 31)) {
		if (--timeout < 0)
			break;
		mdelay(1);
		reg_val = readl(CONTROL_WORD_POINTER(sw_bar2));
	}

	if (timeout < 0) {
		debug("%s ERR: OWN bit won't clear\n", __func__);
		return -1;
	}

	/* Write the command */
	copy32((void *)&cmd->opcode_word, OPCODE_WORD_POINTER(sw_bar2), num_bytes);
	debug("0x%08x --> %p\n", cmd->control_word, CONTROL_WORD_POINTER(sw_bar2));
	writel(cmd->control_word, CONTROL_WORD_POINTER(sw_bar2));
	return 0;
}

int check_bootch_cmd_status(u8 *return_code)
{
	u32 mailbox_offset;
	u32 reg_val;
	int timeout;

	/* Check Boot INIT_DONE */
	mailbox_offset = readl(MAILBOX_POINTER(sw_bar2));
	reg_val = readl(BOOT_STATUS_POINTER(sw_bar2));
	if (reg_val != SW_BOOT_INIT_DONE)
		return -1;

	/* Wait for OWN bit to clear */
	timeout = 100;
	reg_val = readl(CONTROL_WORD_POINTER(sw_bar2));
	while (reg_val & (1 << 31)) {
		if (--timeout < 0)
			break;
		mdelay(1);
		reg_val = readl(CONTROL_WORD_POINTER(sw_bar2));
	}

	if (timeout < 0) {
		debug("%s ERR: OWN bit won't clear\n", __func__);
		return -1;
	}

	/* Read control word for status */
	*return_code = readl(CONTROL_WORD_POINTER(sw_bar2)) & 0xFF;

	return 0;
}

int get_cmd_response(u8 num_bytes, void *buffer)
{
	copy32(OPCODE_WORD_POINTER(sw_bar2), buffer, num_bytes);

	return 0;
}

u8 switch_cmd_opcode4(u32 profile_num)
{
	struct boot_chan cmd;
	u8 status = 0;

	/* Prepare command */
	/* 2 bytes op-code + 4 bytes bit number + 2 bytes pad */
	create_cmd(&cmd, 4, 1, profile_num);
	cmd.control_word = (1 << 31) | (8 << 18);

	if (!get_switch_dev()) {
		debug("%s: Switch Device NOT Detected\n", __func__);
		return status;
	}

	/* Send command */
	if (send_bootch_cmd(&cmd, 8))
		debug("%s %d ERR: Opcode 4 send error\n", __func__, __LINE__);

	/* Check response */
	if (check_bootch_cmd_status(&status))
		debug("%s %d ERR: Opcode 4 response error[%d]\n",
		      __func__, __LINE__, status);

	return status;
}

int switch_cmd_opcode5(char *buffer)
{
	struct boot_chan cmd;
	u8 status = 0;

	/* Prepare command */
	/* 2 bytes op-code + 2 bytes pad */
	create_cmd(&cmd, 5, 0);
	cmd.control_word = (1 << 31) | (0x2c << 18);

	/* Send command */
	if (send_bootch_cmd(&cmd, 4)) {
		debug("%s %d ERR: Opcode 5 send error\n", __func__, __LINE__);
		return -1;
	}

	/* Check response */
	if (check_bootch_cmd_status(&status)) {
		debug("%s %d ERR: Opcode 5 response error [%d]\n", __func__, __LINE__, status);
		return -1;
	}

	/* Four version strings, each 32bytes long */
	get_cmd_response(32 * 4, (void *)buffer);
	return 0;
}

int switch_cmd_opcode6(u32 dev_num, u32 interface_num, u32 *status)
{
	struct boot_chan cmd;
	u8 resp = 0;

	/* Prepare command */
	/* 2 bytes op-code + 4 bytes dev num + 4 bytes interface num + 2 bytes pad
	 */
	create_cmd(&cmd, 6, 2, dev_num, interface_num);
	cmd.control_word = (1 << 31) | (12 << 18);

	/* Send command */
	if (send_bootch_cmd(&cmd, 12)) {
		debug("%s %d ERR: Opcode 6 send error\n", __func__, __LINE__);
		return -1;
	}

	/* Check response */
	if (check_bootch_cmd_status(&resp)) {
		debug("%s %d ERR: Opcode 6 response error [%d]\n", __func__, __LINE__, resp);
		return -1;
	}

	/* 16-bit status */
	get_cmd_response(4, status);
	return 0;
}

int switch_cmd_opcode7(u32 group_num, u32 id, char *buffer)
{
	struct boot_chan cmd;
	u8 status = 0;

	/* Prepare command */
	/* 2 bytes op-code + 4 bytes bit-map + 4 bytes group num + 2 bytes pad
	 */
	create_cmd(&cmd, 7, 2, group_num, id);
	cmd.control_word = (1 << 31) | (12 << 18);

	/* Send command */
	if (send_bootch_cmd(&cmd, 12)) {
		debug("%s %d ERR: Opcode 7 send error\n", __func__, __LINE__);
		return -1;
	}

	/* Check response */
	if (check_bootch_cmd_status(&status)) {
		debug("%s %d ERR: Opcode 7 response error [%d]\n", __func__, __LINE__, status);
		return -1;
	}

	/* string buffer  */
	get_cmd_response(252, buffer);
	return 0;
}

int switch_cmd_opcode10(u32 dev_num, u32 interface_num, u32 speed, u32 mode, u32 fec)
{
	struct boot_chan cmd;
	u8 status = 0;

	/* Prepare command */
	/* 2 bytes op-code + 4 bytes dev-num + 4 bytes interface num
	 * + 4 bytes speed + 4 bytes mode + 4 bytes FEC + 2 bytes pad
	 */
	create_cmd(&cmd, 10, 5, dev_num, interface_num, speed, mode, fec);
	cmd.control_word = (1 << 31) | (24 << 18);

	/* Send command */
	if (send_bootch_cmd(&cmd, 24)) {
		debug("%s %d ERR: Opcode 10 send error\n", __func__, __LINE__);
		return -1;
	}

	/* Check response */
	if (check_bootch_cmd_status(&status)) {
		debug("%s %d ERR: Opcode 10 response error [%d]\n", __func__, __LINE__, status);
		return -1;
	}

	return 0;
}

int switch_cmd_opcode11(u32 dev_num, u32 interface_num, u32 lane_num, u32 prbs_mode, u32 enable)
{
	struct boot_chan cmd;
	u8 status = 0;

	/* Prepare command */
	/* 2 bytes op-code + 4 bytes dev-num + 4 bytes interface num + 4 bytes lane number
	 * + 4 bytes prbs mode + 4 bytes enable/disable flag  + 2 bytes pad
	 */
	create_cmd(&cmd, 11, 5, dev_num, interface_num, lane_num, prbs_mode, enable);
	cmd.control_word = (1 << 31) | (24 << 18);

	/* Send command */
	if (send_bootch_cmd(&cmd, 24)) {
		debug("%s %d ERR: Opcode 11 send error\n", __func__, __LINE__);
		return -1;
	}

	/* Check response */
	if (check_bootch_cmd_status(&status)) {
		debug("%s %d ERR: Opcode 11 response error [%d]\n", __func__, __LINE__, status);
		return -1;
	}

	return 0;
}

int switch_cmd_opcode12(u32 dev_num, u32 interface_num, u32 lane_num, u32 clear_count, u32 *count)
{
	struct boot_chan cmd;
	u8 status = 0;

	/* Prepare command */
	/* 2 bytes op-code + 4 bytes dev num + 4 bytes interface num + 4 bytes lane number
	 * + 4 bytes clear/accumulate flag  + 2 bytes pad
	 */
	create_cmd(&cmd, 12, 4, dev_num, interface_num, lane_num, clear_count);
	cmd.control_word = (1 << 31) | (20 << 18);

	/* Send command */
	if (send_bootch_cmd(&cmd, 20)) {
		debug("%s %d ERR: Opcode 12 send error\n", __func__, __LINE__);
		return -1;
	}

	/* Check response */
	if (check_bootch_cmd_status(&status)) {
		debug("%s %d ERR: Opcode 12 response error [%d]\n", __func__, __LINE__, status);
		return -1;
	}

	/* 32-bit PRBS count */
	get_cmd_response(4, count);
	return 0;
}

int switch_cmd_opcode13(u32 dev_num, u32 interface_num, u32 amplitude, u32 tx_amp_adj,
			int emph_0, int emph_1, u32 amp_shift)
{
	struct boot_chan cmd;
	u8 status = 0;

	/* Prepare command */
	/* 2 bytes op-code + 4 bytes dev num + 4 bytes interface num + 4 bytes amplitude
	 * + 4 bytes tx amplitude adjust + 4 bytes emphasis 0 (-7 to 15)
	 * + 4 bytes emphasis 1 (-31 to 31)   + 4 bytes amplitude shift + 2 bytes pad
	 */
	create_cmd(&cmd, 13, 7, dev_num, interface_num, amplitude, tx_amp_adj,
		   emph_0, emph_1, amp_shift);
	cmd.control_word = (1 << 31) | (32 << 18);

	/* Send command */
	if (send_bootch_cmd(&cmd, 32)) {
		debug("%s %d ERR: Opcode 13 send error\n", __func__, __LINE__);
		return -1;
	}

	/* Check response */
	if (check_bootch_cmd_status(&status)) {
		debug("%s %d ERR: Opcode 13 response error [%d]\n", __func__, __LINE__, status);
		return -1;
	}

	return 0;
}

int switch_cmd_opcode14(u32 dev_num, u32 interface_num, u32 lane_num, u32 dc_gain,
			u32 lf_gain, u32 hf_gain, u32 ctle_bw, u32 ctle_loop_bw, u32 sq_thresh)
{
	struct boot_chan cmd;
	u8 status = 0;

	/* Prepare command */
	/* 2 bytes op-code + 4 bytes dev num + 4 bytes interface num + 4 bytes lane num
	 * + 4 bytes DC gain (0 to 15) + 4 bytes low freq gain (0 to 15)
	 * + 4 bytes high freq gain (0 to 15)   + 4 bytes CTLE bandwidth (0 to 15)
	 * + 4 bytes CTLE loop bandwidth (0 to 15) + 4 bytes squelch detector threshold (0 to 310)
	 * + 2 bytes pad
	 */
	create_cmd(&cmd, 14, 9, dev_num, interface_num, lane_num, dc_gain,
		   lf_gain, hf_gain, ctle_bw, ctle_loop_bw, sq_thresh);
	cmd.control_word = (1 << 31) | (40 << 18);

	/* Send command */
	if (send_bootch_cmd(&cmd, 40)) {
		debug("%s %d ERR: Opcode 14 send error\n", __func__, __LINE__);
		return -1;
	}

	/* Check response */
	if (check_bootch_cmd_status(&status)) {
		debug("%s %d ERR: Opcode 14 response error [%d]\n", __func__, __LINE__, status);
		return -1;
	}

	return 0;
}

int switch_cmd_opcode15(u32 dev_num, u32 interface_num, u32 lane_num, u32 fc_pause,
			u32 fc_am_dir, u32 fec_supp, u32 fec_req, u32 mode, u32 speed)
{
	struct boot_chan cmd;
	u8 status = 0;

	/* Prepare command */
	/* 2 bytes op-code + 4 bytes dev num + 4 bytes interface num + 4 bytes lane num
	 * + 4 bytes FC pause + 4 bytes FC amplitude direction
	 * + 4 bytes FEC supported (0 = OFF, 1 = FC, 2 = RS, 3 = BOTH)
	 * + 4 bytes FEC required (0 = OFF, 1 = FC, 2 = RS, 3 = BOTH))
	 * + 4 bytes port mode + 4 bytes port speed
	 * + 2 bytes pad
	 */
	create_cmd(&cmd, 15, 9, dev_num, interface_num, lane_num, fc_pause, fc_am_dir,
		   fec_supp, fec_req, mode, speed);
	cmd.control_word = (1 << 31) | (40 << 18);

	/* Send command */
	if (send_bootch_cmd(&cmd, 40)) {
		debug("%s %d ERR: Opcode 15 send error\n", __func__, __LINE__);
		return -1;
	}

	/* Check response */
	if (check_bootch_cmd_status(&status)) {
		debug("%s %d ERR: Opcode 15 response error [%d]\n", __func__, __LINE__, status);
		return -1;
	}

	return 0;
}

int switch_cmd_opcode17(u32 dev_num, u32 interface_num, u32 lane_num, u32 speed,
			u32 sq_thresh, u32 lf_gain, u32 hf_gain, u32 dc_gain,
			u32 ctle_bw, u32 ctle_loop_bw, u32 etl_min_delay,
			u32 etl_max_delay, u32 etl_enable, u32 override_bitmap)
{
	struct boot_chan cmd;
	u8 status = 0;

	/* Prepare command */
	/* 2 bytes op-code + 4 bytes dev num + 4 bytes interface num + 4 bytes lane num
	 * + 4 bytes speed + 4 bytes squelch threshold (0 to 310)
	 * + 4 bytes low freq gain (0 to 15) + 4 bytes high freq gain (0 to 15)
	 * + 4 bytes DC gain (0 to 15) + 4 bytes CTLE bandwidth (0 to 15)
	 * + 4 bytes CTLE loop bandwidth (0 to 15) + 4 bytes ETL min delay (0 to 31)
	 * + 4 bytes ETL max delay (0 to 31) + 4 bytes ETL enable
	 * + 4 bytes field override bitmap + 2 bytes pad
	 */
	create_cmd(&cmd, 17, 14, dev_num, interface_num, lane_num, speed, sq_thresh,
		   lf_gain, hf_gain, dc_gain, ctle_bw, ctle_loop_bw, etl_min_delay,
		   etl_max_delay, etl_enable, override_bitmap);
	cmd.control_word = (1 << 31) | (60 << 18);

	/* Send command */
	if (send_bootch_cmd(&cmd, 60)) {
		debug("%s %d ERR: Opcode 17 send error\n", __func__, __LINE__);
		return -1;
	}

	/* Check response */
	if (check_bootch_cmd_status(&status)) {
		debug("%s %d ERR: Opcode 17 response error [%d]\n", __func__, __LINE__, status);
		return -1;
	}

	return 0;
}

int switch_cmd_opcode18(u32 dev_num, u32 interface_num, u32 lane_num, u32 speed,
			int tx_amp_offset, int emph0_offset, int emph1_offset)
{
	struct boot_chan cmd;
	u8 status = 0;

	/* Prepare command */
	/* 2 bytes op-code + 4 bytes dev num + 4 bytes interface num + 4 bytes speed
	 * + 4 bytes tx amplitude offset (-15 to 15 in steps of 2)
	 * + 4 bytes emphasis 0 offset (-15 to 15 in steps of 2)
	 * + 4 bytes emphasis 1 offset (-15 to 15 in steps of 2)   + 2 bytes pad
	 */
	create_cmd(&cmd, 18, 6, dev_num, interface_num, lane_num, speed, tx_amp_offset,
		   emph0_offset, emph1_offset);
	cmd.control_word = (1 << 31) | (28 << 18);

	/* Send command */
	if (send_bootch_cmd(&cmd, 28)) {
		debug("%s %d ERR: Opcode 18 send error\n", __func__, __LINE__);
		return -1;
	}

	/* Check response */
	if (check_bootch_cmd_status(&status)) {
		debug("%s %d ERR: Opcode 18 response error [%d]\n", __func__, __LINE__, status);
		return -1;
	}

	return 0;
}

int switch_cmd_opcode19(u32 dev_num, u32 intf_num, u32 lpbk_mode)
{
	struct boot_chan cmd;
	u8 status = 0;

	/* Prepare command */
	/* 2 bytes op-code + 4 bytes dev num + 4 bytes interface num
	 * + 4 bytes loopback mode (0 = no lpbk, 1 = SERDES tx2rx, 2 = SERDES rx2tx, 3 = MAC tx2rx)
	 * + 2 bytes pad
	 */
	create_cmd(&cmd, 19, 3, dev_num, intf_num, lpbk_mode);
	cmd.control_word = (1 << 31) | (16 << 18);

	/* Send command */
	if (send_bootch_cmd(&cmd, 16)) {
		debug("%s %d ERR: Opcode 19 send error\n", __func__, __LINE__);
		return -1;
	}

	/* Check response */
	if (check_bootch_cmd_status(&status)) {
		debug("%s %d ERR: Opcode 19 response error [%d]\n", __func__, __LINE__, status);
		return -1;
	}

	return 0;
}

int switch_cmd_opcode20(u32 dev_num, u32 intf_num, u32 nego_mode)
{
	struct boot_chan cmd;
	u8 status = 0;

	/* Prepare command */
	/* 2 bytes op-code + 4 bytes dev num + 4 bytes interface num
	 * + 4 bytes negotiation mode (10h = 10Mbps half duplex, 10f = 10Mbps full dupex,
	 * 100h = 100Mbps half, 100f= 100Mbps full, 1000f = 1Gbps full, 10000f = 10Gbps full)
	 * + 2 bytes pad
	 */
	create_cmd(&cmd, 20, 3, dev_num, intf_num, nego_mode);
	cmd.control_word = (1 << 31) | (16 << 18);

	/* Send command */
	if (send_bootch_cmd(&cmd, 16)) {
		debug("%s %d ERR: Opcode 20 send error\n", __func__, __LINE__);
		return -1;
	}

	/* Check response */
	if (check_bootch_cmd_status(&status)) {
		debug("%s %d ERR: Opcode 20 response error [%d]\n", __func__, __LINE__, status);
		return -1;
	}

	return 0;
}

int switch_cmd_opcode21(u32 dev_num, u32 intf_num, u32 lane_num, u32 *buffer)
{
	struct boot_chan cmd;
	u8 status = 0;

	/* Prepare command */
	/* 2 bytes op-code + 4 bytes dev num + 4 bytes interface num
	 * + 4 bytes lane num + 2 bytes pad
	 */
	create_cmd(&cmd, 21, 3, dev_num, intf_num, lane_num);
	cmd.control_word = (1 << 31) | (16 << 18);

	/* Send command */
	if (send_bootch_cmd(&cmd, 16)) {
		debug("%s %d ERR: Opcode 21 send error\n", __func__, __LINE__);
		return -1;
	}

	/* Check response */
	if (check_bootch_cmd_status(&status)) {
		debug("%s %d ERR: Opcode 21 response error [%d]\n", __func__, __LINE__, status);
		return -1;
	}

	/* SERDES TX Parameters */
	get_cmd_response(20, buffer);

	return 0;
}

int switch_cmd_opcode22(u32 dev_num, u32 intf_num, u32 lane_num, u32 *buffer)
{
	struct boot_chan cmd;
	u8 status = 0;

	/* Prepare command */
	/* 2 bytes op-code + 4 bytes dev num + 4 bytes interface num
	 * + 4 bytes lane num + 2 bytes pad
	 */
	create_cmd(&cmd, 22, 3, dev_num, intf_num, lane_num);
	cmd.control_word = (1 << 31) | (16 << 18);

	/* Send command */
	if (send_bootch_cmd(&cmd, 16)) {
		debug("%s %d ERR: Opcode 22 send error\n", __func__, __LINE__);
		return -1;
	}

	/* Check response */
	if (check_bootch_cmd_status(&status)) {
		debug("%s %d ERR: Opcode 22 response error [%d]\n", __func__, __LINE__, status);
		return -1;
	}

	/* SERDES RX Parameters */
	get_cmd_response(24, buffer);

	return 0;
}

void board_switch_reset(void)
{
	struct udevice *dev;
	void *sw_bar0, *sw_bar2;
	u32 sw_bar2_lo, sw_bar2_hi;
	unsigned long val;
	char buffer[128];
	char temp[4];
	int ret;

	dev = get_switch_dev();
	if (!dev)
		return;

	sw_bar0 = dm_pci_map_bar(dev, PCI_BASE_ADDRESS_0, PCI_REGION_MEM);
	if (!sw_bar0) {
		debug("Switch device BAR not configured\n");
		return;
	}
	sw_bar2 = dm_pci_map_bar(dev, PCI_BASE_ADDRESS_2, PCI_REGION_MEM);
	if (!sw_bar2) {
		debug("Switch device BAR not configured\n");
		return;
	}
	sw_bar2_lo = (u32)((ulong)sw_bar2 & 0xffffffff);
	sw_bar2_hi = (u32)(((ulong)sw_bar2 >> 32) & 0xffffffff);

	/* Check version for Hitless start-up */
	memset(buffer, 0, sizeof(buffer));
	ret = switch_cmd_opcode5(buffer);
	if (ret) {
		pr_debug("%s %d : version cmd error [0x%x]\n",
			 __func__, __LINE__, ret);
		printf("Skipping switch soft-reset as version cmd fail\n");
		return;
	}
	temp[0] = buffer[1];
	temp[1] = buffer[3];
	temp[2] = buffer[4];
	temp[3] = '\0';
	ret = strict_strtoul(temp, 10, &val);
	/* Above V1.10 support Hitless start-up so do not reset */
	if (val > 110)
		return;

	/*
	 * Open 1M iATU address translation window into Prestera DFX-server,
	 * at ‘second window’ of BAR2 , for addresses with prefix 0x000xxxxx
	 * (DFX-server)
	 * mw 0x872000001500 0x00000000; ATU ctrl reg 1
	 * mw 0x872000001504 0x80000000; ATU ctrl reg 2 (bit 31: enable window)
	 * mw 0x872000001508 0x00900000; ATU lower base address (start of BAR2)
	 * mw 0x87200000150c 0x00008720; ATU upper base address
	 * mw 0x872000001510 0x009fffff; ATU limit address (limit to 1MB)
	 * mw 0x872000001514 0x00000000; ATU target lower base address
					 (start of DFX-server)
	 * mw 0x872000001518 0x00000000; ATU target upper base address
	 */
	writel(0x0, sw_bar0 + 0x1500);
	writel(0x80000000, sw_bar0 + 0x1504);
	writel(sw_bar2_lo + 0x100000, sw_bar0 + 0x1508);
	writel(sw_bar2_hi, sw_bar0 + 0x150c);
	writel(sw_bar2_lo + 0x1fffff, sw_bar0 + 0x1510);
	writel(0x0, sw_bar0 + 0x1514);
	writel(0x0, sw_bar0 + 0x1518);

	/*
	 * Open 1M iATU address translation window into Prestera CNM-RUNIT,
	 * at ‘third window’ of BAR2 , for addresses with prefix 0x3c0xxxxx
	 * (CNM-RUNIT) :
	 * mw 0x872000001700 0x00000000; ATU ctrl reg 1
	 * mw 0x872000001704 0x80000000; ATU ctrl reg 2 (bit 31: enable window)
	 * mw 0x872000001708 0x00a00000; ATU lower base address (start of BAR2)
	 * mw 0x87200000170c 0x00008720; ATU upper base address
	 * mw 0x872000001710 0x00afffff; ATU limit address (limit to 1MB)
	 * mw 0x872000001714 0x3c000000; ATU target lower base address
				 (start of CNM-RUNIT)
	 * mw 0x872000001718 0x00000000; ATU target upper base address
	 */
	writel(0x0, sw_bar0 + 0x1700);
	writel(0x80000000, sw_bar0 + 0x1704);
	writel(sw_bar2_lo + 0x200000, sw_bar0 + 0x1708);
	writel(sw_bar2_hi, sw_bar0 + 0x170c);
	writel(sw_bar2_lo + 0x2fffff, sw_bar0 + 0x1710);
	writel(0x3c000000, sw_bar0 + 0x1714);
	writel(0x0, sw_bar0 + 0x1718);

	/*
	 * Write_mask 0x8720009F806C DATA 0x00000000 MASK 0x00000002
	 * Write_mask 0x8720009F800C DATA 0x00000000 MASK 0x00001010
	 * Write_mask 0x872000a00044 DATA 0x00000001 MASK 0x00000001
	 * Write_mask 0x872000a00044 DATA 0x00000000 MASK 0x00000001
	 */
	val = readl(sw_bar2 + 0x100000 + 0xf806c);
	val &= ~0x00000002;
	val |= 0x0;
	writel(val, sw_bar2 + 0x100000 + 0xf806c);
	val = readl(sw_bar2 + 0x100000 + 0xf800c);
	val &= ~0x00001010;
	val |= 0x0;
	writel(val, sw_bar2 + 0x100000 + 0xf800c);
	val = readl(sw_bar2 + 0x200000 + 0x44);
	val &= ~0x00000001;
	val |= 0x1;
	writel(val, sw_bar2 + 0x200000 + 0x44);
	val = readl(sw_bar2 + 0x200000 + 0x44);
	val &= ~0x00000001;
	val |= 0x0;
	writel(val, sw_bar2 + 0x200000 + 0x44);
	mdelay(5);
}

void board_switch_init(void)
{
	struct udevice *dev;
	struct pci_child_platdata *pplat;
	void *sw_bar0/*, *sw_bar2*/;
	u32 sw_bar2_lo, sw_bar2_hi;
	u64 cm3_img_sz;
	int timeout;
	u64 mailbox_offset;

	dev = get_switch_dev();
	if (!dev)
		return;

	sw_bar0 = dm_pci_map_bar(dev, PCI_BASE_ADDRESS_0, PCI_REGION_MEM);
	if (!sw_bar0) {
		debug("Switch device BAR not configured\n");
		return;
	}
	sw_bar2 = dm_pci_map_bar(dev, PCI_BASE_ADDRESS_2, PCI_REGION_MEM);
	if (!sw_bar2) {
		debug("Switch device BAR not configured\n");
		return;
	}
	sw_bar2_lo = (u32)((ulong)sw_bar2 & 0xffffffff);
	sw_bar2_hi = (u32)(((ulong)sw_bar2 >> 32) & 0xffffffff);

	pplat = dev_get_parent_platdata(dev);
	printf("Switch device [%x:%x] detected\n", pplat->vendor, pplat->device);
	debug("BAR0 %p BAR2 %p\n", sw_bar0, sw_bar2);
	printf("Executing micro-init sequence... ");
	/*
	 * Open 1M iATU address translation window into Prestera MG0, at start of BAR2:
	 *	wr bar0 + 1300 0x00000000;	ATU ctrl reg 1
	 *	wr bar0 + 1304 0x80000000; 	ATU ctrl reg 2 (bit 31: enable window)
	 *	wr bar0 + 1308 0x00800000; 	ATU lower base address (start of BAR2)
	 *	wr bar0 + 130c 0x00008720;	ATU upper base address
	 *	wr bar0 + 1310 0x008fffff; 	ATU limit address (limit to 1MB)
	 * 	wr bar0 + 1314 0x3c200000; 	ATU target lower base address (start of MG0)
	 * 	wr bar0 + 1318 0x00000000; 			ATU target upper base address

	 * In MG0 confi processor window settings, configure window #1 (Address decoding for CM3):
	 *	wr bar2 + 0490 0x10000803; 	Base address for CM3 address space
	 * 	wr bar2 + 0494 0x00ff0000; 	Size 16MB
	 * 	wr bar2 + 0498 0x00000000;
	 *	wr bar2 + 049c 0xf200000e; 	Target address 0xf2000000 (address > 2G will go out to PCIe)

	 * Open 1M oATU address translation window out to host CPU DDR with target offset 0xabcd03000000:
	 *	wr bar0 + 1200 0x00000000; 	oATU ctrl reg 1
	 * 	wr bar0 + 1204 0x80000000; 	oATU ctrl reg 2 (bit 31: enable window)
	 *	wr bar0 + 1208 0xf2000000; 	oATU lower base address (start of 'DDR' in terms internal CM3 memory space)
	 * 	wr bar0 + 120c 0x00000000; 	oATU upper base address
	 * 	wr bar0 + 1210 0xf23fffff; 	oATU limit address (limit to 4MB)
	 *	wr bar0 + 1214 0x10000000; 	oATU target lower base address (start of DDR on PCIe)
	 *	wr bar0 + 1218 0x00000000; 	oATU target upper base address (start of DDR on PCIe)

	 * MG0 completion registers:
	 * 	wr bar2 + 0124 0x0001;		MG0 window mapping for proper CM3 access to the switch (register0)
	 * 	wr bar2 + 0140 0x8102;		MG0 window mapping for proper CM3 access to the switch (register1)

	 * Load super-image to 0x10040000: (super-image must reside 0x40000 after base offset)
	 * Will be loaded directly from SPI flash or tftpboot 0x10040000 super_image.bin;

	 * Load MI FW to 0x10100000 by tftp, then copy to SRAM:
	 * Will be loaded directly from SPI flash or tftpboot 0x10200000 firmware_cm3.bin;

	 * cp.l 0x10100000 bar2 + 0x80000 $filesize; 	SRAM based at MG0 + 0x80000 (32bit at a time)

	 * MG0 Register 0x500 (Confi Processor Global Configuration) – disable and then enable CM3 CPU (bits 28:29):
	 * wr bar2 + 0500 0x000e1a80;		CM3 CPU (bits 28:29) disable.
	 * wr bar2 + 0500 0x300e1a80;		CM3 CPU (bits 28:29) enable.
	 */
	writel(0x0, sw_bar0 + 0x1300);
	writel(0x80000000, sw_bar0 + 0x1304);
	writel(sw_bar2_lo, sw_bar0 + 0x1308);
	writel(sw_bar2_hi, sw_bar0 + 0x130c);
	writel(sw_bar2_lo + 0xfffff, sw_bar0 + 0x1310);
	writel(0x3c200000, sw_bar0 + 0x1314);
	writel(0x0, sw_bar0 + 0x1318);

	writel(0x10000803, sw_bar2 + 0x0490);
	writel(0x00ff0000, sw_bar2 + 0x0494);
	writel(0x0, sw_bar2 + 0x0498);
	writel(0xf200000e, sw_bar2 + 0x049c);

	writel(0x0, sw_bar0 + 0x1200);
	writel(0x80000000, sw_bar0 + 0x1204);
	writel(0xf2000000, sw_bar0 + 0x1208);
	writel(0x0, sw_bar0 + 0x120c);
	writel(0xf23fffff, sw_bar0 + 0x1210);
	writel(SUPER_FW_RAM_ADDR - 0x40000, sw_bar0 + 0x1214);
	writel(0x0, sw_bar0 + 0x1218);

	writel(0x0001, sw_bar2 + 0x0124);
	writel(0x8102, sw_bar2 + 0x0140);

	if (load_switch_images(&cm3_img_sz)) {
		printf("Loading switch firmware to RAM failed\n");
		return;
	}

	copy32((void *)CM3_FW_RAM_ADDR, sw_bar2 + 0x80000, cm3_img_sz);

	writel(0x000e1a80, sw_bar2 + 0x0500);
	writel(0x300e1a80, sw_bar2 + 0x0500);

	debug("done.%p %p\n", sw_bar0, sw_bar2);
	mdelay(5000);

	/* Check for successful initialization of switch firmware */
	/* Check first magic word at fixed location */
	timeout = 10;
	while (readl(sw_bar2 + SW_MAGWRD_OFFSET) != SW_MAGIC_WORD) {
		mdelay(1);
		if (--timeout < 0)
			break;
	}

	if (readl(sw_bar2 + SW_MAGWRD_OFFSET) != SW_MAGIC_WORD) {
		printf("%s ERROR: Switch not init![0x%x]\n", __func__,
		       readl(sw_bar2 + SW_MAGWRD_OFFSET));
		return;
	}

	/* Check boot status */
	timeout = 10;
	mailbox_offset = readl(sw_bar2 + SW_MAGWRD2_OFFSET);
	debug("%s Mailbox Offset:0x%llx\n", __func__, mailbox_offset);
	while (readl(sw_bar2 + mailbox_offset + 4) != SW_BOOT_INIT_DONE) {
		mdelay(1);
		if (--timeout < 0)
			break;
	}
	debug("%s Boot Status:0x%x[0x%llx]\n", __func__,
	      readl(sw_bar2 + mailbox_offset + 4), mailbox_offset + 4);
	if (readl(sw_bar2 + mailbox_offset + 4) != SW_BOOT_INIT_DONE) {
		/* Check firmware boot error code */
		printf("%s ERROR: Switch not init! [Boot Status:0x%x]\n",
		       __func__, readl(sw_bar2 + mailbox_offset + 4));
		printf("Boot Err Code:0x%x General Err Code:0x%x\n",
		       readl(sw_bar2 + mailbox_offset + 8),
		       readl(sw_bar2 + mailbox_offset + 12));
		return;
	}

	printf("Switch Init Success\n");
}

