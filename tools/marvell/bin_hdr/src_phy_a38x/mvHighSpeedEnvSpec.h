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


#ifndef _MV_HIGHSPEED_ENV_SPEC_H
#define _MV_HIGHSPEED_ENV_SPEC_H

#include "mv_os.h"
#include "bin_hdr_twsi.h"
#include "mvUart.h"
#include "util.h"

/********************************* Definitions ********************************/
#define MAX_SERDES_LANES            6
#define NA                          8
#define DEFAULT_PARAM               0


/* For setting or clearing a certain bit
   (bit is a number between 0 and 31) in the data*/
#define SET_BIT(data, bit)          (data | (0x1 << bit))
#define CLEAR_BIT(data, bit)        (data & (~(0x1 << bit)))

/* For checking function return values */
#define CHECK_STATUS(origFunc) \
{ \
    MV_STATUS mvStatus; \
    mvStatus = origFunc; \
    if (MV_OK != mvStatus) \
    { \
        return mvStatus; \
    } \
}

/************************************ enums ***********************************/

/* Serdes lane types */
typedef enum
{
    PEX0,
    PEX1,
    PEX2,
    PEX3,
    SATA0,
    SATA1,
    SATA2,
    SATA3,
    SGMII0,
    SGMII1,
    SGMII2,
    QSGMII,
    USB3_HOST,
    USB3_DEVICE,
    DEFAULT_SERDES,
    LAST_SERDES_TYPE
} SERDES_TYPE;

/* Serdes baud rates */
typedef enum
{
    __1_25Gbps,
    __1_5Gbps,
    __2_5Gbps,
    __3Gbps,
    __3_125Gbps,
    __5Gbps,
    __6Gbps,
    __6_25Gbps,
    LAST_SERDES_SPEED
} SERDES_SPEED;

/* Serdes modes */
typedef enum
{
    PEX_ROOT_COMPLEX_x1,
    PEX_ROOT_COMPLEX_x4,
    PEX_END_POINT_x1,
    PEX_END_POINT_x4,

    SERDES_DEFAULT_MODE, /* not pex */

    SERDES_LAST_MODE
} SERDES_MODE;

/* Serdes ref clock options */
typedef enum
{
    REF_CLOCK__25MHz,
    REF_CLOCK__100MHz,
    REF_CLOCK_UNSUPPORTED
} REF_CLOCK;

/* Serdes reset options */
typedef enum
{
    ANALOG_RESET_ON,
    ANALOG_RESET_OFF
} ANALOG_RESET;

typedef enum
{
    DIGITAL_RESET_ON,
    DIGITAL_RESET_OFF
} DIGITAL_RESET;

/* Patterns for the test generator*/
typedef enum
{
    _1_T,
    _2_T,
    _5_T,
    _10_T,
    PRBS7,
    PRBS9,
    PRBS15,
    PRBS23,
    PRBS31,
    DFE_TRAINING
} MV_PATTERN;

/* For the test generator */
typedef enum
{
    ENABLE_MODE,
    DISABLE_MODE
} TEST_MODE;

/* For serdesPolarity */
typedef enum
{
    INVERT,
    NORMAL
} MV_INVERT;

/*********************************** Globals **********************************/

/*********************************** Structs **********************************/

typedef struct
{
    SERDES_TYPE 	serdesType;
    SERDES_SPEED	serdesSpeed;
    SERDES_MODE		serdesMode;
} SERDES_MAP;

/*************************** Functions declarations ***************************/

/**************************************************************************
* mvCtrlHighSpeedSerdesPhyConfig -
*
* DESCRIPTION:          Configures the power up sequence of the serdes
* INPUT:                None.
* OUTPUT:               None.
* RETURNS:              MV_OK           -   for success
***************************************************************************/
MV_STATUS mvHwsCtrlHighSpeedSerdesPhyConfig(MV_VOID);

/**************************************************************************
* mvSerdesPowerUpCtrl -
*
* DESCRIPTION:          Executes the serdes power up/down
* INPUT:                serdesNum       -   Serdes lane number
*                       serdesPowerUp   -   True for power up
*                                           False for power down
*                       serdesType      -   PEX, SATA, SGMII
*                                           or USB3 (host or device)
*                       baudRate        -   serdes speed
*                       refClock        -   ref clock (25 or 100)
* OUTPUT:               None.
* RETURNS:              MV_OK           -   for success
***************************************************************************/
MV_STATUS mvSerdesPowerUpCtrl(MV_U32        serdesNum,
							  MV_BOOL       serdesPowerUp,
                             SERDES_TYPE    serdesType,
                             SERDES_SPEED   baudRate,
                             REF_CLOCK      refClock);

/**************************************************************************
* mvSerdesReset -
*
* DESCRIPTION:          Clears the serdes registers
* INPUT:                serdesNum       -   Serdes lane number
*                       analogReset     -   reset or unreset
*                       digitalReset    -   reset or unreset
* OUTPUT:               None.
* RETURNS:              MV_OK           -   for success
***************************************************************************/
MV_STATUS mvSerdesReset(MV_U32          serdesNum,
                        ANALOG_RESET    analogReset,
                        DIGITAL_RESET   digitalReset);




/***************************************************************************/
MV_STATUS serdesTestGenerator(MV_U32            serdesNum,
                              MV_PATTERN        pattern,
                              TEST_MODE         testMode);

MV_STATUS serdesTestGeneratorStatus(MV_U32          serdesNum,
                                    MV_PATTERN      pattern,
                                    MV_U32          *errorCounter,
                                    MV_BOOL         *isLocked,
                                    MV_U32          *frameCounter);

MV_STATUS serdesPolarity(MV_U32     serdesNum,
                         MV_INVERT  TxInvert,
                         MV_INVERT  RxInvert);

/***************************************************************************/


#endif /* _MV_HIGHSPEED_ENV_SPEC_H */