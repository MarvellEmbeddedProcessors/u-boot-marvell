/*
 * ***************************************************************************
 * Copyright (C) 2015 Marvell International Ltd.
 * ***************************************************************************
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ***************************************************************************
 */

#ifndef _COMPHY_DATA_H_
#define _COMPHY_DATA_H_

#define PHY_SPEED_1_25G			0
#define PHY_SPEED_1_5G			1
#define PHY_SPEED_2_5G			2
#define PHY_SPEED_3G			3
#define PHY_SPEED_3_125G		4
#define PHY_SPEED_5G			5
#define PHY_SPEED_6G			6
#define PHY_SPEED_6_25G			7
#define PHY_SPEED_10_3125G		8
#define PHY_SPEED_MAX			9
#define PHY_SPEED_INVALID		0xff

#define PHY_TYPE_UNCONNECTED		0
#define PHY_TYPE_PEX0			1
#define PHY_TYPE_PEX1			2
#define PHY_TYPE_PEX2			3
#define PHY_TYPE_PEX3			4
#define PHY_TYPE_SATA0			5
#define PHY_TYPE_SATA1			6
#define PHY_TYPE_SATA2			7
#define PHY_TYPE_SATA3			8
#define PHY_TYPE_SGMII0			9
#define PHY_TYPE_SGMII1			10
#define PHY_TYPE_SGMII2			11
#define PHY_TYPE_SGMII3			12
#define PHY_TYPE_QSGMII			13
#define PHY_TYPE_USB3_HOST0		14
#define PHY_TYPE_USB3_HOST1		15
#define PHY_TYPE_USB3_DEVICE		16
#define PHY_TYPE_XAUI0			17
#define PHY_TYPE_XAUI1			18
#define PHY_TYPE_XAUI2			19
#define PHY_TYPE_XAUI3			20
#define PHY_TYPE_RXAUI0			21
#define PHY_TYPE_RXAUI1			22
#define PHY_TYPE_KR			23
#define PHY_TYPE_MAX			24
#define PHY_TYPE_INVALID		0xff

#define PHY_POLARITY_NO_INVERT		0
#define PHY_POLARITY_TXD_INVERT		1
#define PHY_POLARITY_RXD_INVERT		2
#define PHY_POLARITY_ALL_INVERT		(PHY_POLARITY_TXD_INVERT | PHY_POLARITY_RXD_INVERT)


#define UTMI_PHY_TO_USB_HOST0		0
#define UTMI_PHY_TO_USB_HOST1		1
#define UTMI_PHY_TO_USB_DEVICE0		2
#define UTMI_PHY_INVALID		0xff

#endif /* _COMPHY_DATA_H_ */

