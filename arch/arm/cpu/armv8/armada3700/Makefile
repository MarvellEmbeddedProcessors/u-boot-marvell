#
# ***************************************************************************
# Copyright (C) 2015 Marvell International Ltd.
# ***************************************************************************
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation, either version 2 of the License, or any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
# ***************************************************************************
#

obj-y += soc-init.o
obj-y += clock.o
obj-$(CONFIG_ARMV8_PSCI)	+= psci.o
obj-$(CONFIG_MVEBU_A3700_IO_ADDR_DEC) += mvebu_io_addr_dec.o
obj-$(CONFIG_MVEBU_GPIO) += gpio.o
obj-$(CONFIG_MVEBU_A3700_MISC_INIT) += mvebu_misc_init.o
obj-$(CONFIG_MVEBU_A3700_CLOCK) += mvebu_clock.o
obj-$(CONFIG_MVEBU_BOOTMODE_SWITCH_SUPPORT) += mvebu_bootmode.o
obj-$(CONFIG_MVEBU_EFUSE_SUPPORT) += a3700_mbox.o
obj-$(CONFIG_MVEBU_A3700_PM) += pm.o
obj-$(CONFIG_MVEBU_A3700_AVS) += avs.o
