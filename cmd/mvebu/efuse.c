/*
 * ***************************************************************************
 * Copyright (C) 2017 Marvell International Ltd.
 * ***************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of Marvell nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ***************************************************************************
 */
 #include <config.h>
 #include <common.h>
 #include <command.h>
 #include <vsprintf.h>
 #include <errno.h>
 #include <asm/arch-armada3700/efuse.h>

int do_efuse_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	const char		*cmd = NULL;
	const char		*efuse = NULL;
	const char		*writeval = NULL;
	char			readval[MVEBU_EFUSE_MAX_STRLEN];
	enum efuse_id		fid = EFUSE_ID_MAX;
	struct efuse_info	finfo[] = MVEBU_EFUSE_INFO;
	int			status = CMD_RET_SUCCESS;

	if (argc < 2) {
		printf("Error: No command supplied\n");
		return CMD_RET_USAGE;
	} else {
		cmd = argv[1];
	}

	if ((strcmp(cmd, "read") == 0) && (argc < 3)) {
		printf("Error: Please specify eFuse ID\n");
		return CMD_RET_USAGE;
	}

	if ((strcmp(cmd, "write") == 0) && (argc < 4)) {
		printf("Error: Please specify eFuse ID and value\n");
		return CMD_RET_USAGE;
	}


	if (argc > 2) {
		efuse = argv[2];
		for (fid = EFUSE_ID_BOOT_DEVICE; fid < EFUSE_ID_MAX; fid++) {
			if (strcmp(efuse, finfo[fid].name) == 0)
				break;
		}
		if (fid == EFUSE_ID_MAX) {
			printf("%s - Invalid eFuse ID\n", efuse);
			return CMD_RET_USAGE;
		} else if (efuse_id_valid(fid) != 1) {
			printf("%s - Unsupported eFuse ID\n", efuse);
			return CMD_RET_USAGE;
		}
	}

	if (argc > 3)
		writeval = argv[3];


	if (strcmp(cmd, "list") == 0) {
		/* LIST */
		printf("  EFUSE ID\t\tDESCRIPTION\n");
		printf("---------------------------------------------------------\n");
		for (fid = EFUSE_ID_BOOT_DEVICE; fid < EFUSE_ID_MAX; fid++) {
			if (efuse_id_valid(fid))
				printf("%s\t%s\n", finfo[fid].name,
				       finfo[fid].note);
		}

	} else if (strcmp(cmd, "dump") == 0) {
		/* DUMP */
		printf("  EFUSE ID\t\tVALUE\n");
		printf("---------------------------------------------------------\n");
		for (fid = EFUSE_ID_BOOT_DEVICE; fid < EFUSE_ID_MAX; fid++) {
			if (efuse_id_valid(fid)) {
				if (efuse_read(fid, readval) == 0) {
					printf("%s\t%s%s\n", finfo[fid].name,
					       strlen(finfo[fid].name) < 8 ?
					       "\t" : "", readval);
				} else {
					printf("%s\t=== ERROR READING EFUSE VALUE ===\n",
					       finfo[fid].name);
					status = CMD_RET_FAILURE;
				}
			}
		}

	} else if (strcmp(cmd, "read") == 0) {
		/* READ */
		if (efuse_read(fid, readval) == 0) {
			printf("%s\t%s%s\n", finfo[fid].name,
			       strlen(finfo[fid].name) < 8 ? "\t" : "",
				readval);
		} else {
			printf("%s\t=== ERROR READING EFUSE VALUE ===\n",
			       finfo[fid].name);
			status = CMD_RET_FAILURE;
		}

	} else if (strcmp(cmd, "write") == 0) {
		/* WRITE */
		if (efuse_write(fid, writeval) == 0) {
			if (efuse_read(fid, readval) == 0) {
				printf("Returned EFUSE value after write:\n");
				printf("%s\t%s\n", finfo[fid].name, readval);
			} else {
				printf("%s\t=== ERROR READING BACK EFUSE VALUE ===\n",
				       finfo[fid].name);
				status = CMD_RET_FAILURE;
			}
		} else {
			printf("%s\t=== ERROR WRITING EFUSE VALUE ===\n",
			       finfo[fid].name);
			status = CMD_RET_FAILURE;
		}
	} else if (strcmp(cmd, "raw") == 0) {
		/* RAW DUMP */
		efuse_raw_dump();
	} else {
		/* ERROR */
		printf("Usupported command \"%s\"\n", cmd);
		status = CMD_RET_USAGE;
	}

	return status;
}

U_BOOT_CMD(
	efuse,      4,     1,      do_efuse_cmd,
	"efuse - read/Write SoC eFuse entries\n",
	"\n"
	"Access to SoC eFuse entry values\n"
	"\tlist         - Display all supported eFuse entry ids\n"
	"\tdump         - Dump all supported eFuse entries\n"
	"\traw          - Dump all eFuses in raw format\n"
	"\tread id      - Read eFuse entry \"id\"\n"
	"\twrite id val - Write \"val\" to eFuse entry \"id\"\n"
);
