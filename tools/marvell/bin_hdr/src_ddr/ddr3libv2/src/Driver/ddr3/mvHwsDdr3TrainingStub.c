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
* mvHwsDdr3TrainingStub.c
*
* DESCRIPTION:
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 204 $
******************************************************************************/
#include "mv_os.h"
#include "mvSiliconIf.h"
#include "stdarg.h"
#include "printf.h"


GT_VOID gtBreakOnFail(GT_VOID){
}

GT_STATUS gtStatus;

GT_STATUS mvHwsDelay(GT_U8  devNum, GT_U32 portGroup, GT_U32 mils){
    mvOsDelay(mils);
	return 0;
}

MV_OS_EXACT_DELAY_FUNC   hwsOsExactDelayPtr = mvHwsDelay;

void * osMemCpy( void * destination, const void * source, GT_U32 size){
    GT_U32 cnt;
    for(cnt=0; cnt<size; cnt++) {
        ((GT_U8*)destination)[cnt] = ((GT_U8*)source)[cnt];
    }
    
    return 0;
}

void *osMemSet(void *s, int c, GT_U32 n)
{
    unsigned char* p=s;
    while(n--)
        *p++ = (unsigned char)c;
    return s;
}
