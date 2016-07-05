/*
 * ***************************************************************************
 * Copyright (C) 2016 Marvell International Ltd.
 * ***************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of Marvell nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ***************************************************************************
 */

/* #define DEBUG */
#include <config.h>
#include <common.h>
#include <asm/system.h>
#include <asm/io.h>
#include <fdtdec.h>
#include <errno.h>
#include <asm/arch-mvebu/fdt.h>
#include <asm/arch-mvebu/mvebu.h>
#include <asm/arch/pinctrl.h>

DECLARE_GLOBAL_DATA_PTR;

struct pinctrl_setting_bitmap {
	/*
	 * The mask is used for read/write pinctrl register bits of the function seletion.
	 * For the pinctrls dedicated to gpio function which is not controlled by any pinctrl
	 * register bits, the mask should be set to '0'.
	 */
	unsigned int mask;
	unsigned int func_num;	/* pinctrl configuration modes number */
	/*
	 * This array maps the pinctrl's modes into the relevant pinctrl's register bits. It
	 * is indexed by pinctrl's modes, which is defined in armada_3700_nb/sb_pinctrl_modes.
	 */
	unsigned int reg_values[MAX_FUNC_PER_PIN];
};

struct pinctrl_bank_conf {
	char	*bank_name;
	u8	*base_reg;
	u32	pin_cnt;
	struct	pinctrl_setting_bitmap pinctrl_setting_bitmap[MAX_PINS_PER_BANK];
};

struct pinctrl_bank_conf bank_data[MAX_PINCTL_BANKS] = PINCTL_BANK_CONF;

struct pinctrl_bank_conf __attribute__((section(".data"))) bank_data[MAX_PINCTL_BANKS];
u32 __attribute__((section(".data"))) pin_func_buf[MAX_PINS_PER_BANK];

int pinctl_set_pin_func(int bank, int pin_id, int func)
{
	struct pinctrl_bank_conf *pinctl =  &bank_data[bank];
	u32 reg, mask, func_num;
	struct pinctrl_setting_bitmap *pinctrl_setting_bitmap;

	pinctrl_setting_bitmap = pinctl->pinctrl_setting_bitmap;

	if (pin_id > pinctl->pin_cnt) {
		error("pin id %u is out of range!\n", pin_id);
		return -EINVAL;
	}

	/* check whether the func value is valid */
	func_num = pinctrl_setting_bitmap[pin_id].func_num;
	if (func >= func_num) {
		error("For pin %d, invalid function %d!\n", pin_id, func_num);
		return -EINVAL;
	}

	mask = pinctrl_setting_bitmap[pin_id].mask;
	/* The empty mask means the pin has a dedicated function. */
	if (!mask)
		return 0;

	reg = readl(pinctl->base_reg) & ~mask;
	reg = reg | pinctrl_setting_bitmap[pin_id].reg_values[func];
	writel(reg, pinctl->base_reg);

	debug("%s %d pid %d func 0x%x reg 0x%x\n", __func__, __LINE__, pin_id, func, reg);

	return 0;
}

int pinctl_get_pin_func(int bank, int pin_id)
{
	struct pinctrl_bank_conf *pinctl =  &bank_data[bank];
	int reg;
	int func;
	int func_num;
	u32 mask;
	struct pinctrl_setting_bitmap *pinctrl_setting_bitmap;

	pinctrl_setting_bitmap = pinctl->pinctrl_setting_bitmap;

	if (pin_id > pinctl->pin_cnt) {
		error("pin id %u is out of range!\n", pin_id);
		return -EINVAL;
	}

	func_num = pinctrl_setting_bitmap[pin_id].func_num;
	mask = pinctrl_setting_bitmap[pin_id].mask;

	if (!mask) {
		/* The empty mask means the pin has a dedicated function. */
		return 0;
	}

	reg = readl(pinctl->base_reg) & mask;

	for (func = 0; func < func_num; func++)
		if (reg == pinctrl_setting_bitmap[pin_id].reg_values[func])
			break;

	if (func == func_num) {
		error("Unknown config for pin %u, pinctrl reg value(0x%08x)!\n", pin_id, reg);
		return -EFAULT;
	}

	return func;
}

int pinctl_get_bank_id(const char *bank_name)
{
	int bank_id = -1;
	int bank;

	for (bank = 0; bank < MAX_PINCTL_BANKS; bank++) {
		if (strcmp(bank_name, bank_data[bank].bank_name) == 0)
			bank_id = bank;
	}

	if (bank_id == -1)
		error("pinctl: cannot match bank %s to bank_id\n", bank_name);

	return bank_id;
}

int pinctl_get_pin_cnt(int bank_id)
{
	if ((bank_id < 0) || (bank_id >= MAX_PINCTL_BANKS))
		return 0;

	return bank_data[bank_id].pin_cnt;
}

const char *pinctl_get_name(int bank_id)
{
	if ((bank_id < 0) || (bank_id >= MAX_PINCTL_BANKS))
		return NULL;

	return bank_data[bank_id].bank_name;
}

int mvebu_pinctl_probe(void)
{
	struct pinctrl_bank_conf *pinctl;
	const void *blob = gd->fdt_blob;
	int node_list[MAX_PINCTL_BANKS], node;
	int count, i, err, pin, pin_count;
	u32 *pin_func;
	const char *bank_name;

	debug_enter();

	count = fdtdec_find_aliases_for_id(gd->fdt_blob, "pinctl",
			COMPAT_MVEBU_PINCTL_ARMADA_3700, node_list, MAX_PINCTL_BANKS);

	if (count <= 0)
		return -ENODEV;

	if (count > MAX_PINCTL_BANKS) {
		error("pinctl: too many pinctl banks. using first %d banks\n", MAX_PINCTL_BANKS);
		count = MAX_PINCTL_BANKS;
	}

	for (i = 0; i < count; i++) {
		node = node_list[i];

		if (node <= 0)
			continue;

		pinctl = &bank_data[i];
		pinctl->base_reg = fdt_get_regs_offs(blob, node, "reg");
		pin_count = fdtdec_get_int(blob, node, "pin-count", MAX_PINS_PER_BANK);
		if (pinctl->pin_cnt != pin_count) {
			error("pinctl: pin_count of bank %s should be %d, not %d\n",
			      pinctl->bank_name, pinctl->pin_cnt, pin_count);
			continue;
		}
		bank_name = fdt_getprop(blob, node, "bank-name", NULL);
		if (strcmp(pinctl->bank_name, bank_name)) {
			error("pinctl: name of bank No.%d should be %s, not %s\n",
			      i, pinctl->bank_name, bank_name);
			continue;
		}
		pin_func = &pin_func_buf[0];

		err = fdtdec_get_int_array(blob, node, "pin-func", pin_func, pinctl->pin_cnt);
		if (err) {
			error("Failed reading pin functions for bank %s\n", pinctl->bank_name);
			continue;
		}

		for (pin = 0; pin < pinctl->pin_cnt; pin++) {
			if (pin_func[pin] != 0xff) {
				err = pinctl_set_pin_func(i, pin, pin_func[pin]);
				if (err)
					printf("Warning: pin %d is not set for bank %d\n", pin, i);
			} else {
				debug("Warning: pin %d value is not modified (kept as default\n", pin);
			}
		}
	}
	debug_exit();
	return 0;
}
