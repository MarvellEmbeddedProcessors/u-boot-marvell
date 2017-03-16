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
#include <dm.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <errno.h>

DECLARE_GLOBAL_DATA_PTR;

#define A37xx_GPIO_MAX_PORTS	2	/* max number of ports per bank */
#define A37xx_GPIOS_PER_PORT	32	/* number of gpios per port */


/* GPIO Controller registers for a single bank */
struct armada37xx_gpio_regs {
	u32 io_conf[A37xx_GPIO_MAX_PORTS];
	u32 reserved0[2];
	u32 data_in[A37xx_GPIO_MAX_PORTS];
	u32 data_out[A37xx_GPIO_MAX_PORTS];

};

struct armada37xx_gpio_priv {
	struct armada37xx_gpio_regs *regs;
	unsigned gpio_count;
	const char *name;
};

static int armada37xx_gpio_set_value(struct udevice *dev, unsigned gpio,
				int value)
{
	struct armada37xx_gpio_priv *priv = dev_get_priv(dev);
	struct armada37xx_gpio_regs *regs = priv->regs;

	if (gpio >= priv->gpio_count)
		return -1;

	if (value) {
		setbits_le32(&regs->data_out[gpio / A37xx_GPIOS_PER_PORT],
			     BIT(gpio % A37xx_GPIOS_PER_PORT));
	} else {
		clrbits_le32(&regs->data_out[gpio / A37xx_GPIOS_PER_PORT],
			     BIT(gpio % A37xx_GPIOS_PER_PORT));
	}

	return 0;
}

static int armada37xx_gpio_direction_input(struct udevice *dev,
				unsigned int gpio)
{
	struct armada37xx_gpio_priv *priv = dev_get_priv(dev);
	struct armada37xx_gpio_regs *regs = priv->regs;

	if (gpio >= priv->gpio_count)
		return -1;

	clrbits_le32(&regs->io_conf[gpio / A37xx_GPIOS_PER_PORT],
		     BIT(gpio % A37xx_GPIOS_PER_PORT));

	return 0;
}

static int armada37xx_gpio_direction_output(struct udevice *dev, unsigned gpio,
				int value)
{
	struct armada37xx_gpio_priv *priv = dev_get_priv(dev);
	struct armada37xx_gpio_regs *regs = priv->regs;
	int ret;

	if (gpio >= priv->gpio_count)
		return -1;

	ret = armada37xx_gpio_set_value(dev, gpio, value);
	if (ret < 0)
		return -1;

	setbits_le32(&regs->io_conf[gpio / A37xx_GPIOS_PER_PORT],
		     BIT(gpio % A37xx_GPIOS_PER_PORT));

	return 0;
}

static int armada37xx_gpio_get_function(struct udevice *dev, unsigned gpio)
{
	struct armada37xx_gpio_priv *priv = dev_get_priv(dev);
	struct armada37xx_gpio_regs *regs = priv->regs;
	u32 val;

	if (gpio >= priv->gpio_count)
		return -1;

	val = readl(&regs->io_conf[gpio / A37xx_GPIOS_PER_PORT]) &
		    BIT(gpio % A37xx_GPIOS_PER_PORT);

	if (val)
		return GPIOF_OUTPUT;
	else
		return GPIOF_INPUT;
}

static int armada37xx_gpio_get_value(struct udevice *dev, unsigned gpio)
{
	struct armada37xx_gpio_priv *priv = dev_get_priv(dev);
	struct armada37xx_gpio_regs *regs = priv->regs;
	int direction;

	if (gpio >= priv->gpio_count)
		return -1;

	direction = armada37xx_gpio_get_function(dev, gpio);
	if (direction == GPIOF_INPUT) {
		return !!(readl(&regs->data_in[gpio / A37xx_GPIOS_PER_PORT]) &
		    BIT(gpio % A37xx_GPIOS_PER_PORT));
	} else {
		return !!(readl(&regs->data_out[gpio / A37xx_GPIOS_PER_PORT]) &
		    BIT(gpio % A37xx_GPIOS_PER_PORT));
	}
}

static int armada37xx_gpio_probe(struct udevice *dev)
{
	struct gpio_dev_priv *uc_priv = dev_get_uclass_priv(dev);
	struct armada37xx_gpio_priv *priv = dev_get_priv(dev);
	int node = dev->of_offset;
	const void *blob = gd->fdt_blob;

	priv->gpio_count = fdtdec_get_int(blob, node, "ngpios", 0);
	priv->regs = (struct armada37xx_gpio_regs *)dev_get_addr(dev);
	priv->name = fdt_getprop(gd->fdt_blob, dev_of_offset(dev),
		"bank-name", NULL);

	uc_priv->gpio_count = priv->gpio_count;
	uc_priv->bank_name = priv->name;

	return 0;
}

static const struct dm_gpio_ops armada37xx_gpio_ops = {
	.direction_input	= armada37xx_gpio_direction_input,
	.direction_output	= armada37xx_gpio_direction_output,
	.get_value		= armada37xx_gpio_get_value,
	.set_value		= armada37xx_gpio_set_value,
	.get_function		= armada37xx_gpio_get_function,
};

static const struct udevice_id armada37xx_gpio_ids[] = {
	{ .compatible = "marvell,armada-3700-gpio" },
	{ }
};

U_BOOT_DRIVER(gpio_armada_3700) = {
	.name	= "gpio_armada_3700",
	.id	= UCLASS_GPIO,
	.of_match = armada37xx_gpio_ids,
	.ops	= &armada37xx_gpio_ops,
	.probe = armada37xx_gpio_probe,
	.priv_auto_alloc_size = sizeof(struct armada37xx_gpio_priv),
};

