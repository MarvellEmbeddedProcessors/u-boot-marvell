/*
 * ***************************************************************************
 * Copyright (C) 2017 Marvell International Ltd.
 * ***************************************************************************
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ***************************************************************************
 */

#include <asm/arch-armada8k/cache_llc.h>
#include <asm/io.h>
#include <common.h>
#include <mvebu/mvebu_chip_sar.h>

#define CP_DEV_ID_STATUS_REG		(MVEBU_REGISTER(0x2400240))
#define DEVICE_ID_STATUS_MASK		0xffff
#define AP_DEV_ID_STATUS_REG		(SOC_REGS_PHY_BASE + 0x6F8240)
#define AP_DEV_ID_STATUS_MASK		0xfff
#define SW_REV_STATUS_OFFSET		16
#define SW_REV_STATUS_MASK		0xf

struct mochi_module {
	u32 module_type;
	u32 module_rev;
};

struct soc_info {
	struct mochi_module soc;
	char *soc_name;
	struct mochi_module ap;
	struct mochi_module cp;
	u32 ap_num;
	u32 cp_num;
};

static struct soc_info soc_info_table[] = {
	{ {0x6025, 0}, "Armada3900-A0", {0x807, 0}, {0x115, 0}, 1, 1 },
	{ {0x7040, 2}, "Armada3900-Z2", {0x807, 0}, {0x110, 2}, 1, 1 },
	{ {0x7040, 1}, "Armada7040-A1", {0x806, 1}, {0x110, 1}, 1, 1 },
	{ {0x7040, 2}, "Armada7040-A2", {0x806, 1}, {0x110, 2}, 1, 1 },
	{ {0x7045, 0}, "Armada7040-B0", {0x806, 2}, {0x115, 0}, 1, 1 },
	{ {0x8040, 1}, "Armada8040-A1", {0x806, 1}, {0x110, 1}, 1, 2 },
	{ {0x8040, 2}, "Armada8040-A2", {0x806, 1}, {0x110, 2}, 1, 2 },
	{ {0x8045, 0}, "Armada8040-B0", {0x806, 2}, {0x115, 0}, 1, 2 },
};

static int get_soc_type_rev(u32 *type, u32 *rev)
{
	*type = readl(CP_DEV_ID_STATUS_REG) & DEVICE_ID_STATUS_MASK;
	*rev = (readl(CP_DEV_ID_STATUS_REG) >> SW_REV_STATUS_OFFSET)&
		SW_REV_STATUS_MASK;

	return 0;
}

static int get_ap_soc_type(u32 *type)
{
	*type = readl(AP_DEV_ID_STATUS_REG) & AP_DEV_ID_STATUS_MASK;

	return 0;
}

static int get_soc_table_index(u32 *index)
{
	u32 soc_type;
	u32 rev, i, ret = 1;
	u32 ap_type;

	*index = 0;
	get_soc_type_rev(&soc_type, &rev);
	get_ap_soc_type(&ap_type);

	for (i = 0; i < sizeof(soc_info_table) / sizeof(struct soc_info); i++) {
		if ((soc_type ==
			soc_info_table[i].soc.module_type) &&
		   (rev == soc_info_table[i].soc.module_rev) &&
		    ap_type == soc_info_table[i].ap.module_type) {
			*index = i;
			ret = 0;
		}
	}

	if (ret)
		error("using default SoC info: %s\n",
		      soc_info_table[*index].soc_name);

	return ret;
}

static int get_soc_name(char **soc_name)
{
	u32 index;

	get_soc_table_index(&index);
	*soc_name = soc_info_table[index].soc_name;

	return 0;
}

int soc_get_ap_cp_num(void *ap_num, void *cp_num)
{
	u32 index;

	get_soc_table_index(&index);
	*((u32 *)ap_num) = soc_info_table[index].ap_num;
	*((u32 *)cp_num) = soc_info_table[index].cp_num;

	return 0;
}

/* Get SoC's Application Processor (AP) module type and revsion */
static int get_ap_type_rev(u32 *type, u32 *rev)
{
	u32 index;

	get_soc_table_index(&index);
	*type = soc_info_table[index].ap.module_type;
	*rev = soc_info_table[index].ap.module_rev;

	return 0;
}

/* Get SoC's Communication Processor (CP) module type and revsion */
static int get_cp_type_rev(u32 *type, u32 *rev)
{
	u32 index;

	get_soc_table_index(&index);
	*type = soc_info_table[index].cp.module_type;
	*rev = soc_info_table[index].cp.module_rev;

	return 0;
}

/* Print device's SoC name and AP & CP information */
void soc_print_device_info(void)
{
	u32 ap_num, cp_num, ap_type, ap_rev, cp_type, cp_rev;
	char *soc_name = NULL;

	soc_get_ap_cp_num(&ap_num, &cp_num);

	get_soc_name(&soc_name);
	get_ap_type_rev(&ap_type, &ap_rev);
	get_cp_type_rev(&cp_type, &cp_rev);

	printf("SoC: %s; AP%x-A%d; ", soc_name, ap_type, ap_rev);
	/* more than one cp module */
	if (cp_num > 1)
		printf("%dx CP%x-A%d\n", cp_num, cp_type, cp_rev);
	else
		printf("CP%x-A%d\n", cp_type, cp_rev);
}

/* Print System cache (LLC) status and mode */
void soc_print_system_cache_info(void)
{
	u32 val;
	int llc_en = 0, excl = 0;

	val = readl(MVEBU_LLC_BASE + LLC_CTRL_REG_OFFSET);
	if (val & LLC_EN) {
		llc_en = 1;
		if (val & LLC_EXCL_EN)
			excl = 1;
	}

	printf("LLC %s%s\n", llc_en ? "Enabled" : "Disabled",
	       excl ? " (Exclusive Mode)" : "");
}

int boot_from_nand(void)
{
	struct sar_val sar;

	mvebu_sar_value_get(SAR_BOOT_SRC, &sar);
	if (sar.bootsrc.type == BOOTSRC_NAND)
		return 1;
	else
		return 0;
}

int soc_early_init_f(void)
{
#ifdef CONFIG_MVEBU_SAR
	/* Sample at reset register init */
	mvebu_sar_init();
#endif

	return 0;
}
