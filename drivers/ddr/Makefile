obj-y += mvebu_ddr.o
obj-$(CONFIG_MVEBU_DUNIT_SUPPORT) += ddr_dunit.o
ifdef CONFIG_MVEBU_MK6_SUPPORT
ifdef CONFIG_MVEBU_STATIC_SUPPORT
obj-$(CONFIG_TARGET_ARMADA_LP) +=ddr_mckinley6_arlp.o
obj-$(CONFIG_TARGET_ARMADA_8K) +=ddr_mckinley6_8k.o
endif# CONFIG_MVEBU_STATIC_SUPPORT
# in case of dynamic supprt include the below
ifdef CONFIG_MVEBU_DYNAMIC_SUPPORT
UBOOTINCLUDE += -I$(srctree)/drivers/ddr/mckinley_ddr/h
obj-y += mckinley_ddr/
endif# end of  CONFIG_MVEBU_DYNAMIC_SUPPORT

endif# end od CONFIG_MVEBU_MK6_SUPPORT
