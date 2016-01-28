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

/* #define DEBUG */

#include <common.h>
#include <asm/io.h>
#include <asm/arch-mvebu/soc.h>
#include <asm/arch-mvebu/comphy.h>
#include <asm/arch-mvebu/system_info.h>
#include <asm/arch/regs-base.h>
#include <asm/arch-mvebu/pinctl.h>
#include <asm/arch-mvebu/fdt.h>
#include <asm/arch-armada8k/cache_llc.h>
#include <linux/sizes.h>
#include <netdev.h>
#include <mvebu_chip_sar.h>
#include <fdt_support.h>
#include <asm/arch-mvebu/mvebu.h>

#define RFU_GLOBAL_SW_RST		(MVEBU_RFU_BASE + 0x84)
#define RFU_SW_RESET_OFFSET		0

#define EMMC_PHY_IO_CTRL		(MVEBU_IP_CONFIG_REG)
#define EMMC_PHY_CTRL_SDPHY_EN		(1 << 0)

int soc_early_init_f(void)
{
	debug_enter();
#ifdef CONFIG_MVEBU_CHIP_SAR
	/* Sample at reset register init */
	mvebu_sar_init(gd->fdt_blob);
#endif
#ifdef CONFIG_MVEBU_PINCTL
	mvebu_pinctl_probe();
#endif

	debug_exit();
	return 0;
}

int soc_get_rev(void)
{
	/* This should read the soc rev from some register*/
	return 0;
}

int soc_get_id(void)
{
	/* This should read the soc id from some register*/
	return CONFIG_ARMADA_8K_SOC_ID;
}

void soc_init(void)
{
#ifdef CONFIG_MVEBU_COMPHY_SUPPORT
	if (comphy_init(gd->fdt_blob))
		error("COMPHY initialization failed\n");
#endif
	return;
}

#ifdef CONFIG_MVPP2X
/**
 * cpu_eth_init()
 *	invoke mv_pp2x_initialize for each port, which is the initialization
 *	entrance of mvpp2 driver.
 *
 * Input:
 *	bis - db_info
 *
 * Return:
 *	0 - cool
 */
int cpu_eth_init(bd_t *bis)
{
	/* init mv_pp2x module */
	if (0 != mv_pp2x_initialize(bis)) {
		error("failed to init mv_pp2x\n");
		return 1;
	}

	return 0;
}
#endif /* CONFIG_MVPP2X */

int dram_init(void)
{
	debug_enter();
#ifdef CONFIG_MVEBU_SPL_DDR_OVER_PCI_SUPPORT
	gd->ram_size = CONFIG_DDR_OVER_PCI_SIZE;
#elif defined(CONFIG_PALLADIUM)
	gd->ram_size = 0x20000000;
#else
	u32 cs;
	gd->ram_size = 0;
	for (cs = 0; cs < 4; cs++)
		if (get_info(DRAM_CS0 + cs) == 1)
			gd->ram_size += get_info(DRAM_CS0_SIZE + cs);

	gd->ram_size *= SZ_1M;
	/* if DRAM size == 0, print error message */
	if (gd->ram_size == 0)
		error("DRAM size equal 0, check DRAM configuration\n");
#endif
	debug_exit();
	return 0;
}

phys_size_t get_effective_memsize(void)
{
	/* Set Memory size of U-Boot to 1GB only - for relocation only */
	if (gd->ram_size < SZ_1G)
		return gd->ram_size;

	return SZ_1G;
}

void dram_init_banksize(void)
{
	/* Config 2 DRAM banks:
	** Bank 0 - max size 4G - 256M
	** Bank 1 - max size 4G */
	gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
	if (gd->ram_size <= SZ_4G) {
		gd->bd->bi_dram[0].size = min(gd->ram_size, (phys_size_t)(SZ_4G - SZ_256M));
		return;
	}

	gd->bd->bi_dram[0].size = SZ_4G - SZ_256M;
	gd->bd->bi_dram[1].start = SZ_4G;
	gd->bd->bi_dram[1].size = gd->ram_size - SZ_4G;
}

void reset_cpu(ulong ignored)
{
	debug_enter();
	u32 reg;
	reg = readl(RFU_GLOBAL_SW_RST);
	reg &= ~(1 << RFU_SW_RESET_OFFSET);
	writel(reg, RFU_GLOBAL_SW_RST);
	debug_exit();
}

void print_soc_specific_info(void)
{
	int llc_en, llc_excl_mode;

#ifdef CONFIG_MVEBU_SYS_INFO
	printf("\tDDR %d Bit width\n", get_info(DRAM_BUS_WIDTH));
#endif

	llc_en = llc_mode_get(&llc_excl_mode);
	printf("\tLLC %s%s\n", llc_en ? "Enabled" : "Disabled",
	       llc_excl_mode ? " (Exclusive Mode)" : "");
}

#ifdef CONFIG_XENON_MMC
void mmc_soc_init(void)
{
	u32 reg;

	/* set eMMC/SD PHY output instead of MPPs */
	reg = readl(EMMC_PHY_IO_CTRL);
	reg &= ~EMMC_PHY_CTRL_SDPHY_EN;
	writel(reg, EMMC_PHY_IO_CTRL);
}
#endif

/************************************************************************
   Function:  mvebu_is_in_recovery_mode

   The function checks if the system currently boots into recovery mode.
   The recovery mode is intended to bring up bricked board using UART
   port as the boot device. This mode is either trigered by escape
   sequence or by reset sample jumpers.
   In case of A8K the recovery boot is always running in silent mode with
   RX pin enabled. The UART RX pin is disabled in BootROM run time in all
   other cases.

   Return - 1 if recovery mode is active or 0 otherwise
************************************************************************/
bool mvebu_is_in_recovery_mode(void)
{
	debug_enter();
#ifdef CONFIG_SPL_BUILD
	bool	recovery = 0;
	/* UART RX pin numbers and their MPP functions */
	int	uart_rx_pins[] = MPP_UART_RX_PINS;
	int	uart_rx_func[] = MPP_UART_RX_FUNCTIONS;
	int	i, pin, offs, func;

	/* UART boot is always happen in silent mode */
	if (gd->flags & GD_FLG_SILENT) {
		for (i = 0; i < sizeof(uart_rx_pins)/sizeof(uart_rx_pins[0]); i++) {
			/* Check if UART RX is enabled.
			   This should only happen at the SPL (BIN header) stage
			   when the system boots from UART (i.e. in recovery mode) */
			   pin  = uart_rx_pins[i] % MPPS_PER_REG;
			   offs = (uart_rx_pins[i] / MPPS_PER_REG) * 4;

			   func = readl(MPP_REGS_BASE + offs);
			   func >>= pin * MPP_BIT_CNT;
			   func &= MPP_VAL_MASK;
			if (func == uart_rx_func[i]) {
				recovery = 1;
				break;
			}
		}
	}

	set_info(RECOVERY_MODE, recovery);

	return recovery;
#else
	return get_info(RECOVERY_MODE);
#endif
	debug_exit();
}

#ifdef CONFIG_LAST_STAGE_INIT
int last_stage_init(void)
{
	debug_enter();
#ifdef CONFIG_MULTI_DT_FILE
	uint8_t *fdt_blob;
	fdt_blob = cfg_eeprom_get_fdt();
	set_working_fdt_addr(fdt_blob);
#endif
	debug_exit();
	return 0;
}
#endif

