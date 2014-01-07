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
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "boardEnv/mvBoardEnvLib.h"

typedef struct _boardConfig {
	MV_SATR_TYPE_ID ID;
	char *Name;
	MV_U8 numOfValues;
	char *Value[5];
} MV_BOARD_CONFIG_VALUE;

/* MV_SATR_TYPE_ID,		 Name,		numOfValues,	Possible Values */
MV_BOARD_CONFIG_VALUE boardConfig[] = {
{ MV_CONFIG_MAC0,		"MAC0",				4, {"Switch port 6", "GBE PHY#0", "RGMII-0", "SGMII(DB-6660)"} },
{ MV_CONFIG_MAC1,		"MAC1",				3, {"RGMII-1", "Switch port 4", "GBE PHY#3"} },
{ MV_CONFIG_PON_SERDES,		"PON SERDES",			2, {"PON MAC", "MAC1"} },
{ MV_CONFIG_PON_BEN_POLARITY,	"PON POLARITY (DB-6660)",	2, {"BEN active low", "BEN active low"} },
{ MV_CONFIG_SGMII0_CAPACITY,	"SGMII-0 Capacity",		2, {"1G", "2.5G"} },
{ MV_CONFIG_SGMII1_CAPACITY,	"SGMII-1 Capacity",		2, {"1G", "2.5G"} },
{ MV_CONFIG_LANE1,		"SerDes Lane#1 (DB-6660)",	3, {"PCIe-1", "SGMII-0", "SATA-1"} },
{ MV_CONFIG_LANE2,		"SerDes Lane#2 (DB-6660)",	2, {"SATA-0", "SGMII-0"} },
{ MV_CONFIG_LANE3,		"SerDes Lane#3 (DB-6660)",	3, {"USB3.0", "SGMII-0", "SATA-1"} },
{ MV_CONFIG_MAC0_SW_SPEED,	"MAC0 to Switch Speed",		2, {"2G", "1G"} },
{ MV_CONFIG_DEVICE_BUS_MODULE,	"Device Bus Module (DB-6660)",	4, {"None", "RGMII Module", "I2S Audio Module", "SPDIF Audio Module"} },
{ MV_CONFIG_SLIC_TDM_DEVICE,	"TDM module",			5, {"None", "SSI", "ISI", "ZSI", "TDM"} },
{ MV_CONFIG_DDR_BUSWIDTH,	"DDR bus-width (DB-6660)",	2, {"32-bit", "16-bit"} },
};


MV_CONFIG_TYPE_ID configToEnum(char *name)
{
	if (name == NULL)
		goto error;
	else if (strcmp(name, "mac0") == 0)
		return MV_CONFIG_MAC0;
	else if (strcmp(name, "mac1") == 0)
		return MV_CONFIG_MAC1;
	else if (strcmp(name, "ponserdes") == 0)
		return MV_CONFIG_PON_SERDES;
	else if (strcmp(name, "ponpolarity") == 0)
		return MV_CONFIG_PON_BEN_POLARITY;
	else if (strcmp(name, "sgmii0capacity") == 0)
		return MV_CONFIG_SGMII0_CAPACITY;
	else if (strcmp(name, "sgmii1capacity") == 0)
		return MV_CONFIG_SGMII1_CAPACITY;
	else if (strcmp(name, "serdes1") == 0)
		return MV_CONFIG_LANE1;
	else if (strcmp(name, "serdes2") == 0)
		return MV_CONFIG_LANE2;
	else if (strcmp(name, "serdes3") == 0)
		return MV_CONFIG_LANE3;
	else if (strcmp(name, "mac0_switch_speed") == 0)
		return MV_CONFIG_MAC0_SW_SPEED;
	else if (strcmp(name, "devicebus") == 0)
		return MV_CONFIG_DEVICE_BUS_MODULE;
	else if (strcmp(name, "tdm") == 0)
		return MV_CONFIG_SLIC_TDM_DEVICE;
	else if (strcmp(name, "ddr_buswidth") == 0)
		return MV_CONFIG_DDR_BUSWIDTH;
	else if (strcmp(name, "default") == 0)
		return MV_CONFIG_TYPE_CMD_SET_DEFAULT;

error:
	printf("Error: there is not board configuration named '%s'\n", name);
	return MV_ERROR;
}


/* Description:
 *	reset all configuration fields to it's default setup (0x0 for all fields)
 * Usage:
 *	'boardConfig write default

 *	EEPROM expected mapping:
 *	[0x0] - configuration 1st byte
 *	[0x1] - configuration 2nd byte
 *	[0x2] - configuration 3rd byte
 *	[0x4-0x7] - 32bit pattern to detect if EEPROM is initialized */
static int do_boardCfg_default(void)
{
	MV_U8 i;

	/* 0x0 is the default value for all configuration fields */
	for (i = 0x0; i < 4; i++) {
		/* write default board configuration (default value for all fields is 0x0) */
		if (mvBoardTwsiSet(BOARD_DEV_TWSI_EEPROM, 0, i, 0x0) != MV_OK) {
			mvOsPrintf("%s: Error: Set default configuration to EEPROM failed\n", __func__);
			goto error;
		}
	}

	/* Reset local array data to default as well */
	for (i = 0; i <	MV_CONFIG_TYPE_MAX_OPTION; i++)
		mvCtrlSysConfigSet(i, 0);

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
		val = mvCtrlSysConfigGet(i);
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
	MV_U32 rev;

	if (writeVal >= boardConfig[field].numOfValues) {
		printf("Error: write value is invalid - See 'boardConfig list <field>'\n\n");
		goto error;
	}

	/* SATA-1 support:
	 * - change lanes possibilites in Dip-Switch/EEPROM (move serdes field to 3rd configuration byte
	 * - add support for lane selector more bits
	 * - (see cider differences between common phy selector bits regarding SATA-1, @0x18300)
	 */
	if ((field == MV_CONFIG_LANE1 || field == MV_CONFIG_LANE3) && writeVal == 0x2) {
		rev = mvCtrlRevGet();
		if (rev == MV_88F66X0_Z1_ID || rev == MV_88F66X0_Z2_ID || rev == MV_88F66X0_Z3_ID)
			printf("Error: SATA-1 is not supported in Z stepping revision\n");
		else
			printf("SATA-1 support is not implemented yet\n");
		goto error;
	}

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
	MV_U32 boardId = mvBoardIdGet();
	MV_U8 value;
	MV_CONFIG_TYPE_ID field;
	int status = 0;

	if (boardId != DB_6650_ID && boardId != DB_6660_ID) {
		printf("Error: EEPROM is available only on Development boards (DB-6650/60)\n");
		return 0;
	}

	if (isEepromEnabledFlag == -1)
		isEepromEnabledFlag = (mvBoardIsEepromEnabled() == MV_TRUE) ? 1 : 0;

	if (isEepromEnabledFlag == 0) {
		printf("Error: EEPROM is currently not accessible on board'\n");
		return 0;
	}

	/* need at least another argument 'write'/ 'list' /'read' */
	if (argc < 2) {
		printf("argc <= 2\n");
		goto usage;
	}
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
	"mac0, mac1, mac0_switch_speed, sgmii0capacity, sgmii1capacity,\n"
	"serdes1, serdes2, serdes3, ponserdes, ponpolarity, devicebus, tdm, ddr_buswidth\n\n"
);
#endif /*defined(CONFIG_CMD_BOARDCFG)*/
