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

    * 	Redistributions of source code must retain the above copyright notice,
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

#include <mvCommon.h>
#include <mvOs.h>
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "ctrlEnv/mvCtrlEthCompLib.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "neta/gbe/mvEthRegs.h"

static MV_BOOL gEthComplexSkipInit = MV_FALSE;

/******************************************************************************
* mvEthCompSkipInitSet
*
* DESCRIPTION:
*	Configure the eth-complex to skip initialization.
*
* INPUT:
*	skip - MV_TRUE to skip initialization.
*
* OUTPUT:
*	None.
*
* RETURN:
*	None.
*******************************************************************************/
void mvEthCompSkipInitSet(MV_BOOL skip)
{
	gEthComplexSkipInit = skip;
	return;
}

/******************************************************************************
* mvEthCompMac2SwitchConfig
*
* DESCRIPTION:
*	Configure ethernet complex for MAC0/1 to switch ports 4/6 mode.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration bitmap.
*	muxCfgOnly - MV_TRUE: Configure only the ethernet complex mux'es and
*		     skip other switch reset configurations.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthCompMac2SwitchConfig(MV_U32 ethCompCfg, MV_BOOL muxCfgOnly)
{
	return MV_OK;
}

/******************************************************************************
* mvEthCompSwitchReset
*
* DESCRIPTION:
*	Reset switch device after being configured by ethernet complex functions.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration bitmap.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthCompSwitchReset(MV_U32 ethCompCfg)
{
	return MV_OK;
}

/******************************************************************************
* mvEthCompMac2RgmiiConfig
*
* DESCRIPTION:
*	Configure ethernet complex for MAC0/1 to RGMII output.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration bitmap.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthCompMac2RgmiiConfig(MV_U32 ethCompCfg)
{
	return MV_OK;
}

/******************************************************************************
* mvEthCompSwP56ToRgmiiConfig
*
* DESCRIPTION:
*	Configure ethernet complex for Switch port 5 or 6 to RGMII output.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration bitmap.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthCompSwP56ToRgmiiConfig(MV_U32 ethCompCfg)
{
	return MV_OK;
}


/******************************************************************************
* mvEthCompSataConfig
*
* DESCRIPTION:
*	Configure ethernet complex for sata port output.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration bitmap.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthCompSataConfig(MV_U32 ethCompCfg)
{
	return MV_OK;
}

/******************************************************************************
* mvEthernetComplexShutdownIf
*
* DESCRIPTION:
*	Shutdown ethernet complex interfaces.
*
* INPUT:
*	integSwitch	- MV_TRUE to shutdown the integrated switch.
*	gePhy		- MV_TRUE to shutdown the GE-PHY
*	fePhy		- MV_TRUE to shutdown the 3xFE PHY.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthernetComplexShutdownIf(MV_BOOL integSwitch, MV_BOOL gePhy, MV_BOOL fePhy)
{
	return MV_OK;
}

/******************************************************************************
* mvEthernetComplexPreInit
*
* DESCRIPTION:
*	Perform basic setup that is needed before configuring the eth-complex
*	registers.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthernetComplexPreInit(MV_U32 ethCompCfg)
{
	return MV_OK;
}


/******************************************************************************
* mvEthernetComplexPostInit
*
* DESCRIPTION:
*	Perform basic setup that is needed after configuring the eth-complex
*	registers.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
 MV_STATUS mvEthernetComplexPostInit(MV_U32 ethCompCfg)
{
	return MV_OK;
}


/******************************************************************************
* mvEthernetComplexInit
*
* DESCRIPTION:
*	Initialize the ethernet complex according to the boardEnv setup.
*
* INPUT:
*	None.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthernetComplexInit(void)
{
	MV_U32 ethCompCfg = mvBoardEthComplexConfigGet();

	if (gEthComplexSkipInit == MV_TRUE)
		return MV_OK;

	mvEthernetComplexPreInit(ethCompCfg);

		/*  Reset the switch after all configurations are done. */
		mvEthCompSwitchReset(ethCompCfg);

	mvEthernetComplexPostInit(ethCompCfg);

	return MV_OK;
}


/******************************************************************************
* mvEthernetComplexChangeMode
*
* DESCRIPTION:
*	Change the ethernet complex configuration at runtime.
*	Meanwhile the function supports only the following mode changes:
*		- Moving the switch between MAC0 & MAC1.
*		- Connect / Disconnect GE-PHY to MAC1.
*		- Connect / Disconnect RGMII-B to MAC0.
*
* INPUT:
*	oldCfg	- The old ethernet complex configuration.
*	newCfg	- The new ethernet complex configuration to switch to.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthernetComplexChangeMode(MV_U32 oldCfg, MV_U32 newCfg)
{
	return MV_OK;
}
