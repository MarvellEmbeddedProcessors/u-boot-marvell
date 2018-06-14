/*
 * Copyright (C) 2018 Marvell International Ltd.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 * https://spdx.org/licenses
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/soc.h>

#define CP_DEV_ID_STATUS_REG		(MVEBU_REGISTER(0x2400240))
#define DEVICE_ID_STATUS_MASK		0xffff
#define SW_REV_STATUS_OFFSET		16
#define SW_REV_STATUS_MASK		0xf

#define A8040_DEVICE_ID			0x8040

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
	{ {0x7040, 1}, "Armada7040-A1", {0x806, 1}, {0x110, 1}, 1, 1 },
	{ {0x7040, 2}, "Armada7040-A2", {0x806, 1}, {0x110, 2}, 1, 1 },
	{ {0x8040, 1}, "Armada8040-A1", {0x806, 1}, {0x110, 1}, 1, 2 },
	{ {0x8040, 2}, "Armada8040-A2", {0x806, 1}, {0x110, 2}, 1, 2 },
};

static int get_soc_type_rev(u32 *type, u32 *rev)
{
	*type = readl(CP_DEV_ID_STATUS_REG) & DEVICE_ID_STATUS_MASK;
	*rev = (readl(CP_DEV_ID_STATUS_REG) >> SW_REV_STATUS_OFFSET) &
		SW_REV_STATUS_MASK;

	return 0;
}

static int get_soc_table_index(u32 *index)
{
	u32 soc_type;
	u32 rev, i, ret = 1;

	*index = 0;
	get_soc_type_rev(&soc_type, &rev);

	for (i = 0; i < sizeof(soc_info_table) / sizeof(struct soc_info); i++) {
		if ((soc_type ==
			soc_info_table[i].soc.module_type) &&
		   (rev == soc_info_table[i].soc.module_rev)) {
			*index = i;
			ret = 0;
		}
	}

	return ret;
}

static int get_soc_name(char **soc_name)
{
	u32 index;

	get_soc_table_index(&index);
	*soc_name = soc_info_table[index].soc_name;

	return 0;
}

static int get_ap_cp_num(u32 *ap_num, u32 *cp_num)
{
	u32 index;

	get_soc_table_index(&index);
	*ap_num = soc_info_table[index].ap_num;
	*cp_num = soc_info_table[index].cp_num;

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

/* Get SoC's Application Processor (AP) module type and revision */
static int get_ap_type_rev(u32 *type, u32 *rev)
{
	u32 index;

	get_soc_table_index(&index);
	*type = soc_info_table[index].ap.module_type;
	*rev = soc_info_table[index].ap.module_rev;

	return 0;
}

/* Get SoC's Communication Processor (CP) module type and revision */
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

	get_ap_cp_num(&ap_num, &cp_num);

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

#ifdef CONFIG_ARCH_MISC_INIT
int arch_misc_init(void)
{
	u32 type, rev;

	get_soc_type_rev(&type, &rev);

	/* A8040 A1/A2 doesn't support linux kernel cpuidle feautre,
	 * so U-boot needs to update Linux bootargs according
	 * to the device id:
	 *
	 * Device	Device_ID
	 * -------------------------------
	 * A8040 A1	0x18040
	 * A8040 A2	0x28040
	 * A8040 B0	0x08045
	 *
	 * So we need to check if 16 LSB bits are 0x8040.
	 * The variable 'type', which is returned by
	 * get_soc_type_rev() holds these bits.
	 */
	if (type == A8040_DEVICE_ID)
		env_set("cpuidle", "cpuidle.off=1");

	return 0;
}
#endif
