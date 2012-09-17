/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

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

*******************************************************************************/
/*******************************************************************************
* mvSysEthConfig.h - Marvell Ethernet unit specific configurations
*
* DESCRIPTION:
*       None.
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

#ifndef __mvSysEthConfig_h__
#define __mvSysEthConfig_h__

#include "mvSysHwConfig.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"

/* u-boot is running in legacy mode! */
#ifdef CONFIG_MV_ETH_LEGACY
#define MV_ETH_LEGACY
#define MV_LEGACY_ETH_WA
#endif

#ifdef CONFIG_MV_ETH_NETA
#define MV_ETH_NETA

#ifndef CONFIG_MV_ETH_LEGACY_PARSER
#define CONFIG_MV_ETH_LEGACY_PARSER
#endif /* CONFIG_MV_ETH_LEGACY_PARSER */

#define CONFIG_MV_ETH_TXQ   1
#define CONFIG_MV_ETH_RXQ   1
#define CONFIG_MV_ETH_RXQ_DEF 0
//#define CONFIG_MV_PON
#define CONFIG_MV_PON_TCONTS 1
#endif

/*
** Base address for ethernet registers.
*/
#ifdef CONFIG_MV_PON
#define MV_PON_PORT(p)		((p) == MV_PON_PORT_ID)
#define MV_PON_REG_BASE         MV_PON_REGS_OFFSET
#define MV_ETH_REGS_BASE(p)	(MV_PON_PORT(p) ? MV_PON_REGS_OFFSET : MV_ETH_REGS_OFFSET(p))
#else
#define MV_PON_PORT(p)		MV_FALSE
#define MV_ETH_REGS_BASE(p)	MV_ETH_REGS_OFFSET(p)
#endif /* CONFIG_MV_PON */

#define MV_BM_REG_BASE		MV_BM_REGS_OFFSET
#define MV_PNC_REG_BASE         MV_PNC_REGS_OFFSET
#define MV_ETH_COMPLEX_BASE		(MV_ETH_COMPLEX_OFFSET)
#define MV_ETH_ONLY_REGS_BASE		(MV_ETH_ONLY_REGS_OFFSET)

#if defined(CONFIG_MV_INCLUDE_GIG_ETH)

/* put descriptors in uncached memory */
/* #define ETH_DESCR_UNCACHED */

/* port's default queueus */
#define ETH_DEF_RXQ         0

#ifdef MV_ETH_LEGACY

#ifdef MV_NFP_STATS
#define MV_FP_STATISTICS
#else
#undef MV_FP_STATISTICS
#endif

/* Default configuration for TX_EN workaround: 0 - Disabled, 1 - Enabled */
#define MV_ETH_TX_EN_DEFAULT        0

/* un-comment if you want to perform tx_done from within the poll function */
/* #define ETH_TX_DONE_ISR */

/* put descriptors in uncached memory */
/* #define ETH_DESCR_UNCACHED */

/* Descriptors location: DRAM/internal-SRAM */
#define ETH_DESCR_IN_SDRAM
#undef  ETH_DESCR_IN_SRAM    /* No integrated SRAM in 88Fxx81 devices */

#if defined(ETH_DESCR_IN_SRAM)
#if defined(ETH_DESCR_UNCACHED)
 #define ETH_DESCR_CONFIG_STR    "Uncached descriptors in integrated SRAM"
#else
 #define ETH_DESCR_CONFIG_STR    "Cached descriptors in integrated SRAM"
#endif
#elif defined(ETH_DESCR_IN_SDRAM)
#if defined(ETH_DESCR_UNCACHED)
 #define ETH_DESCR_CONFIG_STR    "Uncached descriptors in DRAM"
#else
 #define ETH_DESCR_CONFIG_STR    "Cached descriptors in DRAM"
#endif
#else
 #error "Ethernet descriptors location undefined"
#endif /* ETH_DESCR_IN_SRAM or ETH_DESCR_IN_SDRAM*/

/* SW Sync-Barrier: not relevant for 88fxx81*/
/* Reasnable to define this macro when descriptors in SRAM and buffers in DRAM */
/* In RX the CPU theoretically might see himself as the descriptor owner,      */
/* although the buffer hadn't been written to DRAM yet. Performance cost.      */
/* #define INCLUDE_SYNC_BARR */

/* Buffers cache coherency method (buffers in DRAM) */
#ifndef MV_CACHE_COHER_SW
/* Taken from mvCommon.h */
/* Memory uncached, HW or SW cache coherency is not needed */
#define MV_UNCACHED             0
/* Memory cached, HW cache coherency supported in WriteThrough mode */
#define MV_CACHE_COHER_HW_WT    1
/* Memory cached, HW cache coherency supported in WriteBack mode */
#define MV_CACHE_COHER_HW_WB    2
/* Memory cached, No HW cache coherency, Cache coherency must be in SW */
#define MV_CACHE_COHER_SW       3

#endif

#define ETHER_DRAM_COHER    MV_CACHE_COHER_SW   /* No HW coherency in 88Fxx81 devices */

#if (ETHER_DRAM_COHER == MV_CACHE_COHER_HW_WB)
 #define ETH_SDRAM_CONFIG_STR    "DRAM HW cache coherency (write-back)"
#elif (ETHER_DRAM_COHER == MV_CACHE_COHER_HW_WT)
 #define ETH_SDRAM_CONFIG_STR    "DRAM HW cache coherency (write-through)"
#elif (ETHER_DRAM_COHER == MV_CACHE_COHER_SW)
 #define ETH_SDRAM_CONFIG_STR    "DRAM SW cache-coherency"
#elif (ETHER_DRAM_COHER == MV_UNCACHED)
#   define ETH_SDRAM_CONFIG_STR  "DRAM uncached"
#else
 #error "Ethernet-DRAM undefined"
#endif /* ETHER_DRAM_COHER */


/****************************************************************/
/************* Ethernet driver configuration ********************/
/****************************************************************/

/* port's default queueus */
#define ETH_DEF_TXQ         0
#define ETH_DEF_RXQ         0

#define MV_ETH_RX_Q_NUM     CONFIG_MV_ETH_RXQ
#define MV_ETH_TX_Q_NUM     CONFIG_MV_ETH_TXQ

/* interrupt coalescing setting */
#define ETH_TX_COAL    		    200
#define ETH_RX_COAL    		    200

/* Checksum offloading */
#define TX_CSUM_OFFLOAD
#define RX_CSUM_OFFLOAD
#endif /* CONFIG_MV_ETH_LEGACY */

#endif /* CONFIG_MV_INCLUDE_GIG_ETH */

#endif /* __mvSysEthConfig_h__ */
