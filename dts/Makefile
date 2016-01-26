#
# Copyright (c) 2011 The Chromium OS Authors.
#
# SPDX-License-Identifier:	GPL-2.0+
#

# This Makefile builds the internal U-Boot fdt if CONFIG_OF_CONTROL is
# enabled. See doc/README.fdt-control for more details.

DEVICE_TREE ?= $(CONFIG_DEFAULT_DEVICE_TREE:"%"=%)
ifeq ($(DEVICE_TREE),)
DEVICE_TREE := unset
endif
ifneq ($(EXT_DTB),)
DTB := $(EXT_DTB)
else
DTB := arch/$(ARCH)/dts/$(DEVICE_TREE).dtb
endif

$(obj)/dt.dtb: arch-dtbs
ifdef CONFIG_MULTI_DT_FILE
#The Makefile must sort the order of the list,
#since the selection of the DT in setup_fdt() is one fixed to the 1st DT element.
	$(eval DTB_WITHOUT_DEFAULT := $(shell echo arch/$(ARCH)/dts/*.dtb))
	$(eval DTB_WITHOUT_DEFAULT:= $(filter-out $(DTB), $(DTB_WITHOUT_DEFAULT)))
	$(eval DTB += $(DTB_WITHOUT_DEFAULT))
	@echo "List Of FDT files:"
	$(foreach i, $(DTB), $(eval DTB_PRINT_LIST += $(i)\n ))
	@echo "$(DTB_PRINT_LIST)"
endif
	$(Q)test -e $(firstword $(DTB)) || (						\
	echo >&2;							\
	echo >&2 "Device Tree Source is not correctly specified.";	\
	echo >&2 "Please define 'CONFIG_DEFAULT_DEVICE_TREE'";		\
	echo >&2 "or build with 'DEVICE_TREE=<device_tree>' argument";	\
	echo >&2;							\
	/bin/false)
	$(shell cat $(DTB) > $@)

targets += dt.dtb

arch-dtbs: clear-dtbs
	$(Q)$(MAKE) $(build)=arch/$(ARCH)/dts dtbs

clear-dtbs:
	$(shell rm -f arch/$(ARCH)/dts/*.dtb 2>/dev/null || true)

.SECONDARY: $(obj)/dt.dtb.S

obj-$(CONFIG_OF_EMBED) := dt.dtb.o

dtbs: $(obj)/dt.dtb
	@:

clean-files := dt.dtb.S

# Let clean descend into dts directories
subdir- += ../arch/arm/dts ../arch/microblaze/dts ../arch/sandbox/dts ../arch/x86/dts
