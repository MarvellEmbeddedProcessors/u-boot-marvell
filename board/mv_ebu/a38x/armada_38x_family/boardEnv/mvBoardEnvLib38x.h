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

*	Redistributions of source code must retain the above copyright notice,
		this list of conditions and the following disclaimer.

*	Redistributions in binary form must reproduce the above copyright
		notice, this list of conditions and the following disclaimer in the
		documentation and/or other materials provided with the distribution.

*	Neither the name of Marvell nor the names of its contributors may be
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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* {{MV_MODULE_TYPE_ID ConfigID, twsi-ID,  Offset, ID,  isActiveForBoard[]}} */
#define MV_MODULE_INFO { \
{ MV_MODULE_MII,		0x1,	0,	 0x4,	{ 0, 1, 0, 0} }, \
{ MV_MODULE_SLIC_TDM_DEVICE,	0x0,	0,	 0x1,	{ 0, 1, 0, 0} }, \
{ MV_MODULE_I2S_DEVICE,		0x1,	0,	 0x3,	{ 0, 1, 0, 0} }, \
{ MV_MODULE_SPDIF_DEVICE,	0x1,	0,	 0x2,	{ 0, 1, 0, 0} }, \
{ MV_MODULE_NOR,		0x4,	0,	 0xF,	{ 0, 1, 0, 0} }, \
{ MV_MODULE_NAND,		0x4,	0,	 0x1,	{ 0, 1, 0, 0} }, \
{ MV_MODULE_SDIO,		0x4,	0,	 0x2,	{ 0, 1, 0, 0} }, \
{ MV_MODULE_SGMII,		0x2,	0,	 0xF,	{ 0, 1, 0, 0} }, \
};

/*						bit    TWSI           Reg	board	*/
/*   name		SATR-ID			Mask  offset  devID   num	active	*/
#define MV_SAR_INFO { \
{ "freq",	MV_SATR_CPU_DDR_L2_FREQ,	0x1F,   0,      1,      0,      {1, 1, 1, 1, 1}, BOARD_SATR_SWAP_BIT},\
{ "coreclock",	MV_SATR_CORE_CLK_SELECT,	0x04,   2,      3,      0,      {0, 1, 0, 0, 0}, 0},\
{ "cpusnum",	MV_SATR_CPU1_ENABLE,		0x01,	0,	2,	0,	{0, 1, 0, 0, 0}, 0},\
{ "sscg",	MV_SATR_SSCG_DISABLE,		0x08,	3,	3,	0,	{0, 1, 0, 0, 0}, 0},\
{ "ddr4select",	MV_SATR_DDR4_SELECT,		0x20,	5,	4,	1,	{0, 1, 0, 0, 0}, BOARD_SATR_READ_ONLY},\
{ "ddrbuswidth",     MV_SATR_DDR_BUS_WIDTH,	0x08,	3,	0,	0,	{1, 1, 1, 1, 1}, 0},\
{ "ddreccenable",    MV_SATR_DDR_ECC_ENABLE,	0x10,	4,	0,	0,	{1, 1, 1, 1, 1}, 0},\
{ "ddreccpupselect", MV_SATR_DDR_ECC_PUP_SEL,	0x20,	5,	0,	0,	{0, 1, 0, 0, 0}, 0},\
{ "sgmiispeed", MV_SATR_SGMII_SPEED,		0x40,	6,	0,	0,	{1, 1, 1, 1, 1}, 0},\
{ "bootsrc",	MV_SATR_BOOT_DEVICE,		0x3,	0,	3,	0,	{0, 1, 0, 0, 0}, BOARD_SATR_SWAP_BIT},\
{ "boarsrc2",	MV_SATR_BOOT2_DEVICE,		0x1E,	1,	2,	0,	{0, 1, 0, 0, 0}, BOARD_SATR_SWAP_BIT},\
{ "boardid",	MV_SATR_BOARD_ID,		0x7,	0,	0,	0,	{1, 1, 1, 1, 1}, 0},\
{ "ecoversion",	MV_SATR_BOARD_ECO_VERSION,	0xff,	0,	0,	1,	{1, 1, 1, 1, 1}, BOARD_SATR_READ_ONLY},\
{ "usb3port0",	MV_SATR_DB_USB3_PORT0,		0x1,	0,	1,	1,	{0, 1, 0, 0, 0}, 0},\
{ "usb3port1",	MV_SATR_DB_USB3_PORT1,		0x2,	1,	1,	1,	{0, 1, 0, 0, 0}, 0},\
{ "rdserdes4",	MV_SATR_RD_SERDES4_CFG,		0x4,	2,	1,	1,	{1, 0, 1, 0, 0}, 0},\
{ "gpserdes5",	MV_SATR_GP_SERDES5_CFG,		0x4,	2,	1,	1,	{0, 0, 0, 0, 1}, 0},\
{ "dbserdes1",	MV_SATR_DB_SERDES1_CFG,		0x7,	0,	0,	1,	{0, 1, 0, 0, 0}, 0},\
{ "dbserdes2",	MV_SATR_DB_SERDES2_CFG,		0x38,	3,	0,	1,	{0, 1, 0, 0, 0}, 0},\
{ "sgmiimode",	MV_SATR_SGMII_MODE,		0x40,	6,	0,	1,	{0, 1, 0, 0, 0}, 0},\
{ "max_option",	MV_SATR_MAX_OPTION,		0x0,	0,	0,	0,	{0, 0, 0, 0, 0}, 0},\
};

/* extra SAR table, for different board implementations:
 * in case a field is used on 2 boards with different i2c mapping */
#define MV_SAR_INFO2 { \
{ "coreclock",	MV_SATR_CORE_CLK_SELECT,	0x08,   3,      2,      0,      {0, 0, 0, 0, 1}, 0},\
{ "sscg",	MV_SATR_SSCG_DISABLE,		0x10,	4,	2,	0,	{0, 0, 0, 0, 1}, 0},\
{ "max_option",	MV_SATR_MAX_OPTION,		0x0,	0,	0,	0,	{0, 0, 0, 0, 0}, 0},\
};

#ifdef CONFIG_CMD_BOARDCFG
#define MV_BOARD_CONFIG_MAX_BYTE_COUNT	8
#define MV_BOARD_CONFIG_DEFAULT_VALUE	{0x1921d0a1, 0x4 }

typedef enum _mvConfigTypeID {
	MV_CONFIG_EXAMPLE0,
	MV_CONFIG_TYPE_MAX_OPTION,
	MV_CONFIG_TYPE_CMD_DUMP_ALL,
	MV_CONFIG_TYPE_CMD_SET_DEFAULT
} MV_CONFIG_TYPE_ID;

#define MV_EEPROM_CONFIG_INFO { \
{ MV_CONFIG_EXAMPLE0,	0x7,		0,	0,	{1, 1, 1, 1} }, \
};

#define MV_BOARD_CONFIG_CMD_STR "example0\n\n"
#define MV_BOARD_CONFIG_CMD_MAX_OPTS 5

/*MV_CMD_TYPE_ID,		command name,		Name,			numOfValues,	Possible Values */
#define MV_BOARD_CONFIG_CMD_INFO {\
{MV_CONFIG_EXAMPLE0,	"example0",	"Example #0",	5,					\
	{"Option0", "Option1", "Option2", "Option3", "Option4"} },				\
};

#endif /* CONFIG_CMD_BOARDCFG */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

