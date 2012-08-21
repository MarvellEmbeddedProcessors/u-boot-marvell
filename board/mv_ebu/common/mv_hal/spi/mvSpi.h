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

#ifndef __INCmvSpihH
#define __INCmvSpihH

#ifdef __cplusplus
extern "C" {
#endif

#include "mvCommon.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "mvSysSpiConfig.h"

typedef struct {
	MV_U16		ctrlModel;
	MV_U32		tclk;
} MV_SPI_HAL_DATA;

#if (MV_SPI_VERSION > 1)

typedef struct {
	MV_BOOL		clockPolLow;
	enum {
		SPI_CLK_HALF_CYC,
		SPI_CLK_BEGIN_CYC
	}		clockPhase;
	MV_BOOL		txMsbFirst;
	MV_BOOL		rxMsbFirst;
} MV_SPI_IF_PARAMS;


typedef struct {
	MV_U8	tcsh;
	MV_U8	tmisoSample;
	MV_U8	tcsSetup;
	MV_U8	tcsHold;
} MV_SPI_TIMING_PARAMS;

#endif

typedef struct {
	/* Does this device support 16 bits access */
	MV_BOOL	en16Bit;
	/* should we assert / disassert CS for each byte we read / write */
	MV_BOOL	byteCsAsrt;
	MV_U32	baudRate;
} MV_SPI_TYPE_INFO;

typedef enum {
	SPI_TYPE_FLASH = 0,
	SPI_TYPE_SLIC
} MV_SPI_TYPE;


/* Function Prototypes */
/* Init */
MV_STATUS mvSpiInit(MV_U8 spiId, MV_U32 serialBaudRate, MV_SPI_HAL_DATA *halData);
MV_STATUS mvSpi8bitDataTxRx(MV_U8 spiId, MV_U8 txData, MV_U8 *pRxData);
MV_STATUS mvSpi16bitDataTxRx(MV_U8 spiId, MV_U16 txData, MV_U16 *pRxData);

/* Set the Frequency of the Spi clock */
MV_STATUS   mvSpiBaudRateSet(MV_U8 spiId, MV_U32 serialBaudRate);

/* Assert the SPI chip select */
MV_VOID     mvSpiCsAssert(MV_U8 spiId);

/* De-assert the SPI chip select */
MV_VOID     mvSpiCsDeassert(MV_U8 spiId);

/* Simultanuous Read and write */
MV_STATUS	mvSpiReadWrite(MV_U8 spiId, MV_U8 *pRxBuff, MV_U8* pTxBuff, MV_U32 buffSize);

/* serialize a buffer on the TX line - Rx is ignored */
MV_STATUS	mvSpiWrite(MV_U8 spiId, MV_U8 *pTxBuff, MV_U32 buffSize);

/* read from the RX line by writing dummy values to the TX line */
MV_STATUS	mvSpiRead(MV_U8 spiId, MV_U8 *pRxBuff, MV_U32 buffSize);

/* Set SPI parameters before any transaction is issued */
MV_STATUS mvSpiParamsSet(MV_U8 spiId, MV_U8 csId, MV_SPI_TYPE type);

#if (MV_SPI_VERSION > 1)

MV_STATUS mvSpiCsSet(MV_U8 spiId, MV_U8 csId);
MV_STATUS mvSpiIfConfigSet(MV_U8 spiId, MV_SPI_IF_PARAMS *ifParams);
MV_STATUS mvSpiTimingParamsSet(MV_U8 spiId, MV_SPI_TIMING_PARAMS *tmngParams);

#endif

#ifdef __cplusplus
}
#endif

#endif /* __INCmvSpihH */
