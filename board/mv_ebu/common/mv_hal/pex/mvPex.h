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

	*		Redistributions of source code must retain the above copyright notice,
			this list of conditions and the following disclaimer.

	*		Redistributions in binary form must reproduce the above copyright
			notice, this list of conditions and the following disclaimer in the
			documentation and/or other materials provided with the distribution.

	*		Neither the name of Marvell nor the names of its contributors may be
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

#ifndef __INCPEXH
#define __INCPEXH

#ifdef __cplusplus
extern "C" {
#endif

#include "mvCommon.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"

	/* NOTE: not supported in this driver */

	/* defines */
	/* The number of supported PEX interfaces depend on Marvell controller */
	/* device number. This device number ID is located on the PEX unit*/
	/* configuration header. This creates a loop where calling PEX */
	/* configuration read/write     routine results a call to get PEX configuration */
	/* information etc. This macro defines a default PEX interface. This PEX */
	/* interface is sure to exist.*/
#define PEX_DEFAULT_IF	0

	/* typedefs */
	/* The Marvell controller supports both root complex and end point devices */
	/* This enumeration describes the PEX type.                                 */
	typedef enum _mvPexType {
		MV_PEX_ROOT_COMPLEX,	/* root complex device */
		MV_PEX_END_POINT	/* end point device */
	} MV_PEX_TYPE;

	typedef enum _mvPexGen {
		MV_PEX_GEN1_1 = 1,
		MV_PEX_GEN2_0
	} MV_PEX_GEN;

	typedef enum _mvPexWidth {
		MV_PEX_WITDH_X1 = 1,
		MV_PEX_WITDH_X2,
		MV_PEX_WITDH_X3,
		MV_PEX_WITDH_X4,
		MV_PEX_WITDH_INVALID
	} MV_PEX_WIDTH;

	/* PEX Bar attributes */
	typedef struct _mvPexMode {
		MV_PEX_TYPE pexType;
		MV_PEX_WIDTH pexWidth;
		MV_BOOL pexLinkUp;
		MV_PEX_GEN pexGen;
	} MV_PEX_MODE;

	/* PEX Bar attributes */
	typedef struct _mvPexBar {
		MV_ADDR_WIN addrWin;	/* An address window */
		MV_BOOL enable;	/* Address decode window is enabled/disabled    */
	} MV_PEX_BAR;

	/* PEX Remap Window attributes */
	typedef struct _mvPexRemapWin {
		MV_ADDR_WIN addrWin;	/* An address window */
		MV_BOOL enable;	/* Address decode window is enabled/disabled    */

	} MV_PEX_REMAP_WIN;

	/* PEX Window attributes */
	typedef struct _mvPexDecWin {
		MV_TARGET target;	/* Target for addr decode window        */
		MV_UNIT_WIN_INFO winInfo;
		MV_U32 targetBar;
		MV_BOOL slvWrSpltCnt;
	} MV_PEX_DEC_WIN;

	/* PEX Wins registers offsets are inconsecutive. This struct describes WIN      */
	/* register offsets     and its function where its is located.                                  */
	/* Also, PEX address remap registers offsets are inconsecutive. This struct     */
	/* describes address remap register offsets                                                                     */
	typedef struct _pexWinRegInfo {
		MV_U32 baseLowRegOffs;
		MV_U32 baseHighRegOffs;
		MV_U32 sizeRegOffs;
		MV_U32 remapLowRegOffs;
		MV_U32 remapHighRegOffs;

	} PEX_WIN_REG_INFO;

	typedef struct {
		MV_U16 ctrlModel;
		MV_U16 maxPexIf;
		MV_U32 ctrlFamily;
	} MV_PEX_HAL_DATA;

	/* Global Functions prototypes */
	/* mvPexInit - Initialize PEX interfaces */
	MV_STATUS mvPexInit(MV_U32 pexIf, MV_PEX_TYPE pexType, MV_PEX_HAL_DATA *halData);

	/* mvPexModeGet - Get Pex If mode */
	MV_U32 mvPexModeGet(MV_U32 pexIf, MV_PEX_MODE *pexMode);

	/* mvPexConfigRead - Read from configuration space */
	MV_U32 mvPexConfigRead(MV_U32 pexIf, MV_U32 bus, MV_U32 dev, MV_U32 func, MV_U32 regOff);

	/* mvPexConfigWrite - Write to configuration space */
	MV_STATUS mvPexConfigWrite(MV_U32 pexIf, MV_U32 bus, MV_U32 dev, MV_U32 func, MV_U32 regOff, MV_U32 data);

	/* mvPexMasterEnable - Enable/disale PEX interface master transactions. */
	MV_STATUS mvPexMasterEnable(MV_U32 pexIf, MV_BOOL enable);

	/* mvPexSlaveEnable - Enable/disale PEX interface slave transactions. */
	MV_STATUS mvPexSlaveEnable(MV_U32 pexIf, MV_U32 bus, MV_U32 dev, MV_BOOL enable);

	/* mvPexLocalBusNumSet - Set PEX interface local bus number. */
	MV_STATUS mvPexLocalBusNumSet(MV_U32 pexIf, MV_U32 busNum);

	/* mvPexLocalBusNumGet - Get PEX interface local bus number. */
	MV_U32 mvPexLocalBusNumGet(MV_U32 pexIf);

	/* mvPexLocalDevNumSet - Set PEX interface local device number. */
	MV_STATUS mvPexLocalDevNumSet(MV_U32 pexIf, MV_U32 devNum);

	/* mvPexLocalDevNumGet - Get PEX interface local device number. */
	MV_U32 mvPexLocalDevNumGet(MV_U32 pexIf);
	/* mvPexForceX1 - Force PEX interface to X1 mode. */
	MV_U32 mvPexForceX1(MV_U32 pexIf);

	/* mvPexIsPowerUp - Is PEX interface Power up? */
	MV_BOOL mvPexIsPowerUp(MV_U32 pexIf);

	/* mvPexPowerDown - Power Down */
	MV_VOID mvPexPhyPowerDown(MV_U32 pexIf);

	/* mvPexPhyRegRead - Pex phy read */
	MV_VOID mvPexPhyRegRead(MV_U32 pexIf, MV_U32 regOffset, MV_U16 *value);

	/* mvPexPhyRegWrite - Pex phy write */
	MV_VOID mvPexPhyRegWrite(MV_U32 pexIf, MV_U32 regOffset, MV_U16 value);

	MV_STATUS mvPexActiveStateLinkPMEnable(MV_U32 pexIf, MV_BOOL enable);
	/* mvPexIfEnable - Enable Pex interface. */
	MV_VOID mvPexIfEnable(MV_U32 pexIf, MV_PEX_TYPE pexType);

	/* mvPexWinInit - Window init */
	MV_STATUS mvPexWinInit(MV_U32 pexIf, MV_PEX_TYPE pexType, MV_UNIT_WIN_INFO *addrWinMap);
	const MV_8 *pexBarNameGet(MV_U32 bar);

	/* mvPexTargetWinWrite - Window set */
	MV_STATUS mvPexTargetWinWrite(MV_U32 pexIf, MV_U32 winNum, MV_PEX_DEC_WIN *pexWin);

	/* mvPexTargetWinRead - Window get */
	MV_STATUS mvPexTargetWinRead(MV_U32 pexIf, MV_U32 winNum, MV_PEX_DEC_WIN *pexWin);

	MV_STATUS mvPexBarEnable(MV_U32 pexIf, MV_U32 barNum, MV_BOOL enable);
	MV_STATUS mvPexTargetWinEnable(MV_U32 pexIf, MV_U32 winNum, MV_BOOL enable);
	MV_STATUS mvPexBarSet(MV_U32 pexIf, MV_U32 barNum, MV_PEX_BAR *pAddrWin);
	MV_STATUS mvPexBarGet(MV_U32 pexIf, MV_U32 barNum, MV_PEX_BAR *pAddrWin);
	MV_STATUS pexWinRegInfoGet(MV_U32 pexIf, MV_U32 winNum, PEX_WIN_REG_INFO *pWinRegInfo);

#ifdef __cplusplus
}
#endif
#endif				/* #ifndef __INCPEXH */
