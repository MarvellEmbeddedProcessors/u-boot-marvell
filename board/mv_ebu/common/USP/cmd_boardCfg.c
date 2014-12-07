/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

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

*******************************************************************************/
#include <common.h>
#if defined(CONFIG_CMD_BOARDCFG)
#include "boardEnv/mvBoardEnvLib.h"

typedef struct _boardConfig {
	MV_SATR_TYPE_ID ID;
	char *cmdName;
	char *Name;
	MV_U8 numOfValues;
	char *Value[MV_BOARD_CONFIG_CMD_MAX_OPTS];
} MV_BOARD_CONFIG_VALUE;

MV_BOARD_CONFIG_VALUE boardConfig[] = MV_BOARD_CONFIG_CMD_INFO;

MV_CONFIG_TYPE_ID configToEnum(char *name)
{
	MV_CONFIG_TYPE_ID i;

	if (name == NULL)
		goto error;

	for (i = 0; i < MV_CONFIG_TYPE_MAX_OPTION; i++)
		if (strcmp(name, boardConfig[i].cmdName) == 0)
			return boardConfig[i].ID;
	if (strcmp(name, "default") == 0)
		return MV_CONFIG_TYPE_CMD_SET_DEFAULT;

error:
	printf("Error: there is no board configuration named '%s'\n", name);
	return MV_ERROR;
}

/* Description:
 *	reset all configuration fields to it's default setup
 * Usage:
 *	'boardConfig write default
*/
static int do_boardCfg_default(void)
{
	MV_U8 i;

	if (mvBoardEepromWriteDefaultCfg() != MV_OK)
		goto error;

	/* Reset local array data to default as well */
	for (i = 0; i <	MV_CONFIG_TYPE_MAX_OPTION; i++)
		mvBoardSysConfigSet(i, mvBoardDefaultValueGet(i));

	return 0;

error:
	printf("Error: failed setting default board configuration - See 'help boardConfig'\n");
	return -1;
}


/* Description:
 *	show possible values for each configuration field
 * Usage:
 *	'boardConfig list <fieldName>'
 */
static int do_boardCfg_list(MV_CONFIG_TYPE_ID field)
{
	MV_U8 i;

	printf("%-21s:\n", boardConfig[field].Name);
	for (i = 0; i < boardConfig[field].numOfValues; i++)
		printf("\t%d - %s\n", i, boardConfig[field].Value[i]);

	return 0;
}

/* Description:
 *	Read a single field value, or dump all field's value
 * Usage:
 *	Read single field: 'boardConfig read <fieldName>'
 *	Dump all fields: 'boardConfig read'
 */
static int do_boardCfg_read(MV_CONFIG_TYPE_ID field)
{
	MV_U32 val;
	MV_CONFIG_TYPE_ID i = 0, loopEnd = MV_CONFIG_TYPE_MAX_OPTION;

	/* if requested a specific field to read --> set loop to run 1 iteration on requested field*/
	if (field != MV_CONFIG_TYPE_CMD_DUMP_ALL) {
		i = field;
		loopEnd = i + 1;
	}

	/* else loop and dump all fields */
	for (; i < loopEnd; i++) {
		val = mvBoardSysConfigGet(i);
		if (val != MV_ERROR)
			printf("%-26s= %d ==> %s\n", boardConfig[i].Name, val, boardConfig[i].Value[val]);
		else { /* don't print error on unused fields when dump all */
			if (field == MV_CONFIG_TYPE_CMD_DUMP_ALL)
				continue;
			else
				mvOsPrintf("Error: Requested board config is invalid for this board\n");
		}
	}

	return 0;
}

/* Description:
 *	write a new value
 * Usage:
 *	'boardConfig write <fieldName> <value>'
 */
static int do_boardCfg_write(MV_CONFIG_TYPE_ID field, MV_U8 writeVal)
{
	if (writeVal >= boardConfig[field].numOfValues) {
		printf("Error: write value is invalid - See 'boardConfig list <field>'\n\n");
		goto error;
	}
	if (mvBoardConfigVerify(field, writeVal) == MV_ERROR)
		goto error;
	if (mvBoardEepromWrite(field, writeVal) == MV_OK)
		return 0;
error:
	printf("Error: failed writing board configuration - See 'help boardConfig'\n");
	return -1;
}

int isEepromEnabledFlag = -1;

int do_boardCfg(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	const char *cmd;
	MV_U8 value;
	MV_CONFIG_TYPE_ID field;
	int status = 0;

	if (isEepromEnabledFlag == -1)
		isEepromEnabledFlag = (mvBoardIsEepromEnabled() == MV_TRUE) ? 1 : 0;

	if (isEepromEnabledFlag == 0) {
		printf("Error: EEPROM is currently not accessible on board\n");
		return 0;
	}

	/* need at least another argument 'write'/ 'list' /'read' */
	if (argc < 2)
		goto usage;

	cmd = argv[1];

	/* if requested to dump all fields - ('boardConfig read') */
	if (argc == 2 && strcmp(cmd, "read") == 0)
		field = MV_CONFIG_TYPE_CMD_DUMP_ALL;
	else /* else get specified field request if any */
		field = configToEnum(argv[2]);

	if (field == MV_ERROR) {
		printf("field == MV_ERROR");
		goto usage;
	}

	if (strcmp(cmd, "list") == 0)
		return do_boardCfg_list(field);			/* if 'boardConfig list <field>' */
	else if (strcmp(cmd, "read") == 0)
		return do_boardCfg_read(field);			/* if 'boardConfig read <field>' */
	else if (strcmp(cmd, "write") == 0) {
		if (field == MV_CONFIG_TYPE_CMD_SET_DEFAULT) { /* if 'boardConfig write default' */
			status = do_boardCfg_default();
			field = MV_CONFIG_TYPE_CMD_DUMP_ALL;
		}
		else if (argc != 4)
			goto usage;
		else {						/* if 'boardConfig write <field> <val>' */
			value = simple_strtoul(argv[3], NULL, 10);
			status = do_boardCfg_write(field, value);
		}

		if (status != 0)
			return 1;
		else {
			do_boardCfg_read(field);
			printf("\nChanges will be applied after reset.\n\n");
			return 0;
		}
	}

usage:
	cmd_usage(cmdtp);
	return 1;
}

U_BOOT_CMD(boardConfig, 6, 1, do_boardCfg,
	"Board Configuration sub-system",

	"list <option>				- list board configuration options\n\n"

	"boardConfig read			- print all board configuration values\n"
	"boardConfig read <option>		- print the requested field value\n\n"

	"boardConfig write default		- restore all board configuration fields to their default value\n"
	"boardConfig write <option> <val>	- write the requested field with a specified value\n\n"

	"Options:\n"
	MV_BOARD_CONFIG_CMD_STR
);
#endif /*defined(CONFIG_CMD_BOARDCFG)*/

