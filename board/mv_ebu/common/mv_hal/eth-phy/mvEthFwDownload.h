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

    *	Redistributions of source code must retain the above copyright notice,
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

#ifndef __INCETHFWDOWNLOAD
#define __INCETHFWDOWNLOAD

#ifdef __cplusplus
extern "C" {
#endif

typedef void *CTX_PTR_TYPE;

/* Master-Slave Protocol Definitions
   MDIO Register to slave 		*/
#define MAX_BUFF_SIZE_OUT_REG		1,49192 /* MDIO Registers used to communicate with slave code */
#define ACTUAL_BUFF_SIZE_IN_REG		1,49193
#define COMMAND_REG			1,49194
#define WORDS_WRITTEN_REG		1,49195
#define LOW_ADDRESS_REG			1,49196
#define HIGH_ADDRESS_REG		1,49197
#define DATA_REG			1,49198
#define CHECKSUM_REG			1,49199
#define WORDS_RCVD_REG			1,49200

/* Host Commands */
#define ERASE_FLASH_PROGRAM_AREA	(0x1)
#define FILL_BUFFER			(0x2)
#define WRITE_VERIFY_BUFFER		(0x6)
#define WRITE_BUFFER 			WRITE_VERIFY_BUFFER
#define VERIFY_FLASH			(0x7)

/* Slave Responses */
#define MV_SLAVE_OK		(0x100)
#define MV_SLAVE_ERR_CMD	(0x200)
#define MV_SLAVE_FLASH_BUSY	(0x300)
#define MV_SLAVE_VERIFY_ERR	(0x400)

#define TRY_IO(xFuncToTry) if (xFuncToTry == MV_FAIL) {*errCode = MV_IO_ERROR; return MV_FAIL;}

/* 88X3240/3220 Device Number Definitions */
#define MV_T_UNIT_PMA_PMD	1
#define MV_X_UNIT		3
#define MV_C_UNIT_GENERAL	31

#define MV_BOOT_STATUS_REG	MV_T_UNIT_PMA_PMD, 0xC050

/* 88X3240/3220 C Unit Registers MMD 31 */
#define MV_CUNIT_MODE_CONFIG	MV_C_UNIT_GENERAL, 0xF000
#define MV_CUNIT_PORT_CTRL	MV_C_UNIT_GENERAL, 0xF001

/* This macro is handy for calling a function when you want to test the
   return value and return MV_FAIL, if the function returned MV_FAIL,
   otherwise continue */
#define ATTEMPT(xFuncToTry) if (xFuncToTry == MV_FAIL) {return MV_FAIL;}


typedef enum {
	X3220_DEV = 0,
	X3310_DEV = 1,
	MVINVALIDDEV = 10
} DEV_ID;

typedef enum {
	Z0 = 0,
	Z1 = 1,
	MVINVALIDREV = 8
} DEV_REV;

typedef struct {
	DEV_ID devID;
	DEV_REV devRev;
} STRUCT_DEVICE_TYPE;

STRUCT_DEVICE_TYPE mvGetDeviceType(CTX_PTR_TYPE contextPtr, MV_U16 port);

MV_U32 mvGetDevMemorySize(CTX_PTR_TYPE contextPtr, MV_U16 port);


/* 88X3240/3220 Device Number Definitions */
#define MV_T_UNIT_PMA_PMD	1
#define MV_T_UNIT_PCS_CU	3
#define MV_H_UNIT		4
#define MV_C_UNIT_GENERAL	31

/* 88X3240/3220 T Unit Registers MMD 1 */
#define MV_BOOT_STATUS_REG	MV_T_UNIT_PMA_PMD, 0xC050

/* 88X3240/3220 C Unit Registers MMD 31 */
#define MV_CUNIT_MODE_CONFIG	MV_C_UNIT_GENERAL, 0xF000
#define MV_CUNIT_PORT_CTRL	MV_C_UNIT_GENERAL, 0xF001

#define MV_MAX_APP_SIZE		(216*1024UL)

#define MV_HEADER_SIZE		32

/* Slave code did not start. - Slave code failed to download properly. */
#define MV_SLAVE_CODE_DID_NOT_START		0xFFE0
/* Flash verifed FAILED! Flash probably corrupted */
#define MV_VERIFY_ERR				0xFFE1
/* Unknown error, downloading the flash failed! */
#define MV_UNKNOWN_DOWNLOAD_TO_FLASH_FAIL	0xFFE2

/* Error reading or writing MDIO register */
#define MV_IO_ERROR				0xFFE5
/*size must be an even number of bytes*/
#define MV_SIZE_NOT_EVEN			0xFFE6
/* Slave encountered error while erasing flash */
#define MV_ERR_ERASING_FLASH			0xFFE7
/* unexpected value read back from download code */
#define MV_ERR_VALUE_READ_BACK			0xFFE8
/* Did not get MV_SLAVE_OK for writing the data */
#define MV_ERR_START_WRITE_DATA			0xFFE9
/* Slave failed to get all the data correctly*/
#define MV_START_WRITE_DATA			0xFFEA
/* Some kind of error occurred on Slave */
#define MV_ERR_ON_SLAVE				0xFFEB
/* Checksum error */
#define MV_ERR_CHECKSUM				0xFFEC
/* Slave didn't write enough words to flash. Some kind of error occurred*/
#define MV_ERR_SLAVE_WRITE_FULL			0xFFED
/* last transfer failed */
#define MV_ERR_LAST_TRANSFER			0xFFEE
/* RAM checksum register (present only in SFT910X RevB or later) had wrong checksum */
#define MV_RAM_HW_CHECKSUM_ERR			0xFFEF
/* PHY wasn't waiting in download mode */
#define MV_PHY_NOT_IN_DOWNLOAD_MODE		0xFFFF



/******************************************************************************
 MV_STATUS mvUpdateFlashImage(CTX_PTR_TYPE contextPtr, MV_U16 port, MV_U8 appData[],
	 MV_U32 appSize, MV_U8 slaveData[], MV_U32 slaveSize, MV_U16 *errCode);

 Inputs:
	contextPtr - pointer to host's private data
	port - MDIO port address, 0-31
	appData[] - application code to be downloaded in to the flash
	appSize - file size
	slaveData[] - slave code to be downloaded in to the RAM
	slaveSize - file size of the slave code

 Outputs:
	errCode - if function returns MV_FAIL, more information is contained in errCode
	as follows:

		 MDIO IO Error
		 MV_IO_ERROR

		 Image larger than the target device's memory
		 MV_IMAGE_TOO_LARGE_TO_DOWNLOAD

		 PHY wasn't waiting in download mode
		 MV_PHY_NOT_IN_DOWNLOAD_MODE

		 RAM checksum register bad checksum (slave code didn't download correctly)
		 MV_RAM_HW_CHECKSUM_ERR
		 Slave code did not start executing
		 MV_SLAVE_CODE_DID_NOT_START

		 size must be an even number of bytes
		 MV_SIZE_NOT_EVEN
		 Slave encountered error while erasing flash
		 MV_ERR_ERASING_FLASH
		 unexpected value read back from download code
		 MV_ERR_VALUE_READ_BACK
		 Did not get MV_SLAVE_OK for writing the data
		 MV_ERR_START_WRITE_DATA
		 Slave failed to get all the data correctly
		 MV_START_WRITE_DATA
		 Some kind of error occurred on Slave
		 MV_ERR_ON_SLAVE
		 Checksum error
		 MV_ERR_CHECKSUM
		 Slave didn't write enough words to flash. Some kind of error occurred
		 MV_ERR_SLAVE_WRITE_FULL
		 last transfer failed
		 MV_ERR_LAST_TRANSFER

		 Flash verifed FAILED! Flash probably corrupted .
		 MV_VERIFY_ERR
		 Unknown error, downloading the flash failed!
		 MV_UNKNOWN_DOWNLOAD_TO_FLASH_FAIL

 Returns:
	MV_OK if successful. MV_FAIL if not.

 Description:
	The function first checks if the app code size is greater than what it
	should be and returns an error if that was the case.
	Then it calls mvPutPhyInMdioDownloadMode() to put the PHY in download mode by
	raising the SPI_CONFIG bit and downloads the slave code in to the RAM and starts executing.
	Once the flash is written with the help of the slave, slave verifies
	the flash program and returns errors if the image is corrupted (the slave
	reads back the image from flash and verifies the checksum matches the expected checksum).
	Any error occurs returns MV_FAIL and additional information in "errCode".

 Side effects:
	None.

 Notes/Warnings:
	This function calls two main functions ...
	1. mvMdioRamDownload() to download the slave helper code
	2. mvMdioFlashDownload() to update the flash using the slave helper code

	The function asserts the SPI_CONFIG when it begins the code download. But it does
	not lower SPI_CONFIG after the download. In order to execute the newly downloaded
	code, SPI_CONFIG must be lowered by calling mvRemovePhyDownloadMode() and the
	PHY must be reset to use the newly updated flash image.

 Example:
	FILE *hFile, *sFile;
	MV_U8 appData[MV_MAX_APP_SIZE+MV_HEADER_SIZE];		// hdr file image
	MV_U8 slaveCode[MV_MAX_SLAVE_SIZE];	// slave code image
	MV_STATUS retStatus;
	MV_U16 error;

	fileSize = mvOpenReadFile(contextPtr, port, &hFile, "image_name.hdr", appData,
									(MV_MAX_APP_SIZE+ MV_HEADER_SIZE));
	slaveFileSize = mvOpenReadFile(contextPtr, port, &sFile,
									"x3240flashdlslave_0_3_0_0_6147.bin", slaveCode,
									MV_MAX_SLAVE_SIZE);
	retStatus = mvUpdateFlashImage(contextPtr, port, appData, fileSize, slaveCode,
								 slaveFileSize, &error);
	if (retStatus == MV_FAIL)
		...check "error" for what happened/type of error that occurred

	fclose(hFile);
	fclose(sFile);

******************************************************************************/
MV_STATUS mvUpdateFlashImage(CTX_PTR_TYPE contextPtr, MV_U16 port, MV_U8 appData[],
			MV_U32 appSize,	MV_U8 slaveData[], MV_U32 slaveSize, MV_U16 *errCode);


/******************************************************************************
MV_STATUS mvRemovePhyMdioDownloadMode(CTX_PTR_TYPE contextPtr, MV_U16 port);

 Inputs:
	contextPtr - pointer to host's private data
	port - MDIO port address, 0-31

 Outputs:
	None

 Returns:
	MV_OK or MV_FAIL

 Description:
	Sets override of SPI_CONFIG to 0, then resets the T-unit so code will
	load from flash.

 Side effects:
	None

 Notes/Warnings:
	None

******************************************************************************/
MV_STATUS mvRemovePhyMdioDownloadMode(CTX_PTR_TYPE contextPtr, MV_U16 port);




#ifdef __cplusplus
}
#endif

#endif /* #ifndef __INCETHFWDOWNLOAD */
