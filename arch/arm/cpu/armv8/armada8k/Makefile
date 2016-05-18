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

obj-y += soc.o
obj-$(CONFIG_SPL_BUILD) += spl.o
obj-y += clock.o
obj-$(CONFIG_ARMV8_PSCI)	+= psci.o
obj-y	+= cache_llc.o
obj-$(CONFIG_MVEBU_CCU) += mvebu_ccu.o
obj-$(CONFIG_MVEBU_RFU) += mvebu_rfu.o
obj-$(CONFIG_MVEBU_IOB) += mvebu_iob.o
obj-$(CONFIG_MVEBU_AMB_ADEC) += mvebu_amb_adec.o
obj-$(CONFIG_MVEBU_FLC) += mvebu_flc.o
obj-$(CONFIG_MVEBU_GPIO) += gpio.o
