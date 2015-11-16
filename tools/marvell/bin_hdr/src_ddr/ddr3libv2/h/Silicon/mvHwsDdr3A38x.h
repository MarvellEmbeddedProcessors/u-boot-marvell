/******************************************************************************
*              Copyright (c) Marvell International Ltd. and its affiliates
*
* This software file (the "File") is owned and distributed by Marvell
* International Ltd. and/or its affiliates ("Marvell") under the following
* alternative licensing terms.
* If you received this File from Marvell, you may opt to use, redistribute
* and/or modify this File under the following licensing terms.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  -   Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*  -   Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*  -    Neither the name of Marvell nor the names of its contributors may be
*       used to endorse or promote products derived from this software without
*       specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************
* mvHwsDdr3A38x.h
*
* DESCRIPTION:
*
*******************************************************************************/
#ifndef __mvHwsDdr3_A38x_H
#define __mvHwsDdr3_A38x_H

/* Termal Sensor Registers */
#define TSEN_CONTROL_LSB_REG					0xE4070
#define TSEN_CONTROL_LSB_TC_TRIM_OFFSET				0
#define TSEN_CONTROL_LSB_TC_TRIM_MASK				(0x7 << TSEN_CONTROL_LSB_TC_TRIM_OFFSET)
#define TSEN_CONTROL_MSB_REG					0xE4074
#define TSEN_CONTROL_MSB_RST_OFFSET				8
#define TSEN_CONTROL_MSB_RST_MASK				(0x1 << TSEN_CONTROL_MSB_RST_OFFSET)
#define TSEN_STATUS_REG						0xE4078
#define TSEN_STATUS_READOUT_VALID_OFFSET	10
#define TSEN_STATUS_READOUT_VALID_MASK		(0x1 << TSEN_STATUS_READOUT_VALID_OFFSET)
#define TSEN_STATUS_TEMP_OUT_OFFSET			0
#define TSEN_STATUS_TEMP_OUT_MASK			(0x3FF << TSEN_STATUS_TEMP_OUT_OFFSET)

/* device ID and revision */
#define DEV_ID_REG					0x18238
#define DEV_VERSION_ID_REG			0x1823C
#define REVISON_ID_OFFS				8
#define REVISON_ID_MASK				0xF00

#endif /*__mvHwsDdr3_A38x_H*/

