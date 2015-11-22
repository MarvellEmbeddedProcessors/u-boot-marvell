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

#ifndef __INCmvBoardEnvLib39h
#define __INCmvBoardEnvLib39h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum _boardMacSpeed {
	BOARD_MAC_SPEED_10M,
	BOARD_MAC_SPEED_100M,
	BOARD_MAC_SPEED_1000M,
	BOARD_MAC_SPEED_2000M,
	BOARD_MAC_SPEED_AUTO,
	BOARD_MAC_UNCONNECTED
} MV_BOARD_MAC_SPEED;

typedef enum _eth_negotiation_port_type {
	SMI,
	XSMI
} MV_PHY_NEGOTIATION_PORT_TYPE;

typedef enum _mvGpConfig {
	MV_GP_CONFIG_EAP_10G,
	MV_GP_CONFIG_EAP_1G,
	MV_GP_CONFIG_HGW_AP_2_5G,
	MV_GP_CONFIG_HGW_AP_2_5G_SATA
} MV_GP_CONFIG;

typedef struct _boardMacInfo {
	MV_BOARD_MAC_SPEED boardMacSpeed;
	MV_32 boardEthSmiAddr;
	MV_32 boardEthSmiAddr0;
	MV_PHY_NEGOTIATION_PORT_TYPE negType;
	MV_BOOL boardMacEnabled;
} MV_BOARD_MAC_INFO;

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
	MV_NETCOMP_GE_MAC0_2_QSGMII	=	BIT4,
	MV_NETCOMP_GE_MAC1_2_SGMII_L1	=	BIT5,
	MV_NETCOMP_GE_MAC1_2_RGMII1	=	BIT6,
	MV_NETCOMP_GE_MAC1_2_SGMII_L2	=	BIT7,
	MV_NETCOMP_GE_MAC1_2_SGMII_L4	=	BIT8,
	MV_NETCOMP_GE_MAC1_2_QSGMII	=	BIT9,
	MV_NETCOMP_GE_MAC2_2_SGMII_L3	=	BIT10,
	MV_NETCOMP_GE_MAC2_2_SGMII_L5	=	BIT11,
	MV_NETCOMP_GE_MAC2_2_QSGMII	=	BIT12,
	MV_NETCOMP_GE_MAC3_2_SGMII_L4	=	BIT13,
	MV_NETCOMP_GE_MAC3_2_SGMII_L6	=	BIT14,
	MV_NETCOMP_GE_MAC3_2_QSGMII	=	BIT15,
	MV_NETCOMP_GE_MAC0_2_SGMII_L6	=	BIT16
} MV_NET_COMPLEX_TOPOLOGY;

typedef enum _mvModuleTypeID {
	MV_MODULE_NO_MODULE                     = 0x000,/* MII board SLM 1362   */
	MV_MODULE_MII                           = BIT0, /* MII board SLM 1362   */
	MV_MODULE_SLIC_TDM_DEVICE               = BIT1, /* TDM board SLM 1360   */
	MV_MODULE_I2S_DEVICE                    = BIT2, /* I2S board SLM 1360   */
	MV_MODULE_SPDIF_DEVICE                  = BIT3, /* SPDIF board SLM 1360 */
	MV_MODULE_NOR                           = BIT4, /* NOR board SLM 1361   */
	MV_MODULE_NAND                          = BIT5, /* NAND board SLM 1361  */
	MV_MODULE_SDIO                          = BIT6, /* SDIO board SLM 1361  */
	MV_MODULE_SGMII                         = BIT7, /* SGMII board SLM 1364 */
	MV_MODULE_SWITCH                        = BIT8, /* SWITCH board SLM 1375 */
	MV_MODULE_NAND_ON_BOARD                 = BIT9,/* ON board nand - detected via S@R bootsrc */
	MV_MODULE_TYPE_MAX_MODULE               = 0,	/* Max detected module - No module supported yet*/
	MV_MODULE_TYPE_MAX_OPTION               = 9	/* Max module options */
} MV_MODULE_TYPE_ID;


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

/* MPP_SPI0_BOOT: change MPPs of: SPI0 (booting from it), RGMII, SDIO
		This MPPs changes disbale NAND/SPI1 */
#define MPP_SPI0_BOOT		{ {2, 0x11266005}, {3, 0x22222011}, {4, 0x22200002}, \
				  {5, 0x51132022}, {6, 0x55550555}, {7, 0x00005550} }


/*enabled: SDIO
  disabled: TDM, SPI0
	SDIO MPP's conflicts with TDM MPP's,
	so TDM must be disabled; SPI0 is not in used without TDM, so disable also SPI MPP's*/
#define MPP_SDIO		{ {1, 0x00660077}, {2, 0x55066000}, {6, 0x55000500}, \
				  {7, 0x00005550} }

/*enabled: SPI0, SDIO
  disabled: TDM, NAND
	SDIO MPP's conflicts with TDM MPP's, so TDM must be disabled; SPI0 used instead of NAND */
#define MPP_SDIO_SPI0		{ {1, 0x00660077}, {2, 0x11066000}, {3, 0x00000011}, \
				  {4, 0x00000000}, {5, 0x11100000}, {6, 0x55000500}, \
				  {7, 0x00005550} }

typedef enum _mvSatRTypeID {
/*  "Bios" Device  */
	MV_SATR_CPU_DDR_L2_FREQ,
	MV_SATR_CORE_CLK_SELECT,
	MV_SATR_BOOT_DEVICE,
	MV_SATR_BOOT2_DEVICE,
	MV_SATR_DEVICE_ID,
	MV_SATR_MAX_OPTION,
} MV_SATR_TYPE_ID;

/*					bit	TWSI	Reg	board	*/
/*   name		SATR-ID		Mask	offset	devID	num	active	*/
#define MV_SAR_INFO { \
{"freq",	MV_SATR_CPU_DDR_L2_FREQ, 0x1F,	0,	1,	0,	{1, 0}, 0},\
{"coreclock",	MV_SATR_CORE_CLK_SELECT, 0x02,	1,	3,	0,	{1, 0}, 0},\
{"bootsrc",	MV_SATR_BOOT_DEVICE,	 0x1F,	0,	2,	0,	{1, 0}, 0},\
{"bootsrc2",	MV_SATR_BOOT2_DEVICE,	 0x01,	0,	3,	0,	{1, 0}, 0},\
{"devid",	MV_SATR_DEVICE_ID,	 0x1C,	2,	3,	0,	{1, 0}, 0},\
{"max_option",	MV_SATR_MAX_OPTION,	 0x0,	0,	0,	0,	{0, 0}, 0},\
};

/* extra SAR table, for different board implementations:
 * in case a field is used on 2 boards with different i2c mapping */
#define MV_SAR_INFO2 { \
{"freq",	MV_SATR_CPU_DDR_L2_FREQ,	0x1F, 0, 1, 0, {0, 1}, SATR_SWAP_BIT},\
{"max_option",	MV_SATR_MAX_OPTION,		0x0,  0, 0, 0, {0, 0}, 0},\
};

#define MV_SATR_BOOT2_VALUE_MASK		0x1
#define MV_SATR_BOOT2_VALUE_OFFSET		5

#ifdef CONFIG_CMD_BOARDCFG
#define MV_BOARD_CONFIG_MAX_BYTE_COUNT	8

#define MV_BOARD_CONFIG_DEFAULT_VALUE	\
			{0x00,	/* boardId => 0 */				\
			 0x51,	/* SerDes0 => PCIe0, SerDes1 => SGMII1 */	\
			 0x41,	/* SerDes2 => PCIe1, SerDes3 => SGMII2 */	\
			 0x87,	/* SerDes4 => PCIe2, SerDes5 => RXAUI */	\
			 0x04,	/* SerDes6 => RXUAI */				\
			 0x38,	/* NSSEnable => Disable, DDR BUSWIDTH => 32bit, Dram ECC => Disabled */	\
				/* EEPROM => Enable, Lane#5-SMIMODE => XSMI, Lane#6-SMIMODE => XSMI */\
			 0x00,	/* BoardConfig valid => valid*/			\
			 0x00}

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
	MV_CONFIG_BOARDCFG_EN,
	MV_CONFIG_5_SMI_MODE,
	MV_CONFIG_6_SMI_MODE,
	MV_CONFIG_GP_CONFIG,
	MV_CONFIG_TYPE_MAX_OPTION, /* limit for user read/write routines */
	MV_CONFIG_BOARDCFG_VALID,
	MV_CONFIG_TYPE_CMD_DUMP_ALL, /* limit for mvBoardConfigTypeGet routine */
	MV_CONFIG_TYPE_CMD_SET_DEFAULT,
} MV_CONFIG_TYPE_ID;

/* {{MV_CONFIG_TYPE_ID ConfigID, MV_U32 Mask,  Offset, byteNum,    isActiveForBoard[]}} */
#define MV_EEPROM_CONFIG_INFO { \
{ MV_CONFIG_BOARDID,		0xFF,	0,	0,	{0, 0, 0} }, \
{ MV_CONFIG_LANE0,		0x0F,	0,	1,	{1, 1, 0} }, \
{ MV_CONFIG_LANE1,		0xF0,	4,	1,	{1, 1, 0} }, \
{ MV_CONFIG_LANE2,		0x0F,	0,	2,	{1, 1, 0} }, \
{ MV_CONFIG_LANE3,		0xF0,	4,	2,	{1, 1, 0} }, \
{ MV_CONFIG_LANE4,		0x0F,	0,	3,	{1, 1, 0} }, \
{ MV_CONFIG_LANE5,		0xF0,	4,	3,	{1, 1, 0} }, \
{ MV_CONFIG_LANE6,		0x0F,	0,	4,	{1, 1, 0} }, \
{ MV_CONFIG_NSS_EN,		0x01,	0,	5,	{1, 1, 0} }, \
{ MV_CONFIG_DDR_BUSWIDTH,	0x02,	1,	5,	{1, 1, 1} }, \
{ MV_CONFIG_DDR_ECC_EN,		0x04,	2,	5,	{1, 1, 1} }, \
{ MV_CONFIG_BOARDCFG_EN,	0x08,	3,	5,	{1, 1, 1} }, \
{ MV_CONFIG_BOARDCFG_VALID,	0x0F,	0,	6,	{1, 1, 1} }, \
{ MV_CONFIG_5_SMI_MODE,		0x10,	4,      5,      {1, 1, 0} }, \
{ MV_CONFIG_6_SMI_MODE,		0x20,	5,      5,      {1, 1, 0} }, \
{ MV_CONFIG_GP_CONFIG,		0xC0,	6,      5,      {1, 1, 0} }, \
};


#define MV_BOARD_CONFIG_CMD_STR "serdes0, serdes1, serdes2, serdes3, serdes4, serdes5, serdes6, nss_en,\n"	\
				"\tddr_buswidth, ddr_ecc, eepromEnable, serdes5Mode, serdes6Mode, gpConfig\n\n"
#define MV_BOARD_CONFIG_CMD_MAX_OPTS 15

/*MV_CMD_TYPE_ID,		command name,		Name,			numOfValues,	Possible Values */
#define MV_BOARD_CONFIG_CMD_INFO {										\
{MV_CONFIG_BOARDID, "boardid",	"Board ID",			1,	{"DB Board"} },				\
{MV_CONFIG_LANE0,	"serdes0",	"SerDes Lane #0",	5,						\
	{"UnConnected", "PCI-e#0", "SATA3 #0", "SGMII #0", "SGMII(v3) #0"} },					\
{MV_CONFIG_LANE1,	"serdes1",	"SerDes Lane #1",	10,						\
	{"UnConnected", "PCI-e#0", "PCI-e#0-1", "SATA3 #0", "SGMII #0", "SGMII #1", "USB3-Host #0", "QSGMII",	\
	"SGMII(v3) #0", "SGMII(v3) #1"} },									\
{MV_CONFIG_LANE2,	"serdes2",	"SerDes Lane #2",	6,						\
	{"UnConnected", "PCI-e#1", "PCI-e#0-2", "SATA3 #1", "SGMII #1", "SGMII(v3) #1"} },			\
{MV_CONFIG_LANE3,	"serdes3",	"SerDes Lane #3",	9,						\
	{"UnConnected", "PCI-e#3", "PCI-e#0-3", "SATA3 #3", "SGMII #2", "USB3-Host #1", "USB-Device",		\
	"SGMII(v3) #2", "XAUI #3"} },										\
{MV_CONFIG_LANE4,	"serdes4",	"SerDes Lane #4",	10,						\
	{"UnConnected", "PCI-e#1", "UnConnected 1", "SGMII #1", "USB3-Host #0", "USB-Device", "SATA3 #2",	\
	"PCI-e#2", "SGMII(v3) #3", "XAUI #2"} },								\
{MV_CONFIG_LANE5,	"serdes5",	"SerDes Lane #5",	9,						\
	{"UnConnected", "PCI-e#2", "SATA3 #2", "SGMII #2", "USB3-Host #1", "USB-Device", "SGMII(v3) #2",	\
	"Reserved", "XAUI #1" } },										\
{MV_CONFIG_LANE6,	"serdes6",	"SerDes Lane #6",	13,						\
	{"UnConnected", "PCI-e#1", "SGMII(v3) #3", "Reserved", "XAUI #1", "Reserved", "Reserved", "Reserved"	\
	, "Reserved", "Reserved", "Reserved", "Reserved", "SGMII(v3) #0"} },					\
{MV_CONFIG_NSS_EN,	"nss_en",	"NSS enable",		2,						\
	{"Disable", "Enable"} },										\
{MV_CONFIG_DDR_BUSWIDTH,	"ddr_buswidth",	"Buswidth enable",		2,				\
	{"32bit", "16bit"} },											\
{MV_CONFIG_DDR_ECC_EN,	"ddr_ecc",	"Dram ECC enable",		2,					\
	{"Disable", "Enable"} },										\
{MV_CONFIG_BOARDCFG_EN,	"eepromEnable",	"EEPROM enable",	2,						\
	{"Disable", "Enable"} },										\
{MV_CONFIG_5_SMI_MODE, "serdes5Mode", "SerDes Lane #5 SMI MODE", 2, {"SMI", "XSMI"} },				\
{MV_CONFIG_6_SMI_MODE, "serdes6Mode", "SerDes Lane #6 SMI MODE", 2, {"SMI", "XSMI"} },				\
{MV_CONFIG_GP_CONFIG, "gpConfig", "GP configurations", 4, {"10G EAP (On board switch)",				\
	"1G EAP (On board switch)", "HGW 2.5G (On board switch, 3x PCIe)",		\
	"HGW 2.5G (On board switch, 2x PCIe, 1 mSATA)"} },							\
};

#endif /* CONFIG_CMD_BOARDCFG */

MV_BOOL mvBoardIsPortInXaui(MV_U32 ethPortNum);
MV_BOOL mvBoardIsPortInRxaui(MV_U32 ethPortNum);
MV_BOOL mvBoardIsPortInQsgmii(MV_U32 ethPortNum);
MV_U32 mvBoardNetComplexConfigGet(MV_VOID);
MV_VOID mvBoardNetComplexConfigSet(MV_U32 ethConfig);
MV_PHY_NEGOTIATION_PORT_TYPE mvBoardPhyNegotiationTypeGet(MV_U32 ethPortNum);
MV_VOID mvBoardPhyNegotiationTypeSet(MV_U32 ethPortNum, MV_PHY_NEGOTIATION_PORT_TYPE negType);
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __INCmvBoardEnvLib39h */
