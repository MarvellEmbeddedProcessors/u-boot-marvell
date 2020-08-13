ifeq ($(CONFIG_ARCH_CN10K),y)
PLATFORM_CPPFLAGS += $(call cc-option,-march=armv8.2-a,)
endif
