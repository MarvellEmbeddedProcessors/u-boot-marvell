/*
 * ***************************************************************************
 * Copyright (C) 2015 Marvell International Ltd.
 * ***************************************************************************
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ***************************************************************************
 */

#include <config.h>
#include <common.h>
#include <command.h>
#include <cli.h>
#include <environment.h>

#include <asm/io.h>
#include <asm/arch-mvebu/thermal.h>

#ifdef CONFIG_MVEBU_CCU
#include <asm/arch-mvebu/ccu.h>
#endif
#ifdef CONFIG_MVEBU_RFU
#include <asm/arch-mvebu/rfu.h>
#endif
#ifdef CONFIG_MVEBU_IOB
#include <asm/arch-mvebu/iob.h>
#endif
#ifdef CONFIG_MVEBU_AMB_ADEC
#include <asm/arch-mvebu/amb_adec.h>
#endif
#ifdef CONFIG_MVEBU_MBUS
#include <asm/arch-mvebu/mbus.h>
#endif
#ifdef CONFIG_MVEBU_FLC
#include <asm/arch-mvebu/flc.h>
#endif

int do_map_cmd(cmd_tbl_t *cmdtp, int flag, int argc,
			char * const argv[])
{
#ifdef CONFIG_MVEBU_CCU
	dump_ccu();
#endif
#ifdef CONFIG_MVEBU_RFU
	dump_rfu();
#endif
#ifdef CONFIG_MVEBU_IOB
	dump_iob();
#endif
#ifdef CONFIG_MVEBU_AMB_ADEC
	dump_amb_adec();
#endif
#ifdef CONFIG_MVEBU_MBUS
	dump_mbus();
#endif
#ifdef CONFIG_MVEBU_FLC
	dump_flc();
#endif
	return 0;
}

U_BOOT_CMD(
	map,      1,     1,      do_map_cmd,
	"Display address decode windows\n",
	"\tDisplay address decode windows\n"
);

#ifdef CONFIG_MVEBU_MBUS
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

	if (remap_mbus(input, output)) {
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
#endif

#ifdef CONFIG_CMD_MVEBU_TSEN
int thermal_sensor_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	printf("Temperature = %d\n", mvebu_thermal_sensor_read());
	return 1;
}

U_BOOT_CMD(
	tsen, 1, 1, thermal_sensor_cmd,
	"tsen - Display the SoC temperature.\n",
	"\n\tDisplay the SoC temperature as read from the on chip thermal sensor.\n"
);
#endif

#if defined(MV_INCLUDE_NOR) || defined(CONFIG_MVEBU_NAND_BOOT) || defined(CONFIG_MVEBU_SPI_BOOT) || \
	defined(CONFIG_ENV_IS_IN_BOOTDEV)
int resetenv_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	set_default_env("## Resetting to default environment\n");
	saveenv();

	return 1;
}

U_BOOT_CMD(
	resetenv,      1,     1,      resetenv_cmd,
	"resetenv - reset all variables to default\n",
	"\n"
	"\t Reset all variables to default, and save to the flash\n"
);
#endif
