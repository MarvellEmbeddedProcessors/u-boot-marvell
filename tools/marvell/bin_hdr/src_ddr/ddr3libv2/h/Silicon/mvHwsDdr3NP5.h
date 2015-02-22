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
* mvHwsDdr3NP5.h
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 15 $
*
*******************************************************************************/

#ifndef __mvHwsDdr3NP5_H
#define __mvHwsDdr3NP5_H


#include "gtGenTypes.h" 
#include "mvDdr3TrainingIp.h"
#include "mvDdr3TrainingIpStatic.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DEFINE_GLOBALS
extern GT_32 np5TopologyMap[];
#else
MV_HWS_TOPOLOGY_MAP np5TopologyMap[] = 
{
    /* 1st board */
    {/* active interfaces */
    0xFFF,
    /*cs_mask, mirror, dqs_swap, ck_swap X PUPs                                     speed_bin             memory_width  mem_size     frequency  casL casWL      temperature */
    {{{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,1,0,0}, {0x2,1,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16, MEM_2G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL},
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,1,0,0}, {0x2,1,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_2G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL},
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,1,0,0}, {0x2,1,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_2G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL},
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,1,0,0}, {0x2,1,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_2G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL},
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,1,0,0}, {0x2,1,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_2G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL},
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,1,0,0}, {0x2,1,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_2G, DDR_FREQ_800, 0,    0 , MV_HWS_TEMP_NORMAL},
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,1,0,0}, {0x2,1,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_2G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL},
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,1,0,0}, {0x2,1,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_2G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL},
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,1,0,0}, {0x2,1,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_2G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL},
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,1,0,0}, {0x2,1,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_2G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL},
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,1,0,0}, {0x2,1,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_2G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL},
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,1,0,0}, {0x2,1,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_2G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL}},
    4, /* Num Of Bus Per Interface*/
    0xF  /* Buses mask */
    },
    /* 1nd board */

    {/* functional board #1*/
    0xFFF,
    /*cs_mask, mirror, dqs_swap, ck_swap X PUPs                                     speed_bin             memory_width  mem_size     frequency  casL casWL      temperature */
    {{{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,1,0,0}, {0x2,1,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL} ,
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,1,0,0}, {0x2,1,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL} ,
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,1,0,0}, {0x2,1,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL} ,
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,1,0,0}, {0x2,1,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL} ,
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,1,0,0}, {0x2,1,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL} ,
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,1,0,0}, {0x2,1,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_800, 0,    0 , MV_HWS_TEMP_NORMAL} ,
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,1,0,0}, {0x2,1,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL} ,
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,1,0,0}, {0x2,1,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL} ,
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,1,0,0}, {0x2,1,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL} ,
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,1,0,0}, {0x2,1,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL} ,
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,1,0,0}, {0x2,1,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL} ,
    {{{0x1,0,0,0}, {0x1,0,0,0}, {0x2,1,0,0}, {0x2,1,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL}} ,
    4, /* Num Of Bus Per Interface*/
    0xF  /* Buses mask */
    },
    /* 3rd board */
    {/* Cisco board #2*/
    0xFFF,
    /*cs_mask, mirror, dqs_swap, ck_swap X PUPs                                     speed_bin             memory_width  mem_size     frequency  casL casWL      temperature */
    {{{{0x1,1,0,0}, {0x1,1,0,0}, {0x2,0,1,0}, {0x2,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL} ,
    {{{0x1,1,1,0}, {0x1,1,0,0}, {0x2,0,0,0}, {0x2,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL} ,
    {{{0x1,1,0,0}, {0x1,1,1,0}, {0x2,0,1,1}, {0x2,0,0,1}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL} ,
    {{{0x1,1,1,0}, {0x1,1,0,0}, {0x2,0,1,0}, {0x2,0,1,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL} ,
    {{{0x1,1,1,1}, {0x1,1,1,1}, {0x2,0,1,1}, {0x2,0,0,1}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL} ,
    {{{0x1,1,0,0}, {0x1,1,0,0}, {0x2,0,1,1}, {0x2,0,0,1}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_800, 0,    0 , MV_HWS_TEMP_NORMAL} ,
    {{{0x1,1,1,0}, {0x1,1,1,0}, {0x2,0,0,0}, {0x2,0,1,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL} ,
    {{{0x1,1,1,0}, {0x1,1,1,0}, {0x2,0,0,0}, {0x2,0,1,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL} ,
    {{{0x1,1,1,0}, {0x1,1,1,0}, {0x2,0,1,1}, {0x2,0,0,1}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL} ,
    {{{0x1,1,1,0}, {0x1,1,1,0}, {0x2,0,0,0}, {0x2,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL} ,
    {{{0x1,1,0,1}, {0x1,1,1,1}, {0x2,0,0,0}, {0x2,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL} ,
    {{{0x1,1,0,0}, {0x1,1,1,0}, {0x2,0,0,0}, {0x2,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, SPEED_BIN_DDR_2133N, BUS_WIDTH_16 , MEM_4G, DDR_FREQ_800, 0 ,   0 , MV_HWS_TEMP_NORMAL}} ,
    4, /* Num Of Bus Per Interface*/
    0xF  /* Buses mask */
    }  
};
#endif

#ifndef DEFINE_GLOBALS
extern GT_32 np5PackageRoundTripDelayArray[];
#else
TripDelayElement np5PackageRoundTripDelayArray[] =  
{ 
/*interface bus DQS-delay CK-delay */
    { 104 , 152 } , 
    { 120 , 152 } , 
    { 103 , 146 } , 
    { 108 , 146 } , 
    { 99  , 166 } , 
    { 96  , 166 } , 
    { 90  , 145 } , 
    { 79  , 145 } , 
    { 79  , 130 } , 
    { 82  , 130 } , 
    { 67  , 130 } , 
    { 73  , 130 } , 
    { 85  , 149 } , 
    { 63  , 149 } , 
    { 81  , 136 } , 
    { 81  , 136 } , 
    { 71  , 141 } , 
    { 82  , 141 } , 
    { 84  , 141 } , 
    { 78  , 141 } , 
    { 81  , 137 } , 
    { 111 , 137 } , 
    { 85  , 127 } , 
    { 111 , 127 } , 
    { 100 , 126 } , 
    { 127 , 126 } , 
    { 90  , 129 } , 
    { 111 , 129 } , 
    { 81  , 109 } , 
    { 94  , 109 } , 
    { 73  , 88  } , 
    { 74  , 88  } , 
    { 65  , 133 } , 
    { 73  , 133 } , 
    { 59  , 138 } , 
    { 60  , 138 } , 
    { 73  , 146 } , 
    { 60  , 146 } , 
    { 66  , 124 } , 
    { 69  , 124 } , 
    { 67  , 95  } , 
    { 86  , 95  } , 
    { 75  , 92  } , 
    { 93  , 92  } , 
    { 84  , 164 } , 
    { 123 , 164 } , 
    { 90  , 176 } , 
    { 124 , 176 }
};
#endif

#ifndef DEFINE_GLOBALS
extern GT_32 np5BoardRoundTripDelayArray[];
#else
/* this array hold the board round trip delay (DQ and CK) per <interfcae,bus> */
/* TBD remove interface & bus from array */
TripDelayElement np5BoardRoundTripDelayArray[] =  
{  /* ETP Board */
/*interface bus DQS-delay CK-delay */
   { 3383,  6728 },  
   { 3003,  6728 },  
   { 3161,  6765 },  
   { 2564,  6765 },  
   { 3646,  7129 },  
   { 3355,  7129 },  
   { 3920,  7130 },  
   { 3538,  7130 },  
   { 3353,  6556 },  
   { 2659,  6556 },  
   { 3242,  6601 },  
   { 2871,  6601 },  
   { 3499,  6797 },  
   { 3074,  6797 },  
   { 3206,  6755 },  
   { 2635,  6755 },  
   { 3940,  6878 },  
   { 3592,  6878 },  
   { 3345,  6875 },  
   { 3094,  6875 },  
   { 3356,  7395 },  
   { 2830,  7395 },  
   { 3338,  7393 },  
   { 2911,  7393 },  
   { 3340,  6907 },  
   { 2688,  6907 },  
   { 3166,  6944 },  
   { 2474,  6944 },  
   { 3581,  6962 },  
   { 3082,  6962 },  
   { 4037,  6963 },  
   { 3501,  6963 },  
   { 3298,  6520 },  
   { 2725,  6520 },  
   { 3429,  6558 },  
   { 2949,  6558 },  
   { 2838,  5664 },  
   { 2231,  5664 },  
   { 2405,  5665 },  
   { 1952,  5665 },  
   {  4204,  6952},  
   {  3680,  6952},  
   {  3350,  6991},  
   {  2969,  6991},  
   {  3411,  6629},  
   {  2726,  6629},  
   {  3225,  6663},  
   {  2904,  6663},   
    
/*Functional*/
   { 1486 , 2202},
   { 1309 , 2202},
   { 1877 , 2069},
   { 1539 , 2069},
   { 1647 , 2129},
   { 1408 , 2129},
   { 1623 , 2218},
   { 1683 , 2218},
   { 1363 , 1633},
   { 1337 , 1633},
   { 1391 , 1922},
   { 1521 , 1922},
   { 1332 , 1776},
   { 1404 , 1776},
   { 1544 , 1828},
   { 1408 , 1828},
   { 1655 , 2237},
   { 1599 , 2237},
   { 1815 , 2228},
   { 1512 , 2228},
   { 1789 , 2014},
   { 1246 , 2014},
   { 1676 , 1972},
   { 1391 , 1972},
   { 1498 , 1934},
   { 1174 , 1934},
   { 1831 , 2164},
   { 1304 , 2164},
   { 1725 , 2123},
   { 1393 , 2123},
   { 1706 , 2215},
   { 1716 , 2215},
   { 1388 , 1558},
   { 1204 , 1558},
   { 1302 , 1919},
   { 1472 , 1919},
   { 1224 , 1720},
   { 1373 , 1720},
   { 1528 , 1772},
   { 1261 , 1772},
   { 1570 , 2202},
   { 1617 , 2202},
   { 1810 , 2305},
   { 1467 , 2305},
   { 1774 , 1863},
   { 1282 , 1863},
   { 1658 , 1810},
   { 1440 , 1810},

  /* Customer */
   { 1533, 2011},
   { 1405, 2011},
   { 1790, 2256},
   { 1729, 2256},
   { 1731, 2217},
   { 1649, 2217},
   { 1932, 2527},
   { 1940, 2527},
   { 1688, 2286},
   { 1577, 2286},
   { 1761, 2413},
   { 1838, 2413},
   { 1614, 2184},
   { 1700, 2184},
   { 1393, 2092},
   { 1383, 2092},
   { 1807, 2448},
   { 1808, 2448},
   { 1603, 2240},
   { 1630, 2240},
   { 1942, 2316},
   { 1830, 2316},
   { 2013, 2502},
   { 1866, 2502},
   { 2030, 2561},
   { 1889, 2561},
   { 1832, 2375},
   { 1833, 2375},
   { 1798, 2332},
   { 1770, 2332},
   { 1953, 2588},
   { 1914, 2588},
   { 1575, 2284},
   { 1575, 2284},
   { 1689, 2317},
   { 1687, 2317},
   { 1715, 2252},
   { 1738, 2252},
   { 1498, 2207},
   { 1532, 2207},
   { 2047, 2678},
   { 2067, 2678},
   { 1912, 2549},
   { 1867, 2459},
   { 1771, 2150},
   { 1660, 2150},
   { 1897, 2266},
   { 1778, 2266} 
};
#endif

/************* silicon delendent ***************/

#ifndef DEFINE_GLOBALS
extern GT_32 np5SiliconDelayOffset[];
#else
GT_32 np5SiliconDelayOffset[] =
{
    /* board 0 */
    1450,
    /* board 1 */
    950,
    /* board 2 */
    950
};
#endif


GT_STATUS ddr3TipInitNp5
(
    GT_U32   devNum,
    GT_U32   boardId
);

#ifdef __cplusplus
}
#endif

#endif /* __mvHwsDdr3NP5_H */

