/* Copyright 2013 Marvell International Ltd, See included file for licensing information */

/********************************************************************
This file contains functions prototypes and global defines/data for
higher-level functions using MDIO access to perform resets, perform
queries of the phy, etc. that are necessary to control and read
status of the Marvell 88X3220/88X3310 ethernet PHY.
********************************************************************/
#include "mvOs.h"
#include "mvSwitch.h"
#include "mvEthPhy.h"
#include "mvSwitchRegs.h"
#include "mvCtrlEnvLib.h"
#include "mvBoardEnvLib.h"

#include "ctrlEnv/mvCtrlNetCompLib.h"
#include "mvEthPhyXsmi.h"

#include "mvEthFwDownload.h"

#define MV_ETH_XCVR_DEBUG          0


#if MV_ETH_XCVR_DEBUG
#define MV_DEBUG(x)    printf(x)
#else
#define MV_DEBUG(x)
#endif

MV_STATUS mvMdioFlashDownload(CTX_PTR_TYPE contextPtr, MV_U16 port,
						MV_U8 data[], MV_U32 size, MV_U16 *errCode);

MV_STATUS mvMdioRamDownload(CTX_PTR_TYPE contextPtr, MV_U8 data[],
			MV_U32 size, MV_U16 port, MV_U8 use_ram_checksum, MV_U16 *errCode);

/******************************************************************************
MV_STATUS mvPutPhyInMdioDownloadMode(CTX_PTR_TYPE contextPtr, MV_U16 port);

 Inputs:
	contextPtr - pointer to host's private data
	port - MDIO port address, 0-31

 Outputs:
	None

 Returns:
	MV_OK or MV_FAIL

 Description:
	Sets override of SPI_CONFIG to 1, then resets the T-unit so boot code will
	wait for MDIO download of code to RAM (or allow code to be downloaded into
	flash for a flash update).

 Side effects:
	None

 Notes/Warnings:
	None
******************************************************************************/
MV_STATUS mvPutPhyInMdioDownloadMode(CTX_PTR_TYPE contextPtr, MV_U16 port);

/******************************************************************************
 MV_STATUS mvTunitHwReset (CTX_PTR_TYPE contextPtr, MV_U16 port )

 Inputs:
	contextPtr - pointer to host's private data
	port - port number, 0-31

 Outputs:
	None

 Returns:
	MV_OK or MV_FAIL

 Description:
	Performs a hardware reset on the T Unit. All register values will return
	to their hardware reset state.

	If the firmware was loaded from flash, it will be reloaded from flash.

	If the chip was strapped to load firmware from the host, the firmware
	must be reloaded.

	This bit self clears.

 Side effects:
	None

 Notes/Warnings:
	Any strap values overriden by register 31.F008 will be latched in and take
	effect when this reset is performed.

	Note that this is the major difference between this hardware reset and
	the special software reset, 1.49152.15 in the T Unit.

	The T Unit hardware reset in the C Unit must be used if any new values
	in 31.F008 need to be re-latched to pick up the new strap values.

******************************************************************************/
MV_STATUS mvTunitHwReset(CTX_PTR_TYPE contextPtr, MV_U16 port);

MV_STATUS mvHwXmdioWrite(CTX_PTR_TYPE contextPtr, MV_U16 port, MV_U16 dev,
				MV_U16 reg, MV_U16 value)
{
	MV_STATUS result = MV_OK;

	/* NSS need to be enabled before each access and disabled right after,
		in order to access the PHY registers via PSS window */
	mvNetComplexNssSelect(1);
	result = mvEthPhyXsmiRegWrite((MV_U32)port, (MV_U32)dev, (MV_U16)reg, (MV_U16)value);
	mvNetComplexNssSelect(0);

	return result;
}

MV_STATUS mvHwXmdioRead(CTX_PTR_TYPE contextPtr, MV_U16 port, MV_U16 dev,
				MV_U16 reg, MV_U16 *data)
{
	MV_STATUS result = MV_OK;

	/* NSS need to be enabled before each access and disabled right after,
		in order to access the PHY registers via PSS window */
	mvNetComplexNssSelect(1);
	result = mvEthPhyXsmiRegRead((MV_U32)port, (MV_U32)dev, (MV_U16)reg, (MV_U16 *)data);
	mvNetComplexNssSelect(0);

	return result;
}

/*
	This macro calculates the mask for partial read/write of register's data.
*/
#define MV_CALC_MASK(fieldOffset, fieldLen, mask)		\
			if ((fieldLen + fieldOffset) >= 16)	  \
				mask = (0 - (1 << fieldOffset));	\
			else									\
				mask = (((1 << (fieldLen + fieldOffset))) - (1 << fieldOffset))


MV_STATUS mvHwGetRegFieldFromWord(MV_U16 regData, MV_U8	fieldOffset,
					MV_U8 fieldLength, MV_U16 *data)
{
	MV_U16 mask;			/* Bits mask to be read */

	MV_CALC_MASK(fieldOffset, fieldLength, mask);
	*data = (regData & mask) >> fieldOffset;

	return MV_OK;
}

MV_STATUS mvHwGetPhyRegField(CTX_PTR_TYPE contextPtr, MV_U16 port, MV_U16 dev,
	MV_U16 regAddr,	MV_U8 fieldOffset, MV_U8 fieldLength, MV_U16 *data)
{
	MV_U16 tmpData;
	MV_STATUS   retVal;

	retVal = mvHwXmdioRead(contextPtr, port, dev, regAddr, &tmpData);
	if (retVal != MV_OK) {
		printf("Failed to read register\n");
		return MV_FAIL;
	}
	mvHwGetRegFieldFromWord(tmpData, fieldOffset, fieldLength, data);

	return MV_OK;
}

MV_STATUS mvHwSetRegFieldToWord(MV_U16 regData, MV_U16 bitFieldData, MV_U8 fieldOffset,
			MV_U8 fieldLength, MV_U16 *data)
{
	MV_U16 mask;			/* Bits mask to be read */

	MV_CALC_MASK(fieldOffset, fieldLength, mask);
	/* Set the desired bits to 0.					   */
	regData &= ~mask;
	/* Set the given data into the above reset bits.	*/
	regData |= ((bitFieldData << fieldOffset) & mask);
	*data = regData;

	return MV_OK;
}

MV_STATUS mvHwSetPhyRegField(CTX_PTR_TYPE contextPtr, MV_U16 port, MV_U16 dev,
		MV_U16 regAddr,	MV_U8 fieldOffset, MV_U8 fieldLength, MV_U16 data)
{
	MV_U16 tmpData, newData;
	MV_STATUS   retVal;

	retVal = mvHwXmdioRead(contextPtr, port, dev, regAddr, &tmpData);
	if (retVal != MV_OK) {
		printf("Failed to read register\n");
		return MV_FAIL;
	}
	mvHwSetRegFieldToWord(tmpData, data, fieldOffset, fieldLength, &newData);
	retVal = mvHwXmdioWrite(contextPtr, port, dev, regAddr, newData);
	if (retVal != MV_OK) {
		printf("Failed to XmdioWrite\n");
		MV_DEBUG("Failed to write register\n");
		return MV_FAIL;
	}

	return MV_OK;
}

MV_STATUS mvTunitHwReset(CTX_PTR_TYPE contextPtr, MV_U16 port)
{
	return mvHwSetPhyRegField(contextPtr, port, MV_CUNIT_PORT_CTRL, 12, 1, 1);
}

MV_STATUS mvPutPhyInMdioDownloadMode(CTX_PTR_TYPE contextPtr, MV_U16 port)
{
	if (mvHwSetPhyRegField(contextPtr, port, 31, 0xF008, 5, 1, 1) == MV_OK) {
		/* mvSpecialSoftwareReset() will not work here. Must use
		   the external hardware T-unit reset */
		return mvTunitHwReset(contextPtr, port);
	} else
		return MV_FAIL;
}

MV_STATUS mvRemovePhyMdioDownloadMode(CTX_PTR_TYPE contextPtr, MV_U16 port)
{
	if (mvHwSetPhyRegField(contextPtr, port, 31, 0xF008, 5, 1, 0) == MV_OK) {
		/* mvSpecialSoftwareReset() will not work here. Must use
		   the external hardware T-unit reset */
		return mvTunitHwReset(contextPtr, port);
	} else
		return MV_FAIL;
}


/* Internal PHY Registers for downloading to RAM */
#define MV_LOW_WORD_REG	 3, 0xD0F0 /* register to set the low part of the address */
#define MV_HI__WORD_REG	 3, 0xD0F1 /* register to set the hi part of the address */
#define MV_RAM_DATA_REG	 3, 0xD0F2 /* register to write or read to/from ram */
#define MV_RAM_CHECKSUM_REG 3, 0xD0F3 /* register to read the checksum from */


/* This handles downloading an image pointed to by data which is size bytes long
   to the phy's flash interfacing with the slave code as a helper program.
   Size must be an even number (the flash can only be written to in words).*/
MV_STATUS mvMdioFlashDownload(CTX_PTR_TYPE contextPtr, MV_U16 port,
					MV_U8 data[], MV_U32 size, MV_U16 *errCode)
{
	MV_U16 buf_checksum, tmp_checksum, reported_checksum, words_rcvd, words_written, tmp;
	MV_U32 maxBuffSize = 0, numTransfers, lastTransferSize, transferIndex;
	MV_U32 byteIndex, stopIndex;

	*errCode = 0;

	if (size%2) {
		/* it's an error, size must be an even number of bytes */
		*errCode = MV_SIZE_NOT_EVEN;
		return MV_FAIL;
	}
	/* first erase the flash*/
	MV_DEBUG("Slave will now erase flash. This may take up to 6 seconds.\n");
	TRY_IO(mvHwXmdioWrite(contextPtr, port, COMMAND_REG, ERASE_FLASH_PROGRAM_AREA));
	TRY_IO(mvHwXmdioRead(contextPtr, port, COMMAND_REG, &tmp));
	while (tmp == ERASE_FLASH_PROGRAM_AREA || tmp == MV_SLAVE_FLASH_BUSY) {
		TRY_IO(mvHwXmdioRead(contextPtr, port, COMMAND_REG, &tmp));
	}
	if (tmp == MV_SLAVE_ERR_CMD) {
		*errCode = MV_ERR_ERASING_FLASH;
		return MV_FAIL;
	} else {
		if (tmp == MV_SLAVE_OK) {
			MV_DEBUG("Flash program areas have been erased.\n");
		} else {
			/* unexpected value read back from download code*/
			MV_DEBUG("Unexpected response from phy. Exiting...\n");
			*errCode = MV_ERR_VALUE_READ_BACK;
			return MV_FAIL;
		}
	}

	/* read in the max buffer size from the slave*/
	/* this is the maximum size that can be written at any 1 time*/
	TRY_IO(mvHwXmdioRead(contextPtr, port, MAX_BUFF_SIZE_OUT_REG, (MV_U16 *)&maxBuffSize));
	maxBuffSize *= 2; /* now size is in bytes */
	numTransfers = size/maxBuffSize;
	lastTransferSize = size%maxBuffSize;
	/* handle all the full transfers */
	byteIndex = 0;
	for (transferIndex = 0; transferIndex < numTransfers; transferIndex++) {
		/* Set the flash start address*/
		TRY_IO(mvHwXmdioWrite(contextPtr, port, LOW_ADDRESS_REG, (MV_U16)byteIndex));
		TRY_IO(mvHwXmdioWrite(contextPtr, port, HIGH_ADDRESS_REG, (MV_U16)(byteIndex>>16)));
		/* Set the size of the buffer we're going to send*/
		TRY_IO(mvHwXmdioWrite(contextPtr, port, ACTUAL_BUFF_SIZE_IN_REG, (MV_U16)(maxBuffSize/2)));
		/* Tell the slave we've written the start address and size
		/ and now we're going to start writing data*/
		TRY_IO(mvHwXmdioWrite(contextPtr, port, COMMAND_REG, FILL_BUFFER));
		/* Wait for MV_SLAVE_OK*/
		TRY_IO(mvHwXmdioRead(contextPtr, port, COMMAND_REG, &tmp));
		while (tmp == ERASE_FLASH_PROGRAM_AREA || tmp == MV_SLAVE_FLASH_BUSY) {
			TRY_IO(mvHwXmdioRead(contextPtr, port, COMMAND_REG, &tmp));
		}
		if (tmp != MV_SLAVE_OK) {
			*errCode = MV_ERR_START_WRITE_DATA;
			return MV_FAIL;
		}
		/* Write a buffer of data to the slave RAM*/
		stopIndex = byteIndex + maxBuffSize;
		buf_checksum = 0;
		while (byteIndex < stopIndex) {
			MV_U16 value;

			value = data[byteIndex++];
			value |= (((MV_U16)data[byteIndex++]) << 8);
			buf_checksum += value;
			TRY_IO(mvHwXmdioWrite(contextPtr, port, DATA_REG, value));
		}
		/* check and see if we can go on to the write*/
		TRY_IO(mvHwXmdioRead(contextPtr, port, CHECKSUM_REG, &tmp_checksum));
		TRY_IO(mvHwXmdioRead(contextPtr, port, WORDS_RCVD_REG, &words_rcvd));
		if (tmp_checksum != buf_checksum || words_rcvd != (MV_U16)(maxBuffSize/2)) {
			/* Host might want to issue a retry here instead failing*/
			/* Note that the flash start address must be reset before resending the buffer*/
			MV_DEBUG("Slave failed to get all the data correctly\n");
			*errCode = MV_START_WRITE_DATA;
			return MV_FAIL;
		}
		/* One full RAM buffer inside DSP is ready to write to flash now*/
		/* Tell the slave to write it*/
		TRY_IO(mvHwXmdioWrite(contextPtr, port, COMMAND_REG, WRITE_BUFFER));
		MV_DEBUG("Waiting for slave to finish programming flash");
		/* Wait for MV_SLAVE_OK */
		TRY_IO(mvHwXmdioRead(contextPtr, port, COMMAND_REG, &tmp));
		while (tmp == WRITE_BUFFER || tmp == MV_SLAVE_FLASH_BUSY) {
			/* this can take several 2-3 seconds, don't poll phy too frequently*/
			udelay(500000);
			/* since every read causes an interrupt on the phy */
			TRY_IO(mvHwXmdioRead(contextPtr, port, COMMAND_REG, &tmp));
		}
		if (tmp == MV_SLAVE_ERR_CMD) {
			MV_DEBUG("\nSome kind of error occurred on Slave. Exiting\n");
			*errCode = MV_ERR_ON_SLAVE;
			return MV_FAIL;
		} else {
			if (tmp != MV_SLAVE_OK) {
				*errCode = MV_ERR_ON_SLAVE;
				return MV_FAIL;

			} else {
				/* readback checksum of what was stored in flash */
				TRY_IO(mvHwXmdioRead(contextPtr, port, CHECKSUM_REG, &reported_checksum));
				if (reported_checksum != buf_checksum) {
					*errCode = MV_ERR_CHECKSUM;
					return MV_FAIL;
				}
			}
		}
		TRY_IO(mvHwXmdioRead(contextPtr, port, WORDS_WRITTEN_REG, &words_written));
		if (words_written != (maxBuffSize/2)) {
			MV_DEBUG("\nSlave didn't write enough words to flash. Error occurred. Exit\n");
			*errCode = MV_ERR_SLAVE_WRITE_FULL;
			return MV_FAIL;

		}
		MV_DEBUG("\n");
	}
	/* now handle last transfer */
	if (lastTransferSize) {
		/* Set the flash start address */
		TRY_IO(mvHwXmdioWrite(contextPtr, port, LOW_ADDRESS_REG, (MV_U16)byteIndex));
		TRY_IO(mvHwXmdioWrite(contextPtr, port, HIGH_ADDRESS_REG, (MV_U16)(byteIndex>>16)));
		/* Set the size of the buffer we're going to send */
		TRY_IO(mvHwXmdioWrite(contextPtr, port, ACTUAL_BUFF_SIZE_IN_REG, (MV_U16)(lastTransferSize/2)));
		/* Tell the slave we've written the start address and size */
		/* and now we're going to start writing data */
		TRY_IO(mvHwXmdioWrite(contextPtr, port, COMMAND_REG, FILL_BUFFER));
		/* Wait for MV_SLAVE_OK */
		TRY_IO(mvHwXmdioRead(contextPtr, port, COMMAND_REG, &tmp));
		while (tmp == FILL_BUFFER) {
			TRY_IO(mvHwXmdioRead(contextPtr, port, COMMAND_REG, &tmp));
		}
		if (tmp != MV_SLAVE_OK) {
			*errCode = MV_ERR_LAST_TRANSFER;
			return MV_FAIL;
		}
		/* Write a buffer of data to the slave RAM */
		stopIndex = byteIndex + lastTransferSize;
		buf_checksum = 0;
		while (byteIndex < stopIndex) {
			MV_U16 value;

			value = data[byteIndex++];
			value |= (((MV_U16)data[byteIndex++]) << 8);
			buf_checksum += value;
			TRY_IO(mvHwXmdioWrite(contextPtr, port, DATA_REG, value));
		}
		/* Last buffer is ready to write to flash now
		   Tell the slave to write it */
		MV_DEBUG("\nTelling slave to save bytes of last buffer\n");
		TRY_IO(mvHwXmdioWrite(contextPtr, port, COMMAND_REG, WRITE_BUFFER));

		/* Wait until the slave is finished */
		MV_DEBUG("Waiting for slave to finish programming last buffer to flash");

		/* Wait for MV_SLAVE_OK */
		TRY_IO(mvHwXmdioRead(contextPtr, port, COMMAND_REG, &tmp));
		while (tmp == WRITE_BUFFER || tmp == MV_SLAVE_FLASH_BUSY) {
			udelay(500000);  /* this can take several 2-3 seconds,
			don't poll phy too frequently since every read causes an interrupt on the phy */
			TRY_IO(mvHwXmdioRead(contextPtr, port, COMMAND_REG, &tmp));
		}
		if (tmp == MV_SLAVE_ERR_CMD) {
			MV_DEBUG("\nSome kind of error occurred on Slave. Exiting\n");
			*errCode = MV_ERR_LAST_TRANSFER;
			return MV_FAIL;
		} else {
			if (tmp != MV_SLAVE_OK) {
				*errCode = MV_ERR_LAST_TRANSFER;
				return MV_FAIL;
			} else {
				/* readback checksum of what was stored in flash */
				TRY_IO(mvHwXmdioRead(contextPtr, port, CHECKSUM_REG, &reported_checksum));
				if (reported_checksum != buf_checksum) {
					*errCode = MV_ERR_CHECKSUM;
					return MV_FAIL;
				}
			}
		}
		TRY_IO(mvHwXmdioRead(contextPtr, port, WORDS_WRITTEN_REG, &words_written));
		if (words_written != (MV_U16)(lastTransferSize/2)) {
			MV_DEBUG("\nSlave didn't write enough words to flash. Error occurred. Exit\n");
			*errCode = MV_ERR_SLAVE_WRITE_FULL;
			return MV_FAIL;
		}
		MV_DEBUG("\n");
	}

	return MV_OK;
}

/* This function downloads code to RAM in the DSP and then starts the application
 which was downloaded. "size" should be an even number (memory can only be written word-wise) */
MV_STATUS mvMdioRamDownload(CTX_PTR_TYPE contextPtr, MV_U8 data[], MV_U32 size,
				 MV_U16 port, MV_U8 use_ram_checksum, MV_U16 *errCode)
{
	MV_U32 buffCount;
	MV_U8  lowByte, highByte;
	MV_U16 tmp;
	MV_U16 ram_checksum;
	MV_U16 expected_checksum;
	*errCode = 0;

	if (size%2) {
		/* it's an error, size must be an even number of bytes */
		*errCode = MV_SIZE_NOT_EVEN;
		return MV_FAIL;
	}
	/* Put PHY in download mode and reset PHY */
	ATTEMPT(mvPutPhyInMdioDownloadMode(contextPtr, port)); /* will leave errCode as 0 if fails */
	/* Allow reset to complete */
	udelay(2500000); /* will leave errCode as 0 if fails */
	/* Make sure we can access the PHY
	   and it's in the correct mode (waiting for download) */
	TRY_IO(mvHwXmdioRead(contextPtr, port, MV_BOOT_STATUS_REG, &tmp));
	if (tmp != 0x000A) {
		MV_DEBUG("Download failed\n");
		MV_DEBUG("PHY is not in waiting on download mode. Expected 0x000A, read another value\n");
		MV_DEBUG("Download failed\n");
		*errCode = MV_PHY_NOT_IN_DOWNLOAD_MODE;
		return MV_FAIL;
	} else {
		MV_DEBUG("Downloading code to PHY RAM, please wait...\n");
	}
	if (use_ram_checksum) {
		/* this read clears the checksum */
		TRY_IO(mvHwXmdioRead(contextPtr, port, MV_RAM_CHECKSUM_REG, &ram_checksum));
	}
	/* Set starting address in RAM to 0x00100000 */
	TRY_IO(mvHwXmdioWrite(contextPtr, port, MV_LOW_WORD_REG, 0));
	TRY_IO(mvHwXmdioWrite(contextPtr, port, MV_HI__WORD_REG, 0x0010));
	/* Copy the code to the phy's internal RAM, calculating checksum as we go */
	buffCount = 0;
	expected_checksum = 0;
	while (buffCount < size) {
		lowByte = data[buffCount++];
		highByte = data[buffCount++];
		expected_checksum += (lowByte + highByte);
		TRY_IO(mvHwXmdioWrite(contextPtr, port, MV_RAM_DATA_REG, (((MV_U16)highByte)<<8)|lowByte));

	}
	if (use_ram_checksum) {
		/* Read hardware checksum register and see if it matches the locally computed checksum */
		TRY_IO(mvHwXmdioRead(contextPtr, port, MV_RAM_CHECKSUM_REG, &ram_checksum));
		if (expected_checksum != ram_checksum) {
			MV_DEBUG("Error downloading code. Got another value from the Expected RAM HW checsum\n");
			*errCode = MV_RAM_HW_CHECKSUM_ERR;
			return MV_FAIL;
		}
	}
	/* Now start code which was downloaded */
	TRY_IO(mvHwSetPhyRegField(contextPtr, port, MV_BOOT_STATUS_REG, 6, 1, 1));
	udelay(100000); /* Give app code time to start */
	return MV_OK;
}

/* Returns the memory size available in the connected device for application code */
MV_U32 mvGetDevMemorySize(CTX_PTR_TYPE contextPtr, MV_U16 port)
{
	STRUCT_DEVICE_TYPE devType;
	MV_U32 ramSize;

	devType = mvGetDeviceType(contextPtr, port);
	switch (devType.devID) {
	case X3220_DEV:
	case X3310_DEV:
		ramSize = MV_MAX_APP_SIZE;
		break;
	case MVINVALIDDEV:
	default:
		ramSize = 0;
	}
	return ramSize;
}

/* Returns the PHY type, Revision of the connected device */
STRUCT_DEVICE_TYPE mvGetDeviceType(CTX_PTR_TYPE contextPtr, MV_U16 port)
{
	STRUCT_DEVICE_TYPE devType;
	MV_U16 reg1_3;
	MV_U16 manuf_model_num;
	MV_U16 rev_num;
	MV_U16 oui1, oui2;

	/* Check if this is a Marvell PHY */
	mvHwXmdioRead(contextPtr, port, 1, 2, &oui1);
	mvHwXmdioRead(contextPtr, port, 1, 3, &reg1_3);
	mvHwGetRegFieldFromWord(reg1_3, 10, 6, &oui2);
	mvHwGetRegFieldFromWord(reg1_3, 4, 6, &manuf_model_num);
	mvHwGetRegFieldFromWord(reg1_3, 0, 4, &rev_num);
	/* Marvell OUI is 0x05043, X3240 model num is 0x18 */
	if (!(oui1 == 0x141 && oui2 == 3 && (manuf_model_num == 0x18 || manuf_model_num == 0x1A))) {
		MV_DEBUG("Device Type not found");
		devType.devID = MVINVALIDDEV;
		return devType;
	}
	/* this is currently the only type chip, can expand this later as needed */
	if (manuf_model_num == 0x18)
		devType.devID = X3220_DEV;
	if (manuf_model_num == 0x1A)
		devType.devID = X3310_DEV;
	devType.devRev = rev_num;
	return devType;
}

MV_STATUS mvUpdateFlashImage(CTX_PTR_TYPE contextPtr, MV_U16 port, MV_U8 appData[],
			MV_U32 appSize, MV_U8 slaveData[], MV_U32 slaveSize, MV_U16 *errCode)
{
	MV_U16 tmp;		   /* holds result of an MdioRead() */
	MV_U8 use_ram_checksum;	 /* tells the RAM download function whether to -
						use hardware RAM checksum register or not */
	*errCode = 0;

	/*******************************************************************************
	Check if the code about to be downloaded can fit into the device's memory
	*******************************************************************************/
	if (appSize > (mvGetDevMemorySize(contextPtr, port) + MV_HEADER_SIZE)) {
		/* App size cannot be larger than the device memory size.
				Code download cannot proceed */
		MV_DEBUG("Image is larger than the device memory size!\n");
		/* *errCode = MV_IMAGE_TOO_LARGE_TO_DOWNLOAD; */
		*errCode = mvGetDevMemorySize(contextPtr, port);
		return MV_FAIL;
	}
	use_ram_checksum = 1; /* All X32X0 have a ram checksum register, can change to 0
				 if wish to ignore it for some reason (not advised but
				 might want to for debug purposes) */
	/*******************************************************************************
	 Download slave code to phy's RAM and start it,
	 this code assists in programming the flash connected by SPI
	*******************************************************************************/
	ATTEMPT(mvMdioRamDownload(contextPtr, slaveData, slaveSize, port, use_ram_checksum, errCode));
	/* make sure the slave code started */
	TRY_IO(mvHwGetPhyRegField(contextPtr, port, MV_BOOT_STATUS_REG, 4, 1, &tmp));
	if (!tmp) {
		MV_DEBUG("Slave code did not start. Expected bit 4 to be 1");
		MV_DEBUG("Slave download failed. Exiting...\n");
		*errCode = MV_SLAVE_CODE_DID_NOT_START;
		return MV_FAIL;
	}
	/*******************************************************************************
	Write the image to flash with slave's help
	*******************************************************************************/
	ATTEMPT(mvMdioFlashDownload(contextPtr, port, appData, appSize, errCode));
	/*******************************************************************************
	Let slave verify image
	*******************************************************************************/

	/* Using slave code to verify image.
	 This commands slave to read in entire flash image and calculate checksum and make sure
	 checksum matches the checksum in the header. A failure means flash was corrupted.
	 Another method would be to reset the phy (with SPI_CONFIG[1]= 0) and see that the new code
	 starts successfully, since a bad checksum will result in the code not being started */

	MV_DEBUG("Flash programming complete. Verifying image via slave.\n");
	TRY_IO(mvHwXmdioWrite(contextPtr, port, COMMAND_REG, VERIFY_FLASH));
	TRY_IO(mvHwXmdioRead(contextPtr, port, COMMAND_REG, &tmp));
	while (tmp == VERIFY_FLASH || tmp == MV_SLAVE_FLASH_BUSY) {
		udelay(100000);  /* Don't poll phy too frequently,
						every read causes an interrupt */
		TRY_IO(mvHwXmdioRead(contextPtr, port, COMMAND_REG, &tmp));
	}
	if (tmp == MV_SLAVE_OK) {
		MV_DEBUG("\nFlash image verified. Reset F_CFG1 to 0 and reboot to execute new code\n");
		return MV_OK;   /*Flash download complete */
	} else {
		if (tmp == MV_SLAVE_VERIFY_ERR) {
			MV_DEBUG("\nFlash verifed FAILED! Flash probably corrupted. Re-try download.\n");
			*errCode = MV_VERIFY_ERR;
			return MV_FAIL;
		} else {
			MV_DEBUG("\nUnknown download to flash fail. Exiting...\n");
			*errCode = MV_UNKNOWN_DOWNLOAD_TO_FLASH_FAIL;
			return MV_FAIL;
		}
	}
}


