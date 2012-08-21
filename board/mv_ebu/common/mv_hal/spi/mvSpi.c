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

#include "mvCommon.h"
#include "mvOs.h"
#include "mvSpiSpec.h"
#include "mvSpi.h"
#include "mvSysSpi.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"

/* #define MV_DEBUG */
#ifdef MV_DEBUG
#define DB(x) x
#else
#define DB(x)
#endif

static MV_SPI_HAL_DATA	spiHalData;

static MV_SPI_TYPE_INFO spiTypes[] = {
	{
		.en16Bit = MV_TRUE,
		.byteCsAsrt = MV_FALSE,
		.baudRate = (20 << 20) /*  20M */
	},
	{
		.en16Bit = MV_FALSE,
		.byteCsAsrt = MV_TRUE,
		.baudRate = _8M
	}
};

MV_SPI_TYPE_INFO *currSpiInfo = NULL;

/*******************************************************************************
* mvSpiParamsSet
*
* DESCRIPTION:
*	Set SPI driver parameters.
*	This will affect the behaviour of the SPI APIs after this call.
*
* INPUT:
*	spiId - Controller ID
*	csId - chip select ID
*	type - The type to set.
*
* OUTPUT:
*	None.
*
* RETURNS:
*	MV_OK on success,
*	MV_ERROR otherwise.
*
********************************************************************************/
MV_STATUS mvSpiParamsSet(MV_U8 spiId, MV_U8 csId, MV_SPI_TYPE type)
{
	if (csId > (MV_SPI_MAX_CS - 1)) {
		mvOsPrintf("Error, csId(%d) exceeded maximum(%d)\n", csId, (MV_SPI_MAX_CS - 1));
		return MV_ERROR;
	}

#if (MV_SPI_VERSION > 1)
	if (MV_OK != mvSpiCsSet(spiId, csId)) {
		mvOsPrintf("Error, setting SPI CS failed\n");
		return MV_ERROR;
	}
#endif

	if (currSpiInfo != (&(spiTypes[type]))) {
		currSpiInfo = &(spiTypes[type]);
		mvSpiBaudRateSet(spiId, currSpiInfo->baudRate);
	}

	return MV_OK;
}


/*******************************************************************************
* mvSpi16bitDataTxRx - Transmt and receive data
*
* DESCRIPTION:
*       Tx data and block waiting for data to be transmitted
*
********************************************************************************/
MV_STATUS mvSpi16bitDataTxRx(MV_U8 spiId, MV_U16 txData, MV_U16 *pRxData)
{
    MV_U32 i;
    MV_BOOL ready = MV_FALSE;


    /* First clear the bit in the interrupt cause register */
    MV_REG_WRITE(MV_SPI_INT_CAUSE_REG(spiId), 0x0);

    /* Transmit data */
    MV_REG_WRITE(MV_SPI_DATA_OUT_REG(spiId), MV_16BIT_LE(txData));

    /* wait with timeout for memory ready */
    for (i = 0; i < MV_SPI_WAIT_RDY_MAX_LOOP; i++) {
	if (MV_REG_READ(MV_SPI_INT_CAUSE_REG(spiId))) {
		ready = MV_TRUE;
		break;
	}
#ifdef MV_SPI_SLEEP_ON_WAIT
	mvOsSleep(1);
#endif /* MV_SPI_SLEEP_ON_WAIT */
    }

    if (!ready)
	return MV_TIMEOUT;

    /* check that the RX data is needed */
    if (pRxData) {
	if ((MV_U32)pRxData &  0x1) { /* check if address is not alligned to 16bit */
#if defined(MV_CPU_LE)
		/* perform the data write to the buffer in two stages with 8bit each */
		MV_U8 *bptr = (MV_U8 *)pRxData;
		MV_U16 data = MV_16BIT_LE(MV_REG_READ(MV_SPI_DATA_IN_REG(spiId)));
		*bptr = (data & 0xFF);
		++bptr;
		*bptr = ((data >> 8) & 0xFF);

#elif defined(MV_CPU_BE)

		/* perform the data write to the buffer in two stages with 8bit each */
		MV_U8 *bptr = (MV_U8 *)pRxData;
		MV_U16 data = MV_16BIT_LE(MV_REG_READ(MV_SPI_DATA_IN_REG(spiId)));
		*bptr = ((data >> 8) & 0xFF);
		++bptr;
		*bptr = (data & 0xFF);

#else
    #error "CPU endianess isn't defined!\n"
#endif

	} else
		*pRxData = MV_16BIT_LE(MV_REG_READ(MV_SPI_DATA_IN_REG(spiId)));
    }

    return MV_OK;
}


/*******************************************************************************
* mvSpi8bitDataTxRx - Transmt and receive data (8bits)
*
* DESCRIPTION:
*       Tx data and block waiting for data to be transmitted
*
********************************************************************************/
MV_STATUS mvSpi8bitDataTxRx(MV_U8 spiId, MV_U8 txData, MV_U8 *pRxData)
{
	MV_U32 i;
	MV_BOOL ready = MV_FALSE;

	if (currSpiInfo->byteCsAsrt)
		mvSpiCsAssert(spiId);

	/* First clear the bit in the interrupt cause register */
	MV_REG_WRITE(MV_SPI_INT_CAUSE_REG(spiId), 0x0);

	/* Transmit data */
	MV_REG_WRITE(MV_SPI_DATA_OUT_REG(spiId), txData);

	/* wait with timeout for memory ready */
	for (i = 0; i < MV_SPI_WAIT_RDY_MAX_LOOP; i++) {
		if (MV_REG_READ(MV_SPI_INT_CAUSE_REG(spiId))) {
			ready = MV_TRUE;
			break;
		}
#ifdef MV_SPI_SLEEP_ON_WAIT
		mvOsSleep(1);
#endif /* MV_SPI_SLEEP_ON_WAIT */
	}

	if (!ready) {
		if (currSpiInfo->byteCsAsrt) {
			mvSpiCsDeassert(spiId);
			/* WA to compansate Zarlink SLIC CS off time */
			mvOsUDelay(4);
		}
		return MV_TIMEOUT;
	}

	/* check that the RX data is needed */
	if (pRxData)
		*pRxData = MV_REG_READ(MV_SPI_DATA_IN_REG(spiId));

	if (currSpiInfo->byteCsAsrt) {
		mvSpiCsDeassert(spiId);
		/* WA to compansate Zarlink SLIC CS off time */
		mvOsUDelay(4);
	}

	return MV_OK;
}

/*
#####################################################################################
#####################################################################################
*/

#if (MV_SPI_VERSION > 1)
/*******************************************************************************
* mvSpiCsSet -
*
* DESCRIPTION:
*	Set the Chip-Select to which the next SPI transaction will be
*	addressed to.
*
* INPUT:
*       csId: The Chip-Select ID to set.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Success or Error code.
*
*******************************************************************************/
MV_STATUS mvSpiCsSet(MV_U8 spiId, MV_U8 csId)
{
	MV_U32	ctrlReg;
	static MV_U8 lastCsId = 0xFF;

	if (csId > 7)
		return MV_BAD_PARAM;

	if (lastCsId == csId)
		return MV_OK;

	ctrlReg = MV_REG_READ(MV_SPI_IF_CTRL_REG(spiId));
	ctrlReg &= ~MV_SPI_CS_NUM_MASK;
	ctrlReg |= (csId << MV_SPI_CS_NUM_OFFSET);
	MV_REG_WRITE(MV_SPI_IF_CTRL_REG(spiId), ctrlReg);

	lastCsId = csId;

	return MV_OK;
}


/*******************************************************************************
* mvSpiIfConfigSet -
*
* DESCRIPTION:
*	Set the SPI interface parameters.
*
* INPUT:
*       spiId: The SPI controller ID to setup.
*	ifParams: The interface parameters.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Success or Error code.
*
*******************************************************************************/
MV_STATUS mvSpiIfConfigSet(MV_U8 spiId, MV_SPI_IF_PARAMS *ifParams)
{
	MV_U32	ctrlReg;

	ctrlReg = MV_REG_READ(MV_SPI_IF_CONFIG_REG(spiId));

	/* Set Clock Polarity */
	ctrlReg &= ~(MV_SPI_CPOL_MASK | MV_SPI_CPHA_MASK |
			MV_SPI_TXLSBF_MASK | MV_SPI_RXLSBF_MASK);
	if (ifParams->clockPolLow)
		ctrlReg |= MV_SPI_CPOL_MASK;

	if (ifParams->clockPhase == SPI_CLK_BEGIN_CYC)
		ctrlReg |= MV_SPI_CPHA_MASK;

	if (ifParams->txMsbFirst)
		ctrlReg |= MV_SPI_TXLSBF_MASK;

	if (ifParams->rxMsbFirst)
		ctrlReg |= MV_SPI_RXLSBF_MASK;

	MV_REG_WRITE(MV_SPI_IF_CONFIG_REG(spiId), ctrlReg);

	return MV_OK;
}


/*******************************************************************************
* mvSpiTimingParamsSet -
*
* DESCRIPTION:
*	Set the SPI timing params.
*
* INPUT:
*       spiId: The SPI controller ID to setup.
*	tmngParams: The interface timing parameters to configure.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Success or Error code.
*
*******************************************************************************/
MV_STATUS mvSpiTimingParamsSet(MV_U8 spiId, MV_SPI_TIMING_PARAMS *tmngParams)
{
	MV_U32	ctrlReg;

	ctrlReg = MV_REG_READ(MV_SPI_TMNG_PARAMS_REG(spiId));

	/* Set Clock Polarity */
	ctrlReg &= ~(MV_SPI_TCSH_MASK | MV_SPI_TMISO_SAMPLE_MASK | MV_SPI_TCS_SETUP_MASK
			| MV_SPI_TCS_HOLD_MASK);
	ctrlReg |= ((tmngParams->tcsh << MV_SPI_TCSH_OFFSET) |
			(tmngParams->tmisoSample << MV_SPI_TMISO_SAMPLE_OFFSET) |
			(tmngParams->tcsSetup << MV_SPI_TCS_SETUP_OFFSET) |
			(tmngParams->tcsHold << MV_SPI_TCS_HOLD_OFFSET));

	MV_REG_WRITE(MV_SPI_TMNG_PARAMS_REG(spiId), ctrlReg);

	return MV_OK;
}

#endif /* (MV_SPI_VERSION > 1) */

/*******************************************************************************
* mvSpiInit - Initialize the SPI controller
*
* DESCRIPTION:
*       Perform the neccessary initialization in order to be able to send an
*		receive over the SPI interface.
*
* INPUT:
*       serialBaudRate: Baud rate (SPI clock frequency)
*		use16BitMode: Whether to use 2bytes (MV_TRUE) or 1bytes (MV_FALSE)
*
* OUTPUT:
*       None.
*
* RETURN:
*       Success or Error code.
*
*
*******************************************************************************/
MV_STATUS mvSpiInit(MV_U8 spiId, MV_U32 serialBaudRate, MV_SPI_HAL_DATA *halData)
{
    MV_STATUS ret;

    mvOsMemcpy(&spiHalData, halData, sizeof(MV_SPI_HAL_DATA));

    /* Set the serial clock */
    ret = mvSpiBaudRateSet(spiId, serialBaudRate);
    if (ret != MV_OK)
	return ret;

	/* Configure the default SPI mode to be 16bit */
    MV_REG_BIT_SET(MV_SPI_IF_CONFIG_REG(spiId), MV_SPI_BYTE_LENGTH_MASK);

	/* Fix ac timing on SPI in 6183, 6183L and 78x00 only */
    if ((spiHalData.ctrlModel == MV_6183_DEV_ID) ||
		(spiHalData.ctrlModel == MV_6183L_DEV_ID) ||
		(spiHalData.ctrlModel == MV_78100_DEV_ID) ||
		(spiHalData.ctrlModel == MV_78200_DEV_ID) ||
		(spiHalData.ctrlModel == MV_76100_DEV_ID) ||
		(spiHalData.ctrlModel == MV_6323_DEV_ID) ||
		(spiHalData.ctrlModel == MV_6322_DEV_ID) ||
		(spiHalData.ctrlModel == MV_6321_DEV_ID))
			MV_REG_BIT_SET(MV_SPI_IF_CONFIG_REG(spiId), BIT14);

    /* Verify that the CS is deasserted */
    mvSpiCsDeassert(spiId);

    mvSpiParamsSet(spiId, 0, SPI_TYPE_FLASH);

    return MV_OK;
}

/*******************************************************************************
* mvSpiBaudRateSet - Set the Frequency of the SPI clock
*
* DESCRIPTION:
*       Set the Prescale bits to adapt to the requested baud rate (the clock
*       used for thr SPI).
*
* INPUT:
*       serialBaudRate: Baud rate (SPI clock frequency)
*
* OUTPUT:
*       None.
*
* RETURN:
*       Success or Error code.
*
*
*******************************************************************************/
#if (MV_SPI_VERSION == 1)
MV_STATUS mvSpiBaudRateSet(MV_U8 spiId, MV_U32 serialBaudRate)
{
	MV_U8 i;
	MV_U8 preScale[14] = { 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30};
	MV_U8 bestPrescaleIndx = 100;
	MV_U32 minBaudOffset = 0xFFFFFFFF;
	MV_U32 cpuClk = spiHalData.tclk; /*mvCpuPclkGet();*/
	MV_U32 tempReg;

	/* Find the best prescale configuration - less or equal */
	for (i = 0; i < 14; i++) {
		/* check for higher - irrelevent */
		if ((cpuClk / preScale[i]) > serialBaudRate)
			continue;

		/* check for exact fit */
		if ((cpuClk / preScale[i]) == serialBaudRate) {
			bestPrescaleIndx = i;
			break;
		}

		/* check if this is better than the previous one */
		if ((serialBaudRate - (cpuClk / preScale[i])) < minBaudOffset) {
			minBaudOffset = (serialBaudRate - (cpuClk / preScale[i]));
			bestPrescaleIndx = i;
		}
	}

	if (bestPrescaleIndx > 14) {
		mvOsPrintf("%s ERROR: SPI baud rate prescale error!\n", __func__);
		return MV_OUT_OF_RANGE;
	}

	/* configure the Prescale */
	tempReg = MV_REG_READ(MV_SPI_IF_CONFIG_REG(spiId));
	tempReg = ((tempReg & ~MV_SPI_CLK_PRESCALE_MASK) | (bestPrescaleIndx + 0x12));

	if (bestPrescaleIndx == 0) /* if we're using the highest clock, enable fast read. */
		tempReg |= MV_SPI_DIRECT_READ_MASK;

	MV_REG_WRITE(MV_SPI_IF_CONFIG_REG(spiId), tempReg);

	return MV_OK;
}

#elif (MV_SPI_VERSION == 2)

MV_STATUS mvSpiBaudRateSet(MV_U8 spiId, MV_U32 serialBaudRate)
{
	MV_U32 spr, sppr;
	MV_U32 divider;
	MV_U32 bestSpr = 0, bestSppr = 0;
	MV_U8 exactMatch = 0;
	MV_U32 minBaudOffset = 0xFFFFFFFF;
	MV_U32 cpuClk = spiHalData.tclk; /*mvCpuPclkGet();*/
	MV_U32 tempReg;

	/* Find the best prescale configuration - less or equal */
	for (spr = 1; spr <= 15; spr++) {
		for (sppr = 0; sppr <= 7; sppr++) {
			divider = spr * (1 << sppr);
			/* check for higher - irrelevent */
			if ((cpuClk / divider) > serialBaudRate)
				continue;

			/* check for exact fit */
			if ((cpuClk / divider) == serialBaudRate) {
				bestSpr = spr;
				bestSppr = sppr;
				exactMatch = 1;
				break;
			}

			/* check if this is better than the previous one */
			if ((serialBaudRate - (cpuClk / divider)) < minBaudOffset) {
				minBaudOffset = (serialBaudRate - (cpuClk / divider));
				bestSpr = spr;
				bestSppr = sppr;
			}
		}

		if (exactMatch == 1)
			break;
	}

	if (bestSpr == 0) {
		mvOsPrintf("%s ERROR: SPI baud rate prescale error!\n", __func__);
		return MV_OUT_OF_RANGE;
	}

	/* configure the Prescale */
	tempReg = MV_REG_READ(MV_SPI_IF_CONFIG_REG(spiId)) & ~(MV_SPI_SPR_MASK | MV_SPI_SPPR_0_MASK |
			MV_SPI_SPPR_HI_MASK);
	tempReg |= ((bestSpr << MV_SPI_SPR_OFFSET) |
			((bestSppr & 0x1) << MV_SPI_SPPR_0_OFFSET) |
			((bestSppr >> 1) << MV_SPI_SPPR_HI_OFFSET));
	MV_REG_WRITE(MV_SPI_IF_CONFIG_REG(spiId), tempReg);

	return MV_OK;
}

#else

#error "MV_SPI_VERSION is not defined."

#endif

/*******************************************************************************
* mvSpiCsAssert - Assert the Chip Select pin indicating a new transfer
*
* DESCRIPTION:
*       Assert The chip select - used to select an external SPI device
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Success or Error code.
*
********************************************************************************/
MV_VOID mvSpiCsAssert(MV_U8 spiId)
{
	/* For devices in which the SPI is muxed on the MPP with other interfaces*/
	mvSysSpiMppConfig(SYS_SPI_MPP_ENABLE);
	mvOsUDelay(1);
	MV_REG_BIT_SET(MV_SPI_IF_CTRL_REG(spiId), MV_SPI_CS_ENABLE_MASK);
}

/*******************************************************************************
* mvSpiCsDeassert - DeAssert the Chip Select pin indicating the end of a
*				  SPI transfer sequence
*
* DESCRIPTION:
*       DeAssert the chip select pin
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Success or Error code.
*
********************************************************************************/
MV_VOID mvSpiCsDeassert(MV_U8 spiId)
{
	MV_REG_BIT_RESET(MV_SPI_IF_CTRL_REG(spiId), MV_SPI_CS_ENABLE_MASK);

	/* For devices in which the SPI is muxed on the MPP with other interfaces*/
	mvSysSpiMppConfig(SYS_SPI_MPP_DEFAULT);
}

/*******************************************************************************
* mvSpiRead - Read a buffer over the SPI interface
*
* DESCRIPTION:
*       Receive (read) a buffer over the SPI interface in 16bit chunks. If the
*		buffer size is odd, then the last chunk will be 8bits. Chip select is not
*       handled at this level.
*
* INPUT:
*		pRxBuff: Pointer to the buffer to hold the received data
*		buffSize: length of the pRxBuff
*
* OUTPUT:
*		pRxBuff: Pointer to the buffer with the received data
*
* RETURN:
*       Success or Error code.
*
*
*******************************************************************************/
MV_STATUS mvSpiRead(MV_U8 spiId, MV_U8 *pRxBuff, MV_U32 buffSize)
{
    MV_STATUS ret;
	MV_U32 bytesLeft = buffSize;
	MV_U16 *rxPtr = (MV_U16 *)pRxBuff;

    /* check for null parameters */
    if (pRxBuff == NULL) {
	mvOsPrintf("%s ERROR: Null pointer parameter!\n", __func__);
	return MV_BAD_PARAM;
    }

    /* Check that the buffer pointer and the buffer size are 16bit aligned */
    if ((currSpiInfo->en16Bit) && (((MV_U32)buffSize & 1) == 0) && (((MV_U32)pRxBuff & 1) == 0)) {
	/* Verify that the SPI mode is in 16bit mode */
	MV_REG_BIT_SET(MV_SPI_IF_CONFIG_REG(spiId), MV_SPI_BYTE_LENGTH_MASK);

	/* TX/RX as long we have complete 16bit chunks */
	while (bytesLeft >= MV_SPI_16_BIT_CHUNK_SIZE) {
		/* Transmitted and wait for the transfer to be completed */
		ret = mvSpi16bitDataTxRx(spiId, MV_SPI_DUMMY_WRITE_16BITS, rxPtr);
		if (ret != MV_OK)
			return ret;

		/* increment the pointers */
		rxPtr++;
		bytesLeft -= MV_SPI_16_BIT_CHUNK_SIZE;
	}

    } else {
	/* Verify that the SPI mode is in 8bit mode */
	MV_REG_BIT_RESET(MV_SPI_IF_CONFIG_REG(spiId), MV_SPI_BYTE_LENGTH_MASK);

	/* TX/RX in 8bit chanks */
	while (bytesLeft > 0) {
		/* Transmitted and wait for the transfer to be completed */
		ret = mvSpi8bitDataTxRx(spiId, MV_SPI_DUMMY_WRITE_8BITS, pRxBuff);
		if (ret != MV_OK)
			return ret;
		/* increment the pointers */
		pRxBuff++;
		bytesLeft--;
	}
    }

	return MV_OK;
}

/*******************************************************************************
* mvSpiWrite - Transmit a buffer over the SPI interface
*
* DESCRIPTION:
*       Transmit a buffer over the SPI interface in 16bit chunks. If the
*		buffer size is odd, then the last chunk will be 8bits. No chip select
*       action is taken.
*
* INPUT:
*		pTxBuff: Pointer to the buffer holding the TX data
*		buffSize: length of the pTxBuff
*
* OUTPUT:
*       None.
*
* RETURN:
*       Success or Error code.
*
*
*******************************************************************************/
MV_STATUS mvSpiWrite(MV_U8 spiId, MV_U8 *pTxBuff, MV_U32 buffSize)
{
    MV_STATUS ret;
	MV_U32 bytesLeft = buffSize;
	MV_U16 *txPtr = (MV_U16 *)pTxBuff;

    /* check for null parameters */
    if (pTxBuff == NULL) {
	mvOsPrintf("%s ERROR: Null pointer parameter!\n", __func__);
	return MV_BAD_PARAM;
    }

    /* Check that the buffer pointer and the buffer size are 16bit aligned */
    if ((currSpiInfo->en16Bit)
	&& (currSpiInfo->en16Bit)
	&& (((MV_U32)buffSize & 1) == 0)
	&& (((MV_U32)pTxBuff & 1) == 0)) {
	/* Verify that the SPI mode is in 16bit mode */
	MV_REG_BIT_SET(MV_SPI_IF_CONFIG_REG(spiId), MV_SPI_BYTE_LENGTH_MASK);

	/* TX/RX as long we have complete 16bit chunks */
	while (bytesLeft >= MV_SPI_16_BIT_CHUNK_SIZE) {
	/* Transmitted and wait for the transfer to be completed */
		ret = mvSpi16bitDataTxRx(spiId, *txPtr, NULL);
		if (ret != MV_OK)
			return ret;
		/* increment the pointers */
		txPtr++;
		bytesLeft -= MV_SPI_16_BIT_CHUNK_SIZE;
	}
    } else {
	/* Verify that the SPI mode is in 8bit mode */
	MV_REG_BIT_RESET(MV_SPI_IF_CONFIG_REG(spiId), MV_SPI_BYTE_LENGTH_MASK);

	/* TX/RX in 8bit chanks */
	while (bytesLeft > 0) {
		/* Transmitted and wait for the transfer to be completed */
		ret = mvSpi8bitDataTxRx(spiId, *pTxBuff, NULL);
		if (ret != MV_OK)
			return ret;

		/* increment the pointers */
		pTxBuff++;
		bytesLeft--;
	}
    }
	return MV_OK;
}


/*******************************************************************************
* mvSpiReadWrite - Read and Write a buffer simultanuosely
*
* DESCRIPTION:
*       Transmit and receive a buffer over the SPI in 16bit chunks. If the
*		buffer size is odd, then the last chunk will be 8bits. The SPI chip
*       select is not handled implicitely.
*
* INPUT:
*       pRxBuff: Pointer to the buffer to write the RX info in
*		pTxBuff: Pointer to the buffer holding the TX info
*		buffSize: length of both the pTxBuff and pRxBuff
*
* OUTPUT:
*       pRxBuff: Pointer of the buffer holding the RX data
*
* RETURN:
*       Success or Error code.
*
*
*******************************************************************************/
MV_STATUS mvSpiReadWrite(MV_U8 spiId, MV_U8 *pRxBuff, MV_U8* pTxBuff, MV_U32 buffSize)
{
    MV_STATUS ret;
    MV_U32 bytesLeft = buffSize;
    MV_U16 *txPtr = (MV_U16 *)pTxBuff;
    MV_U16 *rxPtr = (MV_U16 *)pRxBuff;

	/* check for null parameters */
	if ((pRxBuff == NULL) || (pTxBuff == NULL)) {
		mvOsPrintf("%s ERROR: Null pointer parameter!\n", __func__);
		return MV_BAD_PARAM;
	}

	 /* Check that the buffer pointer and the buffer size are 16bit aligned */
	if ((currSpiInfo->en16Bit)
		&& (((MV_U32)buffSize & 1) == 0)
		&& (((MV_U32)pTxBuff & 1) == 0)
		&& (((MV_U32)pRxBuff & 1) == 0)) {
		/* Verify that the SPI mode is in 16bit mode */
		MV_REG_BIT_SET(MV_SPI_IF_CONFIG_REG(spiId), MV_SPI_BYTE_LENGTH_MASK);

		/* TX/RX as long we have complete 16bit chunks */
		while (bytesLeft >= MV_SPI_16_BIT_CHUNK_SIZE) {
		/* Transmitted and wait for the transfer to be completed */
			ret = mvSpi16bitDataTxRx(spiId, *txPtr, rxPtr);
			if (ret != MV_OK)
				return ret;
			/* increment the pointers */
			txPtr++;
			rxPtr++;
			bytesLeft -= MV_SPI_16_BIT_CHUNK_SIZE;
		}
	} else {
		/* Verify that the SPI mode is in 8bit mode */
		MV_REG_BIT_RESET(MV_SPI_IF_CONFIG_REG(spiId), MV_SPI_BYTE_LENGTH_MASK);

		/* TX/RX in 8bit chanks */
		while (bytesLeft > 0) {
			/* Transmitted and wait for the transfer to be completed */
			ret = mvSpi8bitDataTxRx(spiId, *pTxBuff, pRxBuff);
			if (ret != MV_OK)
				return ret;
			pRxBuff++;
			pTxBuff++;
			bytesLeft--;
		}
	}

	return MV_OK;
}


