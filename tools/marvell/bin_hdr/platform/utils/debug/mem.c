#include "command.h"
#include "printf.h"
#include "mvUart.h"
#include "lib_utils.h"
#include "mv_os.h"
#include "soc_spec.h"
#include "ddr3_hws_hw_training_def.h"
#include "stdint.h"


#define DISP_LINE_LEN 16
#define MAX_LINE_LENGTH_BYTES (64)
#define DEFAULT_LINE_LENGTH_BYTES (16)
#define REG_PHY_DATA		0
#define CONFIG_SYS_MEMTEST_START        0x00400000
#define CONFIG_SYS_MEMTEST_END          0x007fffff

static unsigned int dp_last_addr, dp_last_size;
static unsigned int dp_last_length = 0x40;
static unsigned int mm_last_addr, mm_last_size;

/*******************************************************************************
* printHex
*
* DESCRIPTION:
* Prints unsigned value in hex with specified width
*
* INPUT:
*	x		- value to print
*	width		- data value width.  May be 2, 4, or 8.
*
*******************************************************************************/
static void printHex(MV_U32 x, unsigned int width)
{
	switch (width) {
	case 8:
		mvPrintf(" %08x", x);
		break;
	case 4:
		mvPrintf(" %04x", x);
		break;
	default:
		mvPrintf(" %02x", x);
		break;
	}
}

/*******************************************************************************
* print_buffer
*
* DESCRIPTION:
* Print data buffer in hex and ascii form to the terminal.
* data reads are buffered so that each memory address is only read once.
* Useful when displaying the contents of volatile registers.
*
* INPUT:
*	addr		- Starting address to display at start of line
*	data		- pointer to data buffer
*	width		- data value width.  May be 1, 2, or 4.
*	count		- number of values to display
*	linelen		- Number of values to print per line
*				specify 0 for default length
*
*******************************************************************************/
void print_buffer(unsigned long addr, void *data, unsigned int width, unsigned int count, unsigned int linelen)
{
	/* linebuf as a union causes proper alignment */
	union linebuf {
		MV_U32 ui[MAX_LINE_LENGTH_BYTES/sizeof(MV_U32) + 1];
		MV_U16 us[MAX_LINE_LENGTH_BYTES/sizeof(MV_U16) + 1];
		MV_U8  uc[MAX_LINE_LENGTH_BYTES/sizeof(MV_U8) + 1];
	} lb;
	int i;

	if (linelen*width > MAX_LINE_LENGTH_BYTES)
		linelen = MAX_LINE_LENGTH_BYTES / width;
	if (linelen < 1)
		linelen = DEFAULT_LINE_LENGTH_BYTES / width;

	while (count) {
		mvPrintf("%08lx:", addr);

		/* check for overflow condition */
		if (count < linelen)
			linelen = count;

		/* Copy from memory into linebuf and print hex values */
		for (i = 0; i < linelen; i++) {
			MV_U32 x;
			if (width == 4)
				x = lb.ui[i] = *(volatile MV_U32*)data;
			else if (width == 2)
				x = lb.us[i] = *(volatile MV_U16*)data;
			else
				x = lb.uc[i] = *(volatile MV_U8*)data;
			printHex(x, width*2);
			data += width;
		}

		/* Print data in ASCII characters */
		for (i = 0; i < linelen * width; i++) {
			if (!isprint(lb.uc[i]) || lb.uc[i] >= 0x80)
				lb.uc[i] = '.';
		}
		lb.uc[i] = '\0';
		mvPrintf("    %s\n", lb.uc);

		/* update references */
		addr += linelen * width;
		count -= linelen;
	}
}

/*******************************************************************************
* align_address
*
* DESCRIPTION:
* Unaligned memory access Workaround:
* if size = long/word, & address not aligned to long/word (respectively)
* align address to meet requested size
*
* INPUT:
*	addr		- address in memory to align (1/2/4)
*	size		- size in bytes
*
* RETURN:
*	the aligned address
*
*******************************************************************************/
static unsigned long align_address(unsigned long addr, int size)
{

	if ((size > 1) && (addr % size > 0)) {
		mvPrintf("Error: Requested unaligned memory address (0x%x)\n", (unsigned int)addr);
		addr &= ~(size - 1);
		mvPrintf("Using aligned address (0x%x)\n", (unsigned int)addr);
	}
	return addr;
}

/*******************************************************************************
* do_mem_md
*
* DESCRIPTION:
* The command allows the user to display memory values
*
*******************************************************************************/
int do_mem_md(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned long addr, length;
	int size;
	int rc = 0;
	/* We use the last specified parameters, unless new ones are
	 * entered.
	 */
	addr = dp_last_addr;
	size = dp_last_size;
	length = dp_last_length;

	if (argc < 2)
		return CMD_RET_USAGE;

	if ((flag & CMD_FLAG_REPEAT) == 0) {
		/* New command specified.  Check for a size specification.
		 * Defaults to long if no or incorrect specification.
		 */
		size = cmd_get_data_size(argv[0], 4);
		if (size < 0)
			return 1;

		/* Address is specified since argc > 1
		*/
		addr = simple_strtoul(argv[1], NULL, 16);

		/* If another parameter, it is the length to display.
		 * Length is the number of objects, not number of bytes.
		 */
		if (argc > 2)
			length = simple_strtoul(argv[2], NULL, 16);
	}

	addr = align_address(addr, size);
	/* Print the lines. */
	print_buffer(addr, (void *)addr, size, length, DISP_LINE_LEN/size);
	addr += size*length;


	dp_last_addr = addr;
	dp_last_length = length;
	dp_last_size = size;
	return rc;
}

/*******************************************************************************
* do_mem_mw
*
* DESCRIPTION:
* The command allows the user to write to memory
*
*******************************************************************************/
int do_mem_mw(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned long addr, writeval, count;
	int size;

	if ((argc < 3) || (argc > 4))
		return CMD_RET_USAGE;

	/* Check for size specification.
	*/
	size = cmd_get_data_size(argv[0], 4);
	if (size < 1)
		return 1;

	/* Address is specified since argc > 1
	*/
	addr = simple_strtoul(argv[1], NULL, 16);

	addr = align_address(addr, size);

	/* Get the value to write.
	*/
	writeval = simple_strtoul(argv[2], NULL, 16);

	/* Count ? */
	if (argc == 4)
		count = simple_strtoul(argv[3], NULL, 16);
	else
		count = 1;

	while (count-- > 0) {
		if (size == 4)
			*((unsigned long *)addr) = (unsigned long)writeval;
		else if (size == 2)
			*((unsigned short *)addr) = (unsigned short)writeval;
		else
			*((unsigned char *)addr) = (unsigned char)writeval;
		addr += size;
	}
	return 0;
}

static int mod_mem(cmd_tbl_t *cmdtp, int incrflag, int flag, int argc, char * const argv[])
{
	unsigned long addr, i;
	int nbytes, size;

	if (argc != 2)
		return CMD_RET_USAGE;

	/* We use the last specified parameters, unless new ones are
	 * entered.
	 */
	addr = mm_last_addr;
	size = mm_last_size;

	if ((flag & CMD_FLAG_REPEAT) == 0) {
		/* New command specified.  Check for a size specification.
		 * Defaults to long if no or incorrect specification.
		 */
		size = cmd_get_data_size(argv[0], 4);
		if (size < 0)
			return 1;

		/* Address is specified since argc > 1
		*/
		addr = simple_strtoul(argv[1], NULL, 16);
	}


	addr = align_address(addr, size);

	/* Print the address, followed by value.  Then accept input for
	 * the next value.  A non-converted value exits.
	 */
	do {
		mvPrintf("%08lx:", addr);
		if (size == 4)
			mvPrintf(" %08x", *((unsigned int *)addr));
		else if (size == 2)
			mvPrintf(" %04x", *((unsigned short *)addr));
		else
			mvPrintf(" %02x", *((unsigned char *)addr));

		nbytes = readline(" ? ");
		if (nbytes == 0 || (nbytes == 1 && console_buffer[0] == '-')) {
			/* <CR> pressed as only input, don't modify current
			 * location and move to next. "-" pressed will go back.
			 */
			if (incrflag)
				addr += nbytes ? -size : size;
			nbytes = 1;
		} else {
			char *endp;
			i = simple_strtoul(console_buffer, &endp, 16);
			nbytes = endp - console_buffer;
			if (nbytes) {
				if (size == 4)
					*((unsigned int *)addr) = i;
				else if (size == 2)
					*((unsigned short *)addr) = i;
				else
					*((unsigned char *)addr) = i;
				if (incrflag)
					addr += size;
			}
		}
	} while (nbytes);

	mm_last_addr = addr;
	mm_last_size = size;
	return 0;
}

/*******************************************************************************
* do_mem_mm
*
* DESCRIPTION:
* The command allows the user to modify bytes in memory
*
*******************************************************************************/
int do_mem_mm(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	return mod_mem(cmdtp, 1, flag, argc, argv);
}

/*******************************************************************************
* do_mem_cmp
*
* DESCRIPTION:
* The command allows the user to compare bytes in memory
*
*******************************************************************************/
int do_mem_cmp(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned long addr1, addr2, count, ngood;
	int size;
	int rcode = 0;
	const char *type;

	if (argc != 4)
		return CMD_RET_USAGE;

	/* Check for size specification.
	*/
	size = cmd_get_data_size(argv[0], 4);
	if (size < 0)
		return 1;

	type = size == 4 ? "word" : size == 2 ? "halfword" : "byte";

	addr1 = simple_strtoul(argv[1], NULL, 16);

	addr2 = simple_strtoul(argv[2], NULL, 16);

	addr1 = align_address(addr1, size);
	addr2 = align_address(addr2, size);

	count = simple_strtoul(argv[3], NULL, 16);

	for (ngood = 0; ngood < count; ++ngood) {
		unsigned long word1, word2;
		if (size == 4) {
			word1 = *(unsigned long *)addr1;
			word2 = *(unsigned long *)addr2;
		} else if (size == 2) {
			word1 = *(unsigned short *)addr1;
			word2 = *(unsigned short *)addr2;
		} else {
			word1 = *(unsigned char *)addr1;
			word2 = *(unsigned char *)addr2;
		}
		if (word1 != word2) {
			mvPrintf("%s at 0x%08lx (%#0*lx) != %s at 0x%08lx (%#0*lx)\n",
				type, addr1, size, word1,
				type, addr2, size, word2);
			rcode = 1;
			break;
		}

		addr1 += size;
		addr2 += size;
	}

	mvPrintf("Total of %ld %s(s) were the same\n", ngood, type);
	return rcode;
}

/*******************************************************************************
* do_mem_cp
*
* DESCRIPTION:
* The command allows the user to copy bytes from/to memory
*
*******************************************************************************/
int do_mem_cp(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned long addr, dest, count;
	int size;

	if (argc != 4)
		return CMD_RET_USAGE;

	/* Check for size specification.
	*/
	size = cmd_get_data_size(argv[0], 4);
	if (size < 0)
		return 1;

	addr = simple_strtoul(argv[1], NULL, 16);

	dest = simple_strtoul(argv[2], NULL, 16);

	addr = align_address(addr, size);
	dest = align_address(dest, size);

	count = simple_strtoul(argv[3], NULL, 16);

	if (count == 0) {
		mvPrintf("Zero length ???\n");
		return 1;
	}
	while (count-- > 0) {
		if (size == 4)
			*((unsigned long *)dest) = *((unsigned long *)addr);
		else if (size == 2)
			*((unsigned short *)dest) = *((unsigned short *)addr);
		else
			*((unsigned char *)dest) = *((unsigned char *)addr);
		addr += size;
		dest += size;
	}
	return 0;
}

/*******************************************************************************
* ir_cmd
*
* DESCRIPTION:
* The command allows the user to view the contents of the MV
*                internal registers and modify them.
*
*******************************************************************************/
int ir_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	MV_U32 regNum = 0x0, regVal, regValTmp, res;
	MV_8 regValBin[40];
	MV_8 cmd[40];
	int i;
	int j = 0, flagm = 0;

	if (argc == 2)
		regNum = simple_strtoul(argv[1], NULL, 16);
	else {
		mvPrintf("Usage:\n%s\n", cmdtp->usage);
		return 0;
	}

	regVal = MV_REG_READ(regNum + INTER_REGS_BASE);
	regValTmp = regVal;
	mvPrintf("Internal register 0x%x value : 0x%x\n ", regNum, regVal);
	mvPrintf("\n    31      24        16         8         0");
	mvPrintf("\n     |       |         |         |         |\nOLD: ");

	for (i = 31; i >= 0; i--) {
		if (regValTmp > 0) {
			res = regValTmp % 2;
			regValTmp = (regValTmp - res) / 2;
			if (res == 0)
				regValBin[i] = '0';
			else
				regValBin[i] = '1';
		} else
			regValBin[i] = '0';
	}

	for (i = 0; i < 32; i++) {
		mvPrintf("%c", regValBin[i]);
		if ((((i+1) % 4) == 0) && (i > 1) && (i < 31))
			mvPrintf("-");
	}

	readline("\nNEW: ");
	strcpy(cmd, console_buffer);
	if ((cmd[0] == '0') && (cmd[1] == 'x')) {
		regVal = simple_strtoul(cmd, NULL, 16);
		flagm = 1;
	} else {
		for (i = 0; i < 40; i++) {
			if (cmd[i] == '\0')
				break;
			if (i == 4 || i == 9 || i == 14 || i == 19 || i == 24 || i == 29 || i == 34)
				continue;
			if (cmd[i] == '1') {
				regVal = regVal | (0x80000000 >> j);
				flagm = 1;
			} else if (cmd[i] == '0') {
				regVal = regVal & (~(0x80000000 >> j));
				flagm = 1;
			}
			j++;
		}
	}

	if (flagm == 1) {
		MV_REG_WRITE(regNum + INTER_REGS_BASE, regVal);
		mvPrintf("\nNew value = 0x%x\n\n", MV_REG_READ(regNum +
					INTER_REGS_BASE));
	}
	return 1;
}

unsigned int trainingReadPhyReg(int uiRegAddr, int uiPup, int type)
{
	unsigned int uiReg;
	unsigned int addrLow = 0x3F & uiRegAddr;
	unsigned int addrHi = ((0xC0 & uiRegAddr) >> 6);

	uiReg = (1 << 31) + (uiPup << 22) + (type << 26) + (addrHi << 28) + (addrLow << 16);
	MV_REG_WRITE(REG_PHY_REGISTRY_FILE_ACCESS_ADDR, uiReg & 0x7FFFFFFF);
	MV_REG_WRITE(REG_PHY_REGISTRY_FILE_ACCESS_ADDR, uiReg);

	do {
		uiReg = ((MV_REG_READ(REG_PHY_REGISTRY_FILE_ACCESS_ADDR)) & (1 << 31));
	} while (uiReg);	/* Wait for '0' to mark the end of the transaction */

	uiReg = MV_REG_READ(REG_PHY_REGISTRY_FILE_ACCESS_ADDR);
	uiReg = uiReg & 0xFFFF;

	return uiReg;
}

/*******************************************************************************
* training_cmd
*
* DESCRIPTION:
* The command prints the results of the DDR3 Training
*
*******************************************************************************/
int training_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	MV_U32 uiCsEna, uiCs, uiReg, uiPup, uiPhase, uiDelay;
	MV_U32 uiRdRdyDly, uiRdSmplDly, uiDq, uiCentralTxRes, uiCentralRxRes;
	MV_U32 uiPupNum;

	uiCsEna = MV_REG_READ(REG_DDR3_RANK_CTRL_ADDR) & 0xF;
	mvPrintf("DDR3 Training results:\n");

	uiPupNum = 5;

	for (uiCs = 0; uiCs < 4; uiCs++) {
		if (uiCsEna & (1 << uiCs)) {
			mvPrintf("CS: %d\n", uiCs);
			for (uiPup = 0; uiPup < uiPupNum; uiPup++) {
				uiReg = trainingReadPhyReg(0 + uiCs*4, uiPup, REG_PHY_DATA);
				uiPhase = (uiReg >> 6) & 0x7;
				uiDelay = uiReg & 0x1F;
				mvPrintf("Write Leveling: PUP: %d, Phase: %d, Delay: %d\n", uiPup, uiPhase, uiDelay);
			}

			for (uiPup = 0; uiPup < uiPupNum; uiPup++) {
				uiReg = trainingReadPhyReg(2 + uiCs*4, uiPup, REG_PHY_DATA);
				uiPhase = (uiReg >> 6) & 0x7;
				uiDelay = uiReg & 0x1F;
				mvPrintf("Read Leveling: PUP: %d, Phase: %d, Delay: %d\n", uiPup, uiPhase, uiDelay);
			}

			uiRdRdyDly = ((MV_REG_READ(REG_READ_DATA_READY_DELAYS_ADDR) >> (8*uiCs)) & 0x1F);
			uiRdSmplDly = ((MV_REG_READ(REG_READ_DATA_SAMPLE_DELAYS_ADDR) >> (8*uiCs)) & 0x1F);
			mvPrintf("Read Sample Delay:\t%d\n", uiRdSmplDly);
			mvPrintf("Read Ready Delay:\t%d\n", uiRdRdyDly);

			/* PBS */
			if (uiCs == 0) {
				for (uiPup = 0; uiPup < uiPupNum; uiPup++) {
					for (uiDq = 0; uiDq < 10; uiDq++) {
						if (uiDq == 9)
							uiDq++;

						uiReg = trainingReadPhyReg(0x10+uiDq+uiCs*0x12, uiPup, REG_PHY_DATA);
						uiDelay = uiReg & 0x1F;

						if (uiDq == 0)
							mvPrintf("PBS TX: PUP: %d: ", uiPup);
						mvPrintf("%d ", uiDelay);
					}
					mvPrintf("\n");
				}
				for (uiPup = 0; uiPup < uiPupNum; uiPup++) {
					for (uiDq = 0; uiDq < 9; uiDq++) {
						uiReg = trainingReadPhyReg(0x50 + uiDq + uiCs*0x12,
									   uiPup, REG_PHY_DATA);
						uiDelay = uiReg & 0x1F;

						if (uiDq == 0)
							mvPrintf("PBS RX: PUP: %d: ", uiPup);
						mvPrintf("%d ", uiDelay);
					}
					mvPrintf("\n");
				}
			}

			/*Read Centralization windows sizes for Scratch PHY registers*/
			for (uiPup = 0; uiPup < uiPupNum; uiPup++) {
				uiReg = trainingReadPhyReg(0xC0 + uiCs, uiPup, REG_PHY_DATA);
				uiCentralRxRes = uiReg >> 5;
				uiCentralTxRes = uiReg & 0x1F;
				mvPrintf("Central window size for PUP %d is %d(RX) and %d(TX)\n",
						uiPup, uiCentralRxRes, uiCentralTxRes);
			}
		}
	}
	return 1;
}

/*******************************************************************************
* do_mem_mtest
*
* DESCRIPTION:
* Perform a memory test. A more complete alternative test can be
* configured using CONFIG_SYS_ALT_MEMTEST.
*
*******************************************************************************/
int do_mem_mtest(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	volatile unsigned long	*addr, *start, *end;
	unsigned long	val;
	unsigned long	readback;
	unsigned long	errs = 0;
	int iterations = 1;
	int iteration_limit;
	unsigned long	incr;
	unsigned long	pattern;

	if (argc > 1)
		start = (unsigned long *)simple_strtoul(argv[1], NULL, 16);
	else
		start = (unsigned long *)CONFIG_SYS_MEMTEST_START;

	if (argc > 2)
		end = (unsigned long *)simple_strtoul(argv[2], NULL, 16);
	else
		end = (unsigned long *)(CONFIG_SYS_MEMTEST_END);

	start = (unsigned long *)align_address((unsigned long)start, 4);
	end = (unsigned long *)align_address((unsigned long)end, 4);

	if (argc > 3)
		pattern = (unsigned long)simple_strtoul(argv[3], NULL, 16);
	else
		pattern = 0;

	if (argc > 4)
		iteration_limit = (unsigned long)simple_strtoul(argv[4], NULL, 16);
	else
		iteration_limit = 0;

	incr = 1;
	for (;;) {
		if (iteration_limit && iterations > iteration_limit) {
			mvPrintf("Tested %d iteration(s) with %lu errors.\n",
				iterations-1, errs);
			return errs != 0;
		}
		++iterations;

		mvPrintf("\rPattern %08lX  Writing...", pattern);
		mvPrintf("%12s", "");
		mvPrintf("\b\b\b\b\b\b\b\b\b\b");

		for (addr = start, val = pattern; addr < end; addr++) {
			*addr = val;
			val += incr;
		}

		mvPrintf("Reading...");

		for (addr = start, val = pattern; addr < end; addr++) {
			readback = *addr;
			if (readback != val) {
				mvPrintf("\nMem error @");
				mvPrintf(" 0x%08X: found %08lX, expected %08lX\n",
					(unsigned int)(uintptr_t)addr, readback, val);
				errs++;
			}
			val += incr;
		}

		/*
		 * Flip the pattern each time to make lots of zeros and
		 * then, the next time, lots of ones.  We decrement
		 * the "negative" patterns and increment the "positive"
		 * patterns to preserve this feature.
		 */
		if (pattern & 0x80000000)
			pattern = -pattern;	/* complement & increment */
		else
			pattern = ~pattern;

		incr = -incr;
	}
	return 0;	/* not reached */
}
