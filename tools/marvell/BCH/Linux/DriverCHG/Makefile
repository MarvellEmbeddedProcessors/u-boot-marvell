# make file example for BCH parity generation

SRC_DIR = .
OBJ_DIR = .
LIB_DIR = ..

BCH_MAIN = bch_test

CFLAGS = -ansi -m32 -w -fPIC -O2 -g -D_CONSOLE -DSIG_SYS=-1 -DUNIX

default: $(BCH_MAIN)

# get object files
OBJECTS = $(LIB_DIR)/mrvlFlashBCH_lib.so

$(OBJ_DIR)/$(BCH_MAIN).o : $(SRC_DIR)/driver_mrvlFlashBCH_linuxCHG.cpp
	g++ $(CFLAGS) -I$(SRC_DIR) -c $< -o $@

# link
$(BCH_MAIN): $(OBJECTS) $(OBJ_DIR)/$(BCH_MAIN).o
	g++ -m32 -Wall -o $@ ./$(BCH_MAIN).o $(OBJECTS)

clean :
	rm $(OBJ_DIR)/$(BCH_MAIN).o $(BCH_MAIN)
