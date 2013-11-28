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
#ifndef __INCmvBHboardEnvSpech
#define __INCmvBHboardEnvSpech

#include "mvHighSpeedEnvSpec.h"

/********************************* Definitions ********************************/
/* Armada 38x Family */
#define MV_88F68XX		0x6800
#define MV_6810_DEV_ID		0x6810
#define MV_6820_DEV_ID		0x6820
#define MV_INVALID_DEV_ID	0xffff

#define INTER_REGS_BASE				            0xD0000000

#define COMPHY_H_PIPE3_28LP(lane,reg) 		(0xA0000 + (lane * 0x800) + (reg * 0x4))
#define MV_PEX_IF_REGS_BASE(pexIf)            (((pexIf) == 0) ? 0x80000 : (0x40000 + ((pexIf-1) * 0x4000)))
#define MV_SATA_REGS_OFFSET                     (0xA0000)
#define MV_SATA3_UNIT_OFFSET(u)			((u) ? 0xE0000 : 0xA8000) /* sata register per unit*/
#define MV_SATA3_REGS_OFFSET(port)		(MV_SATA3_UNIT_OFFSET(port/2) + (port & 1) * 0x80)

#define MV_SERDES_MAX_LANES			6
#define MV_SERDES_MAX_LANES_6810		5

#define COMMON_PHYS_SELECTORS_REG               0x183FC
#define COMPHY_SELECT_OFFS(x)			(x * 3)
#define COMPHY_SELECT_MASK(x)			(0x07 << COMPHY_SELECT_OFFS(x))
#define	PCIE0_X4_EN_OFFS			18
#define	PCIE0_X4_EN_MASK			(1 << PCIE0_X4_EN_OFFS)


#define COMMON_PHY_CONFIGURATION1_REG(lane)	(0x18300 + (0x28 * lane))
#define COMMON_PHY_CONFIGURATION2_REG(lane)	(0x18304 + (0x28 * lane))
#define COMMON_PHY_CONFIGURATION3_REG(lane)	(0x18308 + (0x28 * lane))
#define COMMON_PHY_CONFIGURATION4_REG(lane)	(0x1830C + (0x28 * lane))
#define COMMON_PHY_STATUS1_REG(lane)		(0x18318 + (0x28 * lane))

#define	PIN_PLL_READY_TX 			(1 << 3)
#define	PIN_PLL_READY_RX 			(1 << 2)
#define RX_INIT_DONE				(1 << 0)

#define	PHY_PLL_READY				(PIN_PLL_READY_TX | PIN_PLL_READY_RX | RX_INIT_DONE)

#define PEX_CAPABILITIES_REG(pexIf)		((MV_PEX_IF_REGS_BASE(pexIf)) + 0x60)
#define	PEX_CFG_MODE_OFFS			20
#define	PEX_CFG_MODE_MASK			(0x0F << PEX_CFG_MODE_OFFS)
#define	PEX_CFG_MODE_RC_MODE			(4 << PEX_CFG_MODE_OFFS)	/* root complex */
#define	PEX_CFG_MODE_EP_MODE			(4 << PEX_CFG_MODE_OFFS)	/* end point*/

#define PEX_LINK_CAPABILITIES_REG(pexIf)	((MV_PEX_IF_REGS_BASE(pexIf)) + 0x6C)
#define PEX_LINK_CTRL_STATUS_REG(pexIf) 	((MV_PEX_IF_REGS_BASE(pexIf)) + 0x70)
#define COMMON_REF_CLK			BIT6

#define PEX_LINK_CTRL_STATUS2_REG(pexIf) 	((MV_PEX_IF_REGS_BASE(pexIf)) + 0x90)
#define PEX_CTRL_REG(pexIf)			((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A00)
#define PEX_STATUS_REG(pexIf)			((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A04)
#define PEX_COMPLT_TMEOUT_REG(pexIf)		((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A10)
#define PEX_PWR_MNG_EXT_REG(pexIf)		((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A18)
#define PEX_FLOW_CTRL_REG(pexIf)		((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A20)
#define PEX_DYNMC_WIDTH_MNG_REG(pexIf)		((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A30)
#define PEX_ROOT_CMPLX_SSPL_REG(pexif)		((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A0C)
#define PEX_RAM_PARITY_CTRL_REG(pexIf) 		((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A50)
#define PEX_DBG_CTRL_REG(pexIf) 		((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A60)
#define PEX_DBG_STATUS_REG(pexIf)		((MV_PEX_IF_REGS_BASE(pexIf)) + 0x1A64)


#define MAX_LNK_SPEED_5GBS		2
#define MAX_LNK_SPEED_2_5GBS		1
#define MAX_LNK_WDTH_X4			4
#define MAX_LNK_WDTH_X1                 1

#define	SOC_CTRL_REG			0x18204
#define PEX_QUADX4_EN			BIT14
#define PEX3_CLK_EN			BIT7
#define PEX2_CLK_EN			BIT6
#define PEX1_CLK_EN			BIT5
#define PEX0_CLK_EN			BIT4


#define PEX3_EN				BIT3
#define PEX2_EN				BIT2
#define PEX1_EN				BIT1
#define PEX0_EN				BIT0
#define PEXx_EN(p)			(1 << p)



#define DEV_ID_REG				0x18238
#define VENDOR_ID_OFFS				0
#define VENDOR_ID_MASK				0xFFFF
#define DEVICE_ID_OFFS				16
#define DEVICE_ID_MASK				0xFFFF0000

#define GBE_CONFIGURATION_REG			0x18460

/* Sample at Reset */
#define MPP_SAMPLE_AT_RESET			(0x18600)
#define	CORE_CLK_FREQ_OFFS			15
#define	CORE_CLK_FREQ_MASK			(1 << CORE_CLK_FREQ_OFFS)

/* TWSI addresses */
#define BOARD_ID_GET_ADDR                       0x50
#define RD_GET_MODE_ADDR                        0x4C
#define DB_GET_MODE_SLM1363_ADDR                0x25
#define DB_GET_MODE_SLM1364_ADDR                0x24

/* For setting MPP2 and MPP3 to be TWSI mode and MPP 0,1 to UART mode */
#define MPP_CTRL_REG                            0x18000
#define MPP_SET_MASK                            (~(0xFFFF))
#define MPP_SET_DATA                            (0x1111)

/* boards ID numbers */
/* boards ID numbers */
#define BOARD_ID_BASE			0x0

#define RD_NAS_68XX_ID			(BOARD_ID_BASE)
#define DB_68XX_ID			(BOARD_ID_BASE + 1)
#define RD_AP_68XX_ID			(BOARD_ID_BASE + 2)
#define A380_CUSTOMER_ID		(BOARD_ID_BASE + 3)
#define MV_MAX_BOARD_ID			(A380_CUSTOMER_ID + 1)
#define MV_BOARD_ID_MASK		0x7
#define MV_INVALID_BOARD_ID		0xFFFFFFFF

#define MV_BOARD_TCLK_ERROR                     0xFFFFFFFF

/************************************ enums ***********************************/
typedef enum {
	SERDES_UNIT_NA		= 0x0,
	SERDES_UNIT_PEX		= 0x10,
	SERDES_UNIT_SATA	= 0x20,
	SERDES_UNIT_GBE		= 0x30,
	SERDES_UNIT_USB_H	= 0x40,
	SERDES_UNIT_USB		= 0x50,
	SERDES_UNIT_MASK	= 0xF0,
} MV_SERDES_UNIT_INDX;

#define SERDES_PORT_MASK	0x0F

typedef enum {
	PEX0_IF,
	PEX1_IF,
	PEX2_IF,
	PEX3_IF,
	PEXIF_MAX
} MV_PEXIF_INDX;


typedef enum {
	PEX_SPEED_GEN1		= 0x0,
	PEX_SPEED_GEN2
} MV_PEX_SPEED_GEN;

/*********************************** Structs **********************************/

/*********************************** Globals **********************************/
#define SERDES_CFG {	\
/* Lane 0 */ {SERDES_UNIT_NA,		SERDES_UNIT_PEX | PEX0_IF, SERDES_UNIT_SATA | 0,	SERDES_UNIT_GBE  | 0,\
	      SERDES_UNIT_NA,		SERDES_UNIT_NA,		    SERDES_UNIT_NA,		SERDES_UNIT_NA},     \
/* Lane 1 */ {SERDES_UNIT_NA,		SERDES_UNIT_PEX | PEX0_IF, SERDES_UNIT_NA,		SERDES_UNIT_SATA | 0,\
	      SERDES_UNIT_GBE | 0,	SERDES_UNIT_GBE | 1,	    SERDES_UNIT_NA,		SERDES_UNIT_NA},     \
/* Lane 2 */ {SERDES_UNIT_NA,		SERDES_UNIT_PEX | PEX1_IF, SERDES_UNIT_NA,		SERDES_UNIT_SATA | 1,\
	      SERDES_UNIT_GBE | 1,	SERDES_UNIT_NA,		    SERDES_UNIT_NA,		SERDES_UNIT_NA},     \
/* Lane 3 */ {SERDES_UNIT_NA,		SERDES_UNIT_PEX | PEX3_IF, SERDES_UNIT_NA,		SERDES_UNIT_SATA | 3,\
	      SERDES_UNIT_GBE | 2,	SERDES_UNIT_USB_H | 1,	    SERDES_UNIT_USB,		SERDES_UNIT_NA},     \
/* Lane 4 */ {SERDES_UNIT_NA,		SERDES_UNIT_PEX | PEX1_IF, SERDES_UNIT_SATA | 1,	SERDES_UNIT_GBE  | 1,\
	      SERDES_UNIT_USB_H | 0,	SERDES_UNIT_USB,	    SERDES_UNIT_NA,		SERDES_UNIT_NA},     \
/* Lane 5 */ {SERDES_UNIT_NA,		SERDES_UNIT_PEX | PEX2_IF, SERDES_UNIT_SATA | 2,	SERDES_UNIT_GBE  | 2,\
	      SERDES_UNIT_USB_H | 1,	SERDES_UNIT_USB,	    SERDES_UNIT_NA,		SERDES_UNIT_NA},     \
}

/*************************** Functions declarations ***************************/

/**************************************************************************
* mvBoardTclkGet -
*
* DESCRIPTION:          Returns the board id
* INPUT:                None.
* OUTPUT:               None.
* RETURNS:              board id.
***************************************************************************/
MV_U32 mvBoardTclkGet(MV_VOID);

/**************************************************************************
* mvHwsBoardTopologyLoad -
*
* DESCRIPTION:          Loads the board topology
* INPUT:                serdesMapArray  -   The struct that will contain
*                                           the board topology map
* OUTPUT:               The board topology.
* RETURNS:              MV_OK           -   for success
***************************************************************************/
MV_STATUS mvHwsBoardTopologyLoad(SERDES_MAP  *serdesMapArray);

/**************************************************************************
* mvHwsBoardTclkGet -
*
* DESCRIPTION:          Returns the board Tclk
* INPUT:                None.
* OUTPUT:               None.
* RETURNS:              Tclk
***************************************************************************/
MV_U32 mvBoardTclkGet(MV_VOID);

/**************************************************************************
* mvHwsRefClockSet -
*
* DESCRIPTION:          Sets the refClock according to the serdes type
*                       and ref clock given as input.
* INPUT:                serdesNum       - serdes lane number.
*                                         needed for calculating the
*                                         address where the ref clock will
*                                         be set.
*                       serdesType      - PEX, USB3, SATA or SGMII.
*                       refClock        - 25MHz or 100MHz
* OUTPUT:               Sets the refclock in the relevant address.
* RETURNS:              MV_OK           - for success
*                       MV_BAD_PARAMS   - if one or more of the input
*                                         parameters were bad
***************************************************************************/
MV_STATUS mvHwsRefClockSet
(
    MV_U32      serdesNum,
    SERDES_TYPE serdesType,
    REF_CLOCK   refClock
);

/**************************************************************************
* mvUpdateSerdesPhySelectors -
*
* DESCRIPTION:          Sets the bits 0-18 in the COMMON PHYS SELECTORS
*                       register according to the topology map loaded from
*                       the board, using commonPhysSelectorsMap
*
* INPUT:                serdesConfigMap -   the serdes configuration map
*                                           loaded from the board
* OUTPUT:               Sets the bits 0-18 in the COMMON PHYS SELECTORS
*                       register
* RETURNS:              MV_OK           - for success
*                       MV_BAD_PARAM    - for a bad combination of serdes num
*                                         and serdes type
***************************************************************************/
MV_STATUS mvHwsUpdateSerdesPhySelectors(SERDES_MAP  *serdesConfigMap);

#endif
