/*
 * ***************************************************************************
 * Copyright (C) Marvell International Ltd. and its affiliates
 * ***************************************************************************
 * Marvell GPL License Option
 * If you received this File from Marvell, you may opt to use, redistribute
 * and/or modify this File in accordance with the terms and conditions of the
 * General Public License Version 2, June 1991 (the "GPL License"), a copy of
 * which is available along with the File in the license.txt file or by writing
 * to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 or on the worldwide web at http://www.gnu.org/licenses/gpl.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 * EXPRESSLY DISCLAIMED. The GPL License provides additional details about this
 * warranty disclaimer.
 * ***************************************************************************
 */

#include <config.h>
#include <common.h>
#include <command.h>

#include <asm/io.h>
#include <asm/arch-mvebu/adec.h>
#include <asm/arch-mvebu/unit-info.h>

int do_map_cmd(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[])
{
	adec_dump();
	return 0;
}

U_BOOT_CMD(
	map,      1,     1,      do_map_cmd,
	"Display address decode windows\n",
	"\tDisplay address decode windows\n"
);

int do_remap_cmd(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[])
{
	phys_addr_t input;
	phys_addr_t output;

	if (argc < 3) {
		printf("Error: Not enough arguments\n");
		printf("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	input = simple_strtoul(argv[1], NULL, 16);
	output = simple_strtoul(argv[2], NULL, 16);

	if (adec_remap(input, output)) {
		printf("Error: Failed to remap 0x%08x->0x%08x\n", (uint)input, (uint)output);
		return 1;
	}

	printf("Remapped address 0x%08x t0 0x%08x\n", (uint)input, (uint)output);
	return 0;
}

U_BOOT_CMD(
	remap,      3,     1,      do_remap_cmd,
	"Remap the output address of a window",
	"input_addr output_addr\n"
	"  - input_addr: Base address used by CPU\n"
	"  - output_addr: Base address issued by window target\n"
	"The remap command enables modifying the base address used\n"
	"to access a certain HW unit. It is usefull when the PCI\n"
	"address space is different then the CPU address space\n"
	"Remapping the PCI window enables the PCI host to change the ouput\n"
	"address on the PCI bus and so to access different PCI BARs\n"
);


int do_units_cmd(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[])
{
	const char *cmd = argv[1];

	if (strcmp(cmd, "list") == 0)
		dump_unit_info();
	else
		printf("ERROR: unknown command to units: \"%s\"\n", cmd);

	return 0;
}

U_BOOT_CMD(
	units,      2,     1,      do_units_cmd,
	"units	- Display and modify active units\n",
	"\n"
	"Display and modufy units in current setup\n"
	"\tlist - Display current active units\n"
);


#ifdef CONFIG_CMD_MVEBU_IR
int ir_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	u32 offset = 0x0;
	u32 reg, tmp_val, residue;
	char binary[40];
	char cmd[40];
	int i, j = 0;
	int is_modified = 0;
	u8 *regs_base = (u8 *)MVEBU_REGS_BASE;

	if (argc == 2) {
		offset = simple_strtoul(argv[1], NULL, 16);
	} else {
		printf("Usage:\n%s\n", cmdtp->usage);
		return 0;
	}

	reg = readl(regs_base + offset);
	tmp_val = reg;
	printf("Internal register 0x%x value : 0x%x\n", offset, reg);
	printf("\n    31      24        16         8         0");
	printf("\n     |       |         |         |         |\nOLD: ");

	/* Convert the value to binary string */
	/* Be endianes safe so don't use shifts */
	for (i = 31; i >= 0; i--) {
		if (tmp_val > 0) {
			residue = tmp_val % 2;
			tmp_val = (tmp_val - residue) / 2;
			if (residue == 0)
				binary[i] = '0';
			else
				binary[i] = '1';
		} else {
			binary[i] = '0';
		}
	}

	/* Print the binary string */
	for (i = 0; i < 32; i++) {
		printf("%c", binary[i]);
		if ((((i+1) % 4) == 0) && (i > 1) && (i < 31))
			printf("-");
	}

	readline("\nNEW: ");
	strcpy(cmd, console_buffer);
	if ((cmd[0] == '0') && (cmd[1] == 'x')) {
		reg = simple_strtoul(cmd, NULL, 16);
		is_modified = 1;
	} else {
		/* Read binary input */
		for (i = 0; i < 40; i++) {
			if (cmd[i] == '\0')
				break;
			if (i == 4 || i == 9 || i == 14 || i == 19 || i == 24 || i == 29 || i == 34)
				continue;
			if (cmd[i] == '1') {
				reg = reg | (0x80000000 >> j);
				is_modified = 1;
			} else if (cmd[i] == '0') {
				reg = reg & (~(0x80000000 >> j));
				is_modified = 1;
			}
			j++;
		}
	}

	/* Update the register value if modified */
	if (is_modified == 1) {
		writel(reg, regs_base + offset);
		printf("\nNew value = 0x%x\n\n", readl(regs_base + offset));
	}
	return 0;
}

U_BOOT_CMD(
	ir,      2,     1,      ir_cmd,
	"ir	- Reading and changing internal register values.\n",
	" Address - offset inside internal registers space\n"
	"\tDisplays the contents of the internal register in 2 forms, hex and binary.\n"
	"\tIt's possible to change the value by writing a hex value beginning with 0x\n"
	"\tor by writing 0 or 1 in the required place.\n"
	"\tPressing enter without any value keeps the value unchanged.\n"
);
#endif

