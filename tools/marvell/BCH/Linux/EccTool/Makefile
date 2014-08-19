SRC_DIR = .
OBJ_DIR = ./OBJ/
LIB_DIR = ./OBJ/

ECC_TOOL = ecc_tool

CFLAGS = -ansi -m32 -w -fPIC -O2 -g -D_CONSOLE -DSIG_SYS=-1 -DUNIX -DHAS_MRVL_FLASH_LDPC_LIB

default: $(OBJ_DIR)$(ECC_TOOL)

OBJS =	$(OBJ_DIR)ndPageInfo.o \
	$(OBJ_DIR)buildEccImage.o \
	$(OBJ_DIR)eccToolMain.o \
	../mrvlFlashBCH_lib.so
#LIBS = -lmrvlFlashBCH

$(OBJ_DIR)$(ECC_TOOL): $(OBJS)
	g++ -m32 -Wall -L$(LIB_DIR) -o $@ $(OBJS) $(LIBS) -Wl,-R -Wl,./

.SUFFIXES: .cpp .cc .o

$(OBJ_DIR)%.o : %.cpp
	@if test ! -d $(OBJ_DIR); then \
		mkdir $(OBJ_DIR); \
	fi
#	cp ../mrvlFlashBCH_lib.so $(OBJ_DIR)libmrvlFlashBCH.so
	g++ $(CFLAGS) -I$(SRC_DIR) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)
