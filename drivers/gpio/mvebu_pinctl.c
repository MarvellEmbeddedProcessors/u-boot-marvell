/*
 * ***************************************************************************
 * Copyright (C) Marvell International Ltd. and its affiliates
 * ***************************************************************************
 * Marvell GPL License Option
 * If you received this File from Marvell, you may opt to use, redistribute
 * and/or modify this File in accordance with the terms and conditions of the
 * General Public License Version 2, June 1991 (the "GPL License"), a copy of
 * which is available along with the File in the license.txt or on the worldwide
 * web at http://www.gnu.org/licenses/gpl.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 * EXPRESSLY DISCLAIMED. The GPL License provides additional details about this
 * warranty disclaimer.
 * ***************************************************************************
 */

#include <config.h>
#include <common.h>
#include <asm/system.h>
#include <asm/io.h>
#include <fdtdec.h>
#include <malloc.h>
#include <errno.h>
#include <asm/arch-mvebu/fdt.h>

#define CONFIG_MAX_PINCTL_BANKS		4
#define CONFIG_MAX_PINS_PER_BANK	70
#define CONFIG_MAX_FUNC			0xF

#define BITS_PER_PIN		4
#define PIN_FUNC_MASK		((1 << BITS_PER_PIN) - 1)
#define PIN_REG_SHIFT		3
#define PIN_FIELD_MASK		((1 << PIN_REG_SHIFT) - 1)

DECLARE_GLOBAL_DATA_PTR;

struct pinctl_data {
	u8	*base_reg;
	u32	pin_cnt;
	u32	max_func;
	const char    *bank_name;
};
struct pinctl_data __attribute__((section(".data"))) bank_data[CONFIG_MAX_PINCTL_BANKS];

int pinctl_set_pin_func(int bank, int pin_id, int func)
{
	struct pinctl_data *pinctl =  &bank_data[bank];
	int reg_offset;
	int field_offset;
	u32 reg, mask;

	if (pin_id >= pinctl->pin_cnt) {
		error("pin id %d doesn't exist on bank %d\n", pin_id, bank);
		return -EINVAL;
	}

	if (func > pinctl->max_func) {
		error("illegal function %d for pin %d\n", func, pin_id);
		return -EINVAL;
	}

	/* Calculate register address and bit in register */
	reg_offset   = 4 * (pin_id >> (PIN_REG_SHIFT));
	field_offset = (BITS_PER_PIN) * (pin_id & PIN_FIELD_MASK);
	mask = ~(PIN_FUNC_MASK << field_offset);

	/* Clip value to field resolution */
	func &= PIN_FUNC_MASK;

	reg = readl(pinctl->base_reg + reg_offset);
	reg = (reg & mask) | (func << field_offset);
	writel(reg, pinctl->base_reg + reg_offset);

	return 0;
}

int pinctl_get_pin_func(int bank, int pin_id)
{
	struct pinctl_data *pinctl =  &bank_data[bank];
	int reg_offset;
	int field_offset;
	int func;

	if (pin_id >= pinctl->pin_cnt) {
		error("pin id %d doesnt exist on bank %d\n", pin_id, bank);
		return -1;
	}

	reg_offset   = 4 * (pin_id >> (PIN_REG_SHIFT));
	field_offset = (BITS_PER_PIN) * (pin_id & PIN_FIELD_MASK);

	func = (readl(pinctl->base_reg + reg_offset)  >> field_offset) & PIN_FUNC_MASK;
	return func;
}

int pinctl_get_bank_id(const char *bank_name)
{
	int bank_id = -1;
	int bank;

	for (bank = 0; bank < CONFIG_MAX_PINCTL_BANKS; bank++) {
		if (strcmp(bank_name, bank_data[bank].bank_name) == 0)
			bank_id = bank;
	}

	if (bank_id == -1)
		error("pinctl: cannot match bank %s to bank_id\n", bank_name);

	return bank_id;
}

int pinctl_get_pin_cnt(int bank_id)
{
	if ((bank_id < 0) || (bank_id >= CONFIG_MAX_PINCTL_BANKS))
		return 0;

	return bank_data[bank_id].pin_cnt;
}

const char *pinctl_get_name(int bank_id)
{
	if ((bank_id < 0) || (bank_id >= CONFIG_MAX_PINCTL_BANKS))
		return NULL;

	return bank_data[bank_id].bank_name;
}

int mvebu_pinctl_probe(void)
{
	struct pinctl_data *pinctl;
	const void *blob = gd->fdt_blob;
	int node_list[CONFIG_MAX_PINCTL_BANKS], node;
	int count, i, err, pin;
	u32 *pin_func;

	count = fdtdec_find_aliases_for_id(gd->fdt_blob, "pinctl",
			COMPAT_MVEBU_PINCTL, node_list, CONFIG_MAX_PINCTL_BANKS);

	if (count <= 0)
		return -ENODEV;

	if (count > CONFIG_MAX_PINCTL_BANKS) {
		error("pinctl: too many pinctl banks. using first %d banks\n", CONFIG_MAX_PINCTL_BANKS);
		count = CONFIG_MAX_PINCTL_BANKS;
	}

	for (i = 0; i < count ; i++) {
		node = node_list[i];

		if (node <= 0)
			continue;

		pinctl = &bank_data[i];
		pinctl->base_reg = fdt_get_regs_offs(blob, node, "reg");
		pinctl->pin_cnt  = fdtdec_get_int(blob, node, "pin-count", CONFIG_MAX_PINS_PER_BANK);
		pinctl->max_func  = fdtdec_get_int(blob, node, "max-func", CONFIG_MAX_FUNC);
		pinctl->bank_name = fdt_getprop(blob, node, "bank-name", NULL);

		pin_func = malloc(pinctl->pin_cnt * sizeof(u32));
		if (pin_func == NULL) {
			error("pinctl: no memory for pin_func array\n");
			continue;
		}
		err = fdtdec_get_int_array(blob, node, "pin-func", pin_func, pinctl->pin_cnt);
		if (err) {
			error("Failed reading pin functions for bank %s\n", pinctl->bank_name);
			continue;
		}

		for (pin = 0; pin < pinctl->pin_cnt; pin++) {
			err = pinctl_set_pin_func(i, pin, pin_func[pin]);
			if (err)
				printf("Warning: pin %d is not set for bank %d\n", pin, i);
		}
	}

	return 0;
}
