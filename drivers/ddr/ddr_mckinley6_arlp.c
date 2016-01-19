/*
* ***************************************************************************
* Copyright (C) 2015 Marvell International Ltd.
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

#include <common.h>
#include <asm/io.h>
#include <asm/utils.h>
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch-mvebu/ddr.h>
#include <asm/arch-mvebu/system_info.h>

#define MCK6_USER_COMMAND_0_REG	(0x20)
#define SDRAM_INIT_REQ_MASK	(0x1)
#define CMD_CH_ENABLE(c)	(1 << (28 + c))
#define CMD_CS_MASK(m)		((m) << 24)

#define MCK6_CTRL_0_REG		(0x44)
#define CTRL_DATA_WIDTH_OFFSET	8
#define CTRL_DATA_WIDTH_MASK	(0xF << 8)
#define BUS_WIDTH_2_IDX(w)	(((w) <= 16) ? ((w) / 8) : (((w) / 32) + 2))
#define CTRL_DATA_WIDTH(w)	(BUS_WIDTH_2_IDX(w) << 8)
#define CTRL_DATA_WIDTH_CALC(v)	(1 << ((v) + 2))

#define MCK6_MMAP0_LOW_CH(i)	(0x200 + 8*(i))
#define MMAP_AREA_LEN_OFFSET	16
#define MMAP_AREA_LEN_MASK	(0x1F << 16)
#define MMAP_AREA_LEN(x)	((x) << 16)

/* DLL Tune definitions */
#define DLL_PHSEL_START			0x00
#define DLL_PHSEL_END			0x3F
#define DLL_PHSEL_STEP			0x1
#define MC6_CH0_PHY_CONTROL_8		(0x1C)
#define MC6_CH0_PHY_CONTROL_9		(0x20)
#define MC6_CH0_PHY_DLL_CONTROL_B0	(0x50)
#define SDRAM_DIRECT_START		(0x6000000)

#define mck6_writel(v, c)		\
do {							\
	debug("0x%p - 0x08%x\n", c, v);	\
	writel(v, c);					\
} while (0)

enum mvebu_mck_freq_support {
	FREQ_650_HZ = 0,
	MAX_HZ_SUPPORTED,
};

struct mvebu_mckinley_config {
	u32 reg_offset;
	u32 values[MAX_HZ_SUPPORTED];
};

struct mvebu_mckinley_config mckinley_mac_config[] = {
	{ -1, { -1} }
};

struct mvebu_mckinley_config mckinley_phy_config[] = {
	{-1, {-1} },
};

void mvebu_dram_mac_init(struct mvebu_dram_config *dram_config)
{
	void __iomem *base_addr = dram_config->mac_base;
	struct mvebu_mckinley_config *mac_config = &mckinley_mac_config[0];
	u32 freq_indx, reg, idx, size;

	/* This function has to be re-implemented for ArLP. Currently it is invalid */
	return;

	debug_enter();
	debug("Set bypass to clock gate: 0xF06f0098 - 0x0040004e\n");
	writel(0x0040004e, 0xF06f0098);
	debug("Enable vreg power-up: 0xF06F0108 - 0xFFFF0001\n");
	writel(0xFFFF0001, 0xF06F0108);
	debug("Enable channel 0: 0xF0841100 - 0x80000000\n");
	writel(0x80000000, 0xF0841100);

	/* for now set the frequency to 650 (index 0) */
	freq_indx = 0;
	for (; mac_config->reg_offset != -1 ; mac_config++)
		mck6_writel(mac_config->values[freq_indx], base_addr + mac_config->reg_offset);

	/* Override the above configurations, with user parameters. */
	if (dram_config->bus_width != 0) {
		/* DRAM width */
		reg = readl(base_addr + MCK6_CTRL_0_REG);
		reg &= ~CTRL_DATA_WIDTH_MASK;
		reg |= CTRL_DATA_WIDTH(dram_config->bus_width);
		mck6_writel(reg, base_addr + MCK6_CTRL_0_REG);
		debug("DRAM width set to %d.\n", dram_config->bus_width);
	}

	if (dram_config->size_mb != 0) {
		/* DRAM size */
		reg = readl(base_addr + MCK6_MMAP0_LOW_CH(0));
		reg &= ~(MMAP_AREA_LEN_MASK);

		switch (dram_config->size_mb) {
		case(384):
		case(786):
		case(1536):
		case(3072):
			reg |= MMAP_AREA_LEN(log_2_n_round_down(dram_config->size_mb / 384));
			size = dram_config->size_mb;
			break;
		default:
			idx = log_2_n_round_down(dram_config->size_mb / 8);
			reg |= MMAP_AREA_LEN(idx + 7);
			size = (1 << idx) * 8;
			break;
		}
		mck6_writel(reg, base_addr + MCK6_MMAP0_LOW_CH(0));
		debug("DRAM size set to %dMB.\n", size);
	}

	debug_exit();
}

/* set_dram_info - passing dram information from spl to u-boot by saving it on dram, start from address 0x04000000 */
void set_dram_info(void *base_addr)
{
	u32 reg, i, size;
	/* set dram bus width */
	reg = readl(base_addr + MCK6_CTRL_0_REG);
	reg = (reg & CTRL_DATA_WIDTH_MASK) >> CTRL_DATA_WIDTH_OFFSET;
	reg = CTRL_DATA_WIDTH_CALC(reg);
	set_info(DRAM_BUS_WIDTH, reg);

	for (i = 0; i < 4; i++) {
		reg = readl(base_addr + MCK6_MMAP0_LOW_CH(i));
		/* set dram size */
		size = (reg & MMAP_AREA_LEN_MASK) >> MMAP_AREA_LEN_OFFSET;
		switch (size) {
		case(0):
		case(1):
		case(2):
		case(3):
			size = 384 << size;
			break;
		default:
			size = 8 << (size - 7);
			break;
		}
		set_info(DRAM_CS0_SIZE + i, size);

		/* set dram chip select */
		if (reg & 0x1)
			set_info(DRAM_CS0 + i, 1);
		else
			set_info(DRAM_CS0 + i, 0);
	}
}

void mvebu_dram_phy_init(struct mvebu_dram_config *dram_config)
{
	void __iomem *base_addr = dram_config->phy_base;
	struct mvebu_mckinley_config *phy_config = &mckinley_phy_config[0];
	u32 freq_indx, reg, cs_mask;

	/* This function has to be re-implemented for ArLP. Currently it is invalid */
	return;

	debug_enter();
	/* for now set the frequency to 650 (index 0) */
	freq_indx = 0;
	for (; phy_config->reg_offset != -1 ; phy_config++)
		mck6_writel(phy_config->values[freq_indx], base_addr + phy_config->reg_offset);

	/* Trigger DDR init for Channel 0, all Chip-Selects */
	reg = SDRAM_INIT_REQ_MASK;
	reg |= CMD_CH_ENABLE(0);
	cs_mask = 1;
	if (dram_config->cs_count)
		cs_mask = (1 << dram_config->cs_count) - 1;
	reg |= CMD_CS_MASK(cs_mask);
	mck6_writel(reg, base_addr + MCK6_USER_COMMAND_0_REG);
#ifdef CONFIG_MVEBU_SYS_INFO
	set_dram_info(base_addr);
#endif
	debug_exit();
}


static void mvebu_dram_dll_set(unsigned short dll_phsel, unsigned short dll_phsel1, void *base_addr)
{
	debug_enter();

	debug("set dll_phsel=%#x, dll_phsel1=%#x\n", dll_phsel, dll_phsel1);

	mck6_writel((dll_phsel << 16) | (dll_phsel1 << 24), base_addr + MC6_CH0_PHY_DLL_CONTROL_B0 + 0);
	mck6_writel((dll_phsel << 16) | (dll_phsel1 << 24), base_addr + MC6_CH0_PHY_DLL_CONTROL_B0 + 4);
	mck6_writel((dll_phsel << 16) | (dll_phsel1 << 24), base_addr + MC6_CH0_PHY_DLL_CONTROL_B0 + 8);
	mck6_writel((dll_phsel << 16) | (dll_phsel1 << 24), base_addr + MC6_CH0_PHY_DLL_CONTROL_B0 + 12);

	/* Updates DLL master. Block read/MMR for 4096 MCLK cycles to guarantee DLL lock.
	   Either wait 4096 MCLK (memPll/4) cycles, or check DLL lock status
	 */
	mck6_writel(0x20000000, base_addr + MC6_CH0_PHY_CONTROL_9);

	/* Reset PHY DLL. Dll_reset_timer ([31:24] of PHY Control Register 8,
	   Offset 0x41C/0xC1C) is set to 0x10, reset DLL for 128*32=4096 MCLK cycles.
	 */
	udelay(100);

	/* Copy DLL master to DLL slave. Slave controls the actual delay_l.
	   Both DLL bypass and DLL needs 'update'.
	 */
	mck6_writel(0x40000000, base_addr + MC6_CH0_PHY_CONTROL_9);

	/* Update Dll delay_l. When Dll_update_timer ([20:16] of PHY Control Register 8,
	   Offset 0x41C/0xC1C) is 0x11, assert DLL_UPDATE_EN pin for 0x11*16 = 272 MCLK cycles.
	 */
	udelay(50);

	/* KW Finish DLL update*/
	mck6_writel(0x0, base_addr + MC6_CH0_PHY_CONTROL_9);

	debug_exit();
}

static unsigned int mvebu_dram_dll_wr_test(u64 start, u64 size)
{
	u64 i, t;
	u64 *ptr = NULL;

	for (i = 0; i < size; i += sizeof(u64))
		*((u64 *)((u64)(start + i))) = start + i;

	for (i = 0; i < size; i += sizeof(u64)) {
		ptr = (u64 *)((u64)(start + i));
		t = *(ptr);
		if (t != start + i) {
			debug("Error: [%08llX] expect %08llX, get %08llX\n", start + i, start + i, t);
			return 1;
		}
	}
	return 0;
}

static unsigned short mvebu_dram_dll_search(unsigned short dll, unsigned short round, void *base_addr)
{
	unsigned short dll_var, dll_phsel, dll_phsel1;
	unsigned int   optimal_rd_dll;
	unsigned int   MIN_RD_DLL = 0xffff, MAX_RD_DLL = 0xffff;

	debug_enter();

	for (dll_var = DLL_PHSEL_START; dll_var < DLL_PHSEL_END; dll_var += DLL_PHSEL_STEP) {
		if (round == 0) {
			dll_phsel = dll_var;
			dll_phsel1 = dll_var;
		} else if (round == 1) {
			dll_phsel = dll;	/*Pos*/
			dll_phsel1 = dll_var;
		} else {
			dll_phsel = dll_var;
			dll_phsel1 = dll;	/*Neg*/
		}
		/* Set dll */
		mvebu_dram_dll_set(dll_phsel, dll_phsel1, base_addr);

		if (!mvebu_dram_dll_wr_test(SDRAM_DIRECT_START, 1024 * 2)) {/*pass test*/
			if (MIN_RD_DLL == 0xffff)
				MIN_RD_DLL = dll_var;
			MAX_RD_DLL = dll_var;
			debug("Search RD_DLL Pass: DDR CTL = %#x.\n", dll_var);
		} else {
			debug("Search RD_DLL fail: DDR CTL = %#x.\n", dll_var);
			if (MIN_RD_DLL != 0xffff)
				break;
		}
		debug(" RD_DLL = 0x%x.\n", dll_var);
		if (dll_var == 0x3f)
			break;
	} /* end of phase loop */

	if (MIN_RD_DLL == 0xFFFF) {
		debug("DDR: No DLL found.\n");
		optimal_rd_dll = 0xffff;
	} else {
		optimal_rd_dll =  (MAX_RD_DLL - MIN_RD_DLL)/2 + MIN_RD_DLL;
		debug("DDR: end DLL tuning - MIN = %#x, MAX = %#x, optimal = %#x\n",
			MIN_RD_DLL, MAX_RD_DLL, optimal_rd_dll);
		if (round == 0) {
			dll_phsel = optimal_rd_dll;
			dll_phsel1 = optimal_rd_dll;
		} else if (round == 1) {
			dll_phsel = dll;		/*Pos*/
			dll_phsel1 = optimal_rd_dll;
		} else {
			dll_phsel = optimal_rd_dll;
			dll_phsel1 = dll;		/*Neg*/
		}
		/* Set dll */
		mvebu_dram_dll_set(dll_phsel - 5, dll_phsel1 - 5, base_addr);
	}

	debug_exit();

	return optimal_rd_dll;
}

void mvebu_dram_dll_tune(struct mvebu_dram_config *dram_config)
{
	unsigned short dll_median, dll_phsel, dll_phsel1;
	unsigned int regval, dll, status = 0;
	void __iomem *base_addr = dram_config->phy_base;

	debug_enter();

	dll = readl(base_addr + MC6_CH0_PHY_DLL_CONTROL_B0);
	printf("DDR: start DLL tuning with initial phase delays (P) %#x, (N) %#x\n",
		(dll >> 16) & 0x3F, (dll >> 24) & 0x3F);

	dll = (dll >> 16) & 0x3F;
	mck6_writel(0x0, base_addr + MC6_CH0_PHY_CONTROL_9);

	/* Automatically update PHY DLL with interval time set in Dll_auto_update_interval
	   ([15:8] of PHY Control Register 13, Offset 0x248)
	 */
	regval = readl(base_addr + MC6_CH0_PHY_CONTROL_8);
	/* Turn off Dll_auto_manual_update & Dll_auto_update_en
	   DLL_auto_update_en has a known bug. Don't use.
	 */
	regval &= ~0xC;
	/* change Dll_reset_timer to 128*32 cycles*/
	regval |= 0x80000000;
	mck6_writel(regval, base_addr + MC6_CH0_PHY_CONTROL_8);
	dll_median = mvebu_dram_dll_search(0, 0, base_addr);

	if (dll_median == 0xffff) {
		status = 1;
	} else {/* Find Neg dll */
		dll_phsel1 = mvebu_dram_dll_search(dll_median, 1, base_addr);

		if (dll_phsel1 == 0xffff) {
			status = 1;
		} else {/* Find Pos dll */
			dll_phsel = mvebu_dram_dll_search(dll_phsel1, 2, base_addr);

			if (dll_phsel == 0xffff)
				status = 1;
		}
	}

	if (status) {
		printf("DDR: using initial DLL phase delays\n");
		mvebu_dram_dll_set(dll, dll, base_addr);
	} else
		printf("DDR: set new DLL phase delays (P) %#x, (N) %#x\n", dll_phsel, dll_phsel1);


	debug_exit();

	return;
}
