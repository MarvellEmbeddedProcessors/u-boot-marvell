include $(TOPDIR)/config.mk
include ../common/paths.mk

#### Objects list #####
COBJS = mv_main_a375.o \
	cmd_sar.o

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
