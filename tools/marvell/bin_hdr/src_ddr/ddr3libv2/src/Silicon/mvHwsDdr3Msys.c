/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* mvHwsDdr3Msys.c
*
* DESCRIPTION: DDR3 MSYS general functions*
* DEPENDENCIES:
******************************************************************************/

#include "mvDdr3TrainingIpPrvIf.h"

GT_U32 phy1ValTable[DDR_FREQ_LIMIT] =
{
  0,   /* DDR_FREQ_LOW_FREQ */
  0xf, /* DDR_FREQ_400 */
  0xf, /* DDR_FREQ_533 */
  0xf, /* DDR_FREQ_667 */
  0xc, /* DDR_FREQ_800 */
  0x8, /* DDR_FREQ_933 */
  0x8, /* DDR_FREQ_1066 */
  0xf, /* DDR_FREQ_311 */
  0xf, /* DDR_FREQ_333 */
  0xf, /* DDR_FREQ_467 */
  0xc, /*DDR_FREQ_850*/
  0xf, /*DDR_FREQ_600*/
  0xf, /*DDR_FREQ_300*/
  0x8,  /*DDR_FREQ_900*/
  0xf  /*DDR_FREQ_360*/
};


/**************************external ******************************/
extern MV_HWS_TIP_CONFIG_FUNC_DB configFuncInfo[HWS_MAX_DEVICE_NUM];

/******************************************************************************
XSB External read
******************************************************************************/
GT_STATUS    ddr3TipExtRead
(
    GT_U32      devNum,
    GT_U32      interfaceId,
    GT_U32      regAddr,
    GT_U32      numOfBursts,
    GT_U32      *data
)
{
    return configFuncInfo[devNum].tipExternalRead(devNum,interfaceId,regAddr,numOfBursts,data);
}



/*****************************************************************************
XSB External write
******************************************************************************/
GT_STATUS    ddr3TipExtWrite
(
    GT_U32      devNum,
    GT_U32      interfaceId,
    GT_U32      regAddr,
    GT_U32      numOfBursts,
    GT_U32      *addr
)
{
    return configFuncInfo[devNum].tipExternalWrite(devNum,interfaceId,regAddr,numOfBursts,addr);
}





