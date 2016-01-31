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

/*#define DEBUG */
#include <common.h>
#include <asm/io.h>
#include <asm/arch-mvebu/comphy.h>
#include <asm/arch-mvebu/mvebu.h>

#define SOC_CONTROL_REG                         (void __iomem *)0xd0018204

#define MV_PEX_IF_REGS_OFFSET(pex_if)			(pex_if > 0 ? (0x40000 + (pex_if-1)*0x4000) : 0x80000)
#define MV_PEX_IF_REGS_BASE(unit)				\
	((void __iomem *)0xd0000000 + MV_PEX_IF_REGS_OFFSET(unit))
#define PEX_CAPABILITIES_REG(pex_if)			((MV_PEX_IF_REGS_BASE(pex_if)) + 0x60)
#define PEX_LINK_CAPABILITIES_REG(pex_if)		((MV_PEX_IF_REGS_BASE(pex_if)) + 0x6C)
#define PEX_LINK_CTRL_STATUS_REG(pex_if)		((MV_PEX_IF_REGS_BASE(pex_if)) + 0x70)
#define PEX_LINK_CTRL_STATUS2_REG(pex_if)        ((MV_PEX_IF_REGS_BASE(pex_if)) + 0x90)
#define PEX_CTRL_REG(pex_if)                     ((MV_PEX_IF_REGS_BASE(pex_if)) + 0x1A00)
#define PEX_STATUS_REG(pex_if)                   ((MV_PEX_IF_REGS_BASE(pex_if)) + 0x1A04)
#define PEX_DBG_STATUS_REG(pex_if)               ((MV_PEX_IF_REGS_BASE(pex_if)) + 0x1A64)
#define PEX_CFG_ADDR_REG(pex_if)                 ((MV_PEX_IF_REGS_BASE(pex_if)) + 0x18F8)
#define PEX_CFG_DATA_REG(pex_if)                 ((MV_PEX_IF_REGS_BASE(pex_if)) + 0x18FC)
#define PEX_CFG_DIRECT_ACCESS(pex_if, cfg_reg)    (MV_PEX_IF_REGS_BASE(pex_if) + (cfg_reg))
#define PEX_LINK_CAPABILITY_REG                 0x6C
#define PEX_LINK_CTRL_STAT_REG                  0x70
#define PXSR_PEX_DEV_NUM_OFFS                   16  /* Device Number Indication */
#define PXSR_PEX_DEV_NUM_MASK                   (0x1f << PXSR_PEX_DEV_NUM_OFFS)
#define PXSR_PEX_BUS_NUM_OFFS                   8 /* Bus Number Indication */
#define PXSR_PEX_BUS_NUM_MASK                   (0xff << PXSR_PEX_BUS_NUM_OFFS)

/* PEX_CAPABILITIES_REG fields */
#define PCIE0_ENABLE_OFFS                       0
#define PCIE0_ENABLE_MASK                       (0x1 << PCIE0_ENABLE_OFFS)
#define PCIE1_ENABLE_OFFS                       1
#define PCIE1_ENABLE_MASK                       (0x1 << PCIE1_ENABLE_OFFS)
#define PCIE2_ENABLE_OFFS                       2
#define PCIE2_ENABLE_MASK                       (0x1 << PCIE2_ENABLE_OFFS)
#define PCIE3_ENABLE_OFFS                       3
#define PCIE4_ENABLE_MASK                       (0x1 << PCIE3_ENABLE_OFFS)

/* PCI Express Configuration Address Register */
#define PXCAR_REG_NUM_OFFS                      2
#define PXCAR_REG_NUM_MAX                       0x3F
#define PXCAR_REG_NUM_MASK                      (PXCAR_REG_NUM_MAX << PXCAR_REG_NUM_OFFS)
#define PXCAR_FUNC_NUM_OFFS                     8
#define PXCAR_FUNC_NUM_MAX                      0x7
#define PXCAR_FUNC_NUM_MASK                     (PXCAR_FUNC_NUM_MAX << PXCAR_FUNC_NUM_OFFS)
#define PXCAR_DEVICE_NUM_OFFS                   11
#define PXCAR_DEVICE_NUM_MAX                    0x1F
#define PXCAR_DEVICE_NUM_MASK                   (PXCAR_DEVICE_NUM_MAX << PXCAR_DEVICE_NUM_OFFS)
#define PXCAR_BUS_NUM_OFFS                      16
#define PXCAR_BUS_NUM_MAX                       0xFF
#define PXCAR_BUS_NUM_MASK                      (PXCAR_BUS_NUM_MAX << PXCAR_BUS_NUM_OFFS)
#define PXCAR_EXT_REG_NUM_OFFS                  24
#define PXCAR_EXT_REG_NUM_MAX                   0xF

#define PXCAR_REAL_EXT_REG_NUM_OFFS             8
#define PXCAR_REAL_EXT_REG_NUM_MASK             (0xF << PXCAR_REAL_EXT_REG_NUM_OFFS)

#define PXCAR_CONFIG_EN                         (0x1 << 31)
#define PEX_STATUS_AND_COMMAND                  0x004

static u32 pex_config_read(u32 pex_index, u32 bus, u32 dev, u32 func, u32 reg_off)
{
	u32 pex_data, pex_status, temp_data;
	u32 locat_dev, local_bus;

	pex_status = readl(PEX_STATUS_REG(pex_index));
	locat_dev = (pex_status & PXSR_PEX_DEV_NUM_MASK) >> PXSR_PEX_DEV_NUM_OFFS;
	local_bus = (pex_status & PXSR_PEX_BUS_NUM_MASK) >> PXSR_PEX_BUS_NUM_OFFS;

	/* in PCI Express we have only one device number */
	/* and this number is the first number we encounter else that the localDev */
	/* spec pex define return on config read/write on any device */

	if (bus == local_bus) {
		if (local_bus == 0) {
			/* if local dev is 0 then the first number we encounter after 0 is 1 */
			if ((dev != 1) && (dev != locat_dev))
				return -1;
		} else {
			/* if local dev is not 0 then the first number we encounter is 0 */
			if ((dev != 0) && (dev != locat_dev))
				return -1;
		}
	}

	/* Creating PEX address to be passed */
	pex_data = (bus << PXCAR_BUS_NUM_OFFS);
	pex_data |= (dev << PXCAR_DEVICE_NUM_OFFS);
	pex_data |= (func << PXCAR_FUNC_NUM_OFFS);
	pex_data |= (reg_off & PXCAR_REG_NUM_MASK); /* lgacy register space */
	/* extended register space */
	pex_data |= ((reg_off & PXCAR_REAL_EXT_REG_NUM_MASK)
				>> PXCAR_REAL_EXT_REG_NUM_OFFS) << PXCAR_EXT_REG_NUM_OFFS;
	pex_data |= PXCAR_CONFIG_EN;

	/* Write the address to the PEX configuration address register */
	writel(pex_data, PEX_CFG_ADDR_REG(pex_index));

	/* In order to let the PEX controller absorbed the address of the read  */
	/* transaction we perform a validity check that the address was written */
	if (pex_data != readl(PEX_CFG_ADDR_REG(pex_index)))
		return -1;

	/* cleaning Master Abort */
	temp_data = readl(PEX_CFG_DIRECT_ACCESS(pex_index, PEX_STATUS_AND_COMMAND));
	temp_data &= ~(0x1 << 29);
	temp_data |= (0x1 << 29);
	writel(temp_data, PEX_CFG_DIRECT_ACCESS(pex_index, PEX_STATUS_AND_COMMAND));
	/* Read the Data returned in the PEX Data register */
	pex_data = readl(PEX_CFG_DATA_REG(pex_index));

	debug("%s: return address 0x%x", __func__, pex_data);

	return pex_data;
}

void comphy_pcie_config_set(u32 comphy_max_count, struct comphy_map *serdes_map)
{
	u32 pex_index, lane;
	u32 pex_enable_offs = 0;

	debug_enter();
	for (lane = 0; lane < comphy_max_count; lane++, serdes_map++) {
		if (serdes_map->type < PHY_TYPE_PEX0 || serdes_map->type > PHY_TYPE_PEX3)
			continue;
		pex_index = serdes_map->type - PHY_TYPE_PEX0;
		reg_set(PEX_CAPABILITIES_REG(pex_index), 0x4 << 20, 0xf << 20);

		switch (serdes_map->type) {
		case PHY_TYPE_PEX0:
			pex_enable_offs |= 0x1 << PCIE0_ENABLE_OFFS;
			break;
		case PHY_TYPE_PEX1:
			pex_enable_offs |= 0x1 << PCIE1_ENABLE_OFFS;
			break;
		case PHY_TYPE_PEX2:
			pex_enable_offs |= 0x1 << PCIE2_ENABLE_OFFS;
			break;
		case PHY_TYPE_PEX3:
			pex_enable_offs |= 0x1 << PCIE3_ENABLE_OFFS;
			break;
		default:
			break;
		}
	}
	reg_set(SOC_CONTROL_REG, pex_enable_offs, 0x3);
	udelay(10000);

	debug_exit();
}

void comphy_pcie_config_detect(u32 comphy_max_count, struct comphy_map *serdes_map)
{
	u32 pex_index, lane;
	u32 next_bus, first_bus, reg_data, addr;

	debug_enter();
	next_bus = 0;
	for (lane = 0; lane < comphy_max_count; lane++, serdes_map++) {
		if (serdes_map->type < PHY_TYPE_PEX0 || serdes_map->type > PHY_TYPE_PEX3)
			continue;
		pex_index = serdes_map->type - PHY_TYPE_PEX0;
		first_bus = next_bus;
		reg_data = readl(PEX_DBG_STATUS_REG(pex_index));

		if ((reg_data & 0x7f) != 0x7e) {
			printf("PCIe - %d: detected no link\n", pex_index);
			continue;
		}

		next_bus++;
		reg_data = readl(PEX_CFG_DIRECT_ACCESS(pex_index, PEX_LINK_CAPABILITY_REG)) & 0xf;

		if (reg_data != 0x2) {
			printf("PCIe - %d: detected no link\n", pex_index);
			continue;
		}

		reg_data = (readl(PEX_CFG_DIRECT_ACCESS(pex_index, PEX_LINK_CTRL_STAT_REG)) & 0xF0000) >> 16;
		debug("Checking if the link established is gen1\n");
		if (reg_data != 0x1) {
			printf("PCIe - %d: detected no link\n", pex_index);
			continue;
		}

		reg_data = (first_bus << PXSR_PEX_BUS_NUM_OFFS) & PXSR_PEX_BUS_NUM_MASK;
		reg_set(PEX_STATUS_REG(pex_index), reg_data, PXSR_PEX_BUS_NUM_MASK);
		reg_data = (1 << PXSR_PEX_DEV_NUM_OFFS) & PXSR_PEX_DEV_NUM_MASK;
		reg_set(PEX_STATUS_REG(pex_index), reg_data, PXSR_PEX_DEV_NUM_MASK);
		printf("PCIe - %d: Link is Gen1, check the EP capability\n", pex_index);

		/* link is Gen1, check the EP capability */
		addr = pex_config_read(pex_index, first_bus, 0, 0, 0x34) & 0xff;
		if (addr == 0xff) {
			printf("PCIe - %d: remains Gen1.\n", pex_index);
			continue;
		}

		while ((pex_config_read(pex_index, first_bus, 0, 0, addr) & 0xFF) != 0x10)
			addr = (pex_config_read(pex_index, first_bus, 0, 0, addr) & 0xFF00) >> 8;

		/* check for Gen2 and above */
		if ((pex_config_read(pex_index, first_bus, 0, 0, addr + 0xC) & 0xF) >= 0x2) {
			reg_data = readl(PEX_LINK_CTRL_STATUS2_REG(pex_index));
			reg_data &= ~(0x3);
			reg_data |= 0x42;
			writel(reg_data, PEX_LINK_CTRL_STATUS2_REG(pex_index));
			reg_data = readl(PEX_CTRL_REG(pex_index));
			reg_data |= (0x1 << 10);
			writel(reg_data, PEX_CTRL_REG(pex_index));
			/* We need to wait 10ms before reading the PEX_DBG_STATUS_REG in order not
			   to read the status of the former state*/
			udelay(10000);
			printf("PCIe - %d: Link upgraded to Gen2 based on client cpabilities\n", pex_index);
		} else {
			printf("PCIe - %d: remains Gen1\n", pex_index);
		}
	}
	debug_exit();
}

void comphy_pcie_unit_general_config(u32 pex_index)
{
	debug_enter();

	debug("**** start of general configuration for PCIe comphy ****\n");
	/* the following should be done for PEXx1 and for PEX by 4-for the first lane only */
	reg_set(SOC_CONTROL_REG, 0x4000, 0x4000);
	/* Setup link width bit[9:4] */
	reg_set(PEX_LINK_CAPABILITIES_REG(pex_index), 0x10, 0x3F0);
	/* Setup maximum link speed bit[3:0] */
	reg_set(PEX_LINK_CAPABILITIES_REG(pex_index), 0x2, 0xF);
	/* Setup common clock configuration bit[6] */
	reg_set(PEX_LINK_CTRL_STATUS_REG(pex_index), 0x40, 0x40);

	debug_exit();
}
