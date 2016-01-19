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
obj-$(CONFIG_MVEBU_SYS_INFO) += system_info.o
obj-y += misc.o
obj-$(CONFIG_MVEBU_MPP_BUS) += mpp-bus.o
obj-y += fdt.o
obj-y += clock.o
obj-$(CONFIG_MVEBU_TIMER) += timer_mvebu.o
obj-$(CONFIG_GENERIC_TIMER) += generic_timer.o
obj-$(CONFIG_MVEBU_CA9) += platform.o
obj-$(CONFIG_MVEBU_SPL_DDR_OVER_PCI_SUPPORT) += dram_over_pci.o
obj-$(CONFIG_MVEBU_TOOLS_SUPPORT) += tools/
