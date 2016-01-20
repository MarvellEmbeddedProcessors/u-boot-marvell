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

#ifndef __ARCH_MVEBU_GPIO_H__
#define __ARCH_MVEBU_GPIO_H__

#define GPIO_ACTIVE_HIGH	0
#define GPIO_ACTIVE_LOW		1

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
int gpio_set_polarity(unsigned gpio, unsigned polarity);

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
int gpio_get_polarity(unsigned gpio);

#endif /* __ARCH_MVEBU_GPIO_H__ */
