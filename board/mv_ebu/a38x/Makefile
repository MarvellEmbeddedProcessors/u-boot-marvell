include $(TOPDIR)/config.mk
include ../common/paths.mk

#### Objects list #####
COBJS = mv_main_a38x.o \
	cmd_stage_boot.o \
	cmd_boot_menu.o
COBJS-$(CONFIG_ARMADA_39X) = cmd_sar39x.o
COBJS-$(CONFIG_ARMADA_38X) = cmd_sar38x.o
COBJS += $(sort $(COBJS-y))

LIB	= $(obj)lib$(BOARD).o

SRCS	:= $(SOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))
SOBJS	:= $(addprefix $(obj),$(SOBJS))

$(LIB):	$(obj).depend $(OBJS)
	$(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
