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
* ddr3_a38x_training_static.h
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*
*******************************************************************************/

#ifndef __ddr3_a38x_training_static_H
#define __ddr3_a38x_training_static_H

#ifdef __cplusplus
extern "C" {
#endif

/* STATIC_TRAINING - Set only if static parameters for training are set and required */
/*#define STATIC_TRAINING */
typedef struct __mvDramTrainingInit {
	MV_U32 reg_addr;
	MV_U32 reg_value;
} MV_DRAM_TRAINING_INIT;

MV_DRAM_TRAINING_INIT ddr3_db_a38x[MV_MAX_DDR3_STATIC_SIZE] =
{
    /*HW mechanizem*/
    /* WL training*/
	{0x000015B0, 0x80100008},
	/* Preload training pattern to DRAM*/
	{0x000015B0, 0x80100002},
	/* Read Leveling using training pattern at high frequency*/
	{0x000015B0, 0x80100040},
	{0x000200e8, 0x0},		/* RL */
	{0x00020184, 0x0FFFFFE1},	/* RL */
	{0x0, 0x0}
};

#ifdef __cplusplus
}
#endif

#endif /* __ddr3_a38x_training_static_H */
