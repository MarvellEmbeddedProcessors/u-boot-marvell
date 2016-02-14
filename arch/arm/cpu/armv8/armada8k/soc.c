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

#define DEBUG

#include <common.h>
#include <asm/io.h>
#include <asm/arch-mvebu/soc.h>
#include <asm/arch-mvebu/system_info.h>
#include <asm/arch/regs-base.h>
#include <asm/arch/misc-regs.h>
#include <asm/arch-mvebu/pinctl.h>
#include <asm/arch-mvebu/fdt.h>
#include <linux/sizes.h>

#define RFU_GLOBAL_SW_RST		(MVEBU_RFU_BASE + 0x84)
#define RFU_SW_RESET_OFFSET		0

/* TODO: move this to firmware code */
enum axi_attr {
	AXI_ADUNIT_ATTR = 0,
	AXI_COMUNIT_ATTR,
	AXI_EIP197_ATTR,
	AXI_USB3D_ATTR,
	AXI_USB3H0_ATTR,
	AXI_USB3H1_ATTR,
	AXI_SATA0_ATTR,
	AXI_SATA1_ATTR,
	AXI_DAP_ATTR,
	AXI_DFX_ATTR,
	AXI_DBG_TRC_ATTR = 12,
	AXI_SDIO_ATTR,
	AXI_MSS_ATTR,
	AXI_MAX_ATTR,
};

/* Used for Units of CP-110 (e.g. USB device, USB Host, and etc) */
#define MVEBU_AXI_ATTR_BASE			(MVEBU_CP0_REGS_BASE + 0x441300)
#define MVEBU_AXI_ATTR_REG(index)		(MVEBU_AXI_ATTR_BASE + 0x4 * index)
#define MVEBU_AXI_ATTR_ARCACHE_OFFSET		4
#define MVEBU_AXI_ATTR_ARCACHE_MASK		(0xF << MVEBU_AXI_ATTR_ARCACHE_OFFSET)
#define MVEBU_AXI_ATTR_ARDOMAIN_OFFSET		12
#define MVEBU_AXI_ATTR_ARDOMAIN_MASK		(0x3 << MVEBU_AXI_ATTR_ARDOMAIN_OFFSET)
#define MVEBU_AXI_ATTR_AWCACHE_OFFSET		20
#define MVEBU_AXI_ATTR_AWCACHE_MASK		(0xF << MVEBU_AXI_ATTR_AWCACHE_OFFSET)
#define MVEBU_AXI_ATTR_AWDOMAIN_OFFSET		28
#define MVEBU_AXI_ATTR_AWDOMAIN_MASK		(0x3 << MVEBU_AXI_ATTR_AWDOMAIN_OFFSET)

void soc_axi_attr_init(void)
{
	u32 index, data;
	/* This temporary change in U-Boot - should be moved to firmware */
	/* Initialize AXI attributes for Armada-7040 SoC */
	/* check if run on 7040 SoC - and not AP-806 Stand alone */
	if (fdt_node_check_compatible(gd->fdt_blob, 0, "marvell,armada-70x0") == 0) {
		/* Go over the AXI attributes and set Ax-Cache and Ax-Domain */
		for (index = 0; index < AXI_MAX_ATTR; index++) {
			switch (index) {
			/* DFX and MSS unit works with no coherent only -
			** there's no option to configure the Ax-Cache and
			** Ax-Domain */
			case AXI_DFX_ATTR:
			case AXI_MSS_ATTR:
				continue;
			default:
				/* Set Ax-Cache as cacheable, no allocate, modifiable, bufferable
				** The values are different because Read & Write definition
				** is different in Ax-Cache */
				data = readl(MVEBU_AXI_ATTR_REG(index));
				data &= ~MVEBU_AXI_ATTR_ARCACHE_MASK;
				data |= 0xB << MVEBU_AXI_ATTR_ARCACHE_OFFSET;
				data &= ~MVEBU_AXI_ATTR_AWCACHE_MASK;
				data |= 0x7 << MVEBU_AXI_ATTR_AWCACHE_OFFSET;
				/* Set Ax-Domain as Outer domain */
				data &= ~MVEBU_AXI_ATTR_ARDOMAIN_MASK;
				data |= 0x2 << MVEBU_AXI_ATTR_ARDOMAIN_OFFSET;
				data &= ~MVEBU_AXI_ATTR_AWDOMAIN_MASK;
				data |= 0x2 << MVEBU_AXI_ATTR_AWDOMAIN_OFFSET;
				writel(data, MVEBU_AXI_ATTR_REG(index));
			}
		}
	}

	return;
}

int soc_early_init_f(void)
{
#ifdef CONFIG_MVEBU_PINCTL
	mvebu_pinctl_probe();
#endif
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
	soc_axi_attr_init();

	return;
}

int dram_init(void)
{
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
	u32 reg;
	reg = readl(RFU_GLOBAL_SW_RST);
	reg &= ~(1 << RFU_SW_RESET_OFFSET);
	writel(reg, RFU_GLOBAL_SW_RST);
}

void print_soc_specific_info(void)
{
#ifdef CONFIG_MVEBU_SYS_INFO
	printf("\tDDR %d Bit width\n", get_info(DRAM_BUS_WIDTH));
#endif
#ifdef CONFIG_MVEBU_LLC_ENABLE
	printf("\tLLC Enabled");
#ifdef CONFIG_MVEBU_LLC_EXCLUSIVE_EN
	printf(" (Exclusive Mode)");
#endif
	printf("\n");
#else /* CONFIG_MVEBU_LLC_ENABLE */
	printf("\tLLC Disabled\n");
#endif
}

#ifdef CONFIG_USB_XHCI
void board_usb_vbus_init(void)
{
	/* TBD - implement VBUS cycle for here*/
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
}
