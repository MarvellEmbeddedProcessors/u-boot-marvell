## Makefile.am -- Process this file with automake to produce Makefile.in

lib_LTLIBRARIES         = lib@PACKAGE_NAME@.la
lib@PACKAGE_NAME@_la_LDFLAGS = -avoid-version @EXTRA_LIBTOOL_ARGS_DLL@
lib@PACKAGE_NAME@_la_LIBADD  = @EXTRA_LDFLAGS@
lib@PACKAGE_NAME@_la_CFLAGS  = -Iinclude

include_HEADERS = \
        include/aapl.h \
        include/system_includes.h \
        include/aapl_library.h \
        include/aapl_core.h \
        include/sbus.h \
        include/mdio.h \
        include/gpio_mdio.h \
        include/i2c.h \
        include/spico.h \
        include/asic_info.h \
        include/serdes_core.h \
        include/pmd.h \
        include/logging.h

lib@PACKAGE_NAME@_la_SOURCES = \
        library.c \
        aapl_core.c \
        aacs.c \
        sbus.c \
        mdio.c \
        gpio_mdio.c \
        i2c.c \
        spico.c \
        user_code.c \
        serdes_core.c \
        pmd.c \
        logging.c
