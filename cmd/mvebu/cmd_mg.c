/*
 * Copyright (C) 2017 Marvell International Ltd.
 *
 * SPDX-License-Identifier:	GPL-2.0
 * https://spdx.org/licenses
 */

#include <common.h>
#include <asm/io.h>
#include <command.h>
#include <cli.h>
#include <console.h>

#ifdef DEBUG
#define debug_pr printf
#else
#define debug_pr(...)
#endif

#define BIT(nr)			(1UL << (nr))

#define MG_CM3_SRAM_BASE(cpid)		(0xF2100000 + ((cpid) * 0x2000000))
#define MG_CM3_SRAM_MAX_CODE_SIZE	(120 * 1024)

/* CM3 IO ring buffers for console messages exchange */
#define MG_CM3_OUT_BUF_SIZE		(7 * 1024 - 1)
#define MG_CM3_IN_BUF_SIZE		1023

#define MG_CM3_OUT_BUF_BASE(cpid)	\
			    (MG_CM3_SRAM_BASE(cpid) + MG_CM3_SRAM_MAX_CODE_SIZE)
#define MG_CM3_OUT_BUF_START(cpid)	(MG_CM3_OUT_BUF_BASE(cpid) + 4)
#define MG_CM3_OUT_BUF_END(cpid)	\
			       (MG_CM3_OUT_BUF_BASE(cpid) + MG_CM3_OUT_BUF_SIZE)

#define MG_CM3_IN_BUF_BASE(cpid)	(MG_CM3_OUT_BUF_END(cpid) + 1)
#define MG_CM3_IN_BUF_START(cpid)	(MG_CM3_IN_BUF_BASE(cpid) + 4)
#define MG_CM3_IN_BUF_END(cpid)		\
				 (MG_CM3_IN_BUF_BASE(cpid) + MG_CM3_IN_BUF_SIZE)

/* CONFI REGISTERS */
#define MG_CM3_CONFI_GLOB_CFG_REG(cpid)	(MG_CM3_SRAM_BASE(cpid) + 0x2B500)
#define MG_CM3_CONFI_GLOB_CFG_VAL	(0x100E1A80)
#define MG_CM3_MG_INT_MFX_REG(cpid)	(MG_CM3_SRAM_BASE(cpid) + 0x2B054)
#define MG_CM3_MG_INT_MFX_VAL		(0xFFFF0001)

#define CM3_CPU_EN_BIT			BIT(28)

#define MG_CM3_PROMPT			"\nMG-CM3>"
#define MG_CM3_EXIT_CMD			"exit"
#define MG_CM3_POWEROFF_CMD		"poweroff"

static char *local_read_ptrs[2];
static char *local_write_ptrs[2];

static void mg_cli_read(unsigned long cp_nr)
{
	char *read_ptr = local_read_ptrs[cp_nr];
	uintptr_t remote_out_ptr;

	printf("\t"); /* Print CM3 output with indentation */

re_read:
	/* Check CM3 output buffer and print it */
	remote_out_ptr = (uintptr_t)(readl(MG_CM3_OUT_BUF_BASE(cp_nr)) +
						       MG_CM3_SRAM_BASE(cp_nr));

	debug_pr("\n%s: read_ptr %p, remote ptr %p, buf end %p, buf start %p\n",
		 __func__, read_ptr, (char *)remote_out_ptr,
		 (char *)(MG_CM3_OUT_BUF_END(cp_nr)),
		 (char *)(MG_CM3_OUT_BUF_START(cp_nr)));

	while (read_ptr != (char *)remote_out_ptr) {
		putc(readb(read_ptr));

		/* Print CM3 output with indentation */
		if (readb(read_ptr) == '\r')
			printf("\t");

		/* Handle wrap around */
		if (read_ptr < (char *)(MG_CM3_OUT_BUF_END(cp_nr)))
			read_ptr++;
		else
			read_ptr = (char *)(MG_CM3_OUT_BUF_START(cp_nr));
	}

	/* Continue with printing output of previous command if there is
	 * still something to print - this may happen when we have long
	 * CM3 output which grows up after first read of remote_out_ptr.
	 */
	if (read_ptr != (char *)(readl(MG_CM3_OUT_BUF_BASE(cp_nr)) +
						       MG_CM3_SRAM_BASE(cp_nr)))
		goto re_read;

	/* Update the global pointer */
	local_read_ptrs[cp_nr] = read_ptr;
}

static void mg_cli_write(unsigned long cp_nr, int input_len)
{
	char *write_ptr = local_write_ptrs[cp_nr];
	int out_ch;

	debug_pr("\nGot %d chars. Starting write_ptr=%p\n",
		 input_len, write_ptr - MG_CM3_SRAM_BASE(cp_nr));
	for (out_ch = 0; out_ch <= (input_len + 1); out_ch++) {
		if (out_ch == input_len)
			writeb('\r', write_ptr); /* CR */
		else if (out_ch == input_len + 1) {
			/* Mark as end of command */
			writeb(0x0, write_ptr);
			/* Don't increment ptr, so 0x0 will be overwritten in
			 * next cmd execution
			 */
			break;
		} else
			writeb(console_buffer[out_ch], write_ptr);

		/* Handle wrap around */
		if (write_ptr < (char *)(MG_CM3_IN_BUF_END(cp_nr)))
			write_ptr++;
		else
			write_ptr = (char *)(MG_CM3_IN_BUF_START(cp_nr));
	}

	/* Update remote pointer */
	writel((uintptr_t)write_ptr - MG_CM3_SRAM_BASE(cp_nr),
	       MG_CM3_IN_BUF_BASE(cp_nr));

	/* Update the global pointer */
	local_write_ptrs[cp_nr] = write_ptr;
}

static void mg_cm3_reset(unsigned long cp_nr)
{
	int temp;

	temp = readl(MG_CM3_CONFI_GLOB_CFG_REG(cp_nr));
	temp &= ~(CM3_CPU_EN_BIT);
	writel(temp , MG_CM3_CONFI_GLOB_CFG_REG(cp_nr));

	/* Zero whole CM3 memory - it is needed because of emulated UART and it
	 * will prevent for running old command after re-enable CM3
	 */
	memset((void *)MG_CM3_SRAM_BASE(cp_nr), 0, 128 * 1024);
}

static void mg_cli_loop(unsigned long cp_nr)
{
	int input_len;

	printf("Opening MG CM3 console..\n");
	printf("\tUse \"%s\" for power down CM3 and fall back to u-boot prompt\n",
	       MG_CM3_POWEROFF_CMD);
	printf("\tUse \"%s\" falling back to u-boot prompt (CM3 still running)\n",
	       MG_CM3_EXIT_CMD);

	for (;;) {
		/* Read new, unread content from CM3 output buffer (buffer
		 * where CM3 stores it's log e.g. command output)
		 */
		mg_cli_read(cp_nr);

		/* Check the user input and copy it to CM3 */
		input_len = cli_readline(MG_CM3_PROMPT);
		if (input_len <= 0)
			continue;

		/* Handle exit and poweroff commands */
		if (strcmp(console_buffer, MG_CM3_POWEROFF_CMD) == 0) {
			mg_cm3_reset(cp_nr);
			printf("Bye-bye from MG CM3 - it is going down!\n");
			return;
		} else if (strcmp(console_buffer, MG_CM3_EXIT_CMD) == 0) {
			printf("Bye-bye from MG CM3 - CM3 still running\n");
			return;
		}

		/* All other command redirect to CM3 (store them in shared mem
		 * from which CM3 will read them)
		 */
		mg_cli_write(cp_nr, input_len);
	}
}

int mg_boot(char *file_name, unsigned long cp_nr, unsigned long io_redir)
{
	char command[256];
	const char *file_sz_str;
	int ret;
	unsigned long file_size;

	/* Load executable code to MG CM3 SRAM */
	sprintf(command, "tftpboot %lx %s", MG_CM3_SRAM_BASE(cp_nr), file_name);

	ret = run_command(command, 0);
	if (ret != 0) {
		printf("Failed to load MG CM3 image\n");
		return CMD_RET_FAILURE;
	}

	file_sz_str = getenv("filesize");
	if (file_sz_str == NULL) {
		printf("Failed to get \"filesize\" environment variable\n");
		return CMD_RET_FAILURE;
	}

	file_size = simple_strtoul(file_sz_str, NULL, 16);
	if (file_size > MG_CM3_SRAM_MAX_CODE_SIZE) {
		printf("Too big file (%ldB). Max supported size is %dB\n",
		       file_size, MG_CM3_SRAM_MAX_CODE_SIZE);
		return CMD_RET_FAILURE;
	}

	/* Release MG CM3 from reset */
	writel(MG_CM3_CONFI_GLOB_CFG_VAL, MG_CM3_CONFI_GLOB_CFG_REG(cp_nr));
	writel(MG_CM3_MG_INT_MFX_VAL, MG_CM3_MG_INT_MFX_REG(cp_nr));

	/* Take over console IO */
	if (io_redir) {
		/* Initialize start point for read and write msgs */
		local_read_ptrs[cp_nr] = (char *)MG_CM3_OUT_BUF_START(cp_nr);
		local_write_ptrs[cp_nr] = (char *)MG_CM3_IN_BUF_START(cp_nr);

		mg_cli_loop(cp_nr);
	}

	return CMD_RET_SUCCESS;
}

int do_mg_boot(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	char *file_name;
	unsigned long cp_nr, io_redir = 0;

	if (argc > 4 || argc < 3)
		return CMD_RET_USAGE;

	/* Get the image file name */
	file_name = argv[1];

	/* Get CPn number */
	cp_nr = simple_strtoul(argv[2], NULL, 16);
	if (cp_nr > 1) {
		printf("Only CP0 and CP1 are supported!\n");
		return CMD_RET_USAGE;
	}

	if (argc == 4)
		io_redir = simple_strtoul(argv[3], NULL, 16);

	if (io_redir > 1)
		io_redir = 1;

	return mg_boot(file_name, cp_nr, io_redir);
}

U_BOOT_CMD(
	mg_boot,     4,      1,       do_mg_boot,
	"Loads firmware into CPx MG CM3 SRAM and release CM3 from reset",
	"mg_boot <remote_file_name> <cp_nr> [io_redirect]\n"
	"Parameters:\n"
	"\tremote_file_name - TFTP image path to be loaded to MG CM3 SRAM\n"
	"\tcp_nr - CPx number [0 | 1]\n"
	"\tio_redirect - redirect console IO to CM3 [0=no (deflt) | 1=yes]\n"
);

int do_mg_console(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	unsigned long cp_nr;

	if (argc != 2)
		return CMD_RET_USAGE;

	/* Get CPn number */
	cp_nr = simple_strtoul(argv[1], NULL, 16);
	if (cp_nr > 1) {
		printf("Only CP0 and CP1 are supported!\n");
		return CMD_RET_FAILURE;
	}

	mg_cli_loop(cp_nr);

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	mg_console,     2,      1,       do_mg_console,
	"Switches console IO to CPn MG CM3",
	"mg_console [cp_nr]\n"
	"Parameters:\n"
	"\tcp_nr - CPx number [0 | 1]\n"
);
