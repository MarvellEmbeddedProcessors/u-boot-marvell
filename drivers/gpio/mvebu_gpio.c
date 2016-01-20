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
#include <asm/errno.h>
#include <asm/gpio.h>
#include <asm/arch-mvebu/mvebu_gpio.h>
#include <fdtdec.h>
#include <malloc.h>
#include <asm/arch-mvebu/fdt.h>
#include <dm.h>


DECLARE_GLOBAL_DATA_PTR;

struct mvebu_gpio_platdata {
	const char *bank_name;
	unsigned gpio_count;
	void *bank_regs_base;	/* gpio bank registers base address*/
};

static unsigned mvebu_gpio_max_num;

/*
 * GPIO polarity implementation.
 */

__attribute__((weak))
int mvebu_soc_gpio_set_polarity(unsigned gpio, int polarity)
{
	if (gpio >= mvebu_gpio_max_num)
		return -EPERM;

	/* ACTIVE_HIGH is the default value for gpio polarity. */
	/* Even polarity is not supported by SoC, it's the default status. */
	/* Here, we have this weak implementation for most SoCs which doesn't support polarity setting */
	if (GPIO_ACTIVE_HIGH == polarity) {
		return 0;
	} else {
		printf("ERROR: polarity reverting is not supported\n");
		return -EPERM;
	}
}

/**
 * gpio_set_polarity - set gpio pin polarity
 * input
 * @gpio: gpio global id
 * @polarity: pin polarity
 * ouput
 * none
 *
 * Returns 0 on success, others on failure
 *
 **/
int gpio_set_polarity(unsigned gpio, unsigned polarity)
{
	return mvebu_soc_gpio_set_polarity(gpio, polarity);
}

__attribute__((weak))
int mvebu_soc_gpio_get_polarity(unsigned gpio)
{
	if (gpio >= mvebu_gpio_max_num)
		return -EPERM;

	/* ACTIVE_HIGH is the default value for gpio polarity. */
	/* Even polarity is not supported by SoC, it's the default status. */
	/* Here, we have this weak implementation for most SoCs which doesn't support polarity setting */
	/* By default, if no polarity support, it is always ACTIVE_HIGH. */
	return GPIO_ACTIVE_HIGH;
}

/**
 * gpio_get_polarity - get gpio pin polarity
 * input
 * @gpio: gpio global id
 * ouput
 * none
 *
 * Returns gpio pin polarity on success, -EPERM on failure
 *
 **/
int gpio_get_polarity(unsigned gpio)
{
	return mvebu_soc_gpio_get_polarity(gpio);
}

/*
 * Generic GPIO primitives.
 */

static int mvebu_gpio_request(struct udevice *dev, unsigned offset,
			      const char *label)
{
	struct gpio_dev_priv *uc_priv = (struct gpio_dev_priv *)dev->uclass_priv;

	return mvebu_soc_gpio_request(uc_priv->gpio_base + offset, label);
}

static int mvebu_gpio_free(struct udevice *dev, unsigned offset)
{
	struct gpio_dev_priv *uc_priv = (struct gpio_dev_priv *)dev->uclass_priv;

	return mvebu_soc_gpio_free(uc_priv->gpio_base + offset);
}

/* set GPIO pin 'gpio' as an input */
static int mvebu_gpio_direction_input(struct udevice *dev, unsigned offset)
{
	struct gpio_dev_priv *uc_priv = (struct gpio_dev_priv *)dev->uclass_priv;

	return mvebu_soc_gpio_set_direction(uc_priv->gpio_base + offset, GPIOF_INPUT);
}

/* set GPIO pin 'gpio' in output mode with output 'value' */
static int mvebu_gpio_direction_output(struct udevice *dev, unsigned offset,
				       int value)
{
	int ret;
	struct gpio_dev_priv *uc_priv = (struct gpio_dev_priv *)dev->uclass_priv;

	ret = mvebu_soc_gpio_set_value(uc_priv->gpio_base + offset, value);
	if (ret < 0)
		return ret;

	return mvebu_soc_gpio_set_direction(uc_priv->gpio_base + offset, GPIOF_OUTPUT);
}

/* read GPIO IN value of pin 'gpio' */
static int mvebu_gpio_get_value(struct udevice *dev, unsigned offset)
{
	struct gpio_dev_priv *uc_priv = (struct gpio_dev_priv *)dev->uclass_priv;

	return mvebu_soc_gpio_get_value(uc_priv->gpio_base + offset);
}


/* write GPIO OUT value to pin 'gpio' */
static int mvebu_gpio_set_value(struct udevice *dev, unsigned offset, int value)
{
	struct gpio_dev_priv *uc_priv = (struct gpio_dev_priv *)dev->uclass_priv;

	return mvebu_soc_gpio_set_value(uc_priv->gpio_base + offset, value);
}

/* read GPIO direction */
static int mvebu_gpio_get_function(struct udevice *dev, unsigned offset)
{
	struct gpio_dev_priv *uc_priv = (struct gpio_dev_priv *)dev->uclass_priv;

	return mvebu_soc_gpio_get_direction(uc_priv->gpio_base + offset);
}

static const struct dm_gpio_ops mvebu_gpio_ops = {
	.request		= mvebu_gpio_request,
	.free			= mvebu_gpio_free,
	.direction_input	= mvebu_gpio_direction_input,
	.direction_output	= mvebu_gpio_direction_output,
	.get_value		= mvebu_gpio_get_value,
	.set_value		= mvebu_gpio_set_value,
	.get_function		= mvebu_gpio_get_function,
};

static const struct udevice_id mvebu_gpio_ids[] = {
	{ .compatible = "marvell,mvebu-gpio" },
	{ }
};

static int mvebu_gpio_ofdata_to_platdata(struct udevice *bank)
{
	struct mvebu_gpio_platdata *plat = bank->platdata;
	const void *blob = gd->fdt_blob;
	int node = bank->of_offset;

	fdt_get_string(blob, node, "bank-name", &plat->bank_name);
	plat->gpio_count = fdtdec_get_int(blob, node, "ngpios", 0);
	plat->bank_regs_base = fdt_get_regs_offs(blob, node, "reg");
	if (plat->bank_regs_base == NULL) {
		error("Missing registers in %s gpio node\n", plat->bank_name);
		return -FDT_ERR_NOTFOUND;
	}

	return 0;
}

static int mvebu_gpio_probe(struct udevice *bank)
{
	struct mvebu_gpio_platdata *plat = (struct mvebu_gpio_platdata *)dev_get_platdata(bank);
	struct gpio_dev_priv *uc_priv = (struct gpio_dev_priv *)bank->uclass_priv;
	int node = bank->of_offset;
	unsigned int bank_id;

	uc_priv->gpio_count = plat->gpio_count;
	uc_priv->bank_name = plat->bank_name;

	/* do the soc gpio bank specific initialization such as the preparation work
	 * for mapping the gpio global id into its gpio bank and the pin offset of the bank
	 */
	bank_id = mvebu_soc_gpio_init(uc_priv, plat->bank_regs_base, node);
	if (bank_id < 0) {
		error("bank %s initialization failed!\n", uc_priv->bank_name);
		return -EPERM;
	}

	mvebu_gpio_max_num += uc_priv->gpio_count;

	return 0;
}

U_BOOT_DRIVER(mvebu_gpio) = {
	.name	= "mvebu_gpio",
	.id	= UCLASS_GPIO,
	.of_match = mvebu_gpio_ids,
	.ops	= &mvebu_gpio_ops,
	.ofdata_to_platdata = mvebu_gpio_ofdata_to_platdata,
	.platdata_auto_alloc_size = sizeof(struct mvebu_gpio_platdata),
	.probe = mvebu_gpio_probe,
};


