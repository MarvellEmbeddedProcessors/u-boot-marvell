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
#if defined(CONFIG_CMD_EFUSE)
#include "mvCommon.h"
#include "mvOs.h"
#include "mvCpuIf.h"

#undef DEBUG
/* Access DRAM instead of eFuse */
#undef DRY_RUN

#ifdef DEBUG
#define DBPR(x...)		printf(x);
#else
#define DBPR(x...)
#endif

/* EFUSE control */
#define EFUSE_WIN_ID					5
#define EFUSE_WIN_CTRL_VAL				((0xF << 16) | (0x0A << 8) | (0xE << 4) | 0x1)
#ifdef DRY_RUN
#define EFUSE_WIN_BASE_VAL				0x2000000
#else
#define EFUSE_WIN_BASE_VAL				0xF6000000
#endif

#define EFUSE_DFX_SRV_REG_ADDR			(0xF82A4 >> 2) /* Shifted as required by eFuse address field format */
#define EFUSE_BASE_ADDR					0xF9000
#define EFUSE_BIT_31_0_ADDR(num)		(EFUSE_BASE_ADDR + (num) * 16)
#define EFUSE_BIT_63_32_ADDR(num)		(EFUSE_BASE_ADDR + 4 + (num) * 16)
#define EFUSE_BIT_64_ADDR(num)			(EFUSE_BASE_ADDR + 8 + (num) * 16)

#define EFUSE_SW_CTRL_REG				0xE4008
#define EFUSE_SW_CTRL_PROG_EN_OFFS		31
#define EFUSE_SW_CTRL_PROG_EN_MASK		(1 << EFUSE_SW_CTRL_PROG_EN_OFFS)

#define EFUSE_BURN_ADDR_SEC_CTRL		24
#define EFUSE_BURN_ADDR_RSA_KEY_0_63	26
#define EFUSE_BURN_ADDR_CSK_IDX_0_63	31
#define EFUSE_BURN_ADDR_FLASH_ID		47
#define EFUSE_BURN_ADDR_BOX_ID			48

/* Secure boot related eFuse entries */
#define SEC_BOOT_MODE_OFFS				0
#define SEC_BOOT_MODE_MASK				(0x1 << SEC_BOOT_MODE_OFFS)
#define SEC_JTAG_DSBL_OFFS				1
#define SEC_JTAG_DSBL_MASK				(0x1 << SEC_JTAG_DSBL_OFFS)
#define SEC_BOOT_DEV_OFFS				8
#define SEC_BOOT_DEV_MASK				(0xFF << SEC_BOOT_DEV_OFFS)
#define SEC_BOOT_CSK_IDX_OFFS			0
#define SEC_BOOT_CSK_IDX_MASK			(0x1 << SEC_BOOT_CSK_IDX_OFFS)

#define MASK_ALL_32BIT					0xFFFFFFFFUL

#define MV_EFUSE_REG_READ(offset)		\
		MV_MEMIO_LE32_READ(EFUSE_WIN_BASE_VAL | (offset))

#define MV_EFUSE_REG_WRITE(offset, val)	\
		MV_MEMIO_LE32_WRITE(EFUSE_WIN_BASE_VAL | (offset), (val))

#define MV_EFUSE_REG_BIT_SET(offset, bitMask)	\
		MV_MEMIO32_WRITE(EFUSE_WIN_BASE_VAL | (offset), \
		MV_MEMIO32_READ(EFUSE_WIN_BASE_VAL | (offset)) | MV_32BIT_LE_FAST(bitMask))

#define MV_EFUSE_REG_BIT_RESET(offset,bitMask)	\
		MV_MEMIO32_WRITE(EFUSE_WIN_BASE_VAL | (offset), \
		MV_MEMIO32_READ(EFUSE_WIN_BASE_VAL | (offset)) & MV_32BIT_LE_FAST(~bitMask))


typedef enum _efuseDefId {
	EFUSE_RSAKEY_ID = 0,
	EFUSE_CSKIDX_ID,
	EFUSE_BOXID_ID,
	EFUSE_FLASHID_ID,
	EFUSE_JTAGDIS_ID,
	EFUSE_BOOTSRC_ID,
	EFUSE_SECURE_ID,
	EFUSE_INVALID_ID
} efuseDefId;

typedef struct _efuseDef {
	char		*name;
	MV_U32		efuseNum;	/* Efuse number - for multi filed values, the number of the first field */
	efuseDefId	efid;
	MV_U32		length;		/* Number of 32-bit efuse words the value stored in */
	MV_U32		totalBytes;	/* Number of bytes in the field value */
	MV_U32		mask;		/* Value to apply to a single 32-bit word - valid only if length == 1*/
	MV_U32		offset;		/* Position ov bits inside the 32-bit word - valid only if length == 1*/
} efuseDef;

static efuseDef eFuses[] = {
	/* Name		eFuse Number						eFuse ID			Length	totalBytes
														Mask					Offset*/
	{ "RSAKEY",		EFUSE_BURN_ADDR_RSA_KEY_0_63,	EFUSE_RSAKEY_ID,	10, 	32,
														MASK_ALL_32BIT,			0 },
	{ "CSKIDX",		EFUSE_BURN_ADDR_CSK_IDX_0_63,	EFUSE_CSKIDX_ID,	32, 	0,
														SEC_BOOT_CSK_IDX_MASK,	SEC_BOOT_CSK_IDX_OFFS },
	{ "BOXID",		EFUSE_BURN_ADDR_BOX_ID,			EFUSE_BOXID_ID, 	1,		4,
														MASK_ALL_32BIT,			0 },
	{ "FLASHID",	EFUSE_BURN_ADDR_FLASH_ID,		EFUSE_FLASHID_ID,   1,		4,
														MASK_ALL_32BIT,			0 },
	{ "JTAGDIS",	EFUSE_BURN_ADDR_SEC_CTRL,		EFUSE_JTAGDIS_ID,   1,		0,
														SEC_JTAG_DSBL_MASK,		SEC_JTAG_DSBL_OFFS },
	{ "BOOTSRC",	EFUSE_BURN_ADDR_SEC_CTRL,		EFUSE_BOOTSRC_ID,   1,		0,
														SEC_BOOT_DEV_MASK,		SEC_BOOT_DEV_OFFS },
	{ "SECURE",		EFUSE_BURN_ADDR_SEC_CTRL,		EFUSE_SECURE_ID,	1,		0,
														SEC_BOOT_MODE_MASK,		SEC_BOOT_MODE_OFFS }
};

/*******************************************************************************
*    init_efuse_hw
*          Initialize HW required for eFuse access
*    INPUT:
*          none
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
static void init_efuse_hw(void)
{
	static int init_done = 0;

	if (init_done != 0)
		return;

#ifdef DRY_RUN
	memset((void *)(EFUSE_WIN_BASE_VAL | EFUSE_BASE_ADDR), 0, 3 * 8 * 50); /* Each of 50 eFuses is 3 DWORDs long */
#else
	/* Open window to eFuse(DFX) registers */
	MV_REG_WRITE(AHB_TO_MBUS_WIN_CTRL_REG(EFUSE_WIN_ID), EFUSE_WIN_CTRL_VAL);
	MV_REG_WRITE(AHB_TO_MBUS_WIN_BASE_REG(EFUSE_WIN_ID), EFUSE_WIN_BASE_VAL);
	MV_REG_WRITE(AHB_TO_MBUS_WIN_REMAP_LOW_REG(EFUSE_WIN_ID), 0);
	MV_REG_WRITE(AHB_TO_MBUS_WIN_REMAP_HIGH_REG(EFUSE_WIN_ID), 0);
#endif
	init_done = 1;
}

/*******************************************************************************
*    write_efuse_hw
*          Run the procedure required for burning eFuse in the HW
*    INPUT:
*          efDef 	efuse definition structure
*          values	array of efuse entry values
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
static int write_efuse_hw(efuseDef *efDef, MV_U32 values[])
{
	MV_U32	efuseNum;
	int		dwordIdx;	/* current eFuse DWORD (0 or 1) */
	int		ret = 0;
	MV_U32	regVal;
	static MV_U32 secFuse = 0;

	/* Enable eFuse programming */
#ifndef DRY_RUN
	regVal = MV_REG_READ(EFUSE_SW_CTRL_REG);
	DBPR("Read from %08X: [%08X]\n", EFUSE_SW_CTRL_REG, regVal);
	regVal |= EFUSE_SW_CTRL_PROG_EN_MASK;
	MV_REG_WRITE(EFUSE_SW_CTRL_REG, regVal);
	DBPR("Write to  %08X: [%08X]\n", EFUSE_SW_CTRL_REG, regVal);
#endif
	/* Each eFuse is represented by 2 DWORDs + FSB in the HW. Walk trough every DWORD */
	for (dwordIdx = 0, efuseNum = efDef->efuseNum;
		dwordIdx < efDef->length;
		dwordIdx += 2, efuseNum++) {

		/* Skip empty entries - possible when programming CSK indexes */
		if ((values[dwordIdx] == 0) && (values[dwordIdx + 1] == 0))
			continue;

		printf("Burning the eFuse %#0x[%d] \n", EFUSE_BIT_31_0_ADDR(efuseNum), efuseNum);

		/* Prepare register(s) values for the burn procedure */
		/* Re-load eFuse value before write */
		regVal = MV_EFUSE_REG_READ(EFUSE_BIT_31_0_ADDR(efuseNum));
		DBPR("Read from %08X: [%08X]\n", EFUSE_BIT_31_0_ADDR(efuseNum), regVal);
		regVal = MV_EFUSE_REG_READ(EFUSE_BIT_63_32_ADDR(efuseNum));
		DBPR("Read from %08X: [%08X]\n", EFUSE_BIT_63_32_ADDR(efuseNum), regVal);
		regVal = MV_EFUSE_REG_READ(EFUSE_BIT_64_ADDR(efuseNum));
		DBPR("Read from %08X: [%08X]\n", EFUSE_BIT_64_ADDR(efuseNum), regVal);

		/* Since the entire eFuse line should be written as whole, the components
		   for the Secure Boot Entry should be accumulited somewhere prior to writing them to the HW */
		if ((efDef->efid == EFUSE_JTAGDIS_ID) || (efDef->efid == EFUSE_BOOTSRC_ID)) {
			secFuse |= (values[dwordIdx] << efDef->offset) & efDef->mask;
			DBPR("Store eFuse %08X value [%08X] internally\n", EFUSE_BIT_64_ADDR(efuseNum), secFuse);
			break;
		}

		/* Even DWORDs */
		regVal = (values[dwordIdx] << efDef->offset) & efDef->mask;
		if (efDef->efid == EFUSE_SECURE_ID) /* Last write to the Secure Boot Entry */
			regVal |= secFuse;
		MV_EFUSE_REG_WRITE(EFUSE_BIT_31_0_ADDR(efuseNum), regVal);
		DBPR("Write to  %08X: [%08X]\n", EFUSE_BIT_31_0_ADDR(efuseNum), regVal);

		/* Odd DWORDs */
		regVal = (values[dwordIdx + 1] << efDef->offset) & efDef->mask;
		/* Bit[56] of the secure boot entry should be set for indicating "Long form" format.
		   This part must be set on last write before marking the eFuse line as "valid" */
		if (efDef->efid == EFUSE_SECURE_ID)
			regVal = EFUSE_DFX_SRV_REG_ADDR | BIT24;
		MV_EFUSE_REG_WRITE(EFUSE_BIT_63_32_ADDR(efuseNum), regVal);
		DBPR("Write to  %08X: [%08X]\n", EFUSE_BIT_63_32_ADDR(efuseNum), regVal);

		/* Validity bits */
		/* Multi-DWORD eFuses are modified only once. Writing bit[64] writes down the register valules
		   into eFuse line and locked it down preventing from further writes into the same eFuse line.
		   Activation of secure boot mode validates the secure boot eFuse line and all preceding lines.
		   Writing CSK entry must lock the appropriate eFuse line.
		   The eFuse content is valid (and protected from writes) when bit[64] of the eFuse line is set.
		 */
		DBPR("Write to  %08X: [%08X]\n", EFUSE_BIT_64_ADDR(efuseNum), 0x1);
		MV_EFUSE_REG_BIT_SET(EFUSE_BIT_64_ADDR(efuseNum), 0x1);
		mvOsDelay(5);

		/* In case of last write to the secure boot entry, all the preceding eFuse lines
		   must be marked as "valid". This will allow the HW engine accessing the eFuse
		   line by line to reach the secure boot entry itself */
		if (efDef->efid == EFUSE_SECURE_ID) {
			int line;
			MV_U32 val[3];
			for (line = 0; line < EFUSE_BURN_ADDR_SEC_CTRL; line++) {
				val[0] = MV_EFUSE_REG_READ(EFUSE_BIT_31_0_ADDR(line));
				DBPR("Read from %08X: [%08X]\n", EFUSE_BIT_31_0_ADDR(line), val[0]);
				val[1] = MV_EFUSE_REG_READ(EFUSE_BIT_63_32_ADDR(line));
				DBPR("Read from %08X: [%08X]\n", EFUSE_BIT_63_32_ADDR(line), val[1]);
				val[2] = MV_EFUSE_REG_READ(EFUSE_BIT_64_ADDR(line));
				DBPR("Read from %08X: [%08X]\n", EFUSE_BIT_64_ADDR(line), val[2]);

				if (val[2] == 0) { /* If the eFuse not yet burned by tester */
					/* invalidate eFuse data before validating the row */
					val[0] = 0xFFFFFFFF;
					val[1] = 0x00FFFFFF;

					MV_EFUSE_REG_WRITE(EFUSE_BIT_31_0_ADDR(line), val[0]);
					DBPR("Write to  %08X: [%08X]\n", EFUSE_BIT_31_0_ADDR(line), val[0]);
					MV_EFUSE_REG_WRITE(EFUSE_BIT_63_32_ADDR(line), val[1]);
					DBPR("Write to  %08X: [%08X]\n", EFUSE_BIT_63_32_ADDR(line), val[1]);
					MV_EFUSE_REG_BIT_SET(EFUSE_BIT_64_ADDR(line), 0x1);
					DBPR("Write to  %08X: [%08X]\n", EFUSE_BIT_64_ADDR(line), 0x1);
					mvOsDelay(5);
				}
			}
		}

	} /* For each eFuse */

	/* Clear eFuse programming mode */
#ifndef DRY_RUN
	regVal = MV_REG_READ(EFUSE_SW_CTRL_REG);
	DBPR("Read from %08X: [%08X]\n", EFUSE_SW_CTRL_REG, regVal);
	regVal &= ~EFUSE_SW_CTRL_PROG_EN_MASK;
	MV_REG_WRITE(EFUSE_SW_CTRL_REG, regVal);
	DBPR("Write to  %08X: [%08X]\n", EFUSE_SW_CTRL_REG, regVal);
#endif
	return ret;
} /* end of write_efuse_hw */

/*******************************************************************************
*    do_efuse_set
*          Equire efuse value and burn the appropriate efuse field
*    INPUT:
*          name 	efuse entry name
*          value	efuse entry value
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
static int do_efuse_set(char *const name, char *const value)
{
	int 	fuseIdx, dwordIdx, byteIdx;
	int 	ret = -1;
	MV_U32	efuseVal[32];
	char	asciiWord[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	char	*efuseBytes = (char *)efuseVal;
	char	*inputPos;

	memset(efuseVal, 0, sizeof(MV_U32) * 32);
	/* Walk trough the fuses definitions and find the efuse requested for writing */
	for (fuseIdx = 0; fuseIdx < EFUSE_INVALID_ID; fuseIdx++) {

		if (strcmp(eFuses[fuseIdx].name, name) != 0)
			continue;

		if (eFuses[fuseIdx].efid == EFUSE_CSKIDX_ID) {
			/* Handling CSK indexes as decimal values */
			int cskIdx = (int)simple_strtoul(value, NULL, 10);
			int cskEntry;

			if ((cskIdx < 0) || (cskIdx > 15)) {
				printf("Wrong CSK key index %d. Supported values are 0 to 15\n", cskIdx);
				goto efuse_set_stop;
			}

			ret = 0;
			if (cskIdx == 0) {
				printf("No eFuse modification is needed for index 0\n");
				goto efuse_set_stop;
			}

			/* Invalidate all preceding entries. First zero entry counted as valid. */
			for (cskEntry = 0; cskEntry < cskIdx; cskEntry++)
					efuseVal[cskEntry * 2] = 1;

			/* Print the acquired values and warn the user for a last time... */
			printf("\nYou are about to invalidate CSK indexes 0 to %d\n", (cskIdx - 1));

		} else { /* rest of eFuses */

			/* Check that value string does not exeed the boundary */
			if ((eFuses[fuseIdx].length > 1) ||
				(eFuses[fuseIdx].mask == MASK_ALL_32BIT)) {
				if (strlen(value) != eFuses[fuseIdx].totalBytes * 2) {
					printf("\nWrong value length %d, expected %d HEX numbers\n",
						strlen(value), eFuses[fuseIdx].totalBytes * 2);
					goto efuse_set_stop;
				}
			}

			/* Chop input value into chunks and convert to DWORDs */
			for (dwordIdx = 0, inputPos = value; dwordIdx < eFuses[fuseIdx].length; dwordIdx++) {
				if (dwordIdx & 0x1) {
					/* Odd DWORDs - eFuse allows 3 bytes insertion - bit[55:32] */
					strncpy(asciiWord, inputPos, 6);
					inputPos += 6;
					asciiWord[6] = asciiWord[7] = '0';
				} else {
					/* Even DWORDs - eFuse allows 4 bytes insertion - bit[31:0] */
					strncpy(asciiWord, inputPos, 8);
					inputPos += 8;
				}
				/* The multi-DWORD values like RSA are kept in efuse
				   in a network bytes order (BE). SOC registers are always in LE */
				if (eFuses[fuseIdx].length > 1)
					efuseVal[dwordIdx] = (MV_U32)htonl(simple_strtoul(asciiWord, NULL, 16));
				else
					efuseVal[dwordIdx] = (MV_U32)simple_strtoul(asciiWord, NULL, 16);
			}
			if (eFuses[fuseIdx].length == 1) {
				/* For less than single 32-bit word feeds we should make sure that
				   the acquired value fits the specific efuse field */
				if (efuseVal[0] > eFuses[fuseIdx].mask >> eFuses[fuseIdx].offset) {
					printf("\nThe value %#08X cannot fit the efuse %s\n",
						efuseVal[0], eFuses[fuseIdx].name);
					goto efuse_set_stop;
				}
			}

			/* Print the acquired values and warn the user for a last time... */
			printf("\nYou are about to set \"%s\" efuse [%08X] with value(s)\n\n",
				eFuses[fuseIdx].name, EFUSE_BIT_31_0_ADDR(eFuses[fuseIdx].efuseNum));

			if (eFuses[fuseIdx].length > 1) {
				int indexCorrection = 0;
				for (byteIdx = 0; byteIdx < eFuses[fuseIdx].totalBytes; byteIdx++) {
					indexCorrection = byteIdx / 7; /* jump over empty byte in odd DWORDs */
					printf("%02X", efuseBytes[byteIdx + indexCorrection]);
				}
			} else
				printf("%08X", efuseVal[0]);
		}

		ret = 0;
		printf("\n\nThis is irreversible change! Type \"YES\" for continue, or anything else for cancel");
		readline(" ");
		if (strcmp(console_buffer,"YES") != 0) {
			printf("\nCancelling changes on user request!\n");
			goto efuse_set_stop;
		}

		break;

	} /* For every index in efuses array */

	if (ret == 0) {
		printf("\n");
		ret = write_efuse_hw(&eFuses[fuseIdx], efuseVal);
		if (ret != 0)
			printf("\nThe HW burn procedure has failed!\n\n");
	} else
		printf("\nBad efuse name, operation cancelled\n\n");

efuse_set_stop:

	return ret;
} /* end of do_efuse_set */

/*******************************************************************************
*    do_efuse_get
*          read efuse value
*    INPUT:
*          name 	efuse entry name
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
static int do_efuse_get(char *const name)
{
	int 	fuseIdx, byteIdx, efuseNum;
	int 	ret = -1;
	MV_U32	efuseVal[2];
	char	*efuseBytes = (char *)&efuseVal[0];
	int		bytesCount = 0;

	for (fuseIdx = 0; fuseIdx < EFUSE_INVALID_ID; fuseIdx++) {

		if (strcmp(eFuses[fuseIdx].name, name) == 0) {
			ret = 0;

			printf("Value(s) obtained from eFuse \"%s\" [%#08X]:\n",
				eFuses[fuseIdx].name, EFUSE_BIT_31_0_ADDR(eFuses[fuseIdx].efuseNum));

			for (efuseNum = eFuses[fuseIdx].efuseNum;
				 efuseNum <= (eFuses[fuseIdx].efuseNum + (eFuses[fuseIdx].length - 1)/ 2);
				 efuseNum++) {

				efuseVal[0] = MV_EFUSE_REG_READ(EFUSE_BIT_31_0_ADDR(efuseNum));
				efuseVal[1] = MV_EFUSE_REG_READ(EFUSE_BIT_63_32_ADDR(efuseNum));

				if (eFuses[fuseIdx].length > 1) { /* multi-DWORD efuse */
					if (eFuses[fuseIdx].efid == EFUSE_CSKIDX_ID) {
						/* Special case - the first non-zero eFuse entry is the CSK index */
						if (efuseVal[0] == 0) {
							printf("CSK index is %d", efuseNum - eFuses[fuseIdx].efuseNum);
							break;
						}
					} else {
						/* Print 4 bytes from even DWORDs and 3 bytes from odd ones */
						for (byteIdx = 0; byteIdx < 7; byteIdx++) {
							if (++bytesCount <= eFuses[fuseIdx].totalBytes)
								printf("%02X", efuseBytes[byteIdx]);
						}
					}
				} else {
					efuseVal[0] &= eFuses[fuseIdx].mask;
					efuseVal[0] >>= eFuses[fuseIdx].offset;
					printf("  %08X\t", efuseVal[0]);
				} /* multi-DWORD efuse */

			} /* For each efuse */

			printf("\n");
		} /* If the right efuse name is found*/
	} /* For every index in efuses array */

	if (ret == -1)
		printf("\nBad efuse name, operation cancelled\n\n");

	return ret;
} /* end of do_efuse_get */

/*******************************************************************************
*    do_efuse_dump
*          dump all efuse values
*    INPUT:
*          none
*    OUTPUT:
*          none
*    RETURN:
*          0 on success
*******************************************************************************/
static void do_efuse_dump(void)
{
	int 	dwordIdx, bitIdx, efuseNum;
	MV_U32	efuseVal[3];

	printf("    6         5         4         3         2         1         0|                |  |\n");
	printf("43210987654321098765432109876543210987654321098765432109876543210|      HEX       |ID|\n");
	printf("-----------------------------------------------------------------|----------------|--|\n");

	for (efuseNum = EFUSE_BURN_ADDR_SEC_CTRL; efuseNum <= EFUSE_BURN_ADDR_BOX_ID; efuseNum++) {
		efuseVal[0] = MV_EFUSE_REG_READ(EFUSE_BIT_31_0_ADDR(efuseNum));
		efuseVal[1] = MV_EFUSE_REG_READ(EFUSE_BIT_63_32_ADDR(efuseNum));
		efuseVal[2] = MV_EFUSE_REG_READ(EFUSE_BIT_64_ADDR(efuseNum));

		/* Print single bits */
		printf("%d", efuseVal[2] & 0x1);
		for (dwordIdx = 1; dwordIdx >= 0; dwordIdx--) {
			for (bitIdx = 31; bitIdx >=0; bitIdx--)
				printf("%d", (efuseVal[dwordIdx] >> bitIdx) & 0x1);
		}

		/* Print HEX values */
		printf("|%08X%08X|%02d|\n", efuseVal[1], efuseVal[0], efuseNum);
	}

}
/*******************************************************************************
*    burn_efuse_cmd
*          handle efuse set/get command
*    RETURN:
*          0 on success
*******************************************************************************/
int burn_efuse_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	char	*cmd, *name;

	init_efuse_hw();

	if ((argc == 2) &&
		(strncmp(argv[1], "dump", 4) == 0)) {
		do_efuse_dump();
		return 0;
	}

	/* need at least 3 arguments */
	if (argc < 3)
		goto usage;

	cmd = argv[1];
	name = argv[2];

	if (strncmp(cmd, "set", 3) == 0) {
		if (argc != 4)
			goto usage;
		do_efuse_set(name, argv[3]);
	} else if (strncmp(cmd, "get", 3) == 0)
		do_efuse_get(name);


	return 0;
usage:
	return cmd_usage(cmdtp);
} /* end of burn_efuse_cmd */

U_BOOT_CMD(
	efuse,      4,     1,      burn_efuse_cmd,
	"eFuse manipulation subsystem for secure boot mode",
	"set name value - Write value to eFuse \"name\"\n"
	"efuse get name       - Read value from eFuse \"name\"\n"
	"efuse dump           - Print all eFuse entries in RAW format\n"
	"===========================================================================================================\n"
	"Supported | RSAKEY   32 byte - SHA-256 digest of RSA-2048 public key (KAK) in DER format. For example:\n"
	"  eFuse   |                    64C50CA3A4AF47D7FC5F61A7D5C3FA58C483BA2E86F61E99460963A8DF3DCFE0\n"
	"  names   | CSKIDX   dec.num.- Configures DEC index of valid CSK entry. Valid range is 0 to 15.\n"
	"          |                    Pay attention that all preceding CSK indexes will be invalidated.\n"
	"          |                    For instance if programmed index is 3, indexes 0,1,2 will became invalid.\n"
	"          | BOXID    4 byte  - Configures HEX Box ID for secure boot mode. For example DEADBEEF\n"
	"          | FLASHID  4 byte  - Configures HEX Flash ID for secure boot mode. For example BADDF00D\n"
	"          | BOOTSRC  1 byte  - Configures HEX value of boot device available in secure boot mode.\n"
	"          | JTAGDIS  1 bit   - Disables JTAG interface in secure boot mode.\n"
	"          | SECURE   1 bit   - Enables secure boot mode and protects all eFuses from further modifications\n"
	"          |                    Setting this value MUST be the LAST command in eFuse configuration sequence!\n"
	"===========================================================================================================\n"
	"WARNING:   EACH EFUSE CAN BE SET ONLY ONCE! PLEASE DOUBLE-CHECK YOUR DATA!\n"
	"===========================================================================================================\n"
	);

#endif /*defined(CONFIG_CMD_EFUSE)*/
