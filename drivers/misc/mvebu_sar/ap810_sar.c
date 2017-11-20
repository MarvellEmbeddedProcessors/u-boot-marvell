/*
 * Copyright (C) 2018 Marvell International Ltd.
 *
 * SPDX-License-Identifier:	GPL-2.0
 * https://spdx.org/licenses
 */

/* #define DEBUG */
#include <common.h>
#include <asm/io.h>
#include <errno.h>
#include <libfdt.h>
#include <mvebu/mvebu_chip_sar.h>
#include <mach/clock.h>
#include <sar-uclass.h>

#define CPU_CLOCK_ID	0
#define DDR_CLOCK_ID	1
#define RING_CLOCK_ID	2

/* SAR AP810 registers */
#define SAR_CLOCK_FREQ_MODE_OFFSET	0
#define SAR_CLOCK_FREQ_MODE_MASK	(0x7 << SAR_CLOCK_FREQ_MODE_OFFSET)
#define SAR_TEST_MODE_ENABLE_OFFSET	5
#define SAR_TEST_MODE_ENABLE_MASK	(0x1 << SAR_TEST_MODE_ENABLE_OFFSET)
#define SAR_SKIP_LINK_I2C_INIT_OFFSET	6
#define SAR_SKIP_LINK_I2C_INIT_MASK	(0x1 << SAR_SKIP_LINK_I2C_INIT_OFFSET)
#define SAR_POR_BYPASS_OFFSET		7
#define SAR_POR_BYPASS_MASK		(0x1 << SAR_POR_BYPASS_OFFSET)
#define SAR_BOOT_SOURCE_OFFSET		8
#define SAR_BOOT_SOURCE_MASK		(0x7 << SAR_BOOT_SOURCE_OFFSET)
#define SAR_PIDI_C2C_IHB_SELECT_OFFSET	11
#define SAR_PIDI_C2C_IHB_SELECT_MASK	(0x1 << SAR_PIDI_C2C_IHB_SELECT_OFFSET)
#define SAR_I2C_INIT_ENABLE_OFFSET	12
#define SAR_I2C_INIT_ENABLE_MASK	(0x1 << SAR_I2C_INIT_ENABLE_OFFSET)
#define SAR_SSCG_DISABLE_OFFSET		13
#define SAR_SSCG_DISABLE_MASK		(0x1 << SAR_SSCG_DISABLE_OFFSET)
#define SAR_PIDI_HW_TRAINING_DIS_OFFSET	14
#define SAR_PIDI_HW_TRAINING_DIS_MASK	(0x1 << SAR_PIDI_HW_TRAINING_DIS_OFFSET)
#define SAR_CPU_FMAX_REFCLK_OFFSET	15
#define SAR_CPU_FMAX_REFCLK_MASK	(0x1 << SAR_CPU_FMAX_REFCLK_OFFSET)
#define SAR_IHB_DIFF_REFCLK_DIS_OFFSET	16
#define SAR_IHB_DIFF_REFCLK_DIS_MASK	(0x1 << SAR_IHB_DIFF_REFCLK_DIS_OFFSET)
#define SAR_REF_CLK_MSTR_OFFSET		17
#define SAR_REF_CLK_MSTR_MASK		(0x1 << SAR_REF_CLK_MSTR_OFFSET)
#define SAR_CPU_WAKE_UP_OFFSET		18
#define SAR_CPU_WAKE_UP_MASK		(0x1 << SAR_CPU_WAKE_UP_OFFSET)
#define SAR_XTAL_BYPASS_OFFSET		19
#define SAR_XTAL_BYPASS_MASK		(0x1 << SAR_XTAL_BYPASS_OFFSET)
#define SAR_PIDI_LOW_SPEED_OFFSET	20
#define SAR_PIDI_LOW_SPEED_MASK		(0x1 << SAR_PIDI_LOW_SPEED_OFFSET)

#define AP810_SAR_1_REG			4
#define SAR1_PLL2_OFFSET		(9)
#define SAR1_PLL2_MASK			(0x1f << SAR1_PLL2_OFFSET)
#define SAR1_PLL1_OFFSET		(14)
#define SAR1_PLL1_MASK			(0x1f << SAR1_PLL1_OFFSET)
#define SAR1_PLL0_OFFSET		(19)
#define SAR1_PLL0_MASK			(0x1f << SAR1_PLL0_OFFSET)
#define SAR1_PIDI_CONNECT_OFFSET	(24)
#define SAR1_PIDI_CONNECT_MASK		(1 << SAR1_PIDI_CONNECT_OFFSET)

#define EFUSE_FREQ_REG			SOC_REGS_PHY_BASE + 0x6f4410
#define EFUSE_FREQ_OFFSET		24
#define EFUSE_FREQ_MASK			(0x1 << EFUSE_FREQ_OFFSET)

#define SAR_SUPPORTED_FREQ_NUM		8
#define SAR_FREQ_VAL			4
#define SAR_SUPPORTED_TABLES		2

/* Temporay substitute for sampled-at-reset
 * register due to bug in AP810 A0
 * the frequency option is sampled to the 3 LSB in the
 * scratch-pad register.
 */
#define SCRATCH_PAD_FREQ_REG		0xEC6F43E4

/* AP810 revision ID */
#define MVEBU_CSS_GWD_CTRL_IIDR2_REG	((MVEBU_REGISTER(0x6F0000)) + 0x240)
#define GWD_IIDR2_REV_ID_OFFSET		16
#define GWD_IIDR2_REV_ID_MASK		0xF

struct sar_info {
	char *name;
	u32 offset;
	u32 mask;
};

struct sar_info ap810_sar_0[] = {
	{"Clock Freq mode		  ",
		SAR_CLOCK_FREQ_MODE_OFFSET, SAR_CLOCK_FREQ_MODE_MASK },
	{"Test mode enable		  ",
		SAR_TEST_MODE_ENABLE_OFFSET, SAR_TEST_MODE_ENABLE_MASK },
	{"Skip link i2c init		  ",
		SAR_SKIP_LINK_I2C_INIT_OFFSET, SAR_SKIP_LINK_I2C_INIT_MASK },
	{"Por ByPass			  ",
		SAR_POR_BYPASS_OFFSET, SAR_POR_BYPASS_MASK },
	{"Boot Source			  ",
		SAR_BOOT_SOURCE_OFFSET, SAR_BOOT_SOURCE_MASK },
	{"PIDI C2C IHB select		  ",
		SAR_PIDI_C2C_IHB_SELECT_OFFSET, SAR_PIDI_C2C_IHB_SELECT_MASK },
	{"I2C init enable		  ",
		SAR_I2C_INIT_ENABLE_OFFSET, SAR_I2C_INIT_ENABLE_MASK },
	{"SSCG disable			  ",
		SAR_SSCG_DISABLE_OFFSET, SAR_SSCG_DISABLE_MASK },
	{"PIDI hw training disable	  ",
		SAR_PIDI_HW_TRAINING_DIS_OFFSET,
		SAR_PIDI_HW_TRAINING_DIS_MASK },
	{"CPU Fmax refclk select	  ",
		SAR_CPU_FMAX_REFCLK_OFFSET, SAR_CPU_FMAX_REFCLK_MASK },
	{"IHB differential refclk disable ",
		SAR_IHB_DIFF_REFCLK_DIS_OFFSET, SAR_IHB_DIFF_REFCLK_DIS_MASK },
	{"Ref clk mstr			  ",
		SAR_REF_CLK_MSTR_OFFSET, SAR_REF_CLK_MSTR_MASK },
	{"CPU wake up			  ",
		SAR_CPU_WAKE_UP_OFFSET, SAR_CPU_WAKE_UP_MASK },
	{"Xtal ByPass			  ",
		SAR_XTAL_BYPASS_OFFSET, SAR_XTAL_BYPASS_MASK },
	{"PIDI low speed		  ",
		SAR_PIDI_LOW_SPEED_OFFSET, SAR_PIDI_LOW_SPEED_MASK },
	{"",			-1,			-1},
};

struct sar_info ap810_sar_1[] = {
	{"PIDI connect       ", SAR1_PIDI_CONNECT_OFFSET,
				SAR1_PIDI_CONNECT_MASK },
	{"PLL0 Config        ", SAR1_PLL0_OFFSET, SAR1_PLL0_MASK },
	{"PLL1 Config        ", SAR1_PLL1_OFFSET, SAR1_PLL1_MASK },
	{"PLL2 Config        ", SAR1_PLL2_OFFSET, SAR1_PLL2_MASK },
	{"",			-1,			-1},
};

enum clocking_options_HP {
	HP_CPU_1600_DDR_1600_RCLK_1200_IO_800_PIDI_1000 = 0x0,
	HP_CPU_2000_DDR_2400_RCLK_1200_IO_800_PIDI_1000 = 0x1,
	HP_CPU_2000_DDR_2400_RCLK_1400_IO_1000_PIDI_1000 = 0x2,
	HP_CPU_2200_DDR_2400_RCLK_1400_IO_1000_PIDI_1000 = 0x3,
	HP_CPU_2200_DDR_2667_RCLK_1400_IO_1000_PIDI_1000 = 0x4,
	HP_CPU_2500_DDR_2400_RCLK_1400_IO_1000_PIDI_1000 = 0x5,
	HP_CPU_2500_DDR_2933_RCLK_1400_IO_1000_PIDI_1000 = 0x6,
	HP_CPU_2700_DDR_3200_RCLK_1400_IO_1000_PIDI_1000 = 0x7,
};

enum clocking_options_LP {
	LP_CPU_1200_DDR_1600_RCLK_800_IO_800_PIDI_1000 = 0x0,
	LP_CPU_1800_DDR_2400_RCLK_1000_IO_800_PIDI_1000 = 0x1,
	LP_CPU_1800_DDR_2400_RCLK_1100_IO_800_PIDI_1000 = 0x2,
	LP_CPU_1800_DDR_2400_RCLK_1200_IO_800_PIDI_1000 = 0x3,
	LP_CPU_1800_DDR_2400_RCLK_1400_IO_800_PIDI_1000 = 0x4,
	LP_CPU_2000_DDR_2400_RCLK_1100_IO_800_PIDI_1000 = 0x5,
	LP_CPU_2000_DDR_2400_RCLK_1200_IO_800_PIDI_1000 = 0x6,
	LP_CPU_2000_DDR_2400_RCLK_1300_IO_800_PIDI_1000 = 0x7,
};

static const u32 pll_freq_tbls[SAR_SUPPORTED_TABLES]
			      [SAR_SUPPORTED_FREQ_NUM]
			      [SAR_FREQ_VAL] = {
	{
		/* CPU */   /* DDR */   /* Ring */
		{1.6 * GHz, 1.6  * GHz, 1.2  * GHz,
		 HP_CPU_1600_DDR_1600_RCLK_1200_IO_800_PIDI_1000},
		{2.0 * GHz, 2.4 * GHz, 1.2 * GHz,
		 HP_CPU_2000_DDR_2400_RCLK_1200_IO_800_PIDI_1000},
		{2.0 * GHz, 2.4  * GHz, 1.4  * GHz,
		 HP_CPU_2000_DDR_2400_RCLK_1400_IO_1000_PIDI_1000},
		{2.2 * GHz, 2.4 * GHz, 1.4 * GHz,
		 HP_CPU_2200_DDR_2400_RCLK_1400_IO_1000_PIDI_1000},
		{2.2 * GHz, 2.667 * GHz, 1.4 * GHz,
		 HP_CPU_2200_DDR_2667_RCLK_1400_IO_1000_PIDI_1000},
		{2.5 * GHz, 2.4  * GHz, 1.4  * GHz,
		 HP_CPU_2500_DDR_2400_RCLK_1400_IO_1000_PIDI_1000},
		{2.5 * GHz, 2.93  * GHz, 1.4  * GHz,
		 HP_CPU_2500_DDR_2933_RCLK_1400_IO_1000_PIDI_1000},
		{2.7 * GHz, 3.2  * GHz, 1.4  * GHz,
		 HP_CPU_2700_DDR_3200_RCLK_1400_IO_1000_PIDI_1000},
	},
	{
		{1.2 * GHz, 1.6  * GHz, 0.8 * GHz,
		 LP_CPU_1200_DDR_1600_RCLK_800_IO_800_PIDI_1000},
		{1.8 * GHz, 2.4 * GHz, 1.0 * GHz,
		 LP_CPU_1800_DDR_2400_RCLK_1000_IO_800_PIDI_1000},
		{1.8 * GHz, 2.4  * GHz, 1.1  * GHz,
		 LP_CPU_1800_DDR_2400_RCLK_1100_IO_800_PIDI_1000},
		{1.8 * GHz, 2.4 * GHz, 1.2 * GHz,
		 LP_CPU_1800_DDR_2400_RCLK_1200_IO_800_PIDI_1000},
		{1.8 * GHz, 2.4 * GHz, 1.4 * GHz,
		 LP_CPU_1800_DDR_2400_RCLK_1400_IO_800_PIDI_1000},
		{2.0 * GHz, 2.4  * GHz, 1.1  * GHz,
		 LP_CPU_2000_DDR_2400_RCLK_1100_IO_800_PIDI_1000},
		{2.0 * GHz, 2.4  * GHz, 1.2  * GHz,
		 LP_CPU_2000_DDR_2400_RCLK_1200_IO_800_PIDI_1000},
		{2.0 * GHz, 2.4  * GHz, 1.3  * GHz,
		 LP_CPU_2000_DDR_2400_RCLK_1300_IO_800_PIDI_1000},
	},
};

int ap810_sar_value_get(struct udevice *dev, enum mvebu_sar_opts sar_opt,
			struct sar_val *val)
{
	u32 clock_type, clock_freq_mode, efuse_reg, chip_rev;
	struct dm_sar_pdata *priv = NULL;

	switch (sar_opt) {
	case(SAR_AP810_CPU_FREQ):
		clock_type = CPU_CLOCK_ID;
		break;
	case(SAR_AP810_DDR_FREQ):
		clock_type = DDR_CLOCK_ID;
		break;
	case(SAR_AP810_AP_FABRIC_FREQ):
		clock_type = RING_CLOCK_ID;
		break;
	default:
		error("AP810-SAR: Unsupported SAR option %d.\n", sar_opt);
		return -EINVAL;
	}

	/* fetch eFuse value and device whether it's H/L */
	efuse_reg = readl(EFUSE_FREQ_REG);
	efuse_reg &= EFUSE_FREQ_MASK;
	efuse_reg = efuse_reg >> EFUSE_FREQ_OFFSET;

	chip_rev = (readl(MVEBU_CSS_GWD_CTRL_IIDR2_REG) >>
		    GWD_IIDR2_REV_ID_OFFSET) &
		    GWD_IIDR2_REV_ID_MASK;

	/* Read from scratch-pad instead of sampled-at-reset in A0 */
	if (chip_rev) {
		priv = dev_get_priv(dev);
		clock_freq_mode = (readl(priv->sar_base) &
				   SAR_CLOCK_FREQ_MODE_MASK) >>
				   SAR_CLOCK_FREQ_MODE_OFFSET;
	} else {
		clock_freq_mode = readl(SCRATCH_PAD_FREQ_REG);
	}

	if (clock_freq_mode < 0 ||
	    (clock_freq_mode > (SAR_SUPPORTED_FREQ_NUM - 1))) {
		error("sar regs: unsupported clock freq mode %d\n",
		      clock_freq_mode);
		return -EINVAL;
	}
	val->raw_sar_val = clock_freq_mode;
	val->freq = pll_freq_tbls[efuse_reg][clock_freq_mode][clock_type];

	return 0;
}

static int ap810_sar_dump(struct udevice *dev)
{
	u32 reg, val;
	struct sar_info *sar;
	struct dm_sar_pdata *priv = dev_get_priv(dev);

	reg = readl(priv->sar_base);
	printf("AP810 SAR register 0 [0x%08x]:\n", reg);
	printf("----------------------------------\n");
	sar = ap810_sar_0;
	while (sar->offset != -1) {
		val = (reg & sar->mask) >> sar->offset;
		printf("%s  0x%x\n", sar->name, val);
		sar++;
	}

	reg = readl(priv->sar_base + AP810_SAR_1_REG);
	printf("\nAP810 SAR register 1 [0x%08x]:\n", reg);
	printf("----------------------------------\n");
	sar = ap810_sar_1;
	while (sar->offset != -1) {
		val = (reg & sar->mask) >> sar->offset;
		printf("%s  0x%x\n", sar->name, val);
		sar++;
	}
	return 0;
}

int ap810_sar_init(struct udevice *dev)
{
	int ret, i;

	u32 sar_list[] = {
		SAR_AP810_CPU_FREQ,
		SAR_AP810_DDR_FREQ,
		SAR_AP810_AP_FABRIC_FREQ
	};

	for (i = 0; i < ARRAY_SIZE(sar_list); i++) {
		ret = mvebu_sar_id_register(dev, sar_list[i]);
		if (ret) {
			error("Failed to register SAR %d, for AP810.\n",
			      sar_list[i]);
			return ret;
		}
	}
	return 0;
}

static const struct sar_ops ap810_sar_ops = {
	.sar_init_func = ap810_sar_init,
	.sar_value_get_func = ap810_sar_value_get,
	.sar_dump_func = ap810_sar_dump,
};

U_BOOT_DRIVER(ap810_sar) = {
	.name = "ap810_sar",
	.id = UCLASS_SAR,
	.priv_auto_alloc_size = sizeof(struct dm_sar_pdata),
	.ops = &ap810_sar_ops,
};
