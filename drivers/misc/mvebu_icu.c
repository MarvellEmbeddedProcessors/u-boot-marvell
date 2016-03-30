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

/*#define DEBUG*/
#include <common.h>
#include <asm/system.h>
#include <asm/io.h>

#define ICU_REG_BASE		0x1E0000

#define ICU_AXI_ATTR		0x0
#define ICU_SET_SPI_AL(x)	(0x10 + (0x10 * x))
#define ICU_SET_SPI_AH(x)	(0x14 + (0x10 * x))
#define ICU_CLR_SPI_AL(x)	(0x18 + (0x10 * x))
#define ICU_CLR_SPI_AH(x)	(0x1c + (0x10 * x))
#define ICU_INT_CFG(x)		(0x100 + 4 * x)

#define ICU_INT_ENABLE_OFFSET	(24)
#define ICU_IS_EDGE_OFFSET	(28)
#define ICU_GROUP_OFFSET	(29)

enum icu_group {
	ICU_GRP_NSR	= 0,
	ICU_GRP_SR	= 1,
	ICU_GRP_LPI	= 2,
	ICU_GRP_VLPI	= 3,
	ICU_GRP_SEI	= 4,
	ICU_GRP_REI	= 5,
	ICU_GRP_MAX,
};

struct icu_irq {
	u8	icu_id;
	u8	spi_id;
	u8	is_edge;
};

struct icu_msi {
	enum icu_group group;
	uintptr_t set_spi_addr;
	uintptr_t clr_spi_addr;
};

#define NS_MULTI_IRQS		40
#define NS_SINGLE_IRQS		27
#define REI_IRQS		10
#define SEI_IRQS		20
#define MAX_ICU_IRQS		207


/* Allocate the MSI address per interrupt group,
 * unsopprted groups get NULL address */
static struct icu_msi msi_addr[ICU_GRP_MAX] = {
	{ICU_GRP_NSR,  0xf03f0040, 0xf03f0048}, /* Non secure interrupts*/
	{ICU_GRP_SR,   0,	   0x0},	/* Secure interrupts */
	{ICU_GRP_LPI,  0x0,	   0x0},	/* LPI interrupts */
	{ICU_GRP_VLPI, 0x0,	   0x0},	/* Virtual LPI interrupts */
	{ICU_GRP_SEI,  0xf03f0230, 0x0},	/* System error interrupts */
	{ICU_GRP_REI,  0xf03f0270, 0x0},	/* RAM error interrupts */
};

/* Multi instance sources, multipllied in dual CP mode */
static struct icu_irq irq_map_ns_multi[NS_MULTI_IRQS] = {
	{22, 0, 0}, /* PCIx4 INT A interrupt */
	{23, 1, 0}, /* PCIx1 INT A interrupt */
	{24, 2, 0}, /* PCIx1 INT A interrupt */

	{33, 3, 0}, /* PPv2 DBG AXI monitor */
	{34, 3, 0}, /* HB1      AXI monitor */
	{35, 3, 0}, /* AP       AXI monitor */
	{36, 3, 0}, /* PPv2     AXI monitor */

	{38,  4, 0}, /* PPv2 Misc */

	{39,  5, 0}, /* PPv2 irq */
	{40,  6, 0}, /* PPv2 irq */
	{41,  7, 0}, /* PPv2 irq */
	{42,  8, 0}, /* PPv2 irq */
	{43,  9, 0}, /* PPv2 irq */
	{44, 10, 0}, /* PPv2 irq */
	{45, 11, 0}, /* PPv2 irq */
	{46, 12, 0}, /* PPv2 irq */
	{47, 13, 0}, /* PPv2 irq */
	{48, 14, 0}, /* PPv2 irq */
	{49, 15, 0}, /* PPv2 irq */
	{50, 16, 0}, /* PPv2 irq */
	{51, 17, 0}, /* PPv2 irq */
	{52, 18, 0}, /* PPv2 irq */
	{53, 19, 0}, /* PPv2 irq */
	{54, 20, 0}, /* PPv2 irq */

	{78, 21, 0}, /* MG irq */
	{88, 22, 0}, /* EIP-197 ring-0 */
	{89, 23, 0}, /* EIP-197 ring-1 */
	{90, 24, 0}, /* EIP-197 ring-2 */
	{91, 25, 0}, /* EIP-197 ring-3 */
	{92, 26, 0}, /* EIP-197 int */
	{95, 27, 0}, /* EIP-150 irq */
	{102, 28, 0}, /* USB3 Device irq */
	{105, 29, 0}, /* USB3 Host-1 irq */
	{106, 30, 0}, /* USB3 Host-0 irq */
	{107, 31, 0}, /* SATA Host-1 irq */
	{109, 31, 0}, /* SATA Host-0 irq */
	{126, 33, 0}, /* PTP irq */
	{127, 34, 0}, /* GOP-3 irq */
	{128, 35, 0}, /* GOP-2 irq */
	{129, 36, 0}, /* GOP-0 irq */
};

/* Single instance sources, not multiplies in dual CP mode */
static struct icu_irq irq_map_ns_single[NS_SINGLE_IRQS] = {
	{27, 37, 0}, /* SD/MMC */
	{76, 38, 0}, /* Audio */
	{77, 39, 0}, /* MSS RTC */
	{79, 40, 0}, /* GPIO 56-63 */
	{80, 41, 0}, /* GPIO 48-55 */
	{81, 42, 0}, /* GPIO 40-47 */
	{82, 43, 0}, /* GPIO 32-39 */
	{83, 44, 0}, /* GPIO 24-31 */
	{84, 45, 0}, /* GPIO 16-23 */
	{85, 46, 0}, /* GPIO  8-15 */
	{86, 47, 0}, /* GPIO  0-7  */
	{111, 48, 0}, /* TDM-MC func 1 */
	{112, 49, 0}, /* TDM-MC func 0 */
	{113, 50, 0}, /* TDM-MC irq */
	{115, 51, 0}, /* NAND irq */
	{117, 52, 0}, /* SPI-1 irq */
	{118, 53, 0}, /* SPI-0 irq */
	{120, 54, 0}, /* I2C 0 irq */
	{121, 55, 0}, /* I2C 1 irq */
	{122, 56, 0}, /* UART 0 irq */
	{123, 57, 0}, /* UART 1 irq */
	{124, 58, 0}, /* UART 2 irq */
	{125, 59, 0}, /* UART 3 irq */

	/* Temporary PPv2 interrupts
	 * should be moved to second interrupt bank */
	{55, 60, 0}, /* PPv2 irq */
	{56, 61, 0}, /* PPv2 irq */
	{57, 62, 0}, /* PPv2 irq */
	{58, 63, 0}, /* PPv2 irq */
};

/* SEI - System Error Interrupts */
static struct icu_irq irq_map_sei[SEI_IRQS] = {
	{11, 0, 0}, /* SEI error CP-2-CP */
	{15, 1, 0}, /* PIDI-64 SOC */
	{16, 2, 0}, /* D2D error irq */
	{17, 3, 0}, /* D2D irq */
	{18, 4, 0}, /* NAND error */
	{19, 5, 0}, /* PCIx4 error */
	{20, 6, 0}, /* PCIx1_0 error */
	{21, 7, 0}, /* PCIx1_1 error */
	{25, 8, 0}, /* SDIO reg error */
	{75, 9, 0}, /* IOB error */
	{94, 10, 0}, /* EIP150 error */
	{97, 11, 0}, /* XOR-1 system error */
	{99, 12, 0}, /* XOR-0 system error */
	{108, 13, 0}, /* SATA-1 error */
	{110, 14, 0}, /* SATA-0 error */
	{114, 15, 0}, /* TDM-MC error */
	{116, 16, 0}, /* DFX server irq */
	{117, 17, 0}, /* Device bus error */
	{147, 18, 0}, /* Audio error */
	{171, 19, 0}, /* PIDI Sync error */
};

/* REI - RAM Error Interrupts */
static struct icu_irq irq_map_rei[REI_IRQS] = {
	{12, 0, 0}, /* REI error CP-2-CP */
	{26, 1, 0}, /* SDIO memory error */
	{87, 2, 0}, /* EIP-197 ECC error */
	{93, 3, 0}, /* EIP-150 RAM error */
	{96, 4, 0}, /* XOR-1 memory irq */
	{98, 5, 0}, /* XOR-0 memory irq */
	{100, 6, 0}, /* USB3 device tx parity */
	{101, 7, 0}, /* USB3 device rq parity */
	{103, 8, 0}, /* USB3H-1 RAM error */
	{104, 9, 0}, /* USB3H-0 RAM error */
};

static void icu_clear_irq(uintptr_t icu_base, int nr)
{
	writel(0, icu_base + ICU_INT_CFG(nr));
}

static void icu_set_irq(uintptr_t icu_base, struct icu_irq *irq, u32 spi_base, enum icu_group group)
{
	u32 icu_int;

	icu_int  = (irq->spi_id + spi_base) | (1 << ICU_INT_ENABLE_OFFSET);
	icu_int |= irq->is_edge << ICU_IS_EDGE_OFFSET;
	icu_int |= group << ICU_GROUP_OFFSET;

	writel(icu_int, icu_base + ICU_INT_CFG(irq->icu_id));
}

/*
 *  This function uses 2 spi values to initialize the ICU
 *  spi_base: used to set the base of the SPI id in the MSI message
 *            generated by the ICU. AP806-Z1 required spi_base=64 while
 *            AP806-A0 uses spi_base=0
 *  spi_offset: used to shift the multi instance interrupts between CP-0
 *              and CP-1
 */
void icu_init(uintptr_t cp_base, int spi_base, int spi_offset)
{
	int i;
	struct icu_irq *irq;
	struct icu_msi *msi;
	uintptr_t icu_base = cp_base + ICU_REG_BASE;

	/* Set the addres for SET_SPI and CLR_SPI registers in AP */
	msi = msi_addr;
	for (i = 0; i < ICU_GRP_MAX; i++, msi++) {
		writel(msi->set_spi_addr & 0xFFFFFFFF , icu_base + ICU_SET_SPI_AL(msi->group));
		writel(msi->set_spi_addr >> 32, icu_base + ICU_SET_SPI_AH(msi->group));
		writel(msi->clr_spi_addr & 0xFFFFFFFF , icu_base + ICU_CLR_SPI_AL(msi->group));
		writel(msi->clr_spi_addr >> 32, icu_base + ICU_CLR_SPI_AH(msi->group));
	}

	/* Mask all ICU interrupts */
	for (i = 0; i < MAX_ICU_IRQS; i++)
		icu_clear_irq(icu_base, i);

	/* Configure the ICU interrupt lines */
	/* Multi instance interrupts use different SPI ID for CP-1*/
	irq = irq_map_ns_multi;
	for (i = 0; i < NS_MULTI_IRQS; i++, irq++)
		icu_set_irq(icu_base, irq, spi_base + spi_offset, ICU_GRP_NSR);

	irq = irq_map_ns_single;
	for (i = 0; i < NS_MULTI_IRQS; i++, irq++)
		icu_set_irq(icu_base, irq, spi_base, ICU_GRP_NSR);

	irq = irq_map_sei;
	for (i = 0; i < NS_MULTI_IRQS; i++, irq++)
		icu_set_irq(icu_base, irq, spi_base, ICU_GRP_SEI);

	irq = irq_map_rei;
	for (i = 0; i < NS_MULTI_IRQS; i++, irq++)
		icu_set_irq(icu_base, irq, spi_base, ICU_GRP_REI);

	return;
}
