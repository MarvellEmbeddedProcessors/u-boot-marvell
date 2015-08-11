/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* gtEnvDep.h
*
* DESCRIPTION:    Hardware environment depended types definition
*
* DEPENDENCIES:   Operating System.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/

#ifndef __gtEnvDeph
#define __gtEnvDeph


typedef void  GT_VOID, *GT_VOID_PTR;
typedef char  GT_8,   *GT_8_PTR;
typedef short GT_16,  *GT_16_PTR;
typedef int  GT_32,  *GT_32_PTR;

typedef unsigned char  GT_U8,   *GT_U8_PTR;
typedef unsigned short GT_U16,  *GT_U16_PTR;
typedef unsigned int  GT_U32,  *GT_U32_PTR;

typedef void   *GT_PTR;
typedef char  GT_CHAR, *GT_CHAR_PTR;

#ifdef WIN32
typedef __int64 GT_UL64;
#else
typedef unsigned long long GT_UL64;
#endif

#define VAR_32BIT       int
#define GT_UREG_DATA    GT_U32

#ifndef VX6_8
typedef VAR_32BIT    *GT_32_PTR;
typedef unsigned VAR_32BIT   GT_U32,  *GT_U32_PTR;
#endif

typedef VAR_32BIT           GT_INTPTR;
typedef unsigned VAR_32BIT  GT_UINTPTR;
typedef VAR_32BIT           GT_PTRDIFF;
typedef unsigned VAR_32BIT  GT_SIZE_T;

/* Haim - End **/

/* this is obsolete type; use GT_UL64 */
typedef union
{
    GT_U8   c[8];
    GT_U16  s[4];
    GT_U32  l[2];
}GT_U64;

#define GT_FALSE    0
#define GT_TRUE     (!(GT_FALSE))

typedef int     GT_BOOL;


/* added macro for memory barrier synchronize   */

#ifdef PPC_CPU
#define GT_SYNC __asm__("   eieio")
#endif

#ifdef ARM_CPU
#define GT_SYNC __asm__("mcr p15, 0, %0, c7, c10, 4" : : "r" (0))
#endif

#ifdef MIPS_CPU
#define GT_SYNC __asm__ __volatile__ ("sync")
#endif

#ifdef INTEL_CPU
#define GT_SYNC
#endif

#ifdef WIN32
#define GT_SYNC
#endif


/* 4244 - warning C4244: '=' : conversion from 'int ' to 'unsigned short ', */
/*  possible loss of data                                                   */
/* 4100 - warning C4100: 'variable' : unreferenced formal parameter         */
/* warning C4127: conditional expression is constant                        */
#ifdef WIN32
#pragma warning ( disable : 4100 4244 4127 4115)
#endif

#endif   /* __gtEnvDeph */



