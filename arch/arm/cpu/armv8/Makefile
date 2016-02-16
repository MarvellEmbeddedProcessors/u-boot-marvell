#
# (C) Copyright 2000-2003
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# SPDX-License-Identifier:	GPL-2.0+
#

extra-y	:= start.o

obj-y	+= cpu.o
obj-y	+= generic_timer.o
obj-y	+= cache_v8.o
obj-y	+= exceptions.o
obj-y	+= cache.o
obj-y	+= tlb.o
obj-y	+= transition.o
obj-y	+= cpu-dt.o

obj-$(CONFIG_ARMV8_PSCI)	+= psci.o
obj-$(CONFIG_FSL_LSCH3) += fsl-lsch3/
obj-$(if $(filter armada8k,$(SOC)),y) += armada8k/
obj-$(if $(filter armada3700,$(SOC)),y) += armada3700/
