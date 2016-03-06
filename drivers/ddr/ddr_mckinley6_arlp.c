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
#include <asm/arch/misc-regs.h>
#include "ddr_mckinley6.h"

enum mvebu_mck_freq_support {
	FREQ_600_MHZ = 0,
	FREQ_800_MHZ = 1,
	MAX_MHZ_SUPPORTED,
};

struct mvebu_mckinley_config {
	u32 reg_offset;
	u32 values[MAX_MHZ_SUPPORTED];
};

struct mvebu_mckinley_config mckinley_mac_config[] = {
	{ 0x340, {0x0f0f0fef, 0x0f0f0fef} },
	{ 0x344, {0x100000aa, 0x100000aa} },
	{ 0x310, {0x00200000, 0x00200000} },
	{ 0x304, {0x00000000, 0x00000000} },
	{ 0x308, {0x00000000, 0x00000000} },
	{ 0x200, {0x000E0001, 0x000E0001} },
	{ 0x204, {0x00000000, 0x00000000} },
	{ 0x220, {0x13020532, 0x13020532} },
	{ 0x044, {0x00030200, 0x00030200} },
	{ 0x2C0, {0x00006000, 0x00006000} },
	{ 0x2C4, {0x00100020, 0x00100020} },
	{ 0x058, {0x0000143f, 0x0000143f} },
	{ 0x048, {0x00000001, 0x00000001} },
	{ 0x180, {0x00010200, 0x00010200} },
	{ 0x050, {0x000001ff, 0x000001ff} },
	{ 0x04C, {0x00000000, 0x00000000} },
	{ 0x054, {0x00000480, 0x00000480} },
	{ 0x300, {0x00000708, 0x0000080B} },
	{ 0x380, {0x000001F5, 0x0007A120} },
	{ 0x384, {0x000003E9, 0x00030D40} },
	{ 0x388, {0x09600043, 0x0960006B} },
	{ 0x38C, {0x00000200, 0x00000200} },
	{ 0x390, {0x00400100, 0x00400100} },
	{ 0x394, {0x006B03CF, 0x00F003CF} },
	{ 0x398, {0x00720200, 0x00F80200} },
	{ 0x39C, {0x00120707, 0x00000808} },
	{ 0x3A0, {0x00040511, 0x00040614} },
	{ 0x3A4, {0x00000001, 0x00000001} },
	{ 0x3A8, {0x00000C04, 0x00000C04} },
	{ 0x3AC, {0x15210919, 0x202A0C1F} },
	{ 0x3B0, {0x090b0609, 0x0C0C060C} },
	{ 0x3B4, {0x04000600, 0x04000600} },
	{ 0x3B8, {0x00000600, 0x00000800} },
	{ 0x3BC, {0x02020404, 0x02020404} },
	{ 0x3C0, {0x00000000, 0x00000000} },
	{ 0x3C4, {0x00000000, 0x00000000} },
	{ 0x3DC, {0x00081239, 0x00081239} },
	{ 0x2C8, {0x00000000, 0x00000000} },
	{ 0x064, {0x00000006, 0x00000006} },
	{ -1, {-1, -1} }
};

struct mvebu_mckinley_config mckinley_phy_config[] = {
	{ 0x004, {0x10077779, 0x10077779} },
	{ 0x008, {0x1ff00770, 0x1ff00770} },
	{ 0x00C, {0x3f03fc77, 0x3f03fc77} },
	{ 0x010, {0x00100118, 0x00100118} },
	{ 0x028, {0x00000000, 0x00000000} },
	{ 0x030, {0x03800000, 0x03800000} },
	{ 0x034, {0x00000000, 0x00000000} },
	{ 0x040, {0x00000400, 0x00000400} },
	{ 0x0C0, {0x80000001, 0x80000001} },
	{ 0x0D0, {0x00000000, 0x00000000} },
	{ 0x0E0, {0x00011ff0, 0x00011ff0} },
	{ 0x090, {0x00000000, 0x00000000} },
	{ 0x094, {0x00000000, 0x00000000} },
	{ 0x098, {0x00000000, 0x00000000} },
	{ 0x09C, {0x00000000, 0x00000000} },
	{ 0x0A0, {0x00000000, 0x00000000} },
	{ 0x0A4, {0x00000000, 0x00000000} },
	{ 0x0A8, {0x00000000, 0x00000000} },
	{ 0x0AC, {0x00000000, 0x00000000} },
	{ 0x0B0, {0x00000000, 0x00000000} },
	{ 0x000, {0x00044041, 0x00044041} },
	{ 0x014, {0x00080200, 0x00080200} },
	{ 0x038, {0x00000002, 0x00000002} },
	{ 0x03C, {0x00000010, 0x00000010} },
	{ 0x180, {0x0000020a, 0x0000020a} },
	{ 0x184, {0x0000020a, 0x0000020a} },
	{ 0x188, {0x0000020a, 0x0000020a} },
	{ 0x18C, {0x0000020a, 0x0000020a} },
	{ 0x190, {0x0000020a, 0x0000020a} },
	{ 0x194, {0x0000020a, 0x0000020a} },
	{ 0x198, {0x0000020a, 0x0000020a} },
	{ 0x19C, {0x0000020a, 0x0000020a} },
	{ 0x1A0, {0x0000020a, 0x0000020a} },
	{ 0x050, {0x08080000, 0x20200000} },
	{ 0x054, {0x08080000, 0x20200000} },
	{ 0x074, {0x20200000, 0x20200000} },
	{ 0x058, {0x08080000, 0x08080000} },
	{ 0x05C, {0x08080000, 0x08080000} },
	{ 0x060, {0x08080000, 0x08080000} },
	{ 0x064, {0x08080000, 0x08080000} },
	{ 0x068, {0x08080000, 0x08080000} },
	{ 0x06C, {0x08080000, 0x08080000} },
	{ 0x070, {0x08080000, 0x08080000} },
	{ 0x020, {0x20000000, 0x20000000} },
	{ 0x020, {0x40000000, 0x40000000} },
	{ 0x020, {0x80000000, 0x80000000} },
	{-1, {-1, -1} },
};

void mvebu_dram_mac_init(struct mvebu_dram_config *dram_config)
{
#if !defined(SPL_IS_IN_DRAM)
	void __iomem *base_addr = dram_config->mac_base;
	struct mvebu_mckinley_config *mac_config = &mckinley_mac_config[0];
	u32 freq_indx, reg, idx, size;

	debug_enter();

	debug("Set Power-down options: %#lX <<- %#X\n", MVEBU_NB_PM_PWRDWN_OPT_REG,
	      L2_SRAM_LKG_PD_EN | CPU_ROM_PD_EN | AVS_DISABLE_MODE  | DDRPHY_PAD_PWRDWN_EN);
	writel(L2_SRAM_LKG_PD_EN | CPU_ROM_PD_EN |
	       AVS_DISABLE_MODE  | DDRPHY_PAD_PWRDWN_EN, MVEBU_NB_PM_PWRDWN_OPT_REG);

	debug("Set DDR PHY mode: %#lX <<- %#X\n", MVEBU_AXI_DCTRL_CTRL_RST_REG, DDRPHY_MODE_SELECT);
	writel(DDRPHY_MODE_SELECT, MVEBU_AXI_DCTRL_CTRL_RST_REG);

	debug("Reset monitor signal select: %#lX - 0x0000000\n", MVEBU_AXI_DCTRL_MCM_SEL_REG);
	writel(0x00000000, MVEBU_AXI_DCTRL_MCM_SEL_REG);

	switch (dram_config->freq_mhz) {
	case 600:
		freq_indx = FREQ_600_MHZ;
		break;
	case 800:
		freq_indx = FREQ_800_MHZ;
		break;
	default:
		error("DDR MAC: Invalid DDR frequency %d. Falling into default value\n",
		      dram_config->freq_mhz);
		dram_config->freq_mhz = 600;
		freq_indx = FREQ_600_MHZ;
	}

	debug("DDR MAC @ %d MHz\n", dram_config->freq_mhz);

	/* Walk trough MAC static configurations array */
	for (; mac_config->reg_offset != -1 ; mac_config++)
		mck6_writel(mac_config->values[freq_indx], base_addr + mac_config->reg_offset);

	debug("DDR MAC initialization done\n");

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
#endif
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
#if !defined(SPL_IS_IN_DRAM)
	void __iomem *base_addr = dram_config->phy_base;
	struct mvebu_mckinley_config *phy_config = &mckinley_phy_config[0];
	u32 freq_indx, reg, cs_mask;
	s32 timeout;

	debug_enter();

	switch (dram_config->freq_mhz) {
	case 600:
		freq_indx = FREQ_600_MHZ;
		break;
	case 800:
		freq_indx = FREQ_800_MHZ;
		break;
	default:
		error("DDR PHY: Invalid DDR frequency %d. Falling into default 600MHz\n",
		      dram_config->freq_mhz);
		freq_indx = FREQ_600_MHZ;
	}

	debug("DDR PHY @ %d MHz\n", dram_config->freq_mhz);

	/* Walk trough PHY static configurations array */
	for (; phy_config->reg_offset != -1 ; phy_config++)
		mck6_writel(phy_config->values[freq_indx], base_addr + phy_config->reg_offset);

	debug("DDR PHY setup is done. Triggering DDR init.\n");
	/* Trigger DDR init for Channel 0, all Chip-Selects */
	reg = SDRAM_INIT_REQ_MASK;
	reg |= CMD_CH_ENABLE(0);
	cs_mask = 1;
	if (dram_config->cs_count)
		cs_mask = (1 << dram_config->cs_count) - 1;
	reg |= CMD_CS_MASK(cs_mask);
	mck6_writel(reg, dram_config->mac_base + MCK6_USER_COMMAND_0_REG);

	/* wait for ddr init done */
	for (timeout = DRAM_INIT_TIMEOUT, reg = 0; (timeout >= 0) && (reg != INIT_DONE0_CH0); timeout--) {
		reg = readl(dram_config->mac_base + MCK6_DRAM_STATUS_REG) & INIT_DONE0_CH0;
		udelay(10);
	}

	if (timeout < 0)
		error("DDR init timeout!\n");

	debug("DDR init is done.\n");
#endif
#ifdef CONFIG_MVEBU_SYS_INFO
	set_dram_info(dram_config->mac_base);
#endif
	debug_exit();
}


static void mvebu_dram_dll_set(unsigned short dll_phsel, unsigned short dll_phsel1, void *base_addr)
{
	debug_enter();

	debug("set dll_phsel=%#x, dll_phsel1=%#x\n", dll_phsel, dll_phsel1);

	mck6_writel((dll_phsel << DLL_PHASE_POS_SHIFT) |
		    (dll_phsel1 << DLL_PHASE_NEG_SHIFT), base_addr + MC6_CH0_PHY_DLL_CONTROL_B0 + 0);
	mck6_writel((dll_phsel << DLL_PHASE_POS_SHIFT) |
		    (dll_phsel1 << DLL_PHASE_NEG_SHIFT), base_addr + MC6_CH0_PHY_DLL_CONTROL_B0 + 4);
	mck6_writel((dll_phsel << DLL_PHASE_POS_SHIFT) |
		    (dll_phsel1 << DLL_PHASE_NEG_SHIFT), base_addr + MC6_CH0_PHY_DLL_CONTROL_B0 + 8);
	mck6_writel((dll_phsel << DLL_PHASE_POS_SHIFT) |
		    (dll_phsel1 << DLL_PHASE_NEG_SHIFT), base_addr + MC6_CH0_PHY_DLL_CONTROL_B0 + 12);

	/* Updates DLL master. Block read/MMR for 4096 MCLK cycles to guarantee DLL lock.
	   Either wait 4096 MCLK (memPll/4) cycles, or check DLL lock status
	 */
	mck6_writel(PHY_DLL_RESET, base_addr + MC6_CH0_PHY_CONTROL_9);

	/* Reset PHY DLL. Dll_reset_timer ([31:24] of PHY Control Register 8,
	   Offset 0x41C/0xC1C) is set to 0x10, reset DLL for 128*32=4096 MCLK cycles.
	 */
	udelay(DLL_RESET_WAIT_US);

	/* Copy DLL master to DLL slave. Slave controls the actual delay_l.
	   Both DLL bypass and DLL needs 'update'.
	 */
	mck6_writel(DLL_UPDATE_EN_PULSE, base_addr + MC6_CH0_PHY_CONTROL_9);

	/* Update Dll delay_l. When Dll_update_timer ([20:16] of PHY Control Register 8,
	   Offset 0x41C/0xC1C) is 0x11, assert DLL_UPDATE_EN pin for 0x11*16 = 272 MCLK cycles.
	 */
	udelay(DLL_UPDATE_WAIT_US);

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
	unsigned int   MIN_RD_DLL = 0xFFFF, MAX_RD_DLL = 0xFFFF;

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

		if (!mvebu_dram_dll_wr_test(DRAM_DIRECT_START, DRAM_DIRECT_SIZE)) {/*pass test*/
			if (MIN_RD_DLL == 0xFFFF)
				MIN_RD_DLL = dll_var;
			MAX_RD_DLL = dll_var;
			debug("Search RD_DLL Pass: DDR CTL = %#x.\n", dll_var);
		} else {
			debug("Search RD_DLL fail: DDR CTL = %#x.\n", dll_var);
			if (MIN_RD_DLL != 0xFFFF)
				break;
		}
	} /* end of phase loop */

	if (MIN_RD_DLL == 0xFFFF) {
		debug("DDR: No DLL found.\n");
		optimal_rd_dll = 0xFFFF;
	} else {
		optimal_rd_dll =  (MAX_RD_DLL - MIN_RD_DLL)/2 + MIN_RD_DLL;
		debug("DDR: Round %d, End DLL tuning - MIN = %#x, MAX = %#x, optimal = %#x\n",
			round, MIN_RD_DLL, MAX_RD_DLL, optimal_rd_dll);
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

#if !defined(SPL_IS_IN_DRAM)
	printf("DDR: DLL tune should not be called when running from DRAM!\n");
	return;
#endif
	dll = readl(base_addr + MC6_CH0_PHY_DLL_CONTROL_B0);
	printf("DDR: start DLL tuning with initial phase delays (P) %#x, (N) %#x\n",
	(dll >> DLL_PHASE_POS_SHIFT) & DLL_PHASE_SZ_MASK,
	(dll >> DLL_PHASE_NEG_SHIFT) & DLL_PHASE_SZ_MASK);

	dll = (dll >> DLL_PHASE_POS_SHIFT) & DLL_PHASE_SZ_MASK;
	mck6_writel(0x0, base_addr + MC6_CH0_PHY_CONTROL_9);

	/* Automatically update PHY DLL with interval time set in Dll_auto_update_interval
	   ([15:8] of PHY Control Register 13, Offset 0x248)
	 */
	regval = readl(base_addr + MC6_CH0_PHY_CONTROL_8);
	/* Turn off Dll_auto_manual_update & Dll_auto_update_en
	   DLL_auto_update_en has a known bug. Don't use.
	 */
	regval &= ~(DLL_AUTO_UPDATE_EN | DLL_AUTO_MANUAL_UPDATE);
	/* change Dll_reset_timer to 128*32 cycles*/
	regval |= DLL_RESET_TIMER(DLL_RST_TIMER_VAL);
	mck6_writel(regval, base_addr + MC6_CH0_PHY_CONTROL_8);
	dll_median = mvebu_dram_dll_search(0, 0, base_addr);

	if (dll_median == 0xFFFF) {
		status = 1;
	} else {/* Find Neg dll */
		dll_phsel1 = mvebu_dram_dll_search(dll_median, 1, base_addr);

		if (dll_phsel1 == 0xFFFF) {
			status = 1;
		} else {/* Find Pos dll */
			dll_phsel = mvebu_dram_dll_search(dll_phsel1, 2, base_addr);

			if (dll_phsel == 0xFFFF)
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
