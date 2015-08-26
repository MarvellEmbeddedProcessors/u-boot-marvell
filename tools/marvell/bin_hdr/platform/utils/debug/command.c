/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer.

    *   Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.

    *   Neither the name of Marvell nor the names of its contributors may be
	used to endorse or promote products derived from this software without
	specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/
/* includes */

#include "command.h"
#include "mvUart.h"
#include "lib_utils.h"
#include "printf.h"

int do_help(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int do_mem_md(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int do_mem_mw(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int do_mem_mm(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int do_mem_cmp(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int do_mem_cp(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int ir_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int training_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int do_mem_mtest(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int do_xorMemTest(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);

/* Commands Table
 * Every new command is added here, Thus it must be declared in
 * this file, or include the file declaring it
 * For example, To add new command that is implemented in 'hello_world' function
 * this line is added to the declaration:
 *	{ "hello", 2, hello_world, "prints hello world", ""}
 */
cmd_tbl_t commands[] = {
	{ "?", 2, do_help, "prints help message of command", "command-name"},
	{ "md", 3, do_mem_md, "memory display", "[.b, .w, .l] address [# of objects]"},
	{ "mw",	4, do_mem_mw, "memory write (fill)", "[.b, .w, .l] address value [count]"},
	{ "mm",	2, do_mem_mm, "memory modify (auto-incrementing address)", "[.b, .w, .l] address"},
	{ "cmp", 4, do_mem_cmp, "memory compare", "[.b, .w, .l] addr1 addr2 count"},
	{ "cp", 4, do_mem_cp, "memory copy", "[.b, .w, .l] source target count"},
	{ "ir", 2, ir_cmd, "ir	- reading and changing MV internal register values.\n", "address"},
	{ "training", 1, training_cmd, "training	- prints the results of the DDR3 Training.\n"},
	{ "mtest", 5, do_mem_mtest, "simple RAM read/write test", "[start [end [pattern [iterations]]]]"},
	{ "xormtest", 4, do_xorMemTest, "run memory test based on XOR engine",
		"[source [destination [size(bytes) [iterations]]]]"}
};

/* local functions definitions */
cmd_tbl_t *find_cmd(const char *cmd);
void cmd_usage(const cmd_tbl_t *cmdtp);

/*******************************************************************************
* do_help
*
* DESCRIPTION:
* Command that prints the help message of commands
*
*******************************************************************************/
int do_help(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	cmd_tbl_t *cmd;

	if (argc <= 1)
		return 0;

	cmd = find_cmd(argv[1]);
	if (cmd == NULL) {
		mvPrintf("Command not found");
		return 0;
	}

	cmd_usage(cmd);
	return 0;
}

int do_mem_md(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int do_mem_mw(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int do_mem_mm(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int do_mem_cmp(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int do_mem_cp(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int ir_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int training_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
int do_mem_mtest(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);

/*******************************************************************************
* cmd_get_data_size
*
* DESCRIPTION:
* Check for a size specification .b, .w or .l.
*
* INPUT:
*	arg		- command (e.g. "mw.l")
*	default_size	- default size to return if no size was specified
*
* RETURN:
*	if size was specified, return corresponding size
*		(.b = 1, .w = 2, .l = 4)
*	otherwise, return default_size
*
*******************************************************************************/
int cmd_get_data_size(char *arg, int default_size)
{
	int len = strlen(arg);
	if (len > 2 && arg[len-2] == '.') {
		switch (arg[len-1]) {
		case 'b':
			return 1;
		case 'w':
			return 2;
		case 'l':
			return 4;
		case 's':
			return -2;
		default:
			return -1;
		}
	}
	return default_size;
}

/*******************************************************************************
* find_cmd_tbl
*
* DESCRIPTION:
* search for command in command table
*
* INPUT:
*	cmd		- command (e.g. "ir")
*	table		- commands table
*	table_len	- commands table length
*
* RETURN:
*	if command was found, cmd_tbl_t entry corresponding the command
*	otherwise, NULL
*
*******************************************************************************/
cmd_tbl_t *find_cmd_tbl(const char *cmd, cmd_tbl_t *table, int table_len)
{
	cmd_tbl_t *cmdtp;
	cmd_tbl_t *cmdtp_temp = table;	/*Init value */
	const char *p;
	int len;
	int n_found = 0;

	if (!cmd)
		return NULL;
	/*
	 * Some commands allow length modifiers (like "cp.b");
	 * compare command name only until first dot.
	 */
	len = ((p = strchr(cmd, '.')) == NULL) ? strlen(cmd) : (p - cmd);

	for (cmdtp = table;
	     cmdtp != table + table_len;
	     cmdtp++) {
		if (strncmp(cmd, cmdtp->name, len) == 0) {
			if (len == strlen(cmdtp->name))
				return cmdtp;	/* full match */

			cmdtp_temp = cmdtp;	/* abbreviated command ? */
			n_found++;
		}
	}
	if (n_found == 1) {			/* exactly one match */
		return cmdtp_temp;
	}

	return NULL;	/* not found or ambiguous command */
}

/*******************************************************************************
* find_cmd
*
* DESCRIPTION:
* wrapper function for 'find_cmd_tbl'
*
*******************************************************************************/
cmd_tbl_t *find_cmd(const char *cmd)
{
	return find_cmd_tbl(cmd, commands, sizeof(commands)/sizeof(struct cmd_tbl_s));
}

static int cmd_call(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int result;

	result = (cmdtp->cmd)(cmdtp, flag, argc, argv);
	return result;
}

/*******************************************************************************
* cmd_usage
*
* DESCRIPTION:
* prints the help message of the command
*
* INPUT:
*	cmdtp	- command entry pointer
*
*
*******************************************************************************/
void cmd_usage(const cmd_tbl_t *cmdtp)
{
	mvPrintf("%s - %s\n\n", cmdtp->name, cmdtp->usage);
	mvPrintf("Usage:\n%s ", cmdtp->name);

	if (!cmdtp->help)
		return;

	mvPrintf("%s\n", cmdtp->help);
}

/*******************************************************************************
* cmd_process
*
* DESCRIPTION:
* search for command in command table and run it
*
* INPUT:
*	flag	- command flag
*	argc	- number of arguments
*	argv	- arguments array
*
* RETURN:
*	CMD_RET_SUCCESS(0), if command was ran successfully
*	CMD_RET_FAILURE(1), on failure
*	CMD_RET_USAGE(-1), on failure and need to print help message (future use)
*
*
*******************************************************************************/
enum command_ret_t cmd_process(int flag, int argc, char * const argv[])
{
	enum command_ret_t rc = CMD_RET_SUCCESS;
	cmd_tbl_t *cmdtp;

	/* Look up command in command table */
	cmdtp = find_cmd(argv[0]);
	if (cmdtp == NULL) {
		putstring("Unknown command\n");
		return 1;
	}

	/* found - check max args */
	if (argc > cmdtp->maxargs)
		rc = CMD_RET_USAGE;


	/* If OK so far, then do the command */
	if (!rc)
		rc = cmd_call(cmdtp, flag, argc, argv);

	if (rc == CMD_RET_USAGE)
		cmd_usage(cmdtp);

	return rc;
}

/*******************************************************************************
* parse_line
*
* DESCRIPTION:
* parse command and store arguments in argv
*
* INPUT:
*	line	- pointer to the beginning of the command string
*
* OUTPUT:
*	argv	- output array of the parsed arguments
*
* RETURN:
*	number of arguments
*
*******************************************************************************/
int parse_line(char *line, char *argv[])
{
	int nargs = 0;

	while (nargs < CONFIG_SYS_MAXARGS) {

		/* skip any white space */
		while (isblank1(*line))
			++line;

		if (*line == '\0') {	/* end of line, no more args	*/
			argv[nargs] = NULL;
			return nargs;
		}

		argv[nargs++] = line;	/* begin of argument string	*/

		/* find end of string */
		while (*line && !isblank1(*line))
			++line;

		if (*line == '\0') {	/* end of line, no more args	*/
			argv[nargs] = NULL;
			return nargs;
		}

		*line++ = '\0';		/* terminate current arg	 */
	}

	putstring("** Too many args **\n");

	return nargs;
}

/*******************************************************************************
* run_command
*
* DESCRIPTION:
* runs input command
*
* INPUT:
*	cmd	- input command string to run (including parameters).
*	flag	- command flag
*
*******************************************************************************/
void run_command(const char *cmd, int flag)
{
	char cmdbuf[CONFIG_SYS_CBSIZE];	/* working copy of cmd		*/
	char *token;			/* start of token in cmdbuf	*/
	char *sep;			/* end of token (separator) in cmdbuf */
	char *str = cmdbuf;
	char *argv[CONFIG_SYS_MAXARGS + 1];	/* NULL terminated	*/
	int argc, inquotes;

	if (!cmd || !*cmd)
		return;	/* empty command */

	if (strlen(cmd) >= CONFIG_SYS_CBSIZE) {
		putstring("## Command too long!\n");
		return;
	}

	strcpy(cmdbuf, cmd);

	/* Process separators */

	while (*str) {

		/*
		 * Find separator, or string end
		 * Allow simple escape of ';' by writing "\;"
		 */
		for (inquotes = 0, sep = str; *sep; sep++) {
			if ((*sep == '\'') &&
			    (*(sep - 1) != '\\'))
				inquotes = !inquotes;

			if (!inquotes &&
			    (*sep == ';') &&	/* separator		*/
			    (sep != str) &&	/* past string start	*/
			    (*(sep-1) != '\\'))	/* and NOT escaped	*/
				break;
		}

		/*
		 * Limit the token to data between separators
		 */
		token = str;
		if (*sep) {
			str = sep + 1;	/* start of command for next pass */
			*sep = '\0';
		} else
			str = sep;	/* no more commands for next pass */

		/* Extract arguments */
		argc = parse_line(token, argv);
		if (argc == 0)
			continue;

		cmd_process(flag, argc, argv);
	}
}

