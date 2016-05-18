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

#ifndef __MVEBU_GPIO_H__
#define __MVEBU_GPIO_H__

/**
 * mvebu_soc_gpio_request: request a GPIO. This should be called before any of the other functions
 * are used on this GPIO.
 *
 *
 * @gpio: gpio global id
 * @param label User label for this GPIO
 * @return 0 if ok, others on failure
 */
int mvebu_soc_gpio_request(unsigned gpio, const char *label);

/**
 * mvebu_soc_gpio_free - Stop using the GPIO.  This function should not alter pin configuration.
 *
 * @gpio: gpio global id
 * @return 0 if ok, others on failure
 */
int mvebu_soc_gpio_free(unsigned gpio);

/**
 * mvebu_soc_gpio_set_direction - Set GPIO's direction
 *
 * @param gpio: gpio global id
 * @param direction: GPIO direction, 0(GPIOF_INPUT), 1(GPIOF_OUTPUT)
 * @return 0 if ok, others on failure
 */
int mvebu_soc_gpio_set_direction(unsigned gpio, int direction);

/**
 * mvebu_soc_gpio_get_direction - get GPIO's direction
 *
 * @param gpio: gpio global id
 * @return gpio direction 0(GPIOF_INPUT), 1(GPIOF_OUTPUT), others failure
 */
int mvebu_soc_gpio_get_direction(unsigned gpio);

/**
 * mvebu_soc_gpio_set_value - Set an output GPIO's value. The GPIO must already be an output or
 * this function may have no effect.
 *
 * @param gpio	gpio global id
 * @param value	GPIO value (0 for low or 1 for high)
 * @return 0 if ok, others on failure
 */
int mvebu_soc_gpio_set_value(unsigned gpio, int value);

/**
 * mvebu_soc_gpio_get_value - Get a GPIO's value. This will work whether the GPIO is an input
 * or an output.
 *
 * @param gpio	gpio global id
 * @return 0 if low, 1 if high, -1 on error
 */
int mvebu_soc_gpio_get_value(unsigned gpio);

/**
 * mvebu_soc_gpio_set_polarity - set GPIO pin polarity
 * input
 * @gpio: gpio global id
 * @polarity: pin polarity
 * ouput
 * none
 *
 * Returns 0 on success, others on failure
 *
 **/
int mvebu_soc_gpio_set_polarity(unsigned gpio, int polarity);

/**
 * mvebu_soc_gpio_get_polarity - get gpio pin polarity
 * input
 * @gpio: gpio global id
 * ouput
 * none
 *
 * Returns gpio pin polarity on success, -EPERM on failure
 *
 **/
int mvebu_soc_gpio_get_polarity(unsigned gpio);

/**
 * mvebu_soc_gpio_init - GPIO bank initialization
 * @dev: gpio bank device including bank name, gpio count and gpio base
 * @regs_base: gpio registers base address
 * @node_offset: gpio bank fdt node offset, soc driver can get its private
 *                      information from it althrough here armada3700 does not use it
 *
 * Returns valid bank id on success negative error values on failure
 *
 **/
int mvebu_soc_gpio_init(struct gpio_dev_priv *dev, void *regs_base, int node_offset);

#endif /* __MVEBU_GPIO_H__ */
