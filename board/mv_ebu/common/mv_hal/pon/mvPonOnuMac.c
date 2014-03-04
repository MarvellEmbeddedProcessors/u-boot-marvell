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

/* Include Files
   ------------------------------------------------------------------------------*/
#include "mvTypes.h"
#include "mvOs.h"

#include "mvPonOnuDefines.h"
#include "mvPonOnuRegs.h"
#include "mvPonOnuMac.h"

/******************************************************************************/
/******************************************************************************/
/* ========================================================================== */
/* ========================================================================== */
/* ==                                                                      == */
/* ==           =========   =========   =========   ===       ==           == */
/* ==           =========   =========   =========   ====      ==           == */
/* ==           ==          ==     ==   ==     ==   == ==     ==           == */
/* ==           ==          ==     ==   ==     ==   ==  ==    ==           == */
/* ==           =========   =========   ==     ==   ==   ==   ==           == */
/* ==           =========   =========   ==     ==   ==    ==  ==           == */
/* ==           ==     ==   ==          ==     ==   ==     == ==           == */
/* ==           ==     ==   ==          ==     ==   ==      ====           == */
/* ==           =========   ==          =========   ==       ===           == */
/* ==           =========   ==          =========   ==        ==           == */
/* ==                                                                      == */
/* ========================================================================== */
/* ========================================================================== */
/******************************************************************************/
/******************************************************************************/

/* Local Constant
   ------------------------------------------------------------------------------*/
#define __FILE_DESC__ "mv_hal/pon/mvPonOnuMac.c"

/* Global Variables
   ------------------------------------------------------------------------------*/

/* Local Variables
   ------------------------------------------------------------------------------*/
MV_U32 onuGponPortAesStatusImage[GPON_ONU_MAX_GEM_PORTS / 32];
MV_BOOL onuGponPortValidStatusImage[GPON_ONU_MAX_GEM_PORTS / 32];
S_MacTxPloamPm mvOnuGponMacPloamCnt;
S_MacFifoCtrl fifoCtrl;
MV_U32 fifoReadIdx  = 0;
MV_U32 fifoWriteIdx = 0;

MV_32 fifoAuditEnter   = 0;
MV_32 fifoAuditAction  = 0;
MV_32 fifoMacMsgSend   = 0;
MV_32 fifoMacMsgAdd    = 0;
MV_32 fifoMacMsgFree   = 0;
MV_32 fifoMacPloamSend = 0;
MV_32 fifoSupported    = 1;

/* Export Functions
   ------------------------------------------------------------------------------*/

/* Local Functions
   ------------------------------------------------------------------------------*/
static inline MV_STATUS mvOnuGponMacFifoMsgAlloc(MV_U32 **buffer, MV_U32 *bufferIndex);
static inline void      mvOnuGponMacFifoMsgSend(MV_U32 bufferIndex);
static inline MV_STATUS mvOnuGponMacFifoMsgRece(MV_U32 **buffer, MV_U32 *bufferIndex);
static inline void      mvOnuGponMacFifoMsgFree(MV_U32 bufferIndex);
static inline int       mvOnuGponMacFifoEmpty(void);

/******************************************************************************/
/* ========================================================================== */
/*                           RX Low Level Routines                            */
/* ========================================================================== */
/******************************************************************************/

/*******************************************************************************
**
**  mvOnuGponMacVersionGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the asic version
**
**  PARAMETERS:  None
**
**  OUTPUTS:     None
**
**  RETURNS:     asic version
**
*******************************************************************************/
MV_STATUS mvOnuGponMacVersionGet(MV_U32 *version)
{
	MV_STATUS rcode;

	rcode = asicOntGlbRegRead(mvAsicReg_GPON_GEN_MAC_VERSION, version, 0);

	return rcode;
}

/*******************************************************************************
**
**  mvOnuGponMacRxConfigSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set Rx config enable register
**
**  PARAMETERS:  MV_BOOL enable
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxConfigSet(MV_BOOL enable)
{
	MV_STATUS status;
	MV_U32 enBit;

	enBit = (enable == MV_TRUE) ? 1 : 0;
	status = asicOntGlbRegWrite(mvAsicReg_GPON_RX_CONFIG_EN, enBit, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacRxConfigBitOrderSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set Rx config bit order register
**
**  PARAMETERS:  MV_BOOL enable
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxConfigBitOrderSet(MV_U32 value)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_GPON_RX_CONFIG_BIT_ORDER, value, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacRxPsaConfigSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set Rx PSA config register
**
**  PARAMETERS:  MV_U32 syncFsmM1
**               MV_U32 syncFsmM2
**               MV_U32 syncFsmM3
**               MV_U32 fecHyst
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxPsaConfigSet(MV_U32 syncFsmM1,
				     MV_U32 syncFsmM2,
				     MV_U32 syncFsmM3,
				     MV_U32 fecHyst)
{
	MV_STATUS status;

	status  = asicOntGlbRegWrite(mvAsicReg_GPON_RX_PSA_CONFIG_SFM1, syncFsmM1, 0);
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_RX_PSA_CONFIG_SFM2, syncFsmM2, 0);
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_RX_PSA_CONFIG_SFM3, syncFsmM3, 0);
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_RX_PSA_CONFIG_FHM1, fecHyst, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacRxFecHysteresisSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set Rx PSA FEC Hysteresis config register
**
**  PARAMETERS:  MV_U32 fecHyst
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxFecHysteresisSet(MV_U32 fecHyst)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_GPON_RX_PSA_CONFIG_FHM1, fecHyst, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacRxFecConfigSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set Rx FEC config register
**
**  PARAMETERS:  MV_BOOL swIndication
**               MV_BOOL forceSw
**               MV_BOOL ignoreParity
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxFecConfigSet(MV_BOOL swIndication,
				     MV_BOOL forceSw,
				     MV_BOOL ignoreParity)
{
	MV_STATUS status;
	MV_U32 swiBit;
	MV_U32 fswBit;
	MV_U32 ipaBit;

	swiBit  = (swIndication == MV_TRUE) ? 1 : 0;
	status  = asicOntGlbRegWrite(mvAsicReg_GPON_RX_FEC_CONFIG_SWVAL, swiBit, 0);
	fswBit  = (forceSw == MV_TRUE) ? 1 : 0;
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_RX_FEC_CONFIG_FSW, fswBit, 0);
	ipaBit  = (ignoreParity == MV_TRUE) ? 1 : 0;
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_RX_FEC_CONFIG_IGNP, ipaBit, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacRxFecStatusGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function get Rx FEC Ind register
**
**  PARAMETERS:  MV_U32 *fecStatus
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxFecStatusGet(MV_U32 *fecStatus)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_RX_FEC_CONFIG_IND, fecStatus, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacRxPloamDataGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return Rx Ploam data
**
**  PARAMETERS:  MV_U32 *ploamData
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxPloamDataGet(MV_U32 *ploamData)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_RX_PLOAMD_DATA_READ, ploamData, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacRxPloamDataUsedGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the Rx ploam fifo size
**
**  PARAMETERS:  MV_U32 *ploamFifoSize
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxPloamDataUsedGet(MV_U32 *ploamFifoSize)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_RX_PLOAMD_DATA_USED, ploamFifoSize, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacRxPlenConfigSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set Rx Plen config register
**
**  PARAMETERS:  MV_BOOL forceNum
**               MV_BOOL useForce
**               MV_BOOL ignDiff
**               MV_BOOL notFix
**               MV_BOOL ignCrc
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxPlenConfigSet(MV_BOOL forceNum,
				      MV_BOOL useForce,
				      MV_BOOL ignDiff,
				      MV_BOOL notFix,
				      MV_BOOL ignCrc)
{
	MV_STATUS status;
	MV_U32 fnBit;
	MV_U32 ufBit;
	MV_U32 idBit;
	MV_U32 nfBit;
	MV_U32 icBit;

	fnBit   = (forceNum == MV_TRUE) ? 1 : 0;
	status  = asicOntGlbRegWrite(mvAsicReg_GPON_RX_PLEND_CONFIG_FN, fnBit, 0);
	ufBit   = (useForce == MV_TRUE) ? 1 : 0;
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_RX_PLEND_CONFIG_UF, ufBit, 0);
	idBit   = (ignDiff == MV_TRUE) ? 1 : 0;
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_RX_PLEND_CONFIG_IGND, idBit, 0);
	nfBit   = (notFix == MV_TRUE) ? 1 : 0;
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_RX_PLEND_CONFIG_DFIX, nfBit, 0);
	icBit   = (ignCrc == MV_TRUE) ? 1 : 0;
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_RX_PLEND_CONFIG_IGNC, icBit, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacRxPlenConfigSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set Rx Ploam config register
**
**  PARAMETERS:  MV_BOOL ignCrc
**               MV_BOOL ignBrd
**               MV_BOOL fwdAll
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxPloamConfigSet(MV_BOOL ignCrc,
				       MV_BOOL ignBrd,
				       MV_BOOL fwdAll)
{
	MV_STATUS status;
	MV_U32 icBit;
	MV_U32 igBit;
	MV_U32 faBit;

	icBit   = (ignCrc == MV_TRUE) ? 1 : 0;
	status  = asicOntGlbRegWrite(mvAsicReg_GPON_RX_PLOAMD_CONFIG_IGNC, icBit, 0);
	igBit   = (ignBrd == MV_TRUE) ? 1 : 0;
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_RX_PLOAMD_CONFIG_IGNB, igBit, 0);
	faBit   = (fwdAll == MV_TRUE) ? 1 : 0;
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_RX_PLOAMD_CONFIG_FALL, faBit, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacRxBwMapConfigSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set Rx BW Map config register
**
**  PARAMETERS:  MV_U32  minSaDelta
**               MV_BOOL notCrcFix
**               MV_BOOL ignCrc
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxBwMapConfigSet(MV_U32 minSaDelta,
				       MV_BOOL notCrcFix,
				       MV_BOOL ignCrc)
{
	MV_STATUS status;
	MV_U32 ncfBit;
	MV_U32 icBit;

	status  = asicOntGlbRegWrite(mvAsicReg_GPON_RX_BWMAP_CONFIG_MSD, minSaDelta, 0);
	ncfBit  = (notCrcFix == MV_TRUE) ? 1 : 0;
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_RX_BWMAP_CONFIG_DFIX, ncfBit, 0);
	icBit   = (ignCrc == MV_TRUE) ? 1 : 0;
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_RX_BWMAP_CONFIG_IGNC, icBit, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacRxBwMapSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the onu Tx bw map table
**
**  PARAMETERS:  MV_U32  entry
**               MV_U32  allocId
**               MV_U32  tcontCnt
**               MV_BOOL enable
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxBwMapSet(MV_U32 entry,
				 MV_U32 allocId,
				 MV_U32 tcontCnt,
				 MV_BOOL enable)
{
	MV_STATUS status;
	MV_U32 enableVal;

	enableVal = (enable == MV_TRUE) ? 1 : 0;

	status  = asicOntGlbRegWrite(mvAsicReg_GPON_RX_BW_MAP_TCN, tcontCnt, entry);
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_RX_BW_MAP_EN, enableVal, entry);
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_RX_BW_MAP_ALID, allocId, entry);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacRxBwMapGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the onu Tx bw translation table
**
**  PARAMETERS:  MV_U32 entry,
**               MV_U32 *allocId,
**               MV_U32 *enable,
**               MV_U32 *tcontCnt
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxBwMapGet(MV_U32 entry,
				 MV_U32 *allocId,
				 MV_BOOL *enable,
				 MV_U32 *tcontCnt)
{
	MV_STATUS status;
	MV_U32 enableVal;

	status  = asicOntGlbRegRead(mvAsicReg_GPON_RX_BW_MAP_TCN, tcontCnt, entry);
	status |= asicOntGlbRegRead(mvAsicReg_GPON_RX_BW_MAP_EN, &enableVal, entry);
	*enable = (enableVal == 0) ? MV_FALSE : MV_TRUE;
	status |= asicOntGlbRegRead(mvAsicReg_GPON_RX_BW_MAP_ALID, allocId, entry);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacRxEqualizationDelaySet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the onu equalization delay
**
**  PARAMETERS:  MV_U32 delay
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxEqualizationDelaySet(MV_U32 delay)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_GPON_RX_EQULIZATION_DELAY, delay, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacRxEqualizationDelayGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the onu equalization delay
**
**  PARAMETERS:  MV_U32 *delay
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxEqualizationDelayGet(MV_U32 *delay)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_RX_EQULIZATION_DELAY, delay, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacRxInternalDelaySet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the onu const delay
**
**  PARAMETERS:  MV_U32 txConstDelay
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxInternalDelaySet(MV_U32 delay)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_GPON_RX_INTERNAL_DELAY, delay, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacRxInternalDelayGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the onu const delay
**
**  PARAMETERS:  MV_U32 *txConstDelay
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxInternalDelayGet(MV_U32 *delay)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_RX_INTERNAL_DELAY, delay, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacRxSuperFrameCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function reads Super Frame Counter
**
**  PARAMETERS:  MV_U32 *counter
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxSuperFrameCounterGet(MV_U32 *counter)
{
	return asicOntGlbRegRead(mvAsicReg_GPON_RX_SFRAME_COUNTER, counter, 0);
}

/*******************************************************************************
**
**  mvOnuGponMacRxSuperFrameCounterSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function sets Super Frame Counter
**
**  PARAMETERS:  MV_U32 counter
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxSuperFrameCounterSet(MV_U32 counter)
{
	return asicOntGlbRegWrite(mvAsicReg_GPON_RX_SFRAME_COUNTER, counter, 0);
}

/******************************************************************************/
/* ========================================================================== */
/*                           TX Low Level Routines                            */
/* ========================================================================== */
/******************************************************************************/

/*******************************************************************************
**
**  mvOnuGponMacTxDelimiterSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the Tx delimiter
**
**  PARAMETERS:  MV_U32 delimiterPattern
**               MV_U32 delimiterSize
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxDelimiterSet(MV_U32 delimiterPattern, MV_U32 delimiterSize)
{
	MV_STATUS status;

	status  = asicOntGlbRegWrite(mvAsicReg_GPON_TX_DELIMITER_PAT, delimiterPattern, 0);
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_TX_DELIMITER_SIZE, delimiterSize, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacTxDelimiterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the Tx delimiter
**
**  PARAMETERS:  MV_U32 *pattern
**               MV_U32 *size
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxDelimiterGet(MV_U32 *pattern, MV_U32 *size)
{
	MV_STATUS status;

	status  = asicOntGlbRegRead(mvAsicReg_GPON_TX_DELIMITER_PAT, pattern, 0);
	status |= asicOntGlbRegRead(mvAsicReg_GPON_TX_DELIMITER_SIZE, size, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacTxPloamDataFifoSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the Tx ploam data fifo - one entry
**
**  PARAMETERS:  MV_U32 txPloamData
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxPloamDataFifoSet(MV_U32 txPloamData)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_GPON_TX_PLOAM_DATA_WRITE, txPloamData, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacTxPloamDataFreeGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the Tx ploam data fifo size
**
**  PARAMETERS:  MV_U32 *ploamFifoSize
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxPloamDataFreeGet(MV_U32 *ploamFifoSize)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_TX_PLOAM_DATA_WRITE_FREE, ploamFifoSize, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacBurstEnParamSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the burst enable parameters
**
**  PARAMETERS:  MV_U32 mask
**               MV_U32 polarity
**               MV_U32 delay
**               MV_U32 enStop
**               MV_U32 enStart
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxBurstEnParamSet(MV_U32 mask,
					MV_U32 polarity,
					MV_U32 delay,
					MV_U32 enStop,
					MV_U32 enStart)
{
	MV_STATUS status;

	status  = asicOntGlbRegWrite(mvAsicReg_GPON_TX_BURST_EN_PARAM_STR, enStart, 0);
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_TX_BURST_EN_PARAM_STP, enStop, 0);
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_TX_BURST_EN_PARAM_DLY, delay, 0);
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_TX_BURST_EN_PARAM_P, polarity, 0);
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_TX_BURST_EN_PARAM_MASK, mask, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacTxBurstEnPolaritySet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the burst enable polarity
**
**  PARAMETERS:  MV_U32 polarity
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxBurstEnPolaritySet(MV_U32 polarity)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_GPON_TX_BURST_EN_PARAM_P, polarity, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacTxBurstEnParamGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the burst enable parameters
**
**  PARAMETERS:  MV_U32 *mask
**               MV_U32 *polarity
**               MV_U32 *delay
**               MV_U32 *enStop
**               MV_U32 *enStart
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxBurstEnParamGet(MV_U32 *mask,
					MV_U32 *polarity,
					MV_U32 *order,
					MV_U32 *enStop,
					MV_U32 *enStart)
{
	MV_STATUS status;

	status  = asicOntGlbRegRead(mvAsicReg_GPON_TX_BURST_EN_PARAM_STR, enStart, 0);
	status |= asicOntGlbRegRead(mvAsicReg_GPON_TX_BURST_EN_PARAM_STP, enStop, 0);
	status |= asicOntGlbRegRead(mvAsicReg_GPON_TX_BURST_EN_PARAM_DLY, order, 0);
	status |= asicOntGlbRegRead(mvAsicReg_GPON_TX_BURST_EN_PARAM_P, polarity, 0);
	status |= asicOntGlbRegRead(mvAsicReg_GPON_TX_BURST_EN_PARAM_MASK, mask, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacTxFinalDelaySet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the Tx final delay
**
**  PARAMETERS:  MV_U32 txFinalDelay
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxFinalDelaySet(MV_U32 txFinalDelay)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_GPON_TX_FDELAY, txFinalDelay, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacTxFinalDelayGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the Tx final delay
**
**  PARAMETERS:  MV_U32 *txFinalDelay
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxFinalDelayGet(MV_U32 *txFinalDelay)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_TX_FDELAY, txFinalDelay, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacTxPreambleSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the Tx preamble type (1,2,3)
**
**  PARAMETERS:  MV_U32 preamType
**               MV_U32 preamCnt
**               MV_U32 preamVal
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxPreambleSet(MV_U32 type, MV_U32 pattern, MV_U32 size)
{
	MV_STATUS status;

	if (type >= 3)
		return MV_ERROR;

	status  = asicOntGlbRegWrite(mvAsicReg_GPON_TX_PREAMBLE_PATT, pattern, type);
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_TX_PREAMBLE_SIZE, size, type);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacTxPlsSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the Tx PLS
**
**  PARAMETERS:  MV_U32 pattern
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxPlsSet(MV_U32 pattern)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_GPON_TX_PLS_CONSTANT, pattern, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacTxConstIdleMsgSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the Tx const data ram - one entry
**
**  PARAMETERS:  MV_U32 onuId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxConstIdleMsgSet(MV_U32 onuId)
{
	MV_STATUS status;
	MV_U32 dataWord;

	dataWord = ((onuId & 0xFF) << 24) | (ONU_GPON_US_MSG_NO_MESSAGE << 16);

	status  = asicOntGlbRegWrite(mvAsicReg_GPON_TX_CONST_DATA_RAM_IDLE, dataWord, 0);
	dataWord = 0;
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_TX_CONST_DATA_RAM_IDLE, dataWord, 1);
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_TX_CONST_DATA_RAM_IDLE, dataWord, 2);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacTxConstIdleMsgSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the Tx const data ram - one entry
**
**  PARAMETERS:  MV_U32 onuId
**               MV_U8  *serialNumber
**               MV_U32 randomDelay
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxConstSerialNumberMsgSet(MV_U32 onuId,
						MV_U8 *serialNumber,
						MV_U32 randomDelay)
{
	MV_STATUS status;
	MV_U32 dataWord;

	dataWord = ((onuId & 0xFF) << 24) | (ONU_GPON_US_MSG_SN_ONU << 16) |
		   (((MV_U32)(serialNumber[0])) << 8) | (MV_U32)(serialNumber[1]);
	status  = asicOntGlbRegWrite(mvAsicReg_GPON_TX_CONST_DATA_RAM_SN, dataWord, 0);

	dataWord = (((MV_U32)(serialNumber[2])) << 24) | (((MV_U32)(serialNumber[3])) << 16) |
		   (((MV_U32)(serialNumber[4])) << 8) | (MV_U32)(serialNumber[5]);
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_TX_CONST_DATA_RAM_SN, dataWord, 1);

	dataWord = (((MV_U32)(serialNumber[6])) << 24) | (((MV_U32)(serialNumber[7])) << 16) |
		   (((randomDelay >> 4) & 0xFF) << 8) | ((randomDelay & 0xF) << 4 | 5);
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_TX_CONST_DATA_RAM_SN, dataWord, 2);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacTxDbrBlockSizeSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the Tx DBR Block size
**
**  PARAMETERS:  MV_U32 blockSize
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxDbrBlockSizeSet(MV_U32 blockSize)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_GPON_TX_DBR_REPORT_BLOCK_IDX, blockSize, 0);

	return status;
}

/******************************************************************************/
/* ========================================================================== */
/*                           General Low Level Routines                       */
/* ========================================================================== */
/******************************************************************************/

/*******************************************************************************
**
**  mvOnuGponMacOnuIdSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the ONU ID
**
**  PARAMETERS:  MV_U32 onuId
**               MV_BOOL   valid
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacOnuIdSet(MV_U32 onuId, MV_BOOL valid)
{
	MV_STATUS status;
	MV_U32 validBit;

	status = asicOntGlbRegWrite(mvAsicReg_GPON_ONU_ID_OID, onuId, 0);

	validBit = (valid == MV_TRUE) ? 1 : 0;

	status |= asicOntGlbRegWrite(mvAsicReg_GPON_ONU_ID_V, validBit, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacOnuStateSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the onu state
**
**  PARAMETERS:  MV_U32 onuState
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacOnuStateSet(MV_U32 onuState)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_GPON_ONU_STATE, onuState, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacSerialNumberSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set 2 bytes of Serial Number
**				 it is for ASIC random value creation - so the 2 last bytes
**				 should be put here
**
**  PARAMETERS:  MV_U32 serialNum
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacSerialNumberSet(MV_U32 serialNum)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_GPON_TX_SERIAL_NUMBER, serialNum, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponTimeOfDayIntWidthSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function configures TOD INT parameters
**
**  PARAMETERS:  MV_U32 width    - INT width in cycle units
**		 MV_U8  mode     - 0 for BIP INT, 1 for TOD SPF INT
**		 MV_U8  polarity - 0 - active HIGH, 1 - active LOW
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponTimeOfDayIntWidthSet(MV_U32 width,
					MV_U8 mode,
					MV_U8 polarity)
{
	MV_U32 reg = width & 0x0FFFFFFF;

	reg |= (mode != 0)     ? (1 << 28) : 0;
	reg |= (polarity != 0) ? (1 << 29) : 0;

	return asicOntGlbRegWrite(mvAsicReg_GPON_GEN_TOD_INT_WIDTH, reg, 0);
}

/*******************************************************************************
**
**  mvOnuGponTimeOfDayIntWidthGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function obtains TOD INT parameters
**
**  PARAMETERS:  MV_U32 width    - INT width in cycle units
**		 MV_U8  mode     - 0 for BIP INT, 1 for TOD SPF INT
**		 MV_U8  polarity - 0 - active HIGH, 1 - active LOW
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponTimeOfDayIntWidthGet(MV_U32 *width,
					MV_U8  *mode,
					MV_U8  *polarity)
{
	MV_STATUS status;
	MV_U32 reg;

	status = asicOntGlbRegRead(mvAsicReg_GPON_GEN_TOD_INT_WIDTH, &reg, 0);
	if (status == MV_OK) {
		*width     = reg & 0x0FFFFFFF;
		*mode      = (reg >> 28) & 0x1;
		*polarity  = (reg >> 29) & 0x1;
	}

	return status;
}

/*******************************************************************************
**
**  mvOnuGponSuperFrameCouterSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function configures Super Frame Counter value for TOD INT
**
**  PARAMETERS:  MV_U32 sfc   - Super Frame Couter value
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponSuperFrameCouterSet(MV_U32 sfc)
{
	return asicOntGlbRegWrite(mvAsicReg_GPON_GEN_TOD_INT_SFC, sfc, 0);
}

/*******************************************************************************
**
**  mvOnuGponSuperFrameCouterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function obtains Super Frame Counter value for TOD INT
**
**  PARAMETERS:  MV_U32 sfc   - Super Frame Couter value
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponSuperFrameCouterGet(MV_U32 *sfc)
{
	return asicOntGlbRegRead(mvAsicReg_GPON_GEN_TOD_INT_SFC, sfc, 0);
}

/******************************************************************************/
/* ========================================================================== */
/*                           Gem Low Level Routines                           */
/* ========================================================================== */
/******************************************************************************/

/*******************************************************************************
**
**  mvOnuGponMacGemPayloadLenSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the max gem payload length
**
**  PARAMETERS:  MV_U32 payloadlen
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemPayloadLenSet(MV_U32 payloadlen)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_GPON_GEM_CONFIG_PLI, payloadlen, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemPayloadLenGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the max gem payload length
**
**  PARAMETERS:  MV_U32 *payloadlen
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemPayloadLenGet(MV_U32 *payloadlen)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_GEM_CONFIG_PLI, payloadlen, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemEthFrameLenSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the max / min ethernet frame length
**
**  PARAMETERS:  MV_U32 maxFrameLen
**               MV_U32 minFrameLen
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemEthFrameLenSet(MV_U32 maxFrameLen, MV_U32 minFrameLen)
{
	MV_STATUS status;

	status  = asicOntGlbRegWrite(mvAsicReg_GPON_GEM_CONFIG_ETH_FRAME_LEN_MAX, maxFrameLen, 0);
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_GEM_CONFIG_ETH_FRAME_LEN_MIN, minFrameLen, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemEthFrameLenGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the max / min ethernet frame length
**
**  PARAMETERS:  MV_U32 maxFrameLen
**               MV_U32 minFrameLen
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemEthFrameLenGet(MV_U32 *maxFrameLen, MV_U32 *minFrameLen)
{
	MV_STATUS status;

	status  = asicOntGlbRegRead(mvAsicReg_GPON_GEM_CONFIG_ETH_FRAME_LEN_MAX, maxFrameLen, 0);
	status |= asicOntGlbRegRead(mvAsicReg_GPON_GEM_CONFIG_ETH_FRAME_LEN_MIN, minFrameLen, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemOmciFrameLenSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the max / min omci frame length
**
**  PARAMETERS:  MV_U32 maxFrameLen
**               MV_U32 minFrameLen
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemOmciFrameLenSet(MV_U32 maxFrameLen, MV_U32 minFrameLen)
{
	MV_STATUS status;

	status  = asicOntGlbRegWrite(mvAsicReg_GPON_GEM_CONFIG_OMCI_FRAME_LEN_MAX, maxFrameLen, 0);
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_GEM_CONFIG_OMCI_FRAME_LEN_MIN, minFrameLen, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemOmciFrameLenGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the max / min omci frame length
**
**  PARAMETERS:  MV_U32 maxFrameLen
**               MV_U32 minFrameLen
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemOmciFrameLenGet(MV_U32 *maxFrameLen, MV_U32 *minFrameLen)
{
	MV_STATUS status;

	status  = asicOntGlbRegRead(mvAsicReg_GPON_GEM_CONFIG_OMCI_FRAME_LEN_MAX, maxFrameLen, 0);
	status |= asicOntGlbRegRead(mvAsicReg_GPON_GEM_CONFIG_OMCI_FRAME_LEN_MIN, minFrameLen, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemOmciPortIdSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the omci port Id
**
**  PARAMETERS:  MV_U32 portId
**               MV_U32 valid
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemOmciPortIdSet(MV_U32 portId, MV_U32 valid)
{
	MV_STATUS status;

	status  = asicOntGlbRegWrite(mvAsicReg_GPON_GEM_CONFIG_OMCI_PORT_ID,    portId, 0);
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_GEM_CONFIG_OMCI_PORT_VALID, valid, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemOmciPortIdGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the omci port Id
**
**  PARAMETERS:  MV_U32 portId
**               MV_U32 valid
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemOmciPortIdGet(MV_U32 *portId, MV_U32 *valid)
{
	MV_STATUS status;

	status  = asicOntGlbRegRead(mvAsicReg_GPON_GEM_CONFIG_OMCI_PORT_ID,    portId, 0);
	status |= asicOntGlbRegRead(mvAsicReg_GPON_GEM_CONFIG_OMCI_PORT_VALID, valid, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemOmciFastPathSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function enables or disables the omci fast path feature
**
**  PARAMETERS:  MV_U32 valid
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemOmciFastPathSet(MV_U32 valid)
{
	MV_STATUS status;

	/* set register */
	status = asicOntGlbRegWrite(mvAsicReg_GPON_GEM_CONFIG_OMCI_FAST_PATH_VALID, valid, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemOmciFastPathGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the omci fast path status
**
**  PARAMETERS:  None
**
**  OUTPUTS:     MV_U32 valid
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemOmciFastPathGet(MV_U32 *valid)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_GEM_CONFIG_OMCI_FAST_PATH_VALID, valid, 0);

	return status;
}

/******************************************************************************/
/* ========================================================================== */
/*                           UTM Low Level Routines                           */
/* ========================================================================== */
/******************************************************************************/

/*******************************************************************************
**
**  mvOnuGponMacUtmOmciPortIdSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the omci port Id
**
**  PARAMETERS:  MV_U32 portId
**               MV_U32 valid
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacUtmOmciPortIdSet(MV_U32 portId, MV_U32 valid)
{
	MV_STATUS status;

	status  = asicOntGlbRegWrite(mvAsicReg_GPON_UTM_CONFIG_OMCI_PORT_ID,    portId, 0);
	status |= asicOntGlbRegWrite(mvAsicReg_GPON_UTM_CONFIG_OMCI_PORT_VALID, valid, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacUtmOmciPortIdGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the omci port Id
**
**  PARAMETERS:  MV_U32 *portId
**               MV_U32 *valid
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacUtmOmciPortIdGet(MV_U32 *portId, MV_U32 *valid)
{
	MV_STATUS status;

	status  = asicOntGlbRegRead(mvAsicReg_GPON_UTM_CONFIG_OMCI_PORT_ID,    portId, 0);
	status |= asicOntGlbRegRead(mvAsicReg_GPON_UTM_CONFIG_OMCI_PORT_VALID, valid, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacUtmTcPeriodSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the TC period
**
**  PARAMETERS:  MV_U32 period
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacUtmTcPeriodSet(MV_U32 period)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_GPON_UTM_CONFIG_TC_PERIOD, period, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacUtmTcValidSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the TC valid
**
**  PARAMETERS:  MV_U32 valid
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacUtmTcValidSet(MV_U32 valid)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_GPON_UTM_CONFIG_TC_PERIOD_VALID, valid, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacUtmTcConfigGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the TC configuration
**
**  PARAMETERS:  MV_U32 *period
**               MV_U32 *valid
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacUtmTcConfigGet(MV_U32 *period, MV_U32 *valid)
{
	MV_STATUS status;

	status  = asicOntGlbRegRead(mvAsicReg_GPON_UTM_CONFIG_TC_PERIOD,       period, 0);
	status |= asicOntGlbRegRead(mvAsicReg_GPON_UTM_CONFIG_TC_PERIOD_VALID, valid, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacUtmActiveTxBitmapSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the active TX bitmap
**
**  PARAMETERS:  MV_U32 bitmap
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacUtmActiveTxBitmapSet(MV_U32 bitmap)
{
	return asicOntGlbRegWrite(mvAsicReg_GPON_UTM_ACTIVE_TX_BITMAP, bitmap, 0);
}

/*******************************************************************************
**
**  mvOnuGponMacUtmActiveTxBitmapValidSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the active TX bitmap valid
**
**  PARAMETERS:  MV_U32 valid
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacUtmActiveTxBitmapValidSet(MV_U32 valid)
{
	return asicOntGlbRegWrite(mvAsicReg_GPON_UTM_ACTIVE_TX_BITMAP_VALID, valid, 0);
}

/*******************************************************************************
**
**  mvOnuGponMacUtmActiveTxBitmapConfigGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the active TX bitmap configuration
**
**  PARAMETERS:  MV_U32 *bitmap
**               MV_U32 *valid
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacUtmActiveTxBitmapConfigGet(MV_U32 *bitmap, MV_U32 *valid)
{
	MV_STATUS status;

	status  = asicOntGlbRegRead(mvAsicReg_GPON_UTM_ACTIVE_TX_BITMAP,       bitmap, 0);
	status |= asicOntGlbRegRead(mvAsicReg_GPON_UTM_ACTIVE_TX_BITMAP_VALID, valid, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacUtmFlushSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function sets the UTM TX flush
**
**  PARAMETERS:  MV_U32 tcontNum, MV_U32 value
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacUtmFlushSet(MV_U32 tcontNum, MV_U32 value)
{
	MV_U32 regVal;
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_UTM_FLUSH, &regVal, 0);
	if (status)
		return status;

	regVal &= ~(1 << (tcontNum & 0xf));
	regVal |= ((value & 0x1) << (tcontNum & 0xf));

	status = asicOntGlbRegWrite(mvAsicReg_GPON_UTM_FLUSH, regVal, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacUtmFlushGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function gets the UTM TX flush
**
**  PARAMETERS:  MV_U32 tcontNum
**
**  OUTPUTS:     MV_U32 *flush
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacUtmFlushGet(MV_U32 tcontNum, MV_U32 *value)
{
	MV_U32 regVal;
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_UTM_FLUSH, &regVal, 0);
	if (status)
		return status;

	*value = ((regVal >> (tcontNum & 0xf)) & 0x1);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacUtmGeneralSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function sets the UTM TX general
**
**  PARAMETERS:  MV_U32 latencyMode   - UTM latency mode
**               MV_U32 latencyThresh - UTM latency threshold
**               MV_U32 latencyIpg    - UTM latency IPG
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacUtmGeneralSet(MV_U32 latencyMode, MV_U32 latencyThresh, MV_U32 latencyIpg)
{
	MV_U32 regVal;
	MV_STATUS status;

	regVal = ((latencyMode & 0x1) << 24) |
		 ((latencyThresh & 0x1ff) << 8) |
		 (latencyIpg & 0xff);

	status = asicOntGlbRegWrite(mvAsicReg_GPON_UTM_FLUSH, regVal, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacUtmGeneralGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function sets the UTM TX general
**
**  PARAMETERS:  MV_U32 latencyMode   - UTM latency mode
**               MV_U32 latencyThresh - UTM latency threshold
**               MV_U32 latencyIpg    - UTM latency IPG
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacUtmGeneralGet(MV_U32 *latencyMode, MV_U32 *latencyThresh, MV_U32 *latencyIpg)
{
	MV_U32 regVal;
	MV_STATUS status;
	status = asicOntGlbRegRead(mvAsicReg_GPON_UTM_FLUSH, &regVal, 0);
	if (status)
		return status;

	*latencyMode   = ((regVal >> 24) & 0x1);
	*latencyThresh = ((regVal >> 8) & 0x1ff);
	*latencyIpg    = ((regVal >> 0) & 0xff);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacUtmDebugGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function gets the UTM TX flush
**
**  PARAMETERS:  None
**
**  OUTPUTS:     MV_U32 *state
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacUtmDebugGet(MV_U32 tcontNum, MV_U32 *state)
{
	MV_U32 regVal;
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_UTM_DEBUG, &regVal, 0);
	if (status)
		return status;

	*state = ((regVal >> (tcontNum & 0xf)) & 0x1);

	return status;
}

/******************************************************************************/
/* ========================================================================== */
/*                           Interrupt Low Level Routines                     */
/* ========================================================================== */
/******************************************************************************/

/*******************************************************************************
**
**  mvOnuGponMacPonInterruptGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the interrupt event
**
**  PARAMETERS:  MV_U32 *interrupt
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacPonInterruptGet(MV_U32 *interrupt)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_INTERRUPT_PON, interrupt, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacPonInterruptMaskGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the interrupt mask
**
**  PARAMETERS:  MV_U32 *mask
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacPonInterruptMaskGet(MV_U32 *mask)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_INTERRUPT_PON_MASK, mask, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacPonInterruptMaskSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the interrupt mask
**
**  PARAMETERS:  MV_U32 *mask
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacPonInterruptMaskSet(MV_U32 mask)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_GPON_INTERRUPT_PON_MASK, mask, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacBipStatusValueGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the BIP8 general counter
**
**  PARAMETERS:  MV_U32 *value
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacBipStatusValueGet(MV_U32 *value)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_RX_BIP_STATUS, value, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacBipInterruptIntervalSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the BIP8 interrupt interval (in frames)
**
**  PARAMETERS:  MV_U32 interval
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacBipInterruptIntervalSet(MV_U32 interval)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_BIP_INTR_INTERVAL, interval, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacBipStatusValueGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the BIP8 interrupt counter
**
**  PARAMETERS:  MV_U32 *value
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacBipInterruptStatusValueGet(MV_U32 *value)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_RX_BIP_STATUS_FOR_INTERRUPT, value, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacBipInterruptIntervalSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set ram test configuration
**
**  PARAMETERS:  MV_U32 interval
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRamTestCfgSet(MV_U32 config)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_RAM_TEST_CONFIG, config, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacRamTestCfgGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return ram test configuration
**
**  PARAMETERS:  MV_U32 *value
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRamTestCfgGet(MV_U32 *config)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_RAM_TEST_CONFIG, config, 0);

	return status;
}

/******************************************************************************/
/* ========================================================================== */
/*                           Statistics Low Level Routines                    */
/* ========================================================================== */
/******************************************************************************/

/* ========================================================================== */
/*                       GEM Statistics Low Level Routines                    */
/* ========================================================================== */

/*******************************************************************************
**
**  mvOnuGponMacGemRcvdIdleGemFramesCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return received idle gem frames counter
**
**  PARAMETERS:  MV_U32 *cntValue
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemRcvdIdleGemFramesCounterGet(MV_U32 *cntValue)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_GEM_STAT_IDLE_GEM_CNT, cntValue, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemRcvdValidGemFramesCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return received valid gem frames counter
**
**  PARAMETERS:  MV_U32 *cntValue
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemRcvdValidGemFramesCounterGet(MV_U32 *cntValue)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_GEM_STAT_VALID_GEM_CNT, cntValue, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemRcvdUndefGemFramesCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return received undefined gem frames counter
**
**  PARAMETERS:  MV_U32 *cntValue
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemRcvdUndefGemFramesCounterGet(MV_U32 *cntValue)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_GEM_STAT_UNDEF_GEM_CNT, cntValue, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemRcvdOmciFramesCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return received omci frames counter
**
**  PARAMETERS:  MV_U32 *cntValue
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemRcvdOmciFramesCounterGet(MV_U32 *cntValue)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_GEM_STAT_VALID_OMCI_CNT, cntValue, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemDropGemFramesCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return received dropped gem frames counter
**
**  PARAMETERS:  MV_U32 *cntValue
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemDropGemFramesCounterGet(MV_U32 *cntValue)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_GEM_STAT_DROPPED_GEM_CNT, cntValue, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemDropOmciFramesCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return received dropped omci frames counter
**
**  PARAMETERS:  MV_U32 *cntValue
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemDropOmciFramesCounterGet(MV_U32 *cntValue)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_GEM_STAT_DROPPED_OMCI_CNT, cntValue, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemRcvdGemFramesWithUncorrHecErrCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return received gem frames with uncorrect hec error counter
**
**  PARAMETERS:  MV_U32 *cntValue
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemRcvdGemFramesWithUncorrHecErrCounterGet(MV_U32 *cntValue)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_GEM_STAT_ERROR_GEM_CNT, cntValue, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemRcvdGemFramesWithOneFixedHecErrCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return received gem frames with one fixed hec error counter
**
**  PARAMETERS:  MV_U32 *cntValue
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemRcvdGemFramesWithOneFixedHecErrCounterGet(MV_U32 *cntValue)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_GEM_STAT_ONE_FIXED_HEC_ERR_CNT, cntValue, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemRcvdGemFramesWithTwoFixedHecErrCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return received gem frames with two fixed hec error counter
**
**  PARAMETERS:  MV_U32 *cntValue
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemRcvdGemFramesWithTwoFixedHecErrCounterGet(MV_U32 *cntValue)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_GEM_STAT_TWO_FIXED_HEC_ERR_CNT, cntValue, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemRcvdValidGemFramesTotalByteCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return received valid gem frames total bytes counter
**
**  PARAMETERS:  MV_U32 *cntValue
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemRcvdValidGemFramesTotalByteCounterGet(MV_U32 *cntValue)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_GEM_STAT_VALID_GEM_PAYLOAD_CNT, cntValue, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemRcvdUndefGemFramesTotalByteCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return received undefined gem frames total bytes counter
**
**  PARAMETERS:  MV_U32 *cntValue
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemRcvdUndefGemFramesTotalByteCounterGet(MV_U32 *cntValue)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_GEM_STAT_UNDEF_GEM_PAYLOAD_CNT, cntValue, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemReassembleMemoryFlushCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return received gem reassemble memory flush counter
**
**  PARAMETERS:  MV_U32 *cntValue
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemReassembleMemoryFlushCounterGet(MV_U32 *cntValue)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_GEM_STAT_RSMBL_MEM_FLUSH_CNT, cntValue, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemSynchLostCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return gem synch lost counter
**
**  PARAMETERS:  MV_U32 *cntValue
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemSynchLostCounterGet(MV_U32 *cntValue)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_GEM_STAT_LCDGi_CNT, cntValue, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemRcvdEthFramesWithCorrFcsCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return received ethernet frames with correct FCS counter
**
**  PARAMETERS:  MV_U32 *cntValue
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemRcvdEthFramesWithCorrFcsCounterGet(MV_U32 *cntValue)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_GEM_STAT_CORR_ETH_FRAME_CNT, cntValue, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemRcvdEthFramesWithFcsErrCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return received ethernet frames with FCS error counter
**
**  PARAMETERS:  MV_U32 *cntValue
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemRcvdEthFramesWithFcsErrCounterGet(MV_U32 *cntValue)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_GEM_STAT_ERR_ETH_FRAME_CNT, cntValue, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemRcvdOmciFramesWithCorrCrcCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return received omci frames with correct CRC counter
**
**  PARAMETERS:  MV_U32 *cntValue
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemRcvdOmciFramesWithCorrCrcCounterGet(MV_U32 *cntValue)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_GEM_STAT_CORR_OMCI_FRAME_CNT, cntValue, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemRcvdOmciFramesWithCrcErrCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return received omci frames with CRC error counter
**
**  PARAMETERS:  MV_U32 *cntValue
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemRcvdOmciFramesWithCrcErrCounterGet(MV_U32 *cntValue)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_GEM_STAT_ERR_OMCI_FRAME_CNT, cntValue, 0);

	return status;
}

/* ========================================================================== */
/*                  Rx PLOAM Statistics Low Level Routines                    */
/* ========================================================================== */

/******************************************************************************
**
**  mvOnuGponMacRxPloamLostCrcCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return Rx ploam lost crc counter
**
**  PARAMETERS:  MV_U32 *counter
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxPloamLostCrcCounterGet(MV_U32 *counter)
{
	MV_STATUS status;

	*counter = 0;
	status = asicOntGlbRegRead(mvAsicReg_GPON_RX_PLOAMD_LOST_CRC_STATUS, counter, 0);
	if (status != MV_OK)
		return status;

	return MV_OK;
}

/*******************************************************************************
**
**  mvOnuGponMacRxPloamLostFullCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return Rx ploam lost full counter
**
**  PARAMETERS:  MV_U32 *counter
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxPloamLostFullCounterGet(MV_U32 *counter)
{
	MV_STATUS status;

	*counter = 0;
	status = asicOntGlbRegRead(mvAsicReg_GPON_RX_PLOAMD_LOST_FULL_STATUS, counter, 0);
	if (status != MV_OK)
		return status;

	return MV_OK;
}

/*******************************************************************************
**
**  mvOnuGponMacRxPloamRcvdIdleCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return Rx ploam received idle counter
**
**  PARAMETERS:  MV_U32 *counter
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxPloamRcvdIdleCounterGet(MV_U32 *counter)
{
	MV_STATUS status;

	*counter = 0;
	status = asicOntGlbRegRead(mvAsicReg_GPON_RX_PLOAMD_RCVD_IDLE_STATUS, counter, 0);
	if (status != MV_OK)
		return status;

	return MV_OK;
}

/*******************************************************************************
**
**  mvOnuGponMacRxPloamRcvdBroadCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return Rx ploam received broadcast counter
**
**  PARAMETERS:  MV_U32 *counter
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxPloamRcvdBroadCounterGet(MV_U32 *counter)
{
	MV_STATUS status;

	*counter = 0;
	status = asicOntGlbRegRead(mvAsicReg_GPON_RX_PLOAMD_RCVD_BROAD_STATUS, counter, 0);
	if (status != MV_OK)
		return status;

	return MV_OK;
}

/*******************************************************************************
**
**  mvOnuGponMacRxPloamRcvdOnuIdCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return Rx ploam received Onu Id counter
**
**  PARAMETERS:  MV_U32 *counter
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxPloamRcvdOnuIdCounterGet(MV_U32 *counter)
{
	MV_STATUS status;

	*counter = 0;
	status = asicOntGlbRegRead(mvAsicReg_GPON_RX_PLOAMD_RCVD_MYID_STATUS, counter, 0);
	if (status != MV_OK)
		return status;

	return MV_OK;
}

/* ========================================================================== */
/*                        Tx Statistics Low Level Routines                    */
/* ========================================================================== */

/*******************************************************************************
**
**  mvOnuGponMacTxGemPtiTypeOneFrameCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the num of trans gem frames with pti = 1
**
**  PARAMETERS:  MV_U32 *counter
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxGemPtiTypeOneFrameCounterGet(MV_U32 *counter)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_TX_STAT_GEM_PTI1, counter, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacTxGemPtiTypeZeroFrameCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the num of trans gem frames with pti = 0
**
**  PARAMETERS:  MV_U32 *counter
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxGemPtiTypeZeroFrameCounterGet(MV_U32 *counter)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_TX_STAT_GEM_PTI0, counter, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacTxIdleGemFramesCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the num of trans idle gem frames
**
**  PARAMETERS:  MV_U32 *counter
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxIdleGemFramesCounterGet(MV_U32 *counter)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_TX_STAT_GEM_IDLE, counter, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacTxTxEnableCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the num of cycles with tx enable asserted
**
**  PARAMETERS:  MV_U32 *counter
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxTxEnableCounterGet(MV_U32 *counter)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_TX_STAT_TX_EN_CNT, counter, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacTxTxEnableCounterThresholdGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return tx enable counter threshold
**
**  PARAMETERS:  MV_U32 *threshold
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxTxEnableCounterThresholdGet(MV_U32 *threshold)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_TX_CONFIG_EN_THRESHOLD, threshold, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacTxTxEnableCounterThresholdSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set tx enable counter threshold
**
**  PARAMETERS:  MV_U32 threshold
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxTxEnableCounterThresholdSet(MV_U32 threshold)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_GPON_TX_CONFIG_EN_THRESHOLD, threshold, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacTxGseTransCounterThresholdGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return GSE threshold for starting forwarding data to
**               the TX burst FIFO
**
**  PARAMETERS:  MV_U32 *threshold
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxGseTransCounterThresholdGet(MV_U32 *threshold)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_TX_GSE_TRANS_THRESHOLD, threshold, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacTxGseTransCounterThresholdSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set GSE threshold for starting forwarding data to
**               the TX burst FIFO
**
**  PARAMETERS:  MV_U32 threshold
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxGseTransCounterThresholdSet(MV_U32 threshold)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_GPON_TX_GSE_TRANS_THRESHOLD, threshold, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacTxGseTransConfigAcCouplingSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set GSE Transmit Configuration AC coupling
**
**  PARAMETERS:  MV_U32 busrtMode          - GPON_TX_AC_COUPL_BUST_MODE_0 or
**					     GPON_TX_AC_COUPL_BUST_MODE_1
**		 MV_U32 preActiveBurstTime - time from burst to TX pattern2
**		 MV_U8  dataPattern1       - Pattern1
**		 MV_U8  dataPattern2       - Pattern2
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxConfigAcCouplingGet(MV_U32 *busrtMode,
					    MV_U32 *preActiveBurstTime,
					    MV_U8  *dataPattern1,
					    MV_U8  *dataPattern2)
{
	MV_STATUS status;
	MV_U32 reg;

	status = asicOntGlbRegRead(mvAsicReg_GPON_TX_CFG_AC_COUPLING, &reg, 0);
	if (status == MV_OK) {
		*busrtMode           = (reg >> 30) & 0x3;
		*preActiveBurstTime  = (reg >> 16) & 0x3FFF;
		*dataPattern2        = (reg >>  8) & 0xFF;
		*dataPattern1        = reg & 0xFF;
	}

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacTxGseTransConfigAcCouplingSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set GSE Transmit Configuration AC coupling
**
**  PARAMETERS:  MV_U32 busrtMode          - GPON_TX_AC_COUPL_BUST_MODE_0 or
**					     GPON_TX_AC_COUPL_BUST_MODE_1
**		 MV_U32 preActiveBurstTime - time from burst to TX pattern2
**		 MV_U8  dataPattern1       - Pattern1
**		 MV_U8  dataPattern2       - Pattern2
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxConfigAcCouplingSet(MV_U32 busrtMode,
					    MV_U32 preActiveBurstTime,
					    MV_U8 dataPattern1,
					    MV_U8 dataPattern2)
{
	MV_U32 reg = 0;

	if (busrtMode != GPON_TX_AC_COUPL_BUST_MODE_0)
		reg = 1 << 30;

	reg |= (preActiveBurstTime & 0x3FFF) << 16;
	reg |= dataPattern1 | (dataPattern2 << 8);

	return asicOntGlbRegWrite(mvAsicReg_GPON_TX_CFG_AC_COUPLING, reg, 0);
}

/*******************************************************************************
**
**  mvOnuGponMacTxRandomDelayUnitsGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return units of random delay
**
**  PARAMETERS:  MV_U32 units - random delay units
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxRandomDelayUnitsGet(MV_U32 *units)
{
	return asicOntGlbRegRead(mvAsicReg_GPON_TX_CFG_RAND_DELAY, units, 0);
}

/*******************************************************************************
**
**  mvOnuGponMacTxRandomDelayUnitsSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return units of random delay
**
**  PARAMETERS:  MV_U32 units - random delay units
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxRandomDelayUnitsSet(MV_U32 units)
{
	return asicOntGlbRegWrite(mvAsicReg_GPON_TX_CFG_RAND_DELAY, units, 0);
}

/*******************************************************************************
**
**  mvOnuGponMacTxEthFramesTcontiCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return number of trans ethernet frames via tcont
**
**  PARAMETERS:  MV_U32 tcontNum
**               MV_U32 *counter
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxEthFramesTcontiCounterGet(MV_U32 tcontNum, MV_U32 *counter)
{
	MV_STATUS status;

	if (tcontNum >= ONU_GPON_MAX_NUM_OF_T_CONTS)
		return MV_ERROR;

	if (tcontNum < 8)
		status = asicOntGlbRegRead(mvAsicReg_GPON_TX_STAT_TCONT_i_ETH_FRAMES, counter, tcontNum);
	else
		status = asicOntGlbRegRead(mvAsicReg_GPON_TX_STAT_TCONT_i_8_15_ETH_FRAME, counter, (tcontNum - 8));

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacTxEthBytesTcontiCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return number of trans ethernet bytes via tcont
**
**  PARAMETERS:  MV_U32 tcontNum
**               MV_U32 *counter
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxEthBytesTcontiCounterGet(MV_U32 tcontNum, MV_U32 *counter)
{
	MV_STATUS status;

	if (tcontNum >= ONU_GPON_MAX_NUM_OF_T_CONTS)
		return MV_ERROR;

	if (tcontNum < 8)
		status = asicOntGlbRegRead(mvAsicReg_GPON_TX_STAT_TCONT_i_ETH_BYTES, counter, tcontNum);
	else
		status = asicOntGlbRegRead(mvAsicReg_GPON_TX_STAT_TCONT_i_8_15_ETH_BYTES, counter, tcontNum);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacTxGemFramesTcontiCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return number of trans gem frames via tcont
**
**  PARAMETERS:  MV_U32 tcontNum
**               MV_U32 *counter
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxGemFramesTcontiCounterGet(MV_U32 tcontNum, MV_U32 *counter)
{
	MV_STATUS status;

	if (tcontNum >= ONU_GPON_MAX_NUM_OF_T_CONTS)
		return MV_ERROR;

	if (tcontNum < 8)
		status = asicOntGlbRegRead(mvAsicReg_GPON_TX_STAT_TCONT_i_GEM_FRAMES, counter, tcontNum);
	else
		status = asicOntGlbRegRead(mvAsicReg_GPON_TX_STAT_TCONT_i_8_15_GEM_FRAME, counter, tcontNum);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacTxIdleGemFramesTcontiCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return number of trans idle gem frames via tcont
**
**  PARAMETERS:  MV_U32 tcontNum
**               MV_U32 *counter
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxIdleGemFramesTcontiCounterGet(MV_U32 tcontNum, MV_U32 *counter)
{
	MV_STATUS status;

	if (tcontNum >= ONU_GPON_MAX_NUM_OF_T_CONTS)
		return MV_ERROR;

	if (tcontNum < 8)
		status = asicOntGlbRegRead(mvAsicReg_GPON_TX_STAT_TCONT_i_IDLE_GEM, counter, tcontNum);
	else
		status = asicOntGlbRegRead(mvAsicReg_GPON_TX_STAT_TCONT_i_8_15_IDLE_GEM, counter, tcontNum);

	return status;
}

/* ========================================================================== */
/*                       FEC Statistics Low Level Routines                    */
/* ========================================================================== */

/*******************************************************************************
**
**  mvOnuGponMacRxFecBytesCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return Rx FEC Bytes counter
**
**  PARAMETERS:  MV_U32 *counter
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxFecBytesCounterGet(MV_U32 *counter)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_RX_FEC_STAT0, counter, 0);
	if (status != MV_OK)
		return status;

	return MV_OK;
}

/*******************************************************************************
**
**  mvOnuGponMacRxFecCorrectBytesCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return Rx FEC correct Bytes counter
**
**  PARAMETERS:  MV_U32 *bytes
**               MV_U32 *bits
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxFecCorrectCounterGet(MV_U32 *bytes, MV_U32 *bits)
{
	MV_STATUS status;
	MV_U32 regVal;

	*bytes = 0;
	*bits  = 0;
	status = asicOntGlbRegRead(mvAsicReg_GPON_RX_FEC_STAT1, &regVal, 0);
	if (status != MV_OK)
		return status;

	*bits  =  regVal & 0xFFFF;
	*bytes = (regVal >> 16) & 0xFFFF;

	return MV_OK;
}

/*******************************************************************************
**
**  mvOnuGponMacRxFecInorrectCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return Rx FEC Incorrect Bytes counter
**
**  PARAMETERS:  MV_U32 *bytes
**               MV_U32 *bits
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxFecInorrectCounterGet(MV_U32 *bytes, MV_U32 *bits)
{
	MV_STATUS status;
	MV_U32 regVal;

	*bytes = 0;
	*bits  = 0;
	status = asicOntGlbRegRead(mvAsicReg_GPON_RX_FEC_STAT2, &regVal, 0);
	if (status != MV_OK)
		return status;

	*bits  = regVal & 0xFFFF;
	*bytes = (regVal >> 16) & 0xFFFF;

	return MV_OK;
}

/* ========================================================================== */
/*                    Bw Map Statistics Low Level Routines                    */
/* ========================================================================== */

/*******************************************************************************
**
**  mvOnuGponMacRxBwMapAllocatrionsCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return Rx Bw Map allocation counters
**
**  PARAMETERS:  MV_U32 *cor
**               MV_U32 *incor
**               MV_U32 *crcCor
**               MV_U32 *errCrc
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxBwMapAllocatrionsCounterGet(MV_U32 *cor,
						    MV_U32 *incor,
						    MV_U32 *crcCor,
						    MV_U32 *errCrc)
{
	MV_STATUS status;
	MV_U32 regVal;

	*cor    = 0;
	*incor  = 0;
	*crcCor = 0;
	*errCrc = 0;

	status = asicOntGlbRegRead(mvAsicReg_GPON_RX_BWMAP_STATUS0, &regVal, 0);
	if (status != MV_OK)
		return status;

	*incor  = regVal & 0xFF;
	*crcCor = (regVal >> 8) & 0xFF;
	*cor    = (regVal >> 16) & 0xFF;
	*errCrc = (regVal >> 24) & 0xFF;

	return MV_OK;
}

/*******************************************************************************
**
**  mvOnuGponMacRxBwMapTotalBwGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return Rx Bw total Bw counter
**
**  PARAMETERS:  MV_U32 *bw
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxBwMapTotalBwGet(MV_U32 *bw)
{
	MV_STATUS status;
	MV_U32 regVal;

	*bw = 0;
	status = asicOntGlbRegRead(mvAsicReg_GPON_RX_BWMAP_STATUS1, &regVal, 0);
	if (status != MV_OK)
		return status;

	*bw = regVal & 0x7FFFF;

	return MV_OK;
}

/* ========================================================================== */
/*                     Plend Statistics Low Level Routines                    */
/* ========================================================================== */

/*******************************************************************************
**
**  onuGponInComingPlendCntGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return InComing Plend counter
**
**  PARAMETERS:  MV_U32 *cntValue
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacInComingPlendCntGet(MV_U32 *cntValue)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_RX_PLEND_STATUS, cntValue, 0);

	return status;
}

/******************************************************************************/
/* ========================================================================== */
/*                         High-Level MAC Section                             */
/* ========================================================================== */
/******************************************************************************/

/*******************************************************************************
**
**  mvOnuGponMacMessageReceive
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function read message from the message fifo
**               The ploam size is 12 octets - each read get 32 bits
**               3 read actions needed
**
**  PARAMETERS:  msgData  - 10 bytes (buffer is allocated by calling)
**               msgOnuId - the ONU ID on the message
**               msgId    - the message ID on the message
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacMessageReceive(MV_U8 *msgOnuId, MV_U8 *msgId, MV_U8 *msgData)
{
	MV_STATUS rcode;
	MV_U32 ploamFifoData_1;
	MV_U32 ploamFifoData_2;
	MV_U32 ploamFifoData_3;

	rcode  = mvOnuGponMacRxPloamDataGet(&ploamFifoData_1);
	rcode |= mvOnuGponMacRxPloamDataGet(&ploamFifoData_2);
	rcode |= mvOnuGponMacRxPloamDataGet(&ploamFifoData_3);

	if (rcode != MV_OK)
		return rcode;

	/* Outputs */
	*msgOnuId  = (MV_U8)((ploamFifoData_1 >> 24) & UINT8_MASK); /* MSB */
	*msgId     = (MV_U8)((ploamFifoData_1 >> 16) & UINT8_MASK);
	msgData[0] = (MV_U8)((ploamFifoData_1 >> 8)  & UINT8_MASK);
	msgData[1] = (MV_U8)(ploamFifoData_1        & UINT8_MASK);      /* LSB */
	msgData[2] = (MV_U8)((ploamFifoData_2 >> 24) & UINT8_MASK);     /* MSB */
	msgData[3] = (MV_U8)((ploamFifoData_2 >> 16) & UINT8_MASK);
	msgData[4] = (MV_U8)((ploamFifoData_2 >> 8)  & UINT8_MASK);
	msgData[5] = (MV_U8)(ploamFifoData_2        & UINT8_MASK);      /* LSB */
	msgData[6] = (MV_U8)((ploamFifoData_3 >> 24) & UINT8_MASK);     /* MSB */
	msgData[7] = (MV_U8)((ploamFifoData_3 >> 16) & UINT8_MASK);
	msgData[8] = (MV_U8)((ploamFifoData_3 >> 8)  & UINT8_MASK);
	msgData[9] = (MV_U8)(ploamFifoData_3        & UINT8_MASK); /* LSB */

	return MV_OK;
}

/*******************************************************************************
**
**  mvOnuGponMacTxPloamSend
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function writes PLOAM message to HW FIFO
**
**  PARAMETERS:  msgData  - 10 bytes (buffer is allocated by calling)
**               msgOnuId - the ONU ID on the message
**               msgId    - the message ID on the message
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxPloamSend(MV_U8 msgOnuId, MV_U8 msgId, MV_U8 *msgData)
{
	MV_STATUS rcode;
	MV_U32 ploamFifoData;

	fifoMacPloamSend++;

	/* Write ONU ID and Message ID and data[0:1] */
	ploamFifoData =  (((MV_U32)msgOnuId) << 24) | (((MV_U32)msgId) << 16) |
			(((MV_U32)msgData[0]) << 8) | (((MV_U32)msgData[1]) << 0);
	rcode = mvOnuGponMacTxPloamDataFifoSet(ploamFifoData);

	/* Write data[2:5] */
	ploamFifoData =  (((MV_U32)msgData[2]) << 24) | (((MV_U32)msgData[3]) << 16) |
			(((MV_U32)msgData[4]) << 8) | (((MV_U32)msgData[5]) << 0);
	rcode |= mvOnuGponMacTxPloamDataFifoSet(ploamFifoData);

	/* Write data[6:9] */
	ploamFifoData =  (((MV_U32)msgData[6]) << 24) | (((MV_U32)msgData[7]) << 16) |
			(((MV_U32)msgData[8]) << 8) | (((MV_U32)msgData[9]) << 0);
	rcode |= mvOnuGponMacTxPloamDataFifoSet(ploamFifoData);

	return rcode;
}

/*******************************************************************************
**
**  mvOnuGponMacMessageAdd
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function adds message to the SW FIFO
**
**  PARAMETERS:  msgData  - 10 bytes (buffer is allocated by calling)
**               msgOnuId - the ONU ID on the message
**               msgId    - the message ID on the message
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_FULL
**
*******************************************************************************/
MV_STATUS mvOnuGponMacMessageAdd(MV_U8 msgOnuId, MV_U8 msgId, MV_U8 *msgData)
{
	MV_STATUS rcode;
	MV_U8     *buffer;
	MV_U32 bufferIndex;

	/* Get pointer to and index of the free SW FIFO entry */
	rcode = mvOnuGponMacFifoMsgAlloc((MV_U32**)&buffer, &bufferIndex);
	if (rcode == MV_OK) {
		buffer[0] = msgOnuId;
		buffer[1] = msgId;
		memcpy(&buffer[2], msgData, GPON_PLOAM_MSG_BYTES - 2);

		mvOnuGponMacFifoMsgSend(bufferIndex);
	}

	return rcode;
}

/*******************************************************************************
**
**  mvOnuGponMacMessageSend
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function write message to the message fifo
**
**  PARAMETERS:  msgData  - 10 bytes (buffer is allocated by calling)
**               msgOnuId - the ONU ID on the message
**               msgId    - the message ID on the message
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK, MV_FULL or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacMessageSend(MV_U8 msgOnuId, MV_U8 msgId, MV_U8 *msgData)
{
	MV_STATUS rcode;
	MV_U32 ploamFifoSize;
	MV_U8     *buffer;
	MV_U32 bufferIndex;
	int i, moved_num;

	fifoMacMsgSend++;

	/* Check first if PLOAM FIFO has free place */
	rcode = mvOnuGponMacTxPloamDataFreeGet(&ploamFifoSize);

	if (rcode != MV_OK)
		return rcode;

	/* SW FIFO is not supported */
	if (!fifoSupported) {
		if (ploamFifoSize < GPON_PLOAM_MSG_WORDS)
			return MV_ERROR;

		return mvOnuGponMacTxPloamSend(msgOnuId, msgId, msgData);
	}

	/* There is no free entries in  HW FIFO */
	if (ploamFifoSize < GPON_PLOAM_MSG_WORDS)
		rcode = mvOnuGponMacMessageAdd(msgOnuId, msgId, msgData);
	else {
		/* HW FIFO has free entries. Verify whether SW FIFO is empty */
		if (mvOnuGponMacFifoEmpty())
			rcode = mvOnuGponMacTxPloamSend(msgOnuId, msgId, msgData);
		else {
			/* Add received PLOAM to SW FIFO */
			rcode = mvOnuGponMacMessageAdd(msgOnuId, msgId, msgData);

			/* Move up to GPON_MAX_US_MSG_MOVING (3) PLOAMs from SW to HW FIFO */
			moved_num = ploamFifoSize / GPON_PLOAM_MSG_WORDS;
			if (moved_num > GPON_MAX_US_MSG_MOVING)
				moved_num = GPON_MAX_US_MSG_MOVING;

			for (i = 0; i < moved_num; i++) {
				rcode = mvOnuGponMacFifoMsgRece((MV_U32**)&buffer, &bufferIndex);
				if (rcode == MV_EMPTY)
					break;

				rcode = mvOnuGponMacTxPloamSend(buffer[0], buffer[1], &buffer[2]);
				if (rcode != MV_OK)
					break;

				mvOnuGponMacFifoMsgFree(bufferIndex);
			}
		}
	}

	return rcode;
}

/*******************************************************************************
**
**  mvOnuGponMacMessageCleanSwFifo
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function clean the SW transmit message fifo
**
**  PARAMETERS:  None
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK, MV_FULL or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacMessageCleanSwFifo(void)
{
	MV_STATUS rcode;
	MV_U8     *buffer;
	MV_U32 bufferIndex;

	do {
		rcode = mvOnuGponMacFifoMsgRece((MV_U32**)&buffer, &bufferIndex);
		if (rcode == MV_EMPTY)
			break;

		mvOnuGponMacFifoMsgFree(bufferIndex);

	} while (1);

	return MV_OK;
}

/*******************************************************************************
**
**  mvOnuGponMacPreambleSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the onu overhead preamble bytes
**
**  PARAMETERS:  preamType1Val - preamble type 1 value
**               preamType1Cnt - preamble type 1 count
**               preamType2Val - preamble type 2 value
**               preamType2Cnt - preamble type 2 count
**               preamType3Val - preamble type 3 value
**               preamType3Cnt - preamble type 3 count
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacPreambleSet(MV_U32 preamType1Val,
				  MV_U32 preamType1Cnt,
				  MV_U32 preamType2Val,
				  MV_U32 preamType2Cnt,
				  MV_U32 preamType3Val,
				  MV_U32 preamType3Cnt)
{
	MV_STATUS rcode;

	rcode = mvOnuGponMacTxPreambleSet(ONU_GPON_PREM_TYPE_01, preamType1Val, preamType1Cnt);
	if (rcode != MV_OK)
		return rcode;

	rcode = mvOnuGponMacTxPreambleSet(ONU_GPON_PREM_TYPE_02, preamType2Val, preamType2Cnt);
	if (rcode != MV_OK)
		return rcode;

	rcode = mvOnuGponMacTxPreambleSet(ONU_GPON_PREM_TYPE_03, preamType3Val, preamType3Cnt);
	if (rcode != MV_OK)
		return rcode;

	return MV_OK;
}

/*******************************************************************************
**
**  mvOnuGponMacPreambleType3Set
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the onu overhead preamble type3
**
**  PARAMETERS:  preamType3Val - preamble type 3 value
**               preamType3Cnt - preamble type 3 count
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacPreambleType3Set(MV_U32 preamType3Val, MV_U32 preamType3Cnt)
{
	MV_STATUS rcode;

	rcode = mvOnuGponMacTxPreambleSet(ONU_GPON_PREM_TYPE_03, preamType3Val, preamType3Cnt);
	if (rcode != MV_OK)
		return rcode;

	return MV_OK;
}

/******************************************************************************/
/* ========================================================================== */
/*                      Upstream Messages MAC Section                         */
/* ========================================================================== */
/******************************************************************************/

/*******************************************************************************
**
**  mvOnuGponMacDgMessageSend
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function sends dying gasp message in the US direction
**
**  PARAMETERS:  MV_U8  onuId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacDgMessageSend(MV_U8 onuId)
{
	MV_U32 i;
	MV_STATUS rcode;
	MV_U8 data[12];

	for (i = 0; i < 3; i++) {
		rcode = mvOnuGponMacMessageSend(onuId, ONU_GPON_US_MSG_DYING_GASP, data);
		if (rcode != MV_OK) {
			mvOnuGponMacTxSwCountersAdd(ONU_GPON_US_MSG_DYING_GASP, MV_FALSE);
			return rcode;
		} else
			mvOnuGponMacTxSwCountersAdd(ONU_GPON_US_MSG_DYING_GASP, MV_TRUE);
	}
	return MV_OK;
}

/*******************************************************************************
**
**  mvOnuGponMacPasswordMessageSend
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function sends password message in the US direction
**
**  PARAMETERS:  MV_U8  onuId
**               MV_U8  *password
**               MV_U32 counter
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacPasswordMessageSend(MV_U8 onuId, MV_U8 *password, MV_U32 counter)
{
	MV_U32 time;
	MV_U32 i;
	MV_STATUS rcode;

	time = (counter > 3) ? 3 : counter;

	for (i = 0; i < time; i++) {
		rcode = mvOnuGponMacMessageSend(onuId, ONU_GPON_US_MSG_PASSWORD, password);
		if (rcode != MV_OK) {
			mvOnuGponMacTxSwCountersAdd(ONU_GPON_US_MSG_PASSWORD, MV_FALSE);
			return rcode;
		} else
			mvOnuGponMacTxSwCountersAdd(ONU_GPON_US_MSG_PASSWORD, MV_TRUE);
	}
	return MV_OK;
}

/*******************************************************************************
**
**  mvOnuGponMacReiMessageSend
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function sends dying gasp message in the US direction
**
**  PARAMETERS:  MV_U8 onuId
**               MV_U8 errorsCounter
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacReiMessageSend(MV_U8 onuId, MV_U32 errorsCounter)
{
	MV_STATUS rcode;
	static MV_U8 sequenceNumber = 0;
	MV_U8 data[12];

	data[0] = (MV_U8)((errorsCounter >> 24) & 0xFF);
	data[1] = (MV_U8)((errorsCounter >> 16) & 0xFF);
	data[2] = (MV_U8)((errorsCounter >> 8) & 0xFF);
	data[3] = (MV_U8)(errorsCounter & 0xFF);
	data[4] = sequenceNumber;

	sequenceNumber++;
	if (sequenceNumber > 0xF) sequenceNumber = 0;

	rcode = mvOnuGponMacMessageSend(onuId, ONU_GPON_US_MSG_REI, data);
	if (rcode != MV_OK) {
		mvOnuGponMacTxSwCountersAdd(ONU_GPON_US_MSG_REI, MV_FALSE);
		return rcode;
	} else
		mvOnuGponMacTxSwCountersAdd(ONU_GPON_US_MSG_REI, MV_TRUE);

	return rcode;
}

/*******************************************************************************
**
**  mvOnuGponMacAcknowledgeMessageSend
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function sends acknowledge message in the US direction
**
**  PARAMETERS:  MV_U8 onuId
**               MV_U8 errorsCounter
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacAcknowledgeMessageSend(MV_U8 onuId, MV_U8 dmId, MV_U8 *dmByte)
{
	MV_STATUS rcode;
	MV_U8 data[12];

	data[0] = dmId;

	data[1] = onuId;
	data[2] = dmId;
	data[3] = dmByte[0];
	data[4] = dmByte[1];
	data[5] = dmByte[2];
	data[6] = dmByte[3];
	data[7] = dmByte[4];
	data[8] = dmByte[5];
	data[9] = dmByte[6];

	rcode = mvOnuGponMacMessageSend(onuId, ONU_GPON_US_MSG_ACK, data);
	if (rcode != MV_OK) {
		mvOnuGponMacTxSwCountersAdd(ONU_GPON_US_MSG_ACK, MV_FALSE);
		return rcode;
	} else
		mvOnuGponMacTxSwCountersAdd(ONU_GPON_US_MSG_ACK, MV_TRUE);

	return rcode;
}

/*******************************************************************************
**
**  mvOnuGponMacEncryptionKeyMessageSend
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function sends encryption key message in the US direction
**
**  PARAMETERS:  MV_U8 onuId
**               MV_U8 KeyIndex
**               MV_U8 fragIndex
**               MV_U8 *key
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacEncryptionKeyMessageSend(MV_U8 onuId,
					       MV_U8 KeyIndex,
					       MV_U8 fragIndex,
					       MV_U8 *key)
{
	MV_STATUS rcode;
	MV_U8 data[12];

	data[0] = KeyIndex;
	data[1] = fragIndex;
	data[2] = key[0];
	data[3] = key[1];
	data[4] = key[2];
	data[5] = key[3];
	data[6] = key[4];
	data[7] = key[5];
	data[8] = key[6];
	data[9] = key[7];

	rcode = mvOnuGponMacMessageSend(onuId, ONU_GPON_US_MSG_ENCRYPT_KEY, data);
	if (rcode != MV_OK) {
		mvOnuGponMacTxSwCountersAdd(ONU_GPON_US_MSG_ENCRYPT_KEY, MV_FALSE);
		return rcode;
	} else
		mvOnuGponMacTxSwCountersAdd(ONU_GPON_US_MSG_ENCRYPT_KEY, MV_TRUE);

	return rcode;
}

/******************************************************************************/
/* ========================================================================== */
/*                              AES Support                                   */
/* ========================================================================== */
/******************************************************************************/

/*******************************************************************************
**
**  mvOnuGponMacAesInit
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function init onu AES
**
**  PARAMETERS:  None
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacAesInit(void)
{
	MV_U32 i;
	MV_STATUS rcode = MV_OK;

	for (i = 0 ; i < (GPON_ONU_MAX_GEM_PORTS / 32) ; i++) {
		onuGponPortAesStatusImage[i] = 0;
		rcode |= asicOntGlbRegWrite(mvAsicReg_GPON_GEM_AES_PID_TABLE, onuGponPortAesStatusImage[i], i);
	}

	return rcode;
}

/*******************************************************************************
**
**  mvOnuGponMacAesEnableSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function enable onu AES
**
**  PARAMETERS:  MV_BOOL enable
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacAesEnableSet(MV_BOOL enable)
{
	MV_STATUS rcode = MV_OK;
	MV_U32 enableBit;

	enableBit = (enable == MV_TRUE) ? 1 : 0;

	rcode = asicOntGlbRegWrite(mvAsicReg_GPON_GEM_CONFIG_AES, enableBit, 0);

	return rcode;
}

/*******************************************************************************
**
**  mvOnuGponMacAesPortIdSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function AES for port Id
**
**  PARAMETERS:  MV_U32 portId
**               MV_BOOL   status
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacAesPortIdSet(MV_U32 portId, MV_BOOL status)
{
	MV_STATUS rcode;
	MV_U32 group;
	MV_U32 groupOffset;
	MV_U32 regData;
	MV_U32 statusBit;

	group       = portId / 32;
	groupOffset = portId % 32;
	statusBit   = (status == MV_TRUE) ? 1 : 0;

	/* Get Image */
	regData = onuGponPortAesStatusImage[group];

	/* Update the bit */
	regData &= ~(1 << groupOffset);                 /* Clear the bit */
	regData |= (statusBit << groupOffset);          /* Set the status */
	onuGponPortAesStatusImage[group] = regData;     /* Update the Image */
	rcode = asicOntGlbRegWrite(mvAsicReg_GPON_GEM_AES_PID_TABLE, regData, group);

	return rcode;
}

/*******************************************************************************
**
**  mvOnuGponMacAesPortIdGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return port Id AES state
**
**  PARAMETERS:  MV_U32 portId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_BOOL mvOnuGponMacAesPortIdGet(MV_U32 portId)
{
	MV_U32 group;
	MV_U32 groupOffset;
	MV_U32 statusBit;
	MV_BOOL status;

	group       = portId / 32;
	groupOffset = portId % 32;
	statusBit = (onuGponPortAesStatusImage[group] >> groupOffset) & 1;

	status = (statusBit == 0) ? MV_FALSE : MV_TRUE;
	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacAesKeyShadowWrite
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function write AES key to GPON MAC
**
**  PARAMETERS:  MV_U8 *key
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacAesKeyShadowWrite(MV_U8 *key)
{
	MV_U32 i;
	MV_U32 regData;
	MV_STATUS rcode = MV_OK;

	for (i = 0 ; i < 4 ; i++) {
		regData = (((MV_U32)(key[i * 4]))     << 24) |
			  (((MV_U32)(key[i * 4 + 1])) << 16) |
			  (((MV_U32)(key[i * 4 + 2])) <<  8) |
			  ((MV_U32)(key[i * 4 + 3]));
		rcode |= asicOntGlbRegWrite(mvAsicReg_GPON_GEM_AES_SHADOW_KEY, regData, i);
	}

	return rcode;
}

/*******************************************************************************
**
**  mvOnuGponMacIpgValueSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function sets IPG value
**
**  PARAMETERS:  MV_U32 ipgVal
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacIpgValueSet(MV_U32 ipgVal)
{
	return asicOntGlbRegWrite(mvAsicReg_GPON_GEM_CFG_IPG_VALUE, ipgVal, 0);
}

/*******************************************************************************
**
**  mvOnuGponMacIpgValueGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function returns IPG value
**
**  PARAMETERS:  MV_U32 *ipgVal
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacIpgValueGet(MV_U32 *ipgVal)
{
	return asicOntGlbRegRead(mvAsicReg_GPON_GEM_CFG_IPG_VALUE, ipgVal, 0);
}

/*******************************************************************************
**
**  mvOnuGponMacIpgValidSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function enables IPG value settings
**
**  PARAMETERS:  MV_BOOL enable
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacIpgValidSet(MV_BOOL enable)
{
	MV_U32 enableBit = (enable == MV_TRUE) ? 1 : 0;

	return asicOntGlbRegWrite(mvAsicReg_GPON_GEM_CFG_IPG_VALID, enableBit, 0);
}

/*******************************************************************************
**
**  mvOnuGponMacIpgValidGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function get IPG value inabled/disabled status
**
**  PARAMETERS:  MV_BOOL *enable
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacIpgValidGet(MV_BOOL *enable)
{
	MV_STATUS rcode = MV_OK;
	MV_U32 enableBit;

	if (enable == 0)
		return MV_BAD_PARAM;

	rcode  = asicOntGlbRegRead(mvAsicReg_GPON_GEM_CFG_IPG_VALID, &enableBit, 0);
	if (rcode == MV_OK)
		*enable = (enableBit == 0) ? MV_FALSE : MV_TRUE;

	return rcode;
}

/*******************************************************************************
**
**  mvOnuGponMacRxCtrlCmdGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return Rx control command
**
**  PARAMETERS:  None
**
**  OUTPUTS:     MV_U32 *ctrlCmd
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxCtrlCmdGet(MV_U32 *ctrlCmd)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_GEM_RX_CTRL_Q_READ, ctrlCmd, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacRxCtrlDataUsedGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the Rx control data fifo size
**
**  PARAMETERS:  None
**
**  OUTPUTS:     MV_U32 *dataFifoSize
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxCtrlDataUsedGet(MV_U32 *dataFifoSize)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_GEM_RX_CTRL_DATA_USED, dataFifoSize, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacRxCtrlCmdUsedGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the Rx control cmd fifo size
**
**  PARAMETERS:  None
**
**  OUTPUTS:     MV_U32 *ctrlFifoSize
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxCtrlCmdUsedGet(MV_U32 *ctrlFifoSize)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_GEM_RX_CTRL_CMD_USED, ctrlFifoSize, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacRxCtrlHeadLenGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the header length of Rx control cmd or data packet
**
**  PARAMETERS:  None
**
**  OUTPUTS:     MV_U32 *headLen
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacRxCtrlHeadLenGet(MV_U32 *headLen)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_GEM_RX_CTRL_HQ_HEAD_LEN, headLen, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponMacGemInit
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function init onu Gem
**
**  PARAMETERS:  None
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacGemInit(void)
{
	MV_U32 i;
	MV_STATUS rcode = MV_OK;

	for (i = 0 ; i < (GPON_ONU_MAX_GEM_PORTS / 32) ; i++) {
		onuGponPortValidStatusImage[i] = 0;
		rcode |= asicOntGlbRegWrite(mvAsicReg_GPON_GEM_VALID_PID_TABLE, onuGponPortValidStatusImage[i], i);
	}

	return rcode;
}

/*******************************************************************************
**
**  mvOnuGponMacPortIdValidSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set valid for port Id
**
**  PARAMETERS:  MV_U32 portId
**               MV_BOOL   status
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacPortIdValidSet(MV_U32 portId, MV_BOOL status)
{
	MV_STATUS rcode;
	MV_U32 group;
	MV_U32 groupOffset;
	MV_U32 regData;
	MV_U32 statusBit;

	group       = portId / 32;
	groupOffset = portId % 32;
	statusBit   = (status == MV_TRUE) ? 1 : 0;

	/* Get Image */
	regData = onuGponPortValidStatusImage[group];

	/* Update the bit */
	regData &= ~(1 << groupOffset);                 /* Clear the bit */
	regData |= (statusBit << groupOffset);          /* Set the status */
	onuGponPortValidStatusImage[group] = regData;   /* Update the Image */
	rcode = asicOntGlbRegWrite(mvAsicReg_GPON_GEM_VALID_PID_TABLE, regData, group);

	return rcode;
}

/*******************************************************************************
**
**  mvOnuGponMacPortIdValidGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return port Id valid state
**
**  PARAMETERS:  MV_U32 portId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_BOOL mvOnuGponMacPortIdValidGet(MV_U32 portId)
{
	MV_U32 group;
	MV_U32 groupOffset;
	MV_U32 statusBit;
	MV_BOOL status;

	group       = portId / 32;
	groupOffset = portId % 32;
	statusBit   = (onuGponPortValidStatusImage[group] >> groupOffset) & 1;

	status = (statusBit == 0) ? MV_FALSE : MV_TRUE;

	return status;
}

/******************************************************************************/
/* ========================================================================== */
/*                        SW FIFO                                             */
/* ========================================================================== */
/******************************************************************************/

/*******************************************************************************
**
**  mvOnuGponMacFifoInit
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function inits SW FIFO
**
**  PARAMETERS:  None
**
**  OUTPUTS:     None
**
**  RETURNS:     None
**
*******************************************************************************/
void mvOnuGponMacFifoInit(void)
{
	fifoCtrl.fifoReadIndex    = 0;
	fifoCtrl.fifoWriteIndex   = 0;
	fifoCtrl.fifoReparoundNum = 0;

	memset(fifoCtrl.fifoData, 0, GPON_SW_FIFO_SIZE * sizeof(S_MacPloamMsg));
}

/*******************************************************************************
**
**  mvOnuGponMacFifoMsgAlloc
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function allocates a buffer from the FIFO
**
**  PARAMETERS:  None
**
**  OUTPUTS:     MV_U32 **buffer
**               MV_U32 *bufferIndex
**
**  RETURNS:     MV_OK or MV_FULL
**
*******************************************************************************/
static inline MV_STATUS mvOnuGponMacFifoMsgAlloc(MV_U32 **buffer,
						 MV_U32 *bufferIndex)
{
	MV_U32 nextWriteIndex;

	nextWriteIndex = fifoCtrl.fifoWriteIndex + 1;

	/* roll over */
	if (nextWriteIndex >= GPON_SW_FIFO_SIZE) {
		nextWriteIndex = 0;
		fifoCtrl.fifoReparoundNum++;
	}

	if (nextWriteIndex != fifoCtrl.fifoReadIndex) {
		*buffer      = (MV_U32*)(&(fifoCtrl.fifoData[nextWriteIndex]));
		*bufferIndex = nextWriteIndex;

		return MV_OK;
	}

	return MV_FULL;
}

/*******************************************************************************
**
**  mvOnuGponMacFifoMsgSend
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function sets the buffer (message) for transmission
**
**  PARAMETERS:  MV_U32 bufferIndex
**
**  OUTPUTS:     None
**
**  RETURNS:     None
**
*******************************************************************************/
static inline void  mvOnuGponMacFifoMsgSend(MV_U32 bufferIndex)
{
	fifoCtrl.fifoWriteIndex = bufferIndex;
	fifoMacMsgAdd++;
}

/*******************************************************************************
**
**  mvOnuGponMacFifoMsgRece
**  ____________________________________________ ________________________________
**
**  DESCRIPTION: The function receives a message  from the FIFO
**
**  PARAMETERS:  None
**
**  OUTPUTS:     MV_U32 **buffer
**               MV_U32 *bufferIndex
**
**  RETURNS:     MV_OK or MV_EMPTY
**
*******************************************************************************/
static inline MV_STATUS mvOnuGponMacFifoMsgRece(MV_U32 **buffer,
						MV_U32 *bufferIndex)
{
	MV_U32 nextReadIndex;

	if (fifoCtrl.fifoReadIndex != fifoCtrl.fifoWriteIndex) {
		nextReadIndex = fifoCtrl.fifoReadIndex + 1;

		/* roll over */
		if (nextReadIndex >= GPON_SW_FIFO_SIZE)
			nextReadIndex = 0;

		*buffer      = (MV_U32*)(&(fifoCtrl.fifoData[nextReadIndex]));
		*bufferIndex = nextReadIndex;

		return MV_OK;
	}

	return MV_EMPTY;
}

/*******************************************************************************
**
**  mvOnuGponMacFifoMsgFree
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function returns a buffer to the FIFO
**
**  PARAMETERS:  MV_U32 bufferIndex
**
**  OUTPUTS:     None
**
**  RETURNS:     None
**
*******************************************************************************/
static inline void  mvOnuGponMacFifoMsgFree(MV_U32 bufferIndex)
{
	fifoCtrl.fifoReadIndex = bufferIndex;
	fifoMacMsgFree++;
}

/*******************************************************************************
**
**  mvOnuGponMacFifoEmpty
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function returns an answer whether the FIFO is empty
**
**  PARAMETERS:  None
**
**  OUTPUTS:     None
**
**  RETURNS:     1 - SW FIFO is empty
**               0 - SW FIFO includes PLOAM messages
**
*******************************************************************************/
static inline int mvOnuGponMacFifoEmpty(void)
{
	return fifoCtrl.fifoReadIndex == fifoCtrl.fifoWriteIndex;
}

/*******************************************************************************
**
**  mvOnuGponMacFifoGetInfo
**  ____________________________________________________________________________
**
**  DESCRIPTION: Debug function
**
**  PARAMETERS:
**
**  OUTPUTS:     None
**
**  RETURNS:     None
**
*******************************************************************************/
void mvOnuGponMacFifoGetInfo(S_MacFifoCtrl **ptr, MV_32 *supported)
{
	*ptr = &fifoCtrl;
	*supported = fifoSupported;
}

/*******************************************************************************
**
**  mvOnuGponMacFifoGetStat
**  ____________________________________________________________________________
**
**  DESCRIPTION: Debug function
**
**  PARAMETERS:
**
**  OUTPUTS:     None
**
**  RETURNS:     None
**
*******************************************************************************/
void mvOnuGponMacFifoGetStat(MV_32 *callCnt, MV_32 *addSwCnt, MV_32 *freeSwCnt, MV_32 *sendHwCnt)
{
	*callCnt   = fifoMacMsgSend;
	*addSwCnt  = fifoMacMsgAdd;
	*freeSwCnt = fifoMacMsgFree;
	*sendHwCnt = fifoMacPloamSend;
}

/*******************************************************************************
**
**  mvOnuGponMacFifoAuditGetStat
**  ____________________________________________________________________________
**
**  DESCRIPTION: Debug function
**
**  PARAMETERS:
**
**  OUTPUTS:     None
**
**  RETURNS:     None
**
*******************************************************************************/
void mvOnuGponMacFifoAuditGetStat(MV_32 *enterCnt, MV_32 *actionCnt)
{
	*enterCnt  = fifoAuditEnter;
	*actionCnt = fifoAuditAction;
}

/*******************************************************************************
**
**  mvOnuGponMacFifoSupportSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: Debug function
**
**  PARAMETERS:
**
**  OUTPUTS:     None
**
**  RETURNS:     None
**
*******************************************************************************/
void mvOnuGponMacFifoSupportSet(MV_32 value)
{
	fifoSupported = value;
}

/*******************************************************************************
**
**  mvOnuGponMacFifoCountersSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: Debug function
**
**  PARAMETERS:
**
**  OUTPUTS:     None
**
**  RETURNS:     None
**
*******************************************************************************/
void mvOnuGponMacFifoCountersSet(MV_U32 value)
{
	fifoMacMsgSend   = value;
	fifoMacMsgAdd    = value;
	fifoMacMsgFree   = value;
	fifoMacPloamSend = value;
	fifoAuditEnter   = value;
	fifoAuditAction  = value;
}

/*******************************************************************************
**
**  mvOnuGponFifoAuditTimerExpireHndl
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function moves PLOAMs from SW to HW FIFO in case there is
**               no new DS PLOAMs
**
**  PARAMETERS:  None
**
**  OUTPUTS:     None
**
**  RETURNS:     None
**
*******************************************************************************/
void mvOnuGponFifoAuditTimerExpireHndl(void)
{
	MV_STATUS rc;
	MV_U32 hwFifoFreeEntries;
	MV_32 i;
	MV_U8     *buffer;
	MV_U32 bufferIndex;

	fifoAuditEnter++;

	if (fifoAuditEnter <= 0) {
		fifoAuditEnter  = 0;
		fifoAuditAction = 0;
	}

	/* Verify whether SW FIFO is empty */
	if (mvOnuGponMacFifoEmpty())
		return;

	/* Verify whether HW FIFO has free place */
	rc = mvOnuGponMacTxPloamDataFreeGet(&hwFifoFreeEntries);

	if ((rc != MV_OK) || (hwFifoFreeEntries < GPON_PLOAM_MSG_WORDS))
		return;

	/* Verify whether FIFO indexes have been changed from the last Audit cycle */
	if ((fifoReadIdx  == fifoCtrl.fifoReadIndex) &&
	    (fifoWriteIdx == fifoCtrl.fifoWriteIndex)) {
		fifoAuditAction++;

		/* Lock GPON interrupt */
		/*onuPonIrqLock(onuPonResourceTbl_s.onuPonIrqId);   */

		for (i = 0; i < GPON_MAX_US_MSG_TO_PROCESS; i++) {
			if (mvOnuGponMacFifoMsgRece((MV_U32**)&buffer, &bufferIndex) == MV_EMPTY)
				break;

			if (mvOnuGponMacTxPloamSend(buffer[0], buffer[1], &buffer[2]) != MV_OK)
				break;

			mvOnuGponMacFifoMsgFree(bufferIndex);
		}

		/* Unlock GPON interrupt */
		/*onuPonIrqUnlock(onuPonResourceTbl_s.onuPonIrqId); */
		fifoReadIdx = fifoCtrl.fifoReadIndex;
	} else {
		/* Save current values of the FIFO indexes */
		fifoReadIdx  = fifoCtrl.fifoReadIndex;
		fifoWriteIdx = fifoCtrl.fifoWriteIndex;
	}
}

/******************************************************************************/
/* ========================================================================== */
/*                         Low-Level MAC Section                              */
/* ========================================================================== */
/******************************************************************************/

/******************************************************************************/
/* ========================================================================== */
/*                            GENERAL Routines                                */
/* ========================================================================== */

/*******************************************************************************
**
**  mvOnuGponMacAsicVersionGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the onu asic version
**
**  PARAMETERS:  MV_U32 *asicVer
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacAsicVersionGet(MV_U32 *asicVer)
{
	MV_STATUS rcode;
	MV_U32 asicVerVal;

	rcode = asicOntGlbRegRead(mvAsicReg_GPON_GEN_MAC_VERSION, &asicVerVal, 0);
	if (rcode != MV_OK)
		return rcode;
	else
		*asicVer = asicVerVal;

	return MV_OK;
}

/*******************************************************************************
**
**  mvOnuGponMacMicroSecCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return the onu micro sec counter
**
**  PARAMETERS:  MV_U32 *microSec
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacMicroSecCounterGet(MV_U32 *microSec)
{
	MV_STATUS rcode;
	MV_U32 asicVerVal;

	rcode = asicOntGlbRegRead(mvAsicReg_GPON_GEN_MICRO_SEC_CNT, &asicVerVal, 0);
	if (rcode != MV_OK) return
		rcode;
	else
		*microSec = asicVerVal;

	return MV_OK;
}

/*******************************************************************************
**
**  mvOnuGponMacDyingGaspConfigSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function enables and sets GPIO and DeBouncer registers to allow
**               Dying Gasp interrupt.
**
**  PARAMETERS:  id- Dying Gasp 0 or 1.
**               disableEnable 0 Disable 1 Enable.
**               threshold - The number of sampling values in the debouncer before an input is asserted.
**                           1 = 6.4n sec.
**               polarity - 1= active on high, 3 = active on low.
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuGponMacDyingGaspConfigSet(MV_U32 id,
					 MV_U32 disableEnable,
					 MV_U32 polarity,
					 MV_U32 threshold)
{
	return MV_OK;
}

/*******************************************************************************
**
**  mvOnuGponMacXvrReset
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function execute xvr reset sequence
**
**  PARAMETERS:  MV_U32 xvrType
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK
**
*******************************************************************************/
MV_STATUS mvOnuGponMacXvrReset(MV_U32 xvrType)
{
	return MV_OK;
}

/*******************************************************************************
**
**  mvOnuGponMacXvrActivate
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function activates xvr
**
**  PARAMETERS:  None
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK
**
*******************************************************************************/
MV_STATUS mvOnuGponMacXvrActivate(void)
{
	return MV_OK;
}

/*******************************************************************************
**
**  mvOnuGponMacTxSwCountersAdd
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function updates Tx Ploam message counters
**
**  PARAMETERS:  MV_U8   msgId
**               MV_BOOL sucsses
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxSwCountersAdd(MV_U8 msgId, MV_BOOL sucsses)
{
	mvOnuGponMacPloamCnt.txMsgTotalPloamCounter++;

	if (sucsses == MV_FALSE)
		mvOnuGponMacPloamCnt.txErrMsgIdPloamCounter[msgId]++;
	else
		mvOnuGponMacPloamCnt.txMsgIdPloamCounter[msgId]++;

	return MV_OK;
}

/*******************************************************************************
**
**  ponOnuMacTxSwCountersGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function updates Tx Ploam message counters
**
**  PARAMETERS:  MV_U8   msgId
**               MV_BOOL sucsses
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK
**
*******************************************************************************/
MV_STATUS mvOnuGponMacTxSwCountersGet(void* txPloamPm)
{
	MV_U32 numOfMsgIdIndex;
	S_MacTxPloamPm *outTxPloamPm = (S_MacTxPloamPm*)txPloamPm;

	outTxPloamPm->txMsgTotalPloamCounter = mvOnuGponMacPloamCnt.txMsgTotalPloamCounter;

	for (numOfMsgIdIndex = ONU_GPON_US_MSG_SN_ONU;
	     numOfMsgIdIndex <= ONU_GPON_US_MSG_ACK;
	     numOfMsgIdIndex++) {
		outTxPloamPm->txMsgIdPloamCounter[numOfMsgIdIndex]    = mvOnuGponMacPloamCnt.txMsgIdPloamCounter[numOfMsgIdIndex];
		outTxPloamPm->txErrMsgIdPloamCounter[numOfMsgIdIndex] = mvOnuGponMacPloamCnt.txErrMsgIdPloamCounter[numOfMsgIdIndex];
	}

	return MV_OK;
}

/*******************************************************************************
**
**  mvOnuGponHighPriTxEnableSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function sets high priority TX register
**
**  PARAMETERS:  MV_U32 txEnable
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuGponHighPriTxEnableSet(MV_U32 txEnable)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_GPON_TX_HIGH_PRI_CONFIG_EN, txEnable, 0);

	/* clear high priority bitmap flag in case disable this feature */
	if (!txEnable)
		status = asicOntGlbRegWrite(mvAsicReg_GPON_TX_HIGH_PRI_VALID_BITMAP, 0, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponHighPriTxValidBmSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function sets high priority TX valid bitmap
**
**  PARAMETERS:  MV_U32 validBm
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuGponHighPriTxValidBmSet(MV_U32 validBm)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_GPON_TX_HIGH_PRI_VALID_BITMAP, validBm, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponHighPriTxValidBmGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function gets high priority TX valid bitmap
**
**  PARAMETERS:  MV_U32 *validBm
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuGponHighPriTxValidBmGet(MV_U32 *validBm)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_TX_HIGH_PRI_VALID_BITMAP, validBm, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponHighPriTxMap0Set
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function sets high priority TX map 0
**
**  PARAMETERS:  MV_U32 map0
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuGponHighPriTxMap0Set(MV_U32 map0)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_GPON_TX_HIGH_PRI_MAP_0, map0, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponHighPriTxMap0Get
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function gets high priority TX map 0
**
**  PARAMETERS:  MV_U32 *map0
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuGponHighPriTxMap0Get(MV_U32 *map0)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_TX_HIGH_PRI_MAP_0, map0, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponHighPriTxMap1Set
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function sets high priority TX map 1
**
**  PARAMETERS:  MV_U32 map1
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuGponHighPriTxMap1Set(MV_U32 map1)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_GPON_TX_HIGH_PRI_MAP_1, map1, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuGponHighPriTxMap1Get
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function gets high priority TX map 1
**
**  PARAMETERS:  MV_U32 *map1
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuGponHighPriTxMap1Get(MV_U32 *map1)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_GPON_TX_HIGH_PRI_MAP_1, map1, 0);

	return status;
}

/******************************************************************************/
/******************************************************************************/
/* ========================================================================== */
/* ========================================================================== */
/* ==                                                                      == */
/* ==           =========   =========   =========   ===       ==           == */
/* ==           =========   =========   =========   ====      ==           == */
/* ==           ==          ==     ==   ==     ==   == ==     ==           == */
/* ==           ==          ==     ==   ==     ==   ==  ==    ==           == */
/* ==           =========   =========   ==     ==   ==   ==   ==           == */
/* ==           =========   =========   ==     ==   ==    ==  ==           == */
/* ==           ==          ==          ==     ==   ==     == ==           == */
/* ==           ==          ==          ==     ==   ==      ====           == */
/* ==           =========   ==          =========   ==       ===           == */
/* ==           =========   ==          =========   ==        ==           == */
/* ==                                                                      == */
/* ========================================================================== */
/* ========================================================================== */
/******************************************************************************/
/******************************************************************************/

/* ========================================================================== */
/*                        Interrupt Functions Section                         */
/* ========================================================================== */

/*******************************************************************************
**
**  mvOnuEponMacPonInterruptGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return interrupt status
**
**  PARAMETERS:  MV_U32 *interrupt
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacPonInterruptGet(MV_U32 *interrupt)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_EPON_INTR_REG, interrupt, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacPonInterruptrMaskGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return interrupt mask
**
**  PARAMETERS:  MV_U32 *mask
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacPonInterruptMaskGet(MV_U32 *mask)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_EPON_INTR_MASK, mask, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacPonInterruptrMaskSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set interrupt mask
**
**  PARAMETERS:  MV_U32 mask
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacPonInterruptMaskSet(MV_U32 mask)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_INTR_MASK, mask, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacPonDbrLlidInterruptGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return DBR LLID interrupt status
**
**  PARAMETERS:  MV_U32 llid
**               MV_U32 *interrupt
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacPonDbrLlidInterruptGet(MV_U32 llid, MV_U32 *interrupt)
{
	if ((interrupt == 0) || (llid >= EPON_MAX_MAC_NUM))
		return MV_ERROR;

	return asicOntGlbRegRead(mvAsicReg_EPON_DBR_LLID_INT, interrupt, llid);
}

/*******************************************************************************
**
**  mvOnuEponMacPonDbrInterruptGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return DBR interrupt status
**
**  PARAMETERS:  MV_U32 *interrupt
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacPonDbrInterruptGet(MV_U32 *interrupt)
{
	if (interrupt == 0)
		return MV_ERROR;

	return asicOntGlbRegRead(mvAsicReg_EPON_DBR_INT, interrupt, 0);
}

/*******************************************************************************
**
**  mvOnuEponMacPonDbrInterruptMaskGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return DBR interrupt status
**
**  PARAMETERS:  MV_U32 *interrupt
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacPonDbrInterruptMaskGet(MV_U32 *mask)
{
	return asicOntGlbRegRead(mvAsicReg_EPON_DBR_INT_LLID_MASK, mask, 0);
}

/*******************************************************************************
**
**  mvOnuEponMacPonDbrInterruptMaskSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set interrupt mask
**
**  PARAMETERS:  MV_U32 mask
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacPonDbrInterruptMaskSet(MV_U32 mask)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_DBR_INT_LLID_MASK, mask, 0);
	if (mask != 0)
		status |= asicOntGlbRegWrite(mvAsicReg_EPON_DBR_INT_MASK, 1, 0);
	else
		status |= asicOntGlbRegWrite(mvAsicReg_EPON_DBR_INT_MASK, 0, 0);

	return status;
}

/* ========================================================================== */
/*                        General Functions Section                           */
/* ========================================================================== */

/*******************************************************************************
**
**  mvOnuEponMacVersionGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return mac version
**
**  PARAMETERS:  MV_U32 *version
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacVersionGet(MV_U32 *version)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_EPON_GEN_MAC_VERSION_ADDR, version, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacPcsRxEnableSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function sets PcsRxEnable register
**
**  PARAMETERS:  MV_U32 rxEnable
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacPcsRxEnableSet(MV_U32 rxEnable)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_PCS_CONFIGURATION_RX_ENABLE, rxEnable, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacPcsTxEnableSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function sets PcsTxEnable register
**
**  PARAMETERS:  MV_U32 txEnable
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacPcsTxEnableSet(MV_U32 txEnable)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_PCS_CONFIGURATION_TX_ENABLE, txEnable, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacOnuEnableSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu Rx & Tx enable
**
**  PARAMETERS:  MV_U32 rxEnable
**               MV_U32 txEnable
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacOnuEnableSet(MV_U32 rxEnable, MV_U32 txEnable)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_PCS_CONFIGURATION_RX_ENABLE, rxEnable, 0);
	status |= asicOntGlbRegWrite(mvAsicReg_EPON_GEN_ONT_RX_ENABLE, rxEnable, 0);

	status |= asicOntGlbRegWrite(mvAsicReg_EPON_PCS_CONFIGURATION_TX_ENABLE, txEnable, 0);
	status |= asicOntGlbRegWrite(mvAsicReg_EPON_GEN_ONT_TX_ENABLE, txEnable, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacOnuRxEnableSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu Rx & Tx enable
**
**  PARAMETERS:  MV_U32 rxEnable
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacOnuRxEnableSet(MV_U32 rxEnable)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_PCS_CONFIGURATION_RX_ENABLE, rxEnable, 0);

	status |= asicOntGlbRegWrite(mvAsicReg_EPON_GEN_ONT_RX_ENABLE, rxEnable, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacOnuTxEnableSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu Rx & Tx enable
**
**  PARAMETERS:  MV_U32 txEnable
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacOnuTxEnableSet(MV_U32 txEnable, MV_U32 macId)
{
	MV_STATUS status;
	MV_U32 currOnuTxEnable;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status  = asicOntGlbRegRead(mvAsicReg_EPON_GEN_ONT_TX_ENABLE, &currOnuTxEnable, 0);

	if (txEnable == 0)
		currOnuTxEnable &= ~(1 << macId);
	else
		currOnuTxEnable |=  (1 << macId);

	status |= asicOntGlbRegWrite(mvAsicReg_EPON_GEN_ONT_TX_ENABLE, currOnuTxEnable, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacOnuStateSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu state
**
**  PARAMETERS:  MV_U32 onuState
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacOnuStateSet(MV_U32 onuState, MV_U32 macId)
{
	MV_STATUS status;
	MV_U32 currOnuState;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status  = asicOntGlbRegRead(mvAsicReg_EPON_GEN_ONT_STATE_REG, &currOnuState, 0);

	if (onuState == 0)
		currOnuState &= ~(1 << macId);
	else
		currOnuState |=  (1 << macId);

	status |= asicOntGlbRegWrite(mvAsicReg_EPON_GEN_ONT_STATE_REG, currOnuState, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacOnuStateSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu state
**
**  PARAMETERS:  MV_U32 *onuState
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacOnuStateGet(MV_U32 *onuState, MV_U32 macId)
{
	MV_STATUS status;
	MV_U32 currOnuState;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status  = asicOntGlbRegRead(mvAsicReg_EPON_GEN_ONT_STATE_REG, &currOnuState, 0);

	*onuState = ((currOnuState >> macId) & (0x1));

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacOnuRegAutoUpdateStateSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu register auto update params
**
**  PARAMETERS:  MV_U32 value
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacOnuRegAutoUpdateStateSet(MV_U32 value)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_ONT_STATE_REG_AUTO_EN, value, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacOnuReRegAutoUpdateStateSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu re-register auto update params
**
**  PARAMETERS:  MV_U32 value
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacOnuReRegAutoUpdateStateSet(MV_U32 value)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_ONT_STATE_REREG_AUTO_EN, value, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacOnuDeRegAutoUpdateStateSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu de-register auto update params
**
**  PARAMETERS:  MV_U32 value
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacOnuDeRegAutoUpdateStateSet(MV_U32 value)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_ONT_STATE_DEREG_AUTO_EN, value, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacOnuNackAutoUpdateStateSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu nack auto update params
**
**  PARAMETERS:  MV_U32 value
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacOnuNackAutoUpdateStateSet(MV_U32 value)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_ONT_STATE_NACK_AUTO_EN, value, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenTqSizeSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu TQ size
**
**  PARAMETERS:  MV_U32 tqRxSize
**               MV_U32 tqTxSize
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenTqSizeSet(MV_U32 tqRxSize, MV_U32 tqTxSize)
{
	MV_STATUS status;
	MV_U32 tqSize;

	tqSize = ((tqTxSize & 0xF) << 4) |
		 (tqRxSize & 0xF);

	status = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_TQ_SIZE, tqSize, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenLaserParamSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu laser params
**
**  PARAMETERS:  MV_U32 onOffTime
**               MV_U32 onTime
**               MV_U32 offTime
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenLaserParamSet(MV_U32 onOffTime, MV_U32 onTime, MV_U32 offTime)
{
	MV_STATUS status;
	MV_U32 lazerTime;

	lazerTime =  ((onOffTime & 0xFF) << 16) |
		    ((offTime & 0xFF) << 8)  |
		    (onTime & 0xFF);

	status = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_LASER_PARAM, lazerTime, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenLaserParamGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return laser params
**
**  PARAMETERS:  MV_U32 *onOffTime
**               MV_U32 *onTime
**               MV_U32 *offTime
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenLaserParamGet(MV_U32 *onOffTime, MV_U32 *onTime, MV_U32 *offTime)
{
	MV_STATUS status;
	MV_U32 lazerTime;

	status = asicOntGlbRegRead(mvAsicReg_EPON_GEN_LASER_PARAM, &lazerTime, 0);

	*onTime    =  (lazerTime        & 0xFF);
	*offTime   = ((lazerTime >>  8) & 0xFF);
	*onOffTime = ((lazerTime >> 16) & 0xFF);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenProcessingTimeSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu processing time
**
**  PARAMETERS:  MV_U32 packetTailGuard
**               MV_U32 tailGuard
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenTailGuardSet(MV_U32 packetTailGuard, MV_U32 tailGuard)
{
	MV_STATUS status;
	MV_U32 guard;

	guard =  ((packetTailGuard  & 0xFF) << 8)  |
		(tailGuard  & 0xFF);

	status = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_TAIL_GUARD, guard, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenSyncTimeSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu sync time
**
**  PARAMETERS:  MV_U32 syncTime
**               MV_U32 addSyncTime
**               MV_U32 forceSwUpdate
**               MV_U32 disAutoUpdate
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenSyncTimeSet(MV_U32 syncTime,
				     MV_U32 addSyncTime,
				     MV_U32 forceSwUpdate,
				     MV_U32 disGateAutoUpdate,
				     MV_U32 disDiscoveryAutoUpdate)
{
	MV_STATUS status;
	MV_U32 sync;

	sync =         ((addSyncTime   & 0x00FF) << 24) |
	       ((disDiscoveryAutoUpdate & 0x0001) << 18) |
	       ((disGateAutoUpdate & 0x0001) << 17) |
	       ((forceSwUpdate  & 0x0001) << 16) |
	       (syncTime  & 0xFFFF);

	status = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_SYNC_TIME, sync, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenSyncTimeGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return onu sync time
**
**  PARAMETERS:  MV_U32 *syncTime
**               MV_U32 *addSyncTime
**               MV_U32 *forceSwUpdate
**               MV_U32 *disAutoUpdate
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenSyncTimeGet(MV_U32 *syncTime,
				     MV_U32 *addSyncTime,
				     MV_U32 *forceSwUpdate,
				     MV_U32 *disGateAutoUpdate,
				     MV_U32 *disDiscoveryAutoUpdate)
{
	MV_STATUS status;
	MV_U32 sync;

	status = asicOntGlbRegRead(mvAsicReg_EPON_GEN_SYNC_TIME, &sync, 0);

	*addSyncTime            = ((sync >> 24) & 0x00FF);
	*disDiscoveryAutoUpdate = ((sync >> 18) & 0x0001);
	*disGateAutoUpdate      = ((sync >> 17) & 0x0001);
	*forceSwUpdate          = ((sync >> 16) & 0x0001);
	*syncTime               = ((sync)       & 0xFFFF);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenBcastAddrSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu mac broadcast address
**
**  PARAMETERS:  MV_U32 bcastMacAddrHigh
**               MV_U32 bcastMacAddrLow
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenBcastAddrSet(MV_U32 bcastMacAddrHigh, MV_U32 bcastMacAddrLow)
{
	MV_STATUS status;

	status  = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_BROADCAST_ADDR_LOW,  bcastMacAddrLow, 0);
	status |= asicOntGlbRegWrite(mvAsicReg_EPON_GEN_BROADCAST_ADDR_HIGH, bcastMacAddrHigh & 0xFFFF, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenSyncTimeSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu configuration
**
**  PARAMETERS:  MV_U32 rxFec
**               MV_U32 txFec
**               MV_U32 reportAutoResponse
**               MV_U32 ackAutoResponse
**               MV_U32 requestAutoResponse
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenOnuConfigSet(MV_U32 rxPcsFecEn,
				      MV_U32 txPcsFecEn,
				      MV_U32 reportAutoResponse,
				      MV_U32 ackAutoResponse,
				      MV_U32 requestAutoResponse,
				      MV_U32 txFecEn)
{
	MV_STATUS status;
	MV_U32 config;

	config =             ((txFecEn & 0x00FF) << 8) |
		 ((requestAutoResponse & 0x0001) << 6) |
		 ((ackAutoResponse & 0x0001) << 5) |
		 ((reportAutoResponse & 0x0001) << 4) |
		 ((rxPcsFecEn & 0x0001) << 1) |
		 (txPcsFecEn & 0x0001);

	status = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_CONFIGURATION, config, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenOnuConfigAutoReportSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu configuration auto report
**
**  PARAMETERS:  MV_U32 reportAutoResponse
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenOnuConfigAutoReportSet(MV_U32 reportAutoResponse)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_CONFIGURATION_AUTO_REPORT, reportAutoResponse, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenOnuConfigAutoAckSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu configuration auto ack
**
**  PARAMETERS:  MV_U32 ackAutoResponse
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenOnuConfigAutoAckSet(MV_U32 ackAutoResponse)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_CONFIGURATION_AUTO_ACK, ackAutoResponse, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenOnuConfigAutoRequestSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu configuration auto response
**
**  PARAMETERS:  MV_U32 requestAutoResponse
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenOnuConfigAutoRequestSet(MV_U32 requestAutoResponse)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_CONFIGURATION_AUTO_REQUEST, requestAutoResponse, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenSldSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu SLD
**
**  PARAMETERS:  MV_U32 sld
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenSldSet(MV_U32 sld)
{
	MV_STATUS status;

	status  = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_SLD, sld, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenCtrlTypeSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set control type (Ether type)
**
**  PARAMETERS:  MV_U32 sld
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenCtrlTypeSet(MV_U32 type)
{
	MV_STATUS status;

	status  = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_MAC_CONTROL_TYPE, type, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenLocalTimeStampSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu local timestamp
**
**  PARAMETERS:  MV_U32 timeStamp
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenLocalTimeStampSet(MV_U32 timeStamp)
{
	MV_STATUS status;

	status  = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_LOCAL_TIMESTAMP, timeStamp, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenLocalTimeStampGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return onu local timestamp
**
**  PARAMETERS:  MV_U32 *timeStamp
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenLocalTimeStampGet(MV_U32 *timeStamp)
{
	MV_STATUS status;

	status  = asicOntGlbRegRead(mvAsicReg_EPON_GEN_LOCAL_TIMESTAMP, timeStamp, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenPendGrantSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu pending grants
**
**  PARAMETERS:  MV_U32 grant
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenPendGrantSet(MV_U32 grant)
{
	MV_STATUS status;

	status  = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_NUM_OF_PENDING_GRANTS, grant, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenPendGrantSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return onu SGL status
**
**  PARAMETERS:  MV_U32 *status
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenSglStatusGet(MV_U32 *sglStatus)
{
	MV_STATUS status;

	status  = asicOntGlbRegRead(mvAsicReg_EPON_GEN_SGL_STATUS, sglStatus, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenPonPhyDbgSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu PON phy debug
**
**  PARAMETERS:  MV_U32 ponDebug
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenPonPhyDbgSet(MV_U32 ponDebug)
{
	MV_STATUS status;

	status  = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_PON_PHY_DEBUG, ponDebug, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenPonPhyDbgGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return onu PON phy debug
**
**  PARAMETERS:  MV_U32 *ponDebug
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenPonPhyDbgGet(MV_U32 *ponDebug)
{
	MV_STATUS status;

	status  = asicOntGlbRegRead(mvAsicReg_EPON_GEN_PON_PHY_DEBUG, ponDebug, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenSglSwReset
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return onu SGL sw reset
**
**  PARAMETERS:  MV_U32 swReset
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenSglSwReset(MV_U32 swReset)
{
	MV_STATUS status;

	status  = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_SGL_SW_RESET, swReset, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenSglConfig
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu SGL configuration
**
**  PARAMETERS:  startThreshold
**               dataAlignment
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenSglConfig(MV_U32 startThreshold,
				   MV_U32 dataAlignment)
{
	MV_STATUS status;
	MV_U32 config;

	config = ((dataAlignment  & 0x0001) << 4) |
		 (startThreshold & 0x000F);

	status  = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_SGL_CONFIG, config, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenSglDebounceConfig
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu SGL Debounce configuration
**
**  PARAMETERS:  MV_U32 polarity
**               MV_U32 threshold
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenSglDebounceConfig(MV_U32 polarity,
					   MV_U32 threshold)
{
	MV_STATUS status;
	MV_U32 config;

	config = ((polarity  & 0x0001) << 16) |
		 (threshold & 0xFFFF);

	status  = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_SGL_DEBOUNCE_CONFIG, config, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenSglDebounceEnable
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu SGL Debounce enable
**
**  PARAMETERS:  MV_U32 enable
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenSglDebounceEnable(MV_U32 enable)
{
	MV_STATUS status;

	status  = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_SGL_DEBOUNCE_ENABLE, enable, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenChurningKeySet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu churning key
**
**  PARAMETERS:  MV_U32 enable
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenChurningKeySet(MV_U32 key)
{
	MV_STATUS status;

	status  = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_TCH_CHURNING_KEY, key, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenOpcodeSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu additional opcode
**
**  PARAMETERS:  MV_U32 opcodeType
**               MV_U32 value
**               MV_U32 index
**               MV_U32 timestamp
**               MV_U32 valid
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenOpcodeSet(MV_U32 opcodeType,
				   MV_U32 value,
				   MV_U32 index,
				   MV_U32 timestamp,
				   MV_U32 valid)
{
	MV_STATUS status = MV_OK;
	MV_U32 opcode;

	opcode =     ((valid & 0x0001) << 20) |
		 ((timestamp & 0x0001) << 19) |
		 ((index & 0x0007) << 16) |
		 (value & 0xFFFF);

	if (opcodeType == ADDITIONAL_OPCODE_1)
		status = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_ADDITIONAL_OPCODE1, opcode, 0);
	else if (opcodeType == ADDITIONAL_OPCODE_2)
		status = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_ADDITIONAL_OPCODE2, opcode, 0);
	else if (opcodeType == ADDITIONAL_OPCODE_3)
		status = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_ADDITIONAL_OPCODE3, opcode, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenTimestampConfig
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu timestamp configuration
**
**  PARAMETERS:  MV_U32 threshold
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenTimestampConfig(MV_U32 gpmAddSyncTimeToTimestamp,
					 MV_U32 gpmAimestampOffset,
					 MV_U32 txmAddSyncTimeToTimestamp,
					 MV_U32 txmUseTimestampImage,
					 MV_U32 txmTimestampOffset)
{
	MV_STATUS status;
	MV_U32 timestampConfig;

	timestampConfig = ((gpmAddSyncTimeToTimestamp & 0x0001) << 31) |
			  ((gpmAimestampOffset & 0x07FF) << 16) |
			  ((txmAddSyncTimeToTimestamp & 0x0001) << 15) |
			  ((txmUseTimestampImage & 0x0001) << 14) |
			  (txmTimestampOffset & 0x07FF);

	status  = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_TIMESTAMP_CONFIG, timestampConfig, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenRxTimestampConfigSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu RX timestamp configuration
**
**  PARAMETERS:
**		MV_U32 rxpTimestampSelect	- 0 or 1
**		MV_U32 rxpTimestampSign		- 0 or 1
**		MV_U32 rxpTimestampOffset	- 0 to 0x7FF
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenRxTimestampConfigSet(MV_U32 rxpTimestampSelect,
					      MV_U32 rxpTimestampSign,
					      MV_U32 rxpTimestampOffset)
{
	MV_U32 timestampConfig;

	timestampConfig = ((rxpTimestampSelect & 0x1) << 15) |
			  ((rxpTimestampSign & 0x1) << 11) |
			  (rxpTimestampOffset & 0x07FF);

	return asicOntGlbRegWrite(mvAsicReg_EPON_GEN_ONT_RX_TIMESTAMP_CONFIG, timestampConfig, 0);
}

/*******************************************************************************
**
**  mvOnuEponMacGenRxTimestampConfigGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function reads onu RX timestamp configuration
**
**  PARAMETERS:
**		MV_U32 *rxpTimestampSelect	- 0 or 1
**		MV_U32 *rxpTimestampSign	- 0 or 1
**		MV_U32 *rxpTimestampOffset	- 0 to 0x7FF
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenRxTimestampConfigGet(MV_U32 *rxpTimestampSelect,
					      MV_U32 *rxpTimestampSign,
					      MV_U32 *rxpTimestampOffset)
{
	MV_STATUS status;
	MV_U32 timestampConfig;

	if ((rxpTimestampSelect == 0) || (rxpTimestampSign == 0) || (rxpTimestampOffset == 0))
		return MV_BAD_PARAM;

	status = asicOntGlbRegRead(mvAsicReg_EPON_GEN_ONT_RX_TIMESTAMP_CONFIG, &timestampConfig, 0);

	if (status == MV_OK) {
		*rxpTimestampSelect = (timestampConfig >> 15) & 0x1;
		*rxpTimestampSign = (timestampConfig >> 11) & 0x1;
		*rxpTimestampOffset = timestampConfig & 0x7FF;
	}

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenRegPacketPaddingGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function gets onu packet padding
**
**  PARAMETERS:
**		MV_U32 regAckPadding	- padding value for Ack (0 - 0xFF)
**		MV_U32 regReqPadding	- padding value for Request (0 - 0xFF)
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenRegPacketPaddingGet(MV_U32 *regAckPadding, MV_U32 *regReqPadding)
{
	MV_STATUS status;
	MV_U32 paddingConfig;

	if ((regAckPadding == 0) || (regReqPadding == 0))
		return MV_BAD_PARAM;

	status = asicOntGlbRegRead(mvAsicReg_EPON_GEN_ONT_REGISTER_PACKET_PADDING, &paddingConfig, 0);

	if (status == MV_OK) {
		*regAckPadding = (paddingConfig >> 8) & 0xFF;
		*regReqPadding = paddingConfig & 0xFF;
	}

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenRegPacketPaddingSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function sets onu packet padding
**
**  PARAMETERS:
**		MV_U32 *regAckPadding	- padding value for Ack
**		MV_U32 *regReqPadding	- padding value for Request
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenRegPacketPaddingSet(MV_U32 regAckPadding, MV_U32 regReqPadding)
{
	MV_U32 paddingConfig;

	paddingConfig = ((regAckPadding & 0xFF) << 8) | (regReqPadding & 0xFF);

	return asicOntGlbRegWrite(mvAsicReg_EPON_GEN_ONT_REGISTER_PACKET_PADDING, paddingConfig, 0);
}

/*******************************************************************************
**
**  mvOnuEponMacGenDriftThresholdSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu drift threshold
**
**  PARAMETERS:  MV_U32 threshold
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenDriftThresholdSet(MV_U32 threshold)
{
	MV_STATUS status;

	status  = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_DRIFT_THRESHOLD, threshold, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenTimeStampForIntrSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu timestamp for interrupt
**
**  PARAMETERS:  MV_U32 timestamp
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenTimeStampForIntrSet(MV_U32 timestamp)
{
	MV_STATUS status;

	status  = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_TIMESTAMP_VAL_FOR_INTR, timestamp, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenTimeStampForIntrGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function get onu timestamp for interrupt
**
**  PARAMETERS:  MV_U32 *timestamp
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenTimeStampForIntrGet(MV_U32 *timestamp)
{
	MV_STATUS status;

	status  = asicOntGlbRegRead(mvAsicReg_EPON_GEN_TIMESTAMP_VAL_FOR_INTR, timestamp, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenUcastAddrSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu mac address
**
**  PARAMETERS:  MV_U32 ucastMacAddrHigh
**               MV_U32 ucastMacAddrLow
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenUcastAddrSet(MV_U32 ucastMacAddrHigh, MV_U32 ucastMacAddrLow, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status  = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_MAC_ADDR_LOW,  ucastMacAddrLow, macId);
	status |= asicOntGlbRegWrite(mvAsicReg_EPON_GEN_MAC_ADDR_HIGH, ucastMacAddrHigh & 0xFFFF, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenUtmTcPeriodSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu TC period
**
**  PARAMETERS:  MV_U32 tcPeriod
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenUtmTcPeriodSet(MV_U32 tcPeriod)
{
	MV_STATUS status;

	status  = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_UTM_TC_PERIOD, tcPeriod, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenUtmTcPeriodEnSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu TC period enable
**
**  PARAMETERS:  MV_U32 tcPeriodEnable
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenUtmTcPeriodEnSet(MV_U32 tcPeriodEnable)
{
	MV_STATUS status;

	status  = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_UTM_TC_PERIOD_VALID, tcPeriodEnable, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenGemIpgValueSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu IPG value
**
**  PARAMETERS:  MV_U32 ipgVal
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenGemIpgValueSet(MV_U32 ipgVal)
{
	MV_STATUS status;

	status  = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_GEM_IPG_VAL, ipgVal, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenGemIpgValueEnSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu IPG value enable
**
**  PARAMETERS:  MV_U32 ipgValEn
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenGemIpgValueEnSet(MV_U32 ipgValEn)
{
	MV_STATUS status;

	status  = asicOntGlbRegWrite(mvAsicReg_EPON_GEN_GEM_IPG_VAL_VALID, ipgValEn, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenUtmActTxBitmapSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu active TX bitmap
**
**  PARAMETERS:  MV_U32 txBitmap
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenUtmActTxBitmapSet(MV_U32 txBitmap)
{
	return asicOntGlbRegWrite(mvAsicReg_EPON_GEN_ACT_TX_BITMAP, txBitmap, 0);
}

/*******************************************************************************
**
**  mvOnuEponMacGenUtmActTxBitmapEnSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu active TX bitmap enable
**
**  PARAMETERS:  MV_U32 txBitmapEnable
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenUtmActTxBitmapEnSet(MV_U32 txBitmapEnable)
{
	return asicOntGlbRegWrite(mvAsicReg_EPON_GEN_ACT_TX_VALID, txBitmapEnable, 0);
}

/*******************************************************************************
**
**  mvOnuEponMacGenTimeOfDayIntConfigSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set onu active TX bitmap enable
**
**  PARAMETERS:  MV_U32 polarity - 0 = active high, 1 = active low
**               MV_U32 width    - interrupt width in cycles (minimum 5)
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenTimeOfDayIntConfigSet(MV_U32 polarity, MV_U32 width)
{
	MV_U32 reg_val;

	reg_val  = width & 0xFFFFFFF;
	reg_val |= (polarity & 0x1) << 28;

	return asicOntGlbRegWrite(mvAsicReg_EPON_GEN_TOD_INT_WIDTH, reg_val, 0);
}

/*******************************************************************************
**
**  mvOnuEponMacGenTimeOfDayIntConfigGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function get onu active TX bitmap enable
**
**  PARAMETERS:  MV_U32 *polarity - 0 = active high, 1 = active low
**               MV_U32 *width    - interrupt width in cycles (minimum 5)
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenTimeOfDayIntConfigGet(MV_U32 *polarity, MV_U32 *width)
{
	MV_STATUS status;
	MV_U32 reg_val;

	status = asicOntGlbRegRead(mvAsicReg_EPON_GEN_TOD_INT_WIDTH, &reg_val, 0);
	if (status == MV_OK) {
		*width  = reg_val & 0xFFFFFFF;
		*polarity = (reg_val >> 28) & 0x1;
	}

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenUtmExtReportEnSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set the state of extended report
**
**  PARAMETERS:  MV_U32 extReportEnable
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenUtmExtReportEnSet(MV_U32 extReportEnable)
{
	return asicOntGlbRegWrite(mvAsicReg_EPON_TXM_EXT_REPORT_STATE, extReportEnable, 0);
}

/*******************************************************************************
**
**  mvOnuEponMacGenUtmExtReportBitmapEnSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set bitmap state of extended report
**
**  PARAMETERS:  MV_U32 llid
**               MV_U32 reportBitmapEnable
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenUtmExtReportBitmapEnSet(MV_U32 llid, MV_U32 reportBitmapEnable)
{
	MV_STATUS status;
	MV_U32 value;

	status = asicOntGlbRegRead(mvAsicReg_EPON_TXM_EXT_REPORT_STATE_PER_TX, &value, 0);
	if (status == MV_OK) {
		value &= ~(1 << (llid & 0xF));
		value |= ((reportBitmapEnable & 0x1) << (llid & 0xF));
		return asicOntGlbRegWrite(mvAsicReg_EPON_TXM_EXT_REPORT_STATE_PER_TX, value, 0);
	}

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGenUtmExtReportMapSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set extended report map
**
**  PARAMETERS:  MV_U32 host_tx
**               MV_U32 target_tx
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGenUtmExtReportMapSet(MV_U32 host_tx, MV_U32 target_tx)
{
	MV_STATUS status;
	MV_U32 value;

	if (host_tx < 8) {
		status = asicOntGlbRegRead(mvAsicReg_EPON_TXM_EXT_REPORT_MAP_0, &value, 0);
		if (status == MV_OK) {
			value &= ~(0xF << (4 * (host_tx & 0xF)));
			value |= ((target_tx & 0xF) << (4 * (host_tx & 0xF)));
			return asicOntGlbRegWrite(mvAsicReg_EPON_TXM_EXT_REPORT_MAP_0, value, 0);
		}
	} else  {
		status = asicOntGlbRegRead(mvAsicReg_EPON_TXM_EXT_REPORT_MAP_1, &value, 0);
		if (status == MV_OK) {
			value &= ~(0xF << (4 * ((host_tx - 8) & 0xF)));
			value |= ((target_tx & 0xF) << (4 * ((host_tx - 8) & 0xF)));
			return asicOntGlbRegWrite(mvAsicReg_EPON_TXM_EXT_REPORT_MAP_1, value, 0);
		}
	}

	return status;
}

/* ========================================================================== */
/*                        RXP Functions Section                               */
/* ========================================================================== */

/*******************************************************************************
**
**  mvOnuEponMacRxpPacketSizeLimitSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set packet min / mac size
**
**  PARAMETERS:  MV_U32 minSize
**               MV_U32 maxSize
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacRxpPacketSizeLimitSet(MV_U32 minSize,
					    MV_U32 maxSize)
{
	MV_STATUS status;
	MV_U32 size;

	size = ((maxSize & 0x0FFF) << 16) |
	       (minSize & 0x0FFF);

	status = asicOntGlbRegWrite(mvAsicReg_EPON_RXP_PACKET_SIZE_LIMIT, size, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacRxpPacketSizeLimitGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return packet min / mac size
**
**  PARAMETERS:  MV_U32 *minSize
**               MV_U32 *maxSize
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacRxpPacketSizeLimitGet(MV_U32 *minSize,
					    MV_U32 *maxSize)
{
	MV_STATUS status;
	MV_U32 size;

	status = asicOntGlbRegRead(mvAsicReg_EPON_RXP_PACKET_SIZE_LIMIT, &size, 0);

	*minSize =  size        & 0x0FFF;
	*maxSize = (size >> 16) & 0x0FFF;

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacRxpPacketFilterSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set packet filtering
**
**  PARAMETERS:  MV_U32 ignoreLlidCrcError
**               MV_U32 ignoreFcsError
**               MV_U32 ignoreGmiiError
**               MV_U32 ignoreLengthError
**               MV_U32 forwardAllLlid
**               MV_U32 forwardBc0FFF
**               MV_U32 forwardBc1FFF
**               MV_U32 forwardBc1xxx
**               MV_U32 dropBc1nnn
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacRxpPacketFilterSet(MV_U32 ignoreLlidCrcError,
					 MV_U32 ignoreFcsError,
					 MV_U32 ignoreGmiiError,
					 MV_U32 ignoreLengthError,
					 MV_U32 forwardAllLlid,
					 MV_U32 forwardBc0FFF,
					 MV_U32 forwardBc1FFF,
					 MV_U32 forwardBc1xxx,
					 MV_U32 dropBc1nnn)
{
	MV_STATUS status;
	MV_U32 filter;

	filter =        ((dropBc1nnn & 0x0001) << 8) |
		 ((forwardBc1xxx & 0x0001) << 7) |
		 ((forwardBc1FFF & 0x0001) << 6) |
		 ((forwardBc0FFF & 0x0001) << 5) |
		 ((forwardAllLlid & 0x0001) << 4) |
		 ((ignoreLengthError & 0x0001) << 3) |
		 ((ignoreGmiiError & 0x0001) << 2) |
		 ((ignoreFcsError & 0x0001) << 1) |
		 (ignoreLlidCrcError & 0x0001);

	status = asicOntGlbRegWrite(mvAsicReg_EPON_RXP_PACKET_FILTER, filter, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacRxpPacketFilterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return packet filtering
**
**  PARAMETERS:  MV_U32 *ignoreLlidCrcError
**               MV_U32 *ignoreFcsError
**               MV_U32 *ignoreGmiiError
**               MV_U32 *ignoreLengthError
**               MV_U32 *forwardAllLlid
**               MV_U32 *forwardBc0FFF
**               MV_U32 *forwardBc1FFF
**               MV_U32 *forwardBc1xxx
**               MV_U32 *dropBc1nnn
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacRxpPacketFilterGet(MV_U32 *ignoreLlidCrcError,
					 MV_U32 *ignoreFcsError,
					 MV_U32 *ignoreGmiiError,
					 MV_U32 *ignoreLengthError,
					 MV_U32 *forwardAllLlid,
					 MV_U32 *forwardBc0FFF,
					 MV_U32 *forwardBc1FFF,
					 MV_U32 *forwardBc1xxx,
					 MV_U32 *dropBc1nnn)
{
	MV_STATUS status;
	MV_U32 filter;

	status = asicOntGlbRegRead(mvAsicReg_EPON_RXP_PACKET_FILTER, &filter, 0);

	*dropBc1nnn         = ((filter >> 8) & 0x0001);
	*forwardBc1xxx      = ((filter >> 7) & 0x0001);
	*forwardBc1FFF      = ((filter >> 6) & 0x0001);
	*forwardBc0FFF      = ((filter >> 5) & 0x0001);
	*forwardAllLlid     = ((filter >> 4) & 0x0001);
	*ignoreLengthError  = ((filter >> 3) & 0x0001);
	*ignoreGmiiError    = ((filter >> 2) & 0x0001);
	*ignoreFcsError     = ((filter >> 1) & 0x0001);
	*ignoreLlidCrcError = ( filter       & 0x0001);

	return status;
}

#ifdef PON_A0
/*******************************************************************************
**
**  mvOnuEponMacRxpPacketForwardSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set packet forwarding
**
**  PARAMETERS:  MV_U32 ctrlFrameToDataQueue
**               MV_U32 ctrlFrameToCtrlQueue
**               MV_U32 rprtFrameToDataQueue
**               MV_U32 rprtFrameToRprtQueue
**               MV_U32 slowFrameToRprtQueue
**               MV_U32 slowFrameToCtrlQueue
**               MV_U32 rxpTsUpdateFcsError
**               MV_U32 rxpTsUpdateGmiiError
**               MV_U32 rxpTsUpdateLengthError
**               MV_U32 rxpTsUpdateCrcError
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacRxpPacketForwardSet(MV_U32 ctrlFrameToDataQueue,
					  MV_U32 ctrlFrameToCtrlQueue,
					  MV_U32 rprtFrameToDataQueue,
					  MV_U32 rprtFrameToRprtQueue,
					  MV_U32 slowFrameToRprtQueue,
					  MV_U32 slowFrameToCtrlQueue,
					  MV_U32 rxpTsUpdateFcsError,
					  MV_U32 rxpTsUpdateGmiiError,
					  MV_U32 rxpTsUpdateLengthError,
					  MV_U32 rxpTsUpdateCrcError)
{
	MV_STATUS status;
	MV_U32 forward;

	forward = ((rxpTsUpdateCrcError    & 0x0001) << 9) |
		  ((rxpTsUpdateLengthError & 0x0001) << 8) |
		  ((rxpTsUpdateGmiiError   & 0x0001) << 7) |
		  ((rxpTsUpdateFcsError    & 0x0001) << 6) |
		  ((slowFrameToCtrlQueue   & 0x0001) << 5) |
		  ((slowFrameToRprtQueue   & 0x0001) << 4) |
		  ((rprtFrameToRprtQueue   & 0x0001) << 3) |
		  ((rprtFrameToDataQueue   & 0x0001) << 2) |
		  ((ctrlFrameToCtrlQueue   & 0x0001) << 1) |
		  (ctrlFrameToDataQueue    & 0x0001);

	status = asicOntGlbRegWrite(mvAsicReg_EPON_RXP_CTRL_FRAME_FORWARD, forward, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacRxpPacketForwardGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return packet forwarding
**
**  PARAMETERS:  MV_U32 *ctrlFrameToDataQueue
**               MV_U32 *ctrlFrameToCtrlQueue
**               MV_U32 *rprtFrameToDataQueue
**               MV_U32 *rprtFrameToRprtQueue
**               MV_U32 *slowFrameToRprtQueue
**               MV_U32 *slowFrameToCtrlQueue
**               MV_U32 *rxpTsUpdateFcsError
**               MV_U32 *rxpTsUpdateGmiiError
**               MV_U32 *rxpTsUpdateLengthError
**               MV_U32 *rxpTsUpdateCrcError
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacRxpPacketForwardGet(MV_U32 *ctrlFrameToDataQueue,
					  MV_U32 *ctrlFrameToCtrlQueue,
					  MV_U32 *rprtFrameToDataQueue,
					  MV_U32 *rprtFrameToRprtQueue,
					  MV_U32 *slowFrameToRprtQueue,
					  MV_U32 *slowFrameToCtrlQueue,
					  MV_U32 *rxpTsUpdateFcsError,
					  MV_U32 *rxpTsUpdateGmiiError,
					  MV_U32 *rxpTsUpdateLengthError,
					  MV_U32 *rxpTsUpdateCrcError)
{
	MV_STATUS status;
	MV_U32 forward;

	status = asicOntGlbRegRead(mvAsicReg_EPON_RXP_CTRL_FRAME_FORWARD, &forward, 0);

	*rxpTsUpdateCrcError    = ((forward >> 9) & 0x0001);
	*rxpTsUpdateLengthError = ((forward >> 8) & 0x0001);
	*rxpTsUpdateGmiiError   = ((forward >> 7) & 0x0001);
	*rxpTsUpdateFcsError    = ((forward >> 6) & 0x0001);
	*slowFrameToCtrlQueue   = ((forward >> 5) & 0x0001);
	*slowFrameToRprtQueue   = ((forward >> 4) & 0x0001);
	*rprtFrameToRprtQueue   = ((forward >> 3) & 0x0001);
	*rprtFrameToDataQueue   = ((forward >> 2) & 0x0001);
	*ctrlFrameToCtrlQueue   = ((forward >> 1) & 0x0001);
	*ctrlFrameToDataQueue   = (forward       & 0x0001);

	return status;
}

#else
/*******************************************************************************
**
**  mvOnuEponMacRxpPacketForwardSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set packet forwarding
**
**  PARAMETERS:  MV_U32 ctrlFrameToDataQueue
**               MV_U32 ctrlFrameToCtrlQueue
**               MV_U32 rprtFrameToDataQueue
**               MV_U32 rprtFrameToRprtQueue
**               MV_U32 slowFrameToRprtQueue
**               MV_U32 slowFrameToCtrlQueue
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacRxpPacketForwardSet(MV_U32 ctrlFrameToDataQueue,
					  MV_U32 ctrlFrameToCtrlQueue,
					  MV_U32 rprtFrameToDataQueue,
					  MV_U32 rprtFrameToRprtQueue,
					  MV_U32 slowFrameToRprtQueue,
					  MV_U32 slowFrameToCtrlQueue)
{
	MV_STATUS status;
	MV_U32 forward;

	forward = ((slowFrameToCtrlQueue & 0x0001) << 5) |
		  ((slowFrameToRprtQueue & 0x0001) << 4) |
		  ((rprtFrameToRprtQueue & 0x0001) << 3) |
		  ((rprtFrameToDataQueue & 0x0001) << 2) |
		  ((ctrlFrameToCtrlQueue & 0x0001) << 1) |
		  (ctrlFrameToDataQueue & 0x0001);

	status = asicOntGlbRegWrite(mvAsicReg_EPON_RXP_CTRL_FRAME_FORWARD, forward, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacRxpPacketForwardGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return packet forwarding
**
**  PARAMETERS:  MV_U32 *ctrlFrameToDataQueue
**               MV_U32 *ctrlFrameToCtrlQueue
**               MV_U32 *rprtFrameToDataQueue
**               MV_U32 *rprtFrameToRprtQueue
**               MV_U32 *slowFrameToRprtQueue
**               MV_U32 *slowFrameToCtrlQueue
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacRxpPacketForwardGet(MV_U32 *ctrlFrameToDataQueue,
					  MV_U32 *ctrlFrameToCtrlQueue,
					  MV_U32 *rprtFrameToDataQueue,
					  MV_U32 *rprtFrameToRprtQueue,
					  MV_U32 *slowFrameToRprtQueue,
					  MV_U32 *slowFrameToCtrlQueue)
{
	MV_STATUS status;
	MV_U32 forward;

	status = asicOntGlbRegRead(mvAsicReg_EPON_RXP_CTRL_FRAME_FORWARD, &forward, 0);

	*slowFrameToCtrlQueue = ((forward >> 5) & 0x0001);
	*slowFrameToRprtQueue = ((forward >> 4) & 0x0001);
	*rprtFrameToRprtQueue = ((forward >> 3) & 0x0001);
	*rprtFrameToDataQueue = ((forward >> 2) & 0x0001);
	*ctrlFrameToCtrlQueue = ((forward >> 1) & 0x0001);
	*ctrlFrameToDataQueue = ( forward       & 0x0001);

	return status;
}
#endif

/*******************************************************************************
**
**  mvOnuEponMacRxpLlidDataSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set LLID Data entry
**
**  PARAMETERS:  MV_U32 llid
**               MV_U32 index
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacRxpLlidDataSet(MV_U32 llid, MV_U32 index)
{
	MV_STATUS status;

	if (index >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_RXP_LLT_LLID_DATA, llid, index);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacRxpLlidDataGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return LLID Data entry
**
**  PARAMETERS:  MV_U32 *llid
**               MV_U32 index
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacRxpLlidDataGet(MV_U32 *llid, MV_U32 index)
{
	MV_STATUS status;

	if (index >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_RXP_LLT_LLID_DATA, llid, index);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacRxpEncConfigSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set Encryption configuration
**
**  PARAMETERS:  MV_U32 config
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacRxpEncConfigSet(MV_U32 config)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_RXP_ENCRYPTION_CONFIG, config, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacRxpEncConfigGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return Encryption configuration
**
**  PARAMETERS:  MV_U32 *config
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacRxpEncConfigGet(MV_U32 *config)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_EPON_RXP_ENCRYPTION_CONFIG, config, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacRxpEncKeySet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set Encryption key
**
**  PARAMETERS:  MV_U32 key
**               MV_U32 index
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacRxpEncKeySet(MV_U32 key, MV_U32 index, MV_U32 macId)
{
	MV_STATUS status = MV_OK;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	if (index == 0)
		status = asicOntGlbRegWrite(mvAsicReg_EPON_RXP_ENCRYPTION_KEY0, key, macId);
	else if (index == 1)
		status = asicOntGlbRegWrite(mvAsicReg_EPON_RXP_ENCRYPTION_KEY1, key, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacRxpEncKeyGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set Encryption key
**
**  PARAMETERS:  MV_U32 *keyIndex0
**               MV_U32 *keyIndex1
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacRxpEncKeyGet(MV_U32 *keyIndex0, MV_U32 *keyIndex1, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status  = asicOntGlbRegRead(mvAsicReg_EPON_RXP_ENCRYPTION_KEY0, keyIndex0, macId);
	status |= asicOntGlbRegRead(mvAsicReg_EPON_RXP_ENCRYPTION_KEY1, keyIndex1, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacRxpDataFifoThresholdSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function sets Data FIFO Threshold
**
**  PARAMETERS:  MV_U32 threshold
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacRxpDataFifoThresholdSet(MV_U32 threshold)
{
	return asicOntGlbRegWrite(mvAsicReg_EPON_RXP_DATA_FIFO_THRESH, threshold, 0);
}

/*******************************************************************************
**
**  mvOnuEponMacRxpDataFifoThresholdGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function gets Data FIFO Threshold
**
**  PARAMETERS:  MV_U32 *threshold
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacRxpDataFifoThresholdGet(MV_U32 *threshold)
{
	return asicOntGlbRegRead(mvAsicReg_EPON_RXP_DATA_FIFO_THRESH, threshold, 0);
}

/* ========================================================================== */
/*                        GPM Functions Section                               */
/* ========================================================================== */

/*******************************************************************************
**
**  mvOnuEponMacGpmMacFutureGrantTimeSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set future grants
**
**  PARAMETERS:  MV_U32 grant
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGpmMacFutureGrantTimeSet(MV_U32 grant)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_GPM_MAX_FUTURE_GRANT_TIME, grant, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGpmMinProcessingTimeSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set min processing time
**
**  PARAMETERS:  MV_U32 processingTime
**               MV_U32 effectiveLength
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGpmMinProcessingTimeSet(MV_U32 processingTime, MV_U32 effectiveLength)
{
	MV_STATUS status;
	MV_U32 minProcTime;

	minProcTime = ((effectiveLength & 0x00FF) << 16) |
		      (processingTime  & 0xFFFF);

	status = asicOntGlbRegWrite(mvAsicReg_EPON_GPM_MIN_PROCESSING_TIME, minProcTime, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGpmDiscoveryGrantLengthSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set discovery grant length
**
**  PARAMETERS:  MV_U32 grantLength
**               MV_U32 addOffsetForCalc
**               MV_U32 grantLengthMultiTq
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGpmDiscoveryGrantLengthSet(MV_U32 grantLength,
						 MV_U32 addOffsetForCalc,
						 MV_U32 grantLengthMultiTq)
{
	MV_STATUS status;
	MV_U32 discoveryGrantLength;

	discoveryGrantLength = ((grantLengthMultiTq & 0x0FFF) << 16) |
			       ((addOffsetForCalc & 0x000F) << 12) |
			       (grantLength  & 0x0FFF);

	status = asicOntGlbRegWrite(mvAsicReg_EPON_GPM_DISCOVERY_GRANT_LENGTH, discoveryGrantLength, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGpmRxSyncGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return Rx sync time
**
**  PARAMETERS:  MV_U32 *syncTime
**               MV_U32 *syncTimeUpdateInd
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGpmRxSyncGet(MV_U32 *syncTime, MV_U32 *syncTimeUpdateInd)
{
	MV_STATUS status;
	MV_U32 time;

	status = asicOntGlbRegRead(mvAsicReg_EPON_GPM_RX_SYNC_TIME, &time, 0);

	*syncTimeUpdateInd = ((time >> 16) & 0x0001);
	*syncTime          =  (time        & 0xFFFF);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGpmGrantValidCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return GPM counters
**
**  PARAMETERS:  MV_U32 *counter
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGpmGrantValidCounterGet(MV_U32 *counter, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_GPM_GRANT_VALID, counter, macId);

	return status;
}

/*****************************************************S**************************
**
**  mvOnuEponMacGpmGrantMaxFutureTimeErrorCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return GPM counters
**
**  PARAMETERS:  MV_U32 *counter
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGpmGrantMaxFutureTimeErrorCounterGet(MV_U32 *counter, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_GPM_GRANT_MAX_FUTURE_TIME_ERR, counter, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGpmMinProcTimeErrorCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return GPM counters
**
**  PARAMETERS:  MV_U32 *counter
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGpmMinProcTimeErrorCounterGet(MV_U32 *counter, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_GPM_MIN_PROC_TIME_ERR, counter, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGpmLengthErrorCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return GPM counters
**
**  PARAMETERS:  MV_U32 *counter
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGpmLengthErrorCounterGet(MV_U32 *counter, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_GPM_LENGTH_ERR, counter, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGpmDiscoveryAndRegisterCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return GPM counters
**
**  PARAMETERS:  MV_U32 *counter
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGpmDiscoveryAndRegisterCounterGet(MV_U32 *counter, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_GPM_DISCOVERY_AND_REGISTERED_ERR, counter, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGpmFifoFullErrorCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return GPM counters
**
**  PARAMETERS:  MV_U32 *counter
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGpmFifoFullErrorCounterGet(MV_U32 *counter, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_GPM_FIFO_FULL_ERR, counter, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGpmOpcDiscoveryNotRegisterBcastCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return GPM counters
**
**  PARAMETERS:  MV_U32 *counter
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGpmOpcDiscoveryNotRegisterBcastCounterGet(MV_U32 *counter, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_GPM_OPC_DISC_NOT_REG_BCAST, counter, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGpmOpcRegisterNotDiscoveryCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return GPM counters
**
**  PARAMETERS:  MV_U32 *counter
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGpmOpcRegisterNotDiscoveryCounterGet(MV_U32 *counter, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_GPM_OPC_REG_NOT_DISC, counter, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGpmOpcDiscoveryNotRegisterNotBcastCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return GPM counters
**
**  PARAMETERS:  MV_U32 *counter
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGpmOpcDiscoveryNotRegisterNotBcastCounterGet(MV_U32 *counter, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_GPM_OPC_DISC_NOT_REG_NOT_BCAST, counter, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGpmOpcDropGrantCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return GPM counters
**
**  PARAMETERS:  MV_U32 *counter
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGpmOpcDropGrantCounterGet(MV_U32 *counter, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_GPM_OPC_DROPED_GRANT, counter, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGpmOpcHiddenGrantCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return GPM counters
**
**  PARAMETERS:  MV_U32 *counter
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGpmOpcHiddenGrantCounterGet(MV_U32 *counter, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_GPM_OPC_HIDDEN_GRANT, counter, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacGpmOpcBackToBackCounterGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return GPM counters
**
**  PARAMETERS:  MV_U32 *counter
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacGpmOpcBackToBackCounterGet(MV_U32 *counter, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_GPM_OPC_BACK_TO_BACK_GRANT, counter, macId);

	return status;
}

/* ========================================================================== */
/*                        TXM Functions Section                               */
/* ========================================================================== */

/*******************************************************************************
**
**  mvOnuEponMacTxmOverheadSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set TX module overhead
**
**  PARAMETERS:  MV_U32 overhead
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmOverheadSet(MV_U32 overhead)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_TXM_DEFAULT_OVERHEAD, overhead, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxmOverheadGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function get TX module overhead
**
**  PARAMETERS:  None
**
**  OUTPUTS:     MV_U32 overhead
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmOverheadGet(MV_U32 *overhead)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_EPON_TXM_DEFAULT_OVERHEAD, overhead, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxmConfigSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set TX configuration
**
**  PARAMETERS:  MV_U32 mode
**               MV_U32 alignment
**               MV_U32 priority
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmConfigSet(MV_U32 tFec1Cfg, MV_U32 tFec2Cfg, MV_U32 ctrlFifoDisFcs,
				   MV_U32 maskGmiiSignal, MV_U32 blockDataFromDataPath,
				   MV_U32 addIpgToLenCalc, MV_U32 mode, MV_U32 alignment,
				   MV_U32 priority)
{
	MV_STATUS status;
	MV_U32 config;

	config =              ((tFec1Cfg & 0x0007) << 13) |
		 ((tFec2Cfg & 0x0007) << 10) |
		 ((ctrlFifoDisFcs & 0x0001) <<  8) |
		 ((maskGmiiSignal & 0x0001) <<  7) |
		 ((blockDataFromDataPath & 0x0001) <<  6) |
		 ((addIpgToLenCalc & 0x0001) <<  5) |
		 ((mode & 0x0003) <<  3) |
		 ((alignment & 0x0003) <<  1) |
		 (priority & 0x0001);

	status = asicOntGlbRegWrite(mvAsicReg_EPON_TXM_CONFIGURATION, config, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxmPowerUpIndSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set TX Multiplexer power up inidication burst time
**
**  PARAMETERS:  MV_U32 time   - pre-active burst time in clock cycles units
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmPowerUpIndSet(MV_U32 time)
{
	return asicOntGlbRegWrite(mvAsicReg_EPON_TXM_POWER_UP, time, 0);
}

/*******************************************************************************
**
**  mvOnuEponMacTxmPowerUpIndGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set TX Multiplexer power up inidication burst time
**
**  PARAMETERS:  MV_U32 *time   - pre-active burst time in clock cycles units
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmPowerUpIndGet(MV_U32 *time)
{
	return asicOntGlbRegRead(mvAsicReg_EPON_TXM_POWER_UP, time, 0);
}

/*******************************************************************************
**
**  mvOnuEponMacTxmLlidSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set TX LLID
**
**  PARAMETERS:  MV_U32 llid
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmLlidSet(MV_U32 llid, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_TXM_TX_LLID, llid, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxmCppReportConfig
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set TX CPP Report configuration
**
**  PARAMETERS:  MV_U32 config
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmCppReportConfig(MV_U32 queueReportQ2, MV_U32 reportBitmapQ2,
					 MV_U32 queueReport, MV_U32 queueSet,
					 MV_U32 reportBitmap, MV_U32 macId)
{
	MV_STATUS status;
	MV_U32 config;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	config = ((queueReportQ2 & 0x00FF) << 18) |
		 ((reportBitmapQ2 & 0x0001) << 16) |
		 ((queueReport & 0x00FF) << 2)  |
		 ((queueSet & 0x0003) << 1)  |
		 (reportBitmap & 0x0001);

	status = asicOntGlbRegWrite(mvAsicReg_EPON_TXM_CPP_RPRT_CONFIG, config, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxmCppReportBitMap
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set TX CPP Report Bit Map
**
**  PARAMETERS:  MV_U32 bitMap
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmCppReportBitMap(MV_U32 bitMap, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_TXM_CPP_RPRT_BIT_MAP, bitMap, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxmCppReportBitMapGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set TX CPP Report Bit Map
**
**  PARAMETERS:  MV_U32 bitMap
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmCppReportBitMapGet(MV_U32 *bitMap, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_TXM_CPP_RPRT_BIT_MAP, bitMap, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxmCppReportQueueSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set TX CPP Report Queue Set
**
**  PARAMETERS:  MV_U32 queueSet
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmCppReportQueueSet(MV_U32 queueSet, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_SET, queueSet, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxmCppReportQueueSetGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return TX CPP Report Queue Set
**
**  PARAMETERS:  MV_U32 *queueSet
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmCppReportQueueSetGet(MV_U32 *queueSet, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_SET, queueSet, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxmCppReportQueue0
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return TX CPP Report Queue
**
**  PARAMETERS:  MV_U32 queue
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmCppReportQueue0(MV_U32 queue, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_0, queue, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxmCppReportQueue1
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return TX CPP Report Queue
**
**  PARAMETERS:  MV_U32 queue
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmCppReportQueue1(MV_U32 queue, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_1, queue, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxmCppReportQueue2
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return TX CPP Report Queue
**
**  PARAMETERS:  MV_U32 queue
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmCppReportQueue2(MV_U32 queue, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_2, queue, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxmCppReportQueue3
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return TX CPP Report Queue
**
**  PARAMETERS:  MV_U32 queue
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmCppReportQueue3(MV_U32 queue, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_3, queue, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxmCppReportQueue4
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return TX CPP Report Queue
**
**  PARAMETERS:  MV_U32 queue
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmCppReportQueue4(MV_U32 queue, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_4, queue, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxmCppReportQueue5
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return TX CPP Report Queue
**
**  PARAMETERS:  MV_U32 queue
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmCppReportQueue5(MV_U32 queue, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_5, queue, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxmCppReportQueue6
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return TX CPP Report Queue
**
**  PARAMETERS:  MV_U32 queue
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmCppReportQueue6(MV_U32 queue, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_6, queue, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxmCppReportQueue7
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return TX CPP Report Queue
**
**  PARAMETERS:  MV_U32 queue
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmCppReportQueue7(MV_U32 queue, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_TXM_CPP_RPRT_QUEUE_7, queue, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxmCppReportQueueX
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return TX CPP Report Queue 0 - 7
**
**  PARAMETERS:  MV_U32 queue
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmCppReportQueueX(MV_U32 queueNum, MV_U32 queueCfg, MV_U32 macId)
{
	MV_STATUS status = MV_OK;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	if (queueNum == 0)
		status |= mvOnuEponMacTxmCppReportQueue0(queueCfg, macId);
	else if (queueNum == 1) status |= mvOnuEponMacTxmCppReportQueue1(queueCfg, macId);
	else if (queueNum == 2) status |= mvOnuEponMacTxmCppReportQueue2(queueCfg, macId);
	else if (queueNum == 3) status |= mvOnuEponMacTxmCppReportQueue3(queueCfg, macId);
	else if (queueNum == 4) status |= mvOnuEponMacTxmCppReportQueue4(queueCfg, macId);
	else if (queueNum == 5) status |= mvOnuEponMacTxmCppReportQueue5(queueCfg, macId);
	else if (queueNum == 6) status |= mvOnuEponMacTxmCppReportQueue6(queueCfg, macId);
	else if (queueNum == 7) status |= mvOnuEponMacTxmCppReportQueue7(queueCfg, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxmCppReportUpdate
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return TX CPP Report Queue
**
**  PARAMETERS:  MV_U32 queue
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmCppReportUpdate(MV_U32 validQueueReport, MV_U32 highestReportQueue, MV_U32 macId)
{
	MV_STATUS status;
	MV_U32 queueNum;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status  = mvOnuEponMacTxmCppReportConfig(0xFF,                                  /* Queueset2 not supported - set to 0xFF (queueReportQ2) */
						 0x1,                                   /* Queueset2 not supported - set to 0x1, Software (reportBitmapQ2) */
						 ~(validQueueReport & 0xFF),            /* Queueset1 supported - set to 0, means all 8 queues are active (queueReport) */
						 0x1,                                   /* Number of queueSet - support for 1 queueset */
						 0x1,                                   /* Queueset1 supported - set to 0x1, Software (reportBitmap) */
						 macId);
	status |= mvOnuEponMacTxmCppReportBitMap(validQueueReport & 0xFF, macId);       /* Queueset1 - 8 queues enabled(0xFF) , Queueset1 - 8 queues disabled(0x00) */
	status |= mvOnuEponMacTxmCppReportQueueSet(0x1, macId);                         /* Number of queuesets - support for 1 queueset */

	for (queueNum = 0; queueNum < ONU_DBA_MAX_QUEUE; queueNum++) {
		if (validQueueReport & (1 << queueNum)) {
			if (queueNum == highestReportQueue)
				status |= mvOnuEponMacTxmCppReportQueueX(queueNum, 0x000003ff, macId);
			else
				status |= mvOnuEponMacTxmCppReportQueueX(queueNum, 0x000003ff, macId);
		} else
			status |= mvOnuEponMacTxmCppReportQueueX(queueNum, 0x0, macId);
	}

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxmCppReportFecConfig
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set TX CPP Report FEC config
**
**  PARAMETERS:  MV_U32 fecConst1
**               MV_U32 fecConst2
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmCppReportFecConfig(MV_U32 fecConst1, MV_U32 fecConst2)
{
	MV_STATUS status;
	MV_U32 config;

	config = ((fecConst2 & 0x00FF) << 16) |
		 (fecConst1 & 0xFFFF);

	status = asicOntGlbRegWrite(mvAsicReg_EPON_TXM_CPP_RPRT_FEC_CONFIG, config, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxmCppRpmReportConfig
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set RPM report configuration
**
**  PARAMETERS:  MV_U32 packetIndication - report packet generation indication
**               MV_U32 bitmap - bitmap order
**               MV_U32 qset   - qset order
**               MV_U32 mode   - report generation mode
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmCppRpmReportConfigSet(MV_U32 packetIndication,
					       MV_U32 bitmap,
					       MV_U32 qset,
					       MV_U32 mode)
{
	MV_U32 config;

	config = ((packetIndication & 0x1) << 4) |
		 ((bitmap & 0x1) << 3) |
		 ((qset & 0x1)   << 2) |
		 (mode & 0x3);

	return asicOntGlbRegWrite(mvAsicReg_EPON_TXM_CPP_RPM_REPORT_CONFIG, config, 0);
}

/*******************************************************************************
**
**  mvOnuEponMacTxmCppRpmFifoDbaConfig
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set RPM additional DBA for control FIFO.
**
**  PARAMETERS:  MV_U32 fifoEnable  - Whether enable this feature
**               MV_U32 dbaQueue    - report queue for DBA
**               MV_U32 dbaOverhead - report overhead for DBA
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmCppRpmFifoDbaConfig(MV_U32 fifoEnable,
					     MV_U32 dbaQueue,
					     MV_U32 dbaOverhead)
{
	MV_U32 config;

	config = ((fifoEnable & 0x1) << 0) |
		 ((dbaQueue & 0x7)   << 1) |
		 ((dbaOverhead & 0x7f) << 4);

	return asicOntGlbRegWrite(mvAsicReg_EPON_TXM_CPP_RPM_CTRL_FIFO_DBA, config, 0);
}

/*******************************************************************************
**
**  mvOnuEponMacTxmCppRpmIdxReportTableSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set RPM index report table
**
**  PARAMETERS:  MV_U32 llid   - LLID used for table entry access
**               MV_U32 data   - table data (command)
**               MV_U32 addr   - table address
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmCppRpmIdxReportTableSet(MV_U32 llid,
						 MV_U32 data,
						 MV_U32 addr)
{
	MV_U32 entry;

	entry = ((llid & 0x7)  << 5)  |
		((data & 0x3F) << 16) |
		(addr & 0x1F);

	return asicOntGlbRegWrite(mvAsicReg_EPON_TXM_CPP_RPM_ADX_REPORT_TBL, entry, 0);
}

/*******************************************************************************
**
**  mvOnuEponMacTxmTxCtrlFiFoFlush
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function flush the TX control FIFO
**
**  PARAMETERS:  MV_U32 llid   - LLID used to flush
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmTxCtrlFiFoFlush(MV_U32 llid)
{
	MV_U32 value;

	value = (1 << (llid & 0x7));

	return asicOntGlbRegWrite(mvAsicReg_EPON_TXM_CTRL_FIFO_FLUSH, value, 0);
}

/*******************************************************************************
**
**  mvOnuEponMacTxmHighPriBitmapSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set high priority TX bitmap
**
**  PARAMETERS:  MV_U32 llid
**               MV_U32 highPriEn
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmHighPriBitmapSet(MV_U32 llid, MV_U32 highPriEn)
{
	MV_STATUS status;
	MV_U32 value;

	status = asicOntGlbRegRead(mvAsicReg_EPON_TXM_HIGH_PRI_TX_STATE_BITMAP, &value, 0);
	if (status == MV_OK) {
		value &= ~(1 << (llid & 0xF));
		value |= ((highPriEn & 0x1) << (llid & 0xF));
		return asicOntGlbRegWrite(mvAsicReg_EPON_TXM_HIGH_PRI_TX_STATE_BITMAP, value, 0);
	}

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxmHighPriMapSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set high priority TX map
**
**  PARAMETERS:  MV_U32 llid
**               MV_U32 tx_bm
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxmHighPriMapSet(MV_U32 llid, MV_U32 tx_bm)
{
	MV_STATUS status;
	MV_U32 value;

	if (llid < 4) {
		status = asicOntGlbRegRead(mvAsicReg_EPON_TXM_HIGH_PRI_TX_MAP_0, &value, 0);
		if (status == MV_OK) {
			value &= ~(0xFF << (8 * (llid & 0xF)));
			value |= ((tx_bm & 0xFF) << (8 * (llid & 0xF)));
			return asicOntGlbRegWrite(mvAsicReg_EPON_TXM_HIGH_PRI_TX_MAP_0, value, 0);
		}
	} else  {
		status = asicOntGlbRegRead(mvAsicReg_EPON_TXM_HIGH_PRI_TX_MAP_1, &value, 0);
		if (status == MV_OK) {
			value &= ~(0xFF << (8 * ((llid - 4) & 0xF)));
			value |= ((tx_bm & 0xFF) << (8 * ((llid - 4) & 0xF)));
			return asicOntGlbRegWrite(mvAsicReg_EPON_TXM_HIGH_PRI_TX_MAP_1, value, 0);
		}
	}

	return status;
}

/* ========================================================================== */
/*                        PCS Functions Section                               */
/* ========================================================================== */

/*******************************************************************************
**
**  mvOnuEponMacPcsConfigSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set PCS configuration
**
**  PARAMETERS:  MV_U32 config
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacPcsConfigSet(MV_U32 config)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_PCS_CONFIGURATION, config, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacPcsConfigGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function get PCS configuration
**
**  PARAMETERS:  None
**
**  OUTPUTS:     MV_U32 config
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacPcsConfigGet(MV_U32 *config)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_EPON_PCS_CONFIGURATION, config, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacPcsDelaySet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set PCS delay
**
**  PARAMETERS:  MV_U32 delay
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacPcsDelaySet(MV_U32 delay)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_PCS_DELAY_CONFIG, delay, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacPcsStatsGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function get PCS stats (0,1,2,3,4)
**
**  PARAMETERS:
**		MV_U32 *legalFrameCnt,
**		MV_U32 *maxFrameSizeErrCnt,
**		MV_U32 *parityLenErrCnt,
**		MV_U32 *longGateErrCnt,
**		MV_U32 *protocolErrCnt,
**		MV_U32 *minFrameSizeErrCnt,
**		MV_U32 *legalFecFrameCnt,
**		MV_U32 *legalNonFecFrameCnt
**  OUTPUTS:
**		Counter values assigned to input parameters
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacPcsStatsGet(MV_U32 *legalFrameCnt,
				  MV_U32 *maxFrameSizeErrCnt,
				  MV_U32 *parityLenErrCnt,
				  MV_U32 *longGateErrCnt,
				  MV_U32 *protocolErrCnt,
				  MV_U32 *minFrameSizeErrCnt,
				  MV_U32 *legalFecFrameCnt,
				  MV_U32 *legalNonFecFrameCnt)
{
	MV_U32 regVal;
	MV_STATUS status;

	if ((legalFrameCnt == 0) || (maxFrameSizeErrCnt == 0) ||
	    (parityLenErrCnt == 0) || (longGateErrCnt == 0) ||
	    (protocolErrCnt == 0) || (minFrameSizeErrCnt == 0) ||
	    (legalFecFrameCnt == 0) || (legalNonFecFrameCnt == 0))
		return MV_BAD_PARAM;

	status = asicOntGlbRegRead(mvAsicReg_EPON_PCS_STATS_0, &regVal, 0);
	*legalFrameCnt = regVal;

	status |= asicOntGlbRegRead(mvAsicReg_EPON_PCS_STATS_1, &regVal, 0);
	*maxFrameSizeErrCnt = (regVal >> 16) & 0xFFFF;

	status |= asicOntGlbRegRead(mvAsicReg_EPON_PCS_STATS_1, &regVal, 0);
	*parityLenErrCnt = regVal & 0xFFFF;

	status |= asicOntGlbRegRead(mvAsicReg_EPON_PCS_STATS_2, &regVal, 0);
	*longGateErrCnt = (regVal >> 16) & 0xFFFF;

	status |= asicOntGlbRegRead(mvAsicReg_EPON_PCS_STATS_2, &regVal, 0);
	*protocolErrCnt = regVal & 0xFFFF;

	status |= asicOntGlbRegRead(mvAsicReg_EPON_PCS_STATS_3, &regVal, 0);
	*minFrameSizeErrCnt = regVal & 0xFFFF;

	status |= asicOntGlbRegRead(mvAsicReg_EPON_PCS_STATS_4, &regVal, 0);
	*legalFecFrameCnt = (regVal >> 16) & 0xFFFF;

	status |= asicOntGlbRegRead(mvAsicReg_EPON_PCS_STATS_4, &regVal, 0);
	*legalNonFecFrameCnt = regVal & 0xFFFF;

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacPcsFrameSizeLimitsSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set PCS frame size  limits
**
**  PARAMETERS:  MV_U32 size      - maximum frame size
**               MV_U32 latency   - FEC decoder latency
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacPcsFrameSizeLimitsSet(MV_U32 size, MV_U32 latency)
{
	MV_U32 reg_val;

	reg_val  = size & 0xFFF;
	reg_val |= (latency & 0x1FFF) << 16;

	return asicOntGlbRegWrite(mvAsicReg_EPON_PCS_FRAME_SZ_LIMITS, reg_val, 0);
}

/*******************************************************************************
**
**  mvOnuEponMacPcsFrameSizeLimitsGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function get PCS frime size limits
**
**  PARAMETERS:  None
**
**  OUTPUTS:     MV_U32 *size      - maximum framce size
**               MV_U32 *latency   - FEC decoder latency
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacPcsFrameSizeLimitsGet(MV_U32 *size, MV_U32 *latency)
{
	MV_STATUS status;
	MV_U32 reg_val;

	status = asicOntGlbRegRead(mvAsicReg_EPON_PCS_FRAME_SZ_LIMITS, &reg_val, 0);
	if (status == MV_OK) {
		*size = reg_val & 0xFFF;
		*latency = (reg_val >> 16) & 0x1FFF;
	}

	return status;
}

/* ========================================================================== */
/*                        DDM Functions Section                               */
/* ========================================================================== */

/*******************************************************************************
**
**  mvOnuEponMacDdmDelaySet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set DDM delay
**
**  PARAMETERS:  MV_U32 delay
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacDdmDelaySet(MV_U32 delay)
{
	MV_STATUS status;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_DDM_DELAY_CONFIG, delay, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacDdmTxPolaritySet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function set DDM polarity
**
**  PARAMETERS:  MV_U32 txEnable       - mask TX enabled with TX allowed
**               MV_U32 txPolarity     - TX polarity (0 - active high)
**               MV_U32 serdesPolarity - SERDES shutdown polarity (0 - active low)
**               MV_U32 xvrPolarity    - XVR shutdown polarity (0 - active low)
**               MV_U32 burstEnabl     - burst enable mode
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacDdmTxPolaritySet(MV_U32 txEnable,
				       MV_U32 txPolarity,
				       MV_U32 serdesPolarity,
				       MV_U32 xvrPolarity,
				       MV_U32 burstEnable)
{
	MV_U32 config;

	config = ((burstEnable & 0x1)    << 5) |
		 ((xvrPolarity & 0x1)    << 4) |
		 ((serdesPolarity & 0x1) << 3) |
		 ((txEnable & 0x3)       << 1) |
		 (txPolarity & 0x1);

	return asicOntGlbRegWrite(mvAsicReg_EPON_DDM_TX_POLARITY, config, 0);
}

/* ========================================================================== */
/*                        Statistics Functions Section                        */
/* ========================================================================== */

/*******************************************************************************
**
**  mvOnuEponMacRxStatsFcsErrorGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return received with FCS error counter
**
**  PARAMETERS:  MV_U32 *counter
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacRxStatsFcsErrorGet(MV_U32 *counter, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_STAT_RXP_FCS_ERROR_CNT, counter, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacRxStatsShortErrorGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return received short packets error counter
**
**  PARAMETERS:  MV_U32 *counter
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacRxStatsShortErrorGet(MV_U32 *counter, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_STAT_RXP_SHORT_ERROR_CNT, counter, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacRxStatsLongErrorGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return received long packets error counter
**
**  PARAMETERS:  MV_U32 *counter
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacRxStatsLongErrorGet(MV_U32 *counter, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_STAT_RXP_LONG_ERROR_CNT, counter, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacRxStatsDataFrameGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return received data frames counter
**
**  PARAMETERS:  MV_U32 *counter
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacRxStatsDataFrameGet(MV_U32 *counter, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_STAT_RXP_DATA_FRAMES_CNT, counter, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacRxStatsCtrlFrameGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return received control frames counter
**
**  PARAMETERS:  MV_U32 *counter
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacRxStatsCtrlFrameGet(MV_U32 *counter, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_STAT_RXP_CTRL_FRAMES_CNT, counter, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacRxStatsReportFrameGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return received report frames counter
**
**  PARAMETERS:  MV_U32 *counter
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacRxStatsReportFrameGet(MV_U32 *counter, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_STAT_RXP_REPORT_FRAMES_CNT, counter, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacRxStatsGateFrameGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return received gate frames counter
**
**  PARAMETERS:  MV_U32 *counter
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacRxStatsGateFrameGet(MV_U32 *counter, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_STAT_RXP_GATE_FRAMES_CNT, counter, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxStatsCtrlRegReqFrameGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return transmitted REGISTER REQUEST frames counter
**
**  PARAMETERS:  MV_U32 *counter
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxStatsCtrlRegReqFrameGet(MV_U32 *counter, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_STAT_TXP_CTRL_REG_REQ_FRAMES_CNT, counter, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxStatsCtrlRegAckFrameGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return transmitted REGISTER ACK frames counter
**
**  PARAMETERS:  MV_U32 *counter
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxStatsCtrlRegAckFrameGet(MV_U32 *counter, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_STAT_TXP_CTRL_REG_ACK_FRAMES_CNT, counter, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxStatsCtrlReportFrameGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return transmitted report frames counter
**
**  PARAMETERS:  MV_U32 *counter
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxStatsCtrlReportFrameGet(MV_U32 *counter, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_STAT_TXP_CTRL_REPORT_FRAMES_CNT, counter, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxStatsDataFrameGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return transmitted data frames counter
**
**  PARAMETERS:  MV_U32 *counter
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxStatsDataFrameGet(MV_U32 *counter, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_STAT_TXP_DATA_FRAMES_CNT, counter, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacTxStatsTxAllowedByteCountGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return transmitted Tx Allowed byte counter
**
**  PARAMETERS:  MV_U32 *counter
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacTxStatsTxAllowedByteCountGet(MV_U32 *counter, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_STAT_TXP_TX_ALLOWED_BYTE_CNT, counter, macId);

	return status;
}

/* ========================================================================== */
/*                        CPQ Functions Section                               */
/* ========================================================================== */

/*******************************************************************************
**
**  mvOnuEponMacCpqRxCtrlHeaderQueueUsedCountGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return control header queue used count
**
**  PARAMETERS:  MV_U32 *usedCount
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacCpqRxCtrlHeaderQueueUsedCountGet(MV_U32 *usedCount)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_EPON_CPQ_RX_CTRL_HQ_USED, usedCount, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacCpqRxCtrlHeaderQueueReadData
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return control header queue data
**
**  PARAMETERS:  MV_U32 *dataLow
**               MV_U32 *dataHigh
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacCpqRxCtrlHeaderQueueReadData(MV_U32 *dataLow, MV_U32 *dataHigh)
{
	MV_STATUS status;

	status  = asicOntGlbRegRead(mvAsicReg_EPON_CPQ_RX_CTRL_HQ_READ_L, dataLow, 0);
	status |= asicOntGlbRegRead(mvAsicReg_EPON_CPQ_RX_CTRL_HQ_READ_H, dataHigh, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacCpqRxCtrlQueueUsedCountGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return control queue used count
**
**  PARAMETERS:  MV_U32 *usedCount
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacCpqRxCtrlQueueUsedCountGet(MV_U32 *usedCount)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_EPON_CPQ_RX_CTRL_Q_USED, usedCount, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacCpqRxCtrlQueueReadData
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return control queue data
**
**  PARAMETERS:  MV_U32 *data
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacCpqRxCtrlQueueReadData(MV_U32 *data)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_EPON_CPQ_RX_CTRL_Q_READ, data, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacCpqRxRprtHeaderQueueUsedCountGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return report header queue used count
**
**  PARAMETERS:  MV_U32 *usedCount
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacCpqRxRprtHeaderQueueUsedCountGet(MV_U32 *usedCount)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_EPON_CPQ_RX_RPRT_HQ_USED, usedCount, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacCpqRxRprtHeaderQueueReadData
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return report header queue data
**
**  PARAMETERS:  MV_U32 *dataLow
**               MV_U32 *dataHigh
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacCpqRxRprtHeaderQueueReadData(MV_U32 *dataLow, MV_U32 *dataHigh)
{
	MV_STATUS status;

	status  = asicOntGlbRegRead(mvAsicReg_EPON_CPQ_RX_RPRT_HQ_READ_L, dataLow, 0);
	status |= asicOntGlbRegRead(mvAsicReg_EPON_CPQ_RX_RPRT_HQ_READ_H, dataHigh, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacCpqRxRprtQueueUsedCountGet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return report queue used count
**
**  PARAMETERS:  MV_U32 *usedCount
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacCpqRxRprtQueueUsedCountGet(MV_U32 *usedCount)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_EPON_CPQ_RX_RPRT_Q_USED, usedCount, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacCpqRxRprtQueueReadData
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return report queue data
**
**  PARAMETERS:  MV_U32 *data
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacCpqRxRprtQueueReadData(MV_U32 *data)
{
	MV_STATUS status;

	status = asicOntGlbRegRead(mvAsicReg_EPON_CPQ_RX_RPRT_Q_READ, data, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacCpqTxCtrlHeaderQueueFree
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return control header queue free count
**
**  PARAMETERS:  MV_U32 *freeCount
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacCpqTxCtrlHeaderQueueFree(MV_U32 *freeCount, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_CPQ_TX_CTRL_HQ_FREE, freeCount, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacCpqTxCtrlHeaderQueueWrite
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function write control header queue data
**
**  PARAMETERS:  MV_U32 data
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacCpqTxCtrlHeaderQueueWrite(MV_U32 data, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_CPQ_TX_CTRL_HQ_WRITE, data, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacCpqTxCtrlQueueFree
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function return control queue free count
**
**  PARAMETERS:  MV_U32 *freeCount
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacCpqTxCtrlQueueFree(MV_U32 *freeCount, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegRead(mvAsicReg_EPON_CPQ_TX_CTRL_Q_FREE, freeCount, macId);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacCpqTxCtrlQueueWrite
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function write control queue data
**
**  PARAMETERS:  MV_U32 data
**               MV_U32 macId
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacCpqTxCtrlQueueWrite(MV_U32 data, MV_U32 macId)
{
	MV_STATUS status;

	if (macId >= EPON_MAX_MAC_NUM)
		return MV_ERROR;

	status = asicOntGlbRegWrite(mvAsicReg_EPON_CPQ_TX_CTRL_Q_WRITE, data, macId);

	return status;
}

/* ========================================================================== */
/*                        PPv2 Functions Section                               */
/* ========================================================================== */

/*******************************************************************************
**
**  mvOnuEponMacPPv2DbaLlidAndQueueIndirectAccess
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function configure PPv2 indirect register access
**
**  PARAMETERS:  MV_U32 llidIndex
**               MV_U32 queueIndex
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacPPv2DbaLlidAndQueueIndirectAccess(MV_U32 llidIndex, MV_U32 queueIndex)
{
	MV_STATUS status;
	MV_U32 access;

	access = (((llidIndex & 0xF) << 3) | (queueIndex & 0x7));

	status = asicOntMiscRegWrite(mvAsicReg_PPv2_TX_PON_BYTE_CNT_INDEX, access, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacPPv2DbaQueueOverheadeSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function configure PPv2 Queue overhead value
**
**  PARAMETERS:  MV_U32 queueOverhead
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacPPv2DbaQueueOverheadSet(MV_U32 queueOverhead)
{
	MV_STATUS status;

	status = asicOntMiscRegWrite(mvAsicReg_PPv2_TX_PON_BYTE_CNT_CONFIG1_QUE_OVR, queueOverhead, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacPPv2DbaPacketModSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function configure PPv2 packet modification value
**
**  PARAMETERS:  MV_U32 packetMod
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacPPv2DbaPktModSet(MV_U32 packetMod)
{
	MV_STATUS status;

	status = asicOntMiscRegWrite(mvAsicReg_PPv2_TX_PON_BYTE_CNT_CONFIG1_PKT_MOD, packetMod, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacPPv2DbaPostFecSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function configure PPv2 Post FEc value
**
**  PARAMETERS:  MV_U32 postFecOverhead
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacPPv2DbaPostFecSet(MV_U32 postFecOverhead)
{
	MV_STATUS status;

	status = asicOntMiscRegWrite(mvAsicReg_PPv2_TX_PON_BYTE_CNT_CONFIG2_POST_FEC, postFecOverhead, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacPPv2DbaFecStateSet
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function configure PPv2 FEC state
**
**  PARAMETERS:  MV_U32 fecState
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacPPv2DbaFecStateSet(MV_U32 fecState)
{
	MV_STATUS status;

	status = asicOntMiscRegWrite(mvAsicReg_PPv2_PON_FEC, fecState, 0);

	return status;
}

/*******************************************************************************
**
**  mvOnuEponMacSerdesPuRxWrite
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function write to serdes power up receiver
**
**  PARAMETERS:  enable - 0: power down, 1:  power up
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or MV_ERROR
**
*******************************************************************************/
MV_STATUS mvOnuEponMacSerdesPuRxWrite(MV_U32 enable)
{
	MV_STATUS status = MV_OK;

#ifndef PON_FPGA
#ifdef DISABLE_SERDES_FOR_AVANTA_LP
	status = asicOntMiscRegWrite(mvAsicReg_PON_SERDES_PHY_CTRL_0_PU_RX, enable, 0);
#endif
#endif  /* PON_FPGA */
	return status;
}

/******************************************************************************/
/******************************************************************************/
/* ========================================================================== */
/* ========================================================================== */
/* ==                                                                      == */
/* ==           ==     ==   =========   =========   ==        ==           == */
/* ==           ==     ==   =========   =========   ===       ==           == */
/* ==           ==     ==   ==     ==   ==     ==   == ==     ==           == */
/* ==           ==     ==   ==     ==   ==     ==   ==  ==    ==           == */
/* ==           ==     ==   =========   ==     ==   ==   ==   ==           == */
/* ==           ==     ==   =========   ==     ==   ==    ==  ==           == */
/* ==           ==     ==   ==          ==     ==   ==     == ==           == */
/* ==           ==     ==   ==          ==     ==   ==      ====           == */
/* ==           =========   ==          =========   ==       ===           == */
/* ==           =========   ==          =========   ==        ==           == */
/* ==                                                                      == */
/* ========================================================================== */
/* ========================================================================== */
/******************************************************************************/
/******************************************************************************/

/*******************************************************************************
**
**  mvOnuPonMacDgInterruptEn
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function configure Dying Gasp interrupt
**
**  PARAMETERS:  void
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuPonMacDgInterruptEn(MV_U32 dgPol)
{
	MV_STATUS status = MV_OK;

#ifndef PON_FPGA
#ifdef DISABLE_DG_FOR_AVANTA_LP
	status  = asicOntMiscRegWrite(mvAsicReg_PON_DG_CTRL_EN, 0x1, 0); /* Enable */
	if (status != MV_OK)
		return status;

	status  = asicOntMiscRegWrite(mvAsicReg_PON_DG_CTRL_POLARITY, dgPol, 0); /* Active High/Low */
	if (status != MV_OK)
		return status;

	status  = asicOntMiscRegWrite(mvAsicReg_PON_DG_THRESHOLD, 0x61A80, 0); /* Threshold */
	if (status != MV_OK)
		return status;
#endif
#endif  /* PON_FPGA */
	return status;
}

/*******************************************************************************
**
**  mvOnuPonMacDgInterruptDis
**  ____________________________________________________________________________
**
**  DESCRIPTION: The function disable Dying Gasp interrupt
**
**  PARAMETERS:  void
**
**  OUTPUTS:     None
**
**  RETURNS:     MV_OK or error
**
*******************************************************************************/
MV_STATUS mvOnuPonMacDgInterruptDis(void)
{
	MV_STATUS status = MV_OK;

#ifndef PON_FPGA
#ifdef DISABLE_DG_FOR_AVANTA_LP
	status  = asicOntMiscRegWrite(mvAsicReg_PON_DG_CTRL_EN, 0x0, 0); /* Disable */
	if (status != MV_OK)
		return status;
#endif  /* DISABLE_DG_FOR_AVANTA_LP */
#endif  /* PON_FPGA */
	return status;
}
