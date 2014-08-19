SHELL = /bin/sh
# Place driver_mrvlFlashBCH_linux.cpp in CPPDIR directory.
# Place mrvlFlashBCH_lib.so in INTDIR directory.
CPPDIR = ../Src
INTDIR = .

# Final executable output file
PRODUCT = $(INTDIR)/test_x86

GCCDIR = /usr
GCC_INCLUDE_DIR = $(GCCDIR)/include/c++/4.1.2
GCC_LIB_DIR = $(GCCDIR)/lib32
#GCC_LIB_DIR = $(GCCDIR)/lib

CC = $(GCCDIR)/bin/g++

CFLAGS = -m32 -O2 -ansi $(INCS)

LINKFLAGS = -m32 -L$(GCC_LIB_DIR) -L.

LIBNAME = $(INTDIR)/mrvlFlashBCH_lib.so

CPP_SOURCES = $(CPPDIR)/driver_mrvlFlashBCH_linux.cpp

OBJECTS = $(CPP_SOURCES:%.cpp=$(INTDIR)/%.o)

# link
$(PRODUCT) : $(OBJECTS)
	$(CC) $(LINKFLAGS) $(OBJECTS) $(LIBNAME) -o $@
	@rm -f $(CPPDIR)/driver_mrvlFlashBCH_linux.o

# compile
$(INTDIR)/%.o : $(CPPDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@
