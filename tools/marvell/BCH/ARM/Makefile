SHELL = /bin/sh
CPPDIR = ../Src/Example
INTDIR = .

CROSS_COMPILE = arm-marvell-linux-uclibcgnueabi-
#CROSS_COMPILE = arm-marvell-linux-gnueabi-

ARCH = arm

# Final executable output file
PRODUCT = test_arm

CC = $(CROSS_COMPILE)g++

CFLAGS = -O2 -ansi $(INCS)

LIBNAME = mrvlFlashBCH_uclibc.so
#LIBNAME = mrvlFlashBCH_glibc.so

CPP_SOURCES = $(CPPDIR)/driver_mrvlFlashBCH_linux.cpp

OBJECTS = $(CPP_SOURCES:%.cpp=$(INTDIR)/%.o)

# link
$(PRODUCT) : $(OBJECTS)
	$(CC) $(LINKFLAGS) $(OBJECTS) $(LIBNAME) -o $@
	@rm -f $(CPPDIR)/driver_mrvlFlashBCH_linux.o

# compile
$(INTDIR)/%.o : $(CPPDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@
