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
#ifndef __INCmvSata3RegsH
#define __INCmvSata3RegsH


#define SATA3_UNIT_REG_BASE(u)				MV_SATA3_REGS_OFFSET(u)
#define SATA3_PORT_REG_BASE(u, nPort)			(SATA3_UNIT_REG_BASE(u) + (nPort%2)*0x80)

#define SATA3_HBA_CAPABILITY_REG(u)			(SATA3_UNIT_REG_BASE(u) + 0x000)
#define SATA3_GLOBAL_HBA_CONTROL_REG(u)			(SATA3_UNIT_REG_BASE(u) + 0x004)
#define SATA3_INTERRUPT_STATUS_REG(u)			(SATA3_UNIT_REG_BASE(u) + 0x008)
#define SATA3_PORTS_IMPLEMENTED_REG(u)			(SATA3_UNIT_REG_BASE(u) + 0x00C)
#define AHCI_VERSION_REG(u)				(SATA3_UNIT_REG_BASE(u) + 0x010)
#define VENDOR_SPECIFIC_0_ADDR_REG(u)			(SATA3_UNIT_REG_BASE(u) + 0x0A0)
#define VENDOR_SPECIFIC_0_DATA_REG(u)			(SATA3_UNIT_REG_BASE(u) + 0x0A4)

#define SATA3_CMD_LIST_BASE_ADDR_LOW(u, nPort)		(SATA3_PORT_REG_BASE(u, nPort) + 0x100)
#define SATA3_CMD_LIST_BASE_ADDR_HIGH(u, nPort)		(SATA3_PORT_REG_BASE(u, nPort) + 0x104)
#define SATA3_RECEIVED_FIS_BASE_ADDR_LOW(u, nPort)	(SATA3_PORT_REG_BASE(u, nPort) + 0x108)
#define SATA3_RECEIVED_FIS_BASE_ADDR_HIGH(u, nPort)	(SATA3_PORT_REG_BASE(u, nPort) + 0x10C)
#define SATA3_PORT_INTERRUPT_STATUS_REG(u, nPort)	(SATA3_PORT_REG_BASE(u, nPort) + 0x110)
#define SATA3_PORT_INTERRUPT_ENABLE_REG(u, nPort)	(SATA3_PORT_REG_BASE(u, nPort) + 0x114)
#define SATA3_PORT_CMD_STATUS_REG(u, nPort)		(SATA3_PORT_REG_BASE(u, nPort) + 0x118)
#define SATA3_PORT_TASK_FILE_DATA_REG(u, nPort)		(SATA3_PORT_REG_BASE(u, nPort) + 0x120)
#define SATA3_PORT_SIGNATURE_REG(u, nPort)		(SATA3_PORT_REG_BASE(u, nPort) + 0x124)
#define SATA3_STATUS_REG(u, nPort)			(SATA3_PORT_REG_BASE(u, nPort) + 0x128)
#define SATA3_CONTROL_REG(u, nPort)			(SATA3_PORT_REG_BASE(u, nPort) + 0x12C)
#define SATA3_ERROR_REG(u, nPort)			(SATA3_PORT_REG_BASE(u, nPort) + 0x130)
#define SATA3_ACTIVE_REG(u, nPort)			(SATA3_PORT_REG_BASE(u, nPort) + 0x134)
#define SATA3_CMD_ISSUE_REG(u, nPort)			(SATA3_PORT_REG_BASE(u, nPort) + 0x138)
#define SATA3_PORT_NOTIFICATION_REG(u, nPort)		(SATA3_PORT_REG_BASE(u, nPort) + 0x13C)
#define SATA3_PORT_FIS_SWITCH_CNTRL_REG(u, nPort)	(SATA3_PORT_REG_BASE(u, nPort) + 0x140)


#define SATA3_PORT_CMD_LBA_TABLE_BA_REG(u, nPort)	(SATA3_PORT_REG_BASE(u, nPort) + 0x170)
#define VENDOR_SPEC_INTERRUPT_CNTRL_REG(u, nPort)	(SATA3_PORT_REG_BASE(u, nPort) + 0x174)
#define SATA3_INDIRECT_PORT_PHY_ADD_REG(u, nPort)	(SATA3_PORT_REG_BASE(u, nPort) + 0x178)
#define SATA3_INDIRECT_PORT_PHY_DATA_REG(u, nPort)	(SATA3_PORT_REG_BASE(u, nPort) + 0x17C)

#define SATA_GLOBAL_HBA_INTERRUPT_ENABLE_BIT	BIT1
#define SATA_GLOBAL_HBA_AHCI_ENABLE_BIT		BIT31

#define SATA_COMMAND_LIST_BASE_ADDR_ALIGN	0x400
#define SATA_RECEIVED_FIS_BASE_ADDR_ALIGN	0x1000
#define SATA_COMMAND_TABLE_DESC_ADDR_ALIGN	0x80
#define SATA_COMMAND_HEADER_ADDR_ALIGN		0x20

#define SATA_RECEIVED_FIS_LENGTH		0x1000
#define SATA_COMMAND_LIST_LENGTH		0x400

#define SATA_COMMAND_TABLE_CFIS_OFFSET		0x00
#define SATA_COMMAND_TABLE_CATAPI_OFFSET	0x60
#define SATA_COMMAND_TABLE_PRD_OFFSET		0x80
#define SATA_COMMAND_HEADER_ADDR_OFFSET		0x20

#define SATA_PORT_TASK_FILE_STATUS_BITS		0xff

#define MV_SATA3_MAX_ADDR_DECODE_WIN		4


#define MV_SATA3_WIN_CTRL_REG(dev, win)        (SATA3_UNIT_REG_BASE(dev) + 0x60 + ((win)<<4))
#define MV_SATA3_WIN_BASE_REG(dev, win)        (SATA3_UNIT_REG_BASE(dev)  + 0x64 + ((win)<<4))
#define MV_SATA3_WIN_SIZE_REG(dev, win)        (SATA3_UNIT_REG_BASE(dev)  + 0x68 + ((win)<<4))

#endif /*  __INCmvSata3RegsH */
