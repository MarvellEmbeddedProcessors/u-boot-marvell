/*
 * ***************************************************************************
 * Copyright (C) Marvell International Ltd. and its affiliates
 * ***************************************************************************
 * Marvell GPL License Option
 * If you received this File from Marvell, you may opt to use, redistribute
 * and/or modify this File in accordance with the terms and conditions of the
 * General Public License Version 2, June 1991 (the "GPL License"), a copy of
 * which is available along with the File in the license.txt file or by writing
 * to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 or on the worldwide web at http://www.gnu.org/licenses/gpl.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 * EXPRESSLY DISCLAIMED. The GPL License provides additional details about this
 * warranty disclaimer.
 * ***************************************************************************
 */

#define DEBUG

#include <config.h>
#include <common.h>
#include <asm/system.h>
#include <asm/bitops.h>
#include <asm/io.h>
#include <asm/arch-mvebu/mpp.h>
#include <asm/arch-mvebu/soc.h>

char **mpp_get_desc_table(void)
{
	return soc_get_mpp_desc_table();
}

static void set_field(int mpp_id, int value, u32 *base_ptr)
{
	int reg_offset;
	int field_offset;
	u32 reg, mask;

	/* Calculate register address and bit in register */
	reg_offset   = 4 * (mpp_id >> (MPP_FIELD_BITS));
	field_offset = (MPP_BIT_CNT) * (mpp_id & MPP_FIELD_MASK);
	mask = ~(MPP_VAL_MASK << field_offset);

	/* Clip value to bit resolution */
	value &= MPP_VAL_MASK;

	reg = readl(base_ptr + reg_offset);
	reg = (reg & mask) | (value << field_offset);
	writel(reg, base_ptr + reg_offset);
}

static u8 get_field(int mpp_id, u32 *base_ptr)
{
	u32 reg_offset;
	int field_offset;
	u32 reg;
	u8 value;

	/* Calculate register address and bit in register */
	reg_offset   = mpp_id >> (MPP_FIELD_BITS);
	field_offset = (MPP_BIT_CNT) * (mpp_id & MPP_FIELD_MASK);

	reg = readl(base_ptr + reg_offset);
	value = (reg >> field_offset) & MPP_VAL_MASK;

	return value;
}

void mpp_set_pin(int mpp_id, int value)
{
	if (value > MAX_MPP_OPTS)
		printf("Warning: MPP value %d > max value %d\n", value, MAX_MPP_OPTS);

	/* Set the new MPP to HW registers */
	set_field(mpp_id, value, (u32 *)MPP_REGS_BASE);
}

u8 mpp_get_pin(int mpp_id)
{
	u8 value;

	/* Calculate register address and bit in register */
	value = get_field(mpp_id, (u32 *)MPP_REGS_BASE);

	if (value > MAX_MPP_OPTS)
		printf("Warning: MPP value %d > max value %d\n", value, MAX_MPP_OPTS);

	return value;
}

void mpp_set_reg(u32 *mpp_reg, int first_reg, int last_reg)
{
	int reg;
	u32 *base = (u32 *)MPP_REGS_BASE;

	for (reg = first_reg; reg < last_reg; reg++, mpp_reg++)
		writel(*mpp_reg, base + reg);
}

void mpp_set_and_update(u32 *mpp_reg)
{
	int i;
	u32 *update_mask = soc_get_mpp_update_mask();
	u32 *update_val = soc_get_mpp_update_val();
	u32 *protect_mask = soc_get_mpp_protect_mask();
	u32 *base = (u32 *)MPP_REGS_BASE;

	for (i = 0; i < MAX_MPP_REGS; i++) {
		/* Disable modifying protected MPPs */
		update_mask[i] &= ~protect_mask[i];
		update_val[i]  &= ~protect_mask[i];

		/* Make sure the mask and val are synced */
		update_val[i] &= update_mask[i];

		/* Now update the required MPP fields */
		mpp_reg[i] &= ~update_mask[i];
		mpp_reg[i] |= update_val[i];

		debug("Set mpp reg 0x%08x\n", mpp_reg[i]);

		/* Write to register */
		writel(mpp_reg[i], base + i);
	}
}

int mpp_is_bus_enabled(struct mpp_bus *bus)
{
	int bus_alt;
	int pin;

	for (bus_alt = 0; bus_alt < bus->bus_cnt; bus_alt++) {
		for (pin = 0; pin < bus->pin_cnt; pin++) {
			u8 id = bus->pin_data[bus_alt][pin].id;
			u8 val = bus->pin_data[bus_alt][pin].val;
			if (mpp_get_pin(id) != val)
				return 0;
		}
	}

	return 1;
}

int mpp_is_bus_valid(struct mpp_bus *bus)
{
	int valid = (bus->pin_cnt > 0);
	return valid;
}

int mpp_enable_bus(int bus_id, int bus_alt)
{
	int i;
	struct mpp_pin *pin;
	struct mpp_bus *bus = soc_get_mpp_bus(bus_id);
	u32 *update_mask = soc_get_mpp_update_mask();
	u32 *update_val = soc_get_mpp_update_val();

	debug("Enabling MPP bus %s\n", bus->name);

	if (bus_alt < (bus->bus_cnt - 1)) {
		error("Bus alternative %d doesn't exist for bus %s\n", bus_alt, bus->name);
		return 1;
	}

	/* Check if someone already modified one of the pins */
	for (i = 0; i < bus->pin_cnt; i++) {
		pin = &bus->pin_data[bus_alt][i];
		if (get_field(pin->id, update_mask) == MPP_VAL_MASK) {
			error("Pin %d of Bus %s already modified\n", pin->id, bus->name);
			return 1;
		}
	}

	/* Update the mask and value */
	for (i = 0; i < bus->pin_cnt; i++) {
		pin = &bus->pin_data[bus_alt][i];
		debug("Setting [pin, val] = [%d, 0x%x]\n", pin->id, pin->val);
		set_field(pin->id, MPP_VAL_MASK, update_mask);
		set_field(pin->id, pin->val, update_val);
	}

	return 0;
}

