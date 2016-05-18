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
#include <asm/errno.h>
#include <asm/io.h>
#include <asm/gpio.h>
#include <asm/arch-mvebu/fdt.h>
#include <asm/arch-mvebu/pinctl.h>

DECLARE_GLOBAL_DATA_PTR;

/* GPIO Controller registers for a single bank */
struct mvebu_gpio_regs {
	u32 gdo;	/* GPIO Data-Out */
	u32 gdoec;	/* GPIO Data-Out Enable Control */
	u32 gbe;	/* GPIO Blink-Enable */
	u32 gdip;	/* GPIO Data-In Polarity */
	u32 gdi;	/* GPIO Data-In */
	u32 gic;	/* GPIO Interrupt Cause */
	u32 gim;	/* GPIO Interrupt Mask */
	u32 gilm;	/* GPIO Interrupt Level Mask */
	u32 gbcs;	/* GPIO Blink Counter Select */
	u32 gcs;	/* GPIO Counter Set */
	u32 gcc;	/* GPIO Counter Clear */
	u32 gdos;	/* GPIO Data-out Set */
	u32 gdoc;	/* GPIO Data-out Clear */
};

struct mvebu_gpio_bank {
	struct gpio_dev_priv *dev;
	struct mvebu_gpio_regs *regs_base;
};

static struct mvebu_gpio_bank mvebu_gpio_banks[MVEBU_GPIO_BANKS];

/**
 * mvebu_soc_gpio_to_bank_port_offset - get gpio bank number, pin offset in th bank by gpio number
 * input
 * @gpio: global gpio id
 * output
 * @bank: bank id
 * @offset: gpio pin offset in the bank.
 *
 * Returns 0 on success, others on failure
 *
 **/
static int mvebu_soc_gpio_to_bank_port_offset(unsigned gpio,
						unsigned *bank,
						unsigned *offset)
{
	unsigned int i;
	unsigned int gpio_start, gpio_end;
	struct mvebu_gpio_bank *gpio_bank;

	/* Search the gpio bank with the global GPIO ID. */
	/* Each gpio bank has some gpios with continuous global GPIO IDs. */
	/* A bank's gpio IDs start from gpio_bank->dev->gpio_base. */
	/* A bank's gpio IDs' count is gpio_bank->dev->gpio_count. */
	for (i = 0; i < MVEBU_GPIO_BANKS; i++) {
		gpio_bank = mvebu_gpio_banks + i;
		if (!gpio_bank->dev)
			continue;
		gpio_start = gpio_bank->dev->gpio_base;
		gpio_end = gpio_bank->dev->gpio_base + gpio_bank->dev->gpio_count;
		if ((gpio >= gpio_start) && (gpio < gpio_end))
			break;
	}

	if (MVEBU_GPIO_BANKS == i) {
		printf("ERROR: gpio %d is not found in any banks!\n", gpio);
		return -EPERM;
	}

	*bank = i;
	*offset = (gpio - gpio_bank->dev->gpio_base) % MVEBU_GPIOS_PER_PORT;

	return 0;
}

/**
 * mvebu_soc_gpio_request: request a GPIO. This should be called before any of the other functions
 * are used on this GPIO.
 *
 *
 * @gpio: gpio global id
 * @param: label User label for this GPIO
 * @return 0 if ok, others on failure
 */
int mvebu_soc_gpio_request(unsigned gpio, const char *label)
{
	unsigned bank, offset;
	int bank_pincntrl;

	if (0 != mvebu_soc_gpio_to_bank_port_offset(gpio, &bank, &offset) || !mvebu_gpio_banks[bank].regs_base)
		return -EPERM;

	/* Get bank ID of the pin in the pinctrl driver */
	for (bank_pincntrl = 0; bank_pincntrl < MVEBU_GPIO_BANKS; bank_pincntrl++) {
		if (gpio < pinctl_get_pin_cnt(bank_pincntrl))
			break;
		gpio -= pinctl_get_pin_cnt(bank_pincntrl);
	}

	if (bank_pincntrl == MVEBU_GPIO_BANKS) {
		printf("ERROR: pin %d is not found in any pinctrl banks!\n", gpio);
		return -EPERM;
	}

	/* Set PIN function as GPIO */
	pinctl_set_pin_func(bank_pincntrl, gpio, 0);

	return 0;
}

/**
 * mvebu_soc_gpio_free - Stop using the GPIO.  This function should not alter pin configuration.
 *
 * @gpio: gpio global id
 * @return 0 if ok, others on failure
 */
int mvebu_soc_gpio_free(unsigned gpio)
{
	/* Keep this function empty and the requested GPIO as gpio function
	** the driver module release the GPIO, and can use it for other purpuse */
	return 0;
}

/**
 * mvebu_soc_gpio_set_direction - Set GPIO's direction
 *
 * @param gpio: gpio global id
 * @param direction: GPIO direction, 0(GPIOF_INPUT), 1(GPIOF_OUTPUT)
 * @return 0 if ok, others on failure
 */
int mvebu_soc_gpio_set_direction(unsigned gpio, int direction)
{
	u32 val;
	unsigned bank, offset;

	if (0 != mvebu_soc_gpio_to_bank_port_offset(gpio, &bank, &offset) || !mvebu_gpio_banks[bank].regs_base)
		return -EPERM;

	val = (u32)readl(&mvebu_gpio_banks[bank].regs_base->gdoec);

	if (direction == GPIOF_OUTPUT)
		val &= ~(1 << offset);
	else
		val |= (1 << offset);

	writel(val, &mvebu_gpio_banks[bank].regs_base->gdoec);

	return 0;
}

/**
 * mvebu_soc_gpio_get_direction - get GPIO's direction
 *
 * @param gpio: gpio global id
 * @return gpio direction 0(GPIOF_INPUT), 1(GPIOF_OUTPUT), others failure
 */
int mvebu_soc_gpio_get_direction(unsigned gpio)
{
	u32 val;
	unsigned bank, offset;

	if (0 != mvebu_soc_gpio_to_bank_port_offset(gpio, &bank, &offset) || !mvebu_gpio_banks[bank].regs_base)
		return -EPERM;

	val = readl(&mvebu_gpio_banks[bank].regs_base->gdoec);
	if (val & (1 << offset))
		return GPIOF_INPUT;
	else
		return GPIOF_OUTPUT;
}

/**
 * mvebu_soc_gpio_set_value - Set an output GPIO's value. The GPIO must already be an output or
 * this function may have no effect.
 *
 * @param gpio	gpio global id
 * @param value	GPIO value (0 for low or 1 for high)
 * @return 0 if ok, others on failure
 */
int mvebu_soc_gpio_set_value(unsigned gpio, int value)
{
	u32 val;
	unsigned bank, offset;

	if (0 != mvebu_soc_gpio_to_bank_port_offset(gpio, &bank, &offset) || !mvebu_gpio_banks[bank].regs_base)
		return -EPERM;

	val = readl(&mvebu_gpio_banks[bank].regs_base->gdo);
	if (value)
		val |= (1 << offset);
	else
		val &= ~(1 << offset);
	writel(val, &mvebu_gpio_banks[bank].regs_base->gdo);

	return 0;
}

/**
 * mvebu_soc_gpio_get_value - Get a GPIO's value. This will work whether the GPIO is an input
 * or an output.
 *
 * @param gpio	gpio global id
 * @return 0 if low, 1 if high, -1 on error
 */
int mvebu_soc_gpio_get_value(unsigned gpio)
{
	u32 val;
	unsigned bank, offset;
	int direction;
	u32 *gplr;

	if (0 != mvebu_soc_gpio_to_bank_port_offset(gpio, &bank, &offset) || !mvebu_gpio_banks[bank].regs_base)
		return -EPERM;

	direction = mvebu_soc_gpio_get_direction(gpio);
	if (direction == GPIOF_INPUT)
		gplr = &mvebu_gpio_banks[bank].regs_base->gdi;
	else
		gplr = &mvebu_gpio_banks[bank].regs_base->gdo;

	val = readl(gplr);
	val &= (1 << offset);

	return (val >> offset) & 0x01;
}

int mvebu_soc_gpio_set_polarity(unsigned gpio, int polarity)
{
	u32 val;
	unsigned bank, offset;

	if (0 != mvebu_soc_gpio_to_bank_port_offset(gpio, &bank, &offset) || !mvebu_gpio_banks[bank].regs_base)
		return -EPERM;

	val = readl(&mvebu_gpio_banks[bank].regs_base->gdip);
	if (val)
		val |= (1 << offset);
	else
		val &= ~(1 << offset);
	writel(val, &mvebu_gpio_banks[bank].regs_base->gdip);

	return 0;
}

int mvebu_soc_gpio_get_polarity(unsigned gpio)
{
	u32 val;
	unsigned bank, offset;

	if (0 != mvebu_soc_gpio_to_bank_port_offset(gpio, &bank, &offset) || !mvebu_gpio_banks[bank].regs_base)
		return -EPERM;

	val = readl(&mvebu_gpio_banks[bank].regs_base->gdip);
	val &= (1 << offset);

	return (val >> offset) & 0x01;

}

/**
 * mvebu_soc_gpio_init - GPIO bank initialization
 * @dev: gpio bank device including bank name, gpio count and gpio base
 * @regs_base: gpio registers base address
 * @node_offset: gpio bank fdt node offset, soc driver can get its private
 *                      information from it.
 *
 * Returns valid bank id on success negative error values on failure
 *
 **/
int mvebu_soc_gpio_init(struct gpio_dev_priv *dev, void *regs_base, int node_offset)
{
	unsigned int bank;

	/* find an empty bank */
	for (bank = 0; bank < MVEBU_GPIO_BANKS; bank++)
		if (NULL == mvebu_gpio_banks[bank].dev)
			break;

	if (bank >= MVEBU_GPIO_BANKS)
		return -EPERM;

	mvebu_gpio_banks[bank].dev = dev;

	/* renumber gpio bank's gpio_base */
	if (bank == 0)
		mvebu_gpio_banks[bank].dev->gpio_base = 0;
	else
		mvebu_gpio_banks[bank].dev->gpio_base = mvebu_gpio_banks[bank - 1].dev->gpio_base
							+ mvebu_gpio_banks[bank - 1].dev->gpio_count;
	mvebu_gpio_banks[bank].regs_base = (struct mvebu_gpio_regs *)regs_base;

	return bank;
}
