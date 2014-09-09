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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
	MV_PORT_TYPE_RXAUI,
	MV_PORT_TYPE_XAUI,
	MV_PORT_TYPE_SGMII,
	MV_PORT_TYPE_QSGMII,
	MV_PORT_TYPE_RGMII,
	MV_PORT_TYPE_UNKNOWN = -1,
} MV_PORT_TYPE;

typedef enum {
	MV_NETCOMP_GE_MAC0_2_RXAUI	=	BIT0,
	MV_NETCOMP_GE_MAC0_2_XAUI	=	BIT1,
	MV_NETCOMP_GE_MAC0_2_SGMII_L0	=	BIT2,
	MV_NETCOMP_GE_MAC0_2_SGMII_L1	=	BIT3,
	MV_NETCOMP_GE_MAC0_2_QSGMII		=	BIT4,
	MV_NETCOMP_GE_MAC1_2_SGMII_L1	=	BIT5,
	MV_NETCOMP_GE_MAC1_2_RGMII1		=	BIT6,
	MV_NETCOMP_GE_MAC1_2_SGMII_L2	=	BIT7,
	MV_NETCOMP_GE_MAC1_2_SGMII_L4	=	BIT8,
	MV_NETCOMP_GE_MAC1_2_QSGMII		=	BIT9,
	MV_NETCOMP_GE_MAC2_2_SGMII_L3	=	BIT10,
	MV_NETCOMP_GE_MAC2_2_SGMII_L5	=	BIT11,
	MV_NETCOMP_GE_MAC2_2_QSGMII		=	BIT12,
	MV_NETCOMP_GE_MAC3_2_SGMII_L4	=	BIT13,
	MV_NETCOMP_GE_MAC3_2_SGMII_L6	=	BIT14,
	MV_NETCOMP_GE_MAC3_2_QSGMII		=	BIT15
} MV_NET_COMPLEX_TOPOLOGY;

#define MV_MODULE_INFO { \
{ MV_MODULE_MII,		0x1,	0,	 0x4,	{ 0, 1 } }, \
{ MV_MODULE_SLIC_TDM_DEVICE,	0x0,	0,	 0x1,	{ 0, 1 } }, \
{ MV_MODULE_I2S_DEVICE,		0x1,	0,	 0x3,	{ 0, 1 } }, \
{ MV_MODULE_SPDIF_DEVICE,	0x1,	0,	 0x2,	{ 0, 1 } }, \
{ MV_MODULE_NOR,		0x4,	0,	 0xF,	{ 0, 1 } }, \
{ MV_MODULE_NAND,		0x4,	0,	 0x1,	{ 0, 1 } }, \
{ MV_MODULE_SDIO,		0x4,	0,	 0x2,	{ 0, 1 } }, \
{ MV_MODULE_SGMII,		0x2,	0,	 0xF,	{ 0, 1 } }, \
};

/*						bit    TWSI           Reg	board	*/
/*   name		SATR-ID			Mask  offset  devID   num	active	*/
#define MV_SAR_INFO { \
{ "freq",		MV_SATR_CPU_DDR_L2_FREQ,	0x1F,	0, 1, 0, {1, 1}, BOARD_SATR_SWAP_BIT},\
{ "coreclock",		MV_SATR_CORE_CLK_SELECT,	0x04,	2, 3, 0, {0, 1}, 0},\
{ "cpusnum",		MV_SATR_CPU1_ENABLE,		0x01,	0, 2, 0, {0, 1}, 0},\
{ "sscg",		MV_SATR_SSCG_DISABLE,		0x08,	3, 3, 0, {0, 1}, 0},\
{ "ddr4select",		MV_SATR_DDR4_SELECT,		0x20,	5, 4, 1, {0, 1}, BOARD_SATR_READ_ONLY},\
{ "ddrbuswidth",	MV_SATR_DDR_BUS_WIDTH,		0x08,	3, 0, 0, {0, 1}, 0},\
{ "ddreccenable",	MV_SATR_DDR_ECC_ENABLE,		0x10,	4, 0, 0, {0, 1}, 0},\
{ "ddreccpupselect",	MV_SATR_DDR_ECC_PUP_SEL,	0x20,	5, 0, 0, {0, 1}, 0},\
{ "bootsrc",		MV_SATR_BOOT_DEVICE,		0x3,	0, 3, 0, {0, 1}, BOARD_SATR_SWAP_BIT},\
{ "boarsrc2",		MV_SATR_BOOT2_DEVICE,		0x1E,	1, 2, 0, {0, 1}, BOARD_SATR_SWAP_BIT},\
{ "boardid",		MV_SATR_BOARD_ID,		0x7,	0, 0, 0, {1, 1}, 0},\
{ "ecoversion",		MV_SATR_BOARD_ECO_VERSION,	0xff,	0, 0, 1, {1, 1}, BOARD_SATR_READ_ONLY},\
{ "usb3port0",		MV_SATR_DB_USB3_PORT0,		0x1,	0, 1, 1, {0, 1}, 0},\
{ "usb3port1",		MV_SATR_DB_USB3_PORT1,		0x2,	1, 1, 1, {0, 1}, 0},\
{ "rdserdes4",		MV_SATR_RD_SERDES4_CFG,		0x4,	2, 1, 1, {1, 0}, 0},\
{ "max_option",		MV_SATR_MAX_OPTION,		0x0,	0, 0, 0, {0, 0}, 0},\
};

#ifdef CONFIG_CMD_BOARDCFG
#define MV_BOARD_CONFIG_MAX_BYTE_COUNT	8
#define MV_BOARD_CONFIG_DEFAULT_VALUE	{0x1921d0a1, 0x4 }

typedef enum _mvConfigTypeID {
	MV_CONFIG_BOARDID,
	MV_CONFIG_LANE0,
	MV_CONFIG_LANE1,
	MV_CONFIG_LANE2,
	MV_CONFIG_LANE3,
	MV_CONFIG_LANE4,
	MV_CONFIG_LANE5,
	MV_CONFIG_LANE6,
	MV_CONFIG_NSS_EN,
	MV_CONFIG_DDR_BUSWIDTH,
	MV_CONFIG_DDR_ECC_EN,
	MV_CONFIG_TYPE_MAX_OPTION,
	MV_CONFIG_TYPE_CMD_DUMP_ALL,
	MV_CONFIG_TYPE_CMD_SET_DEFAULT
} MV_CONFIG_TYPE_ID;

/* {{MV_CONFIG_TYPE_ID ConfigID, MV_U32 Mask,  Offset, byteNum,    isActiveForBoard[]}} */
#define MV_EEPROM_CONFIG_INFO { \
{ MV_CONFIG_BOARDID,		0xFF000000,	24,	0,	{1 } }, \
{ MV_CONFIG_LANE0,		0x7,		0,	0,	{1 } }, \
{ MV_CONFIG_LANE1,		0x78,		3,	0,	{1 } }, \
{ MV_CONFIG_LANE2,		0x380,		7,	0,	{1 } }, \
{ MV_CONFIG_LANE3,		0x3C00,		10,	0,	{1 } }, \
{ MV_CONFIG_LANE4,		0x3C000,	14,	0,	{1 } }, \
{ MV_CONFIG_LANE5,		0x3C0000,	18,	0,	{1 } }, \
{ MV_CONFIG_LANE6,		0x7,		0,	4,	{1 } }, \
{ MV_CONFIG_NSS_EN,		0x8,		3,	4,	{1 } }, \
{ MV_CONFIG_DDR_BUSWIDTH,	0x10,		4,	4,	{1 } }, \
{ MV_CONFIG_DDR_ECC_EN,		0x20,		5,	4,	{1 } }, \
};

#define MV_BOARD_CONFIG_CMD_STR "serdes0, serdes1, serdes2, serdes3, serdes4, serdes5, serdes6, nss_en,\n"	\
				"\tddr_buswidth, ddr_ecc\n\n"
#define MV_BOARD_CONFIG_CMD_MAX_OPTS 10

/*MV_CMD_TYPE_ID,		command name,		Name,			numOfValues,	Possible Values */
#define MV_BOARD_CONFIG_CMD_INFO {										\
{MV_CONFIG_BOARDID, "boardid",	"Board ID",			1,	{"DB Board"} },	\
{MV_CONFIG_LANE0,	"serdes0",	"SerDes Lane #0",	5,						\
	{"UnConnected", "PCI-e#0", "SATA3 #0", "SGMII #0", "SGMII(v3) #0"} },					\
{MV_CONFIG_LANE1,	"serdes1",	"SerDes Lane #1",	10,						\
	{"UnConnected", "PCI-e#0", "PCI-e#0-1", "SATA3 #0", "SGMII #0", "SGMII #1", "USB3-Host #0", "QSGMII",	\
	"SGMII(v3) #0", "SGMII(v3) #1"} },									\
{MV_CONFIG_LANE2,	"serdes2",	"SerDes Lane #2",	6,						\
	{"UnConnected", "PCI-e#1", "PCI-e#0-2", "SATA3 #1", "SGMII #1", "SGMII(v3) #1"} },			\
{MV_CONFIG_LANE3,	"serdes3",	"SerDes Lane #3",	9,						\
	{"UnConnected", "PCI-e#3", "PCI-e#0-3", "SATA3 #3", "SGMII #2", "USB3-Host #0", "USB-Device",		\
	"SGMII(v3) #2", "XAUI #3"} },										\
{MV_CONFIG_LANE4,	"serdes4",	"SerDes Lane #4",	10,						\
	{"UnConnected", "PCI-e#1", "UnConnected 1", "SGMII #3", "USB3-Host #0", "USB-Device", "SATA3 #2",	\
	"PCI-e#2", "SGMII(v3) #3", "XAUI #2"} },								\
{MV_CONFIG_LANE5,	"serdes5",	"SerDes Lane #5",	9,						\
	{"UnConnected", "PCI-e#2", "SATA3 #2", "SGMII #2", "USB3-Host #1", "USB-Device", "SGMII(v3) #2",	\
	"Reserved", "XAUI #1" } },										\
{MV_CONFIG_LANE6,	"serdes6",	"SerDes Lane #6",	5,						\
	{"UnConnected", "PCI-e#1", "SGMII(v3) #3", "Reserved", "XAUI #1"} },					\
{MV_CONFIG_NSS_EN,	"nss_en",	"NSS enable",		2,						\
	{"Disable", "Enable"} },										\
{MV_CONFIG_DDR_BUSWIDTH,	"ddr_buswidth",	"Buswidth enable",		2,				\
	{"Disable", "Enable"} },										\
{MV_CONFIG_DDR_ECC_EN,	"ddr_ecc",	"Dram ECC enable",		2,					\
	{"32bit", "16bit"} },											\
};

#endif /* CONFIG_CMD_BOARDCFG */

MV_U32 mvBoardPortTypeGet(MV_U32 ethPortNum);
MV_BOOL mvBoardIsPortInXaui(MV_U32 ethPortNum);
MV_BOOL mvBoardIsPortInRxaui(MV_U32 ethPortNum);
MV_BOOL mvBoardIsPortInQsgmii(MV_U32 ethPortNum);
MV_U32 mvBoardNetComplexConfigGet(MV_VOID);
MV_VOID mvBoardNetComplexConfigSet(MV_U32 ethConfig);
#ifdef __cplusplus
}
#endif  /* __cplusplus */
