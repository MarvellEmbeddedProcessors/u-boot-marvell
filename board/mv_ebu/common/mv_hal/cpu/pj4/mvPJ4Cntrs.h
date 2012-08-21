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
#ifndef __mvPJ4Cntrs_h__
#define __mvPJ4Cntrs_h__

#define MV_CPU_CNTR_SIZE			32 /* bits */

#define MRVL_PJ4B_CCNT_BIT_OFFSET 	31

#define	MRVL_PJ4B_PMU_ENABLE	0x001	/* Enable counters */
#define MRVL_PJ4B_PMN_RESET		0x002	/* Reset event counters */
#define	MRVL_PJ4B_CCNT_RESET	0x004	/* Reset cycles counter */
#define	MRVL_PJ4B_PMU_RESET		(MRVL_PJ4B_CCNT_RESET | MRVL_PJ4B_PMN_RESET)
#define MRVL_PJ4B_PMU_CNT64		0x008	/* Make CCNT count every 64th cycle */

/*
* Different types of events that can be counted by the Marvell PJ4 Performance Monitor
*/
enum mrvl_pj4b_perf_types  {
	MRVL_PJ4B_SOFTWARE_INCR = 0x00,			/* software increment */
	MRVL_PJ4B_IFU_IFETCH_REFILL = 0x01,		/* instruction fetch that cause a refill at the lowest level
							   of instruction or unified cache */
	MRVL_PJ4B_IF_TLB_REFILL = 0x02,			/* instruction fetch that cause a TLB refill at the lowest
							   level of TLB */
	MRVL_PJ4B_DATA_RW_CACHE_REFILL = 0x03,		/* data read or write operation that causes a refill of at
							   the lowest level of data or unified cache */
	MRVL_PJ4B_DATA_RW_CACHE_ACCESS = 0x04,		/* data read or write operation that causes a cache access
							   at the lowest level of data or unified cache */
	MRVL_PJ4B_DATA_RW_TLB_REFILL = 0x05,		/* data read or write operation that causes a TLB refill at
							   the lowest level of TLB */
	MRVL_PJ4B_DATA_READ_INST_EXEC = 0x06,		/* data read architecturally executed */
	MRVL_PJ4B_DATA_WRIT_INST_EXEC = 0x07,		/* data write architecturally executed */
	MRVL_PJ4B_INSN_EXECUTED = 0x08,			/* instruction architecturally executed */
	MRVL_PJ4B_EXCEPTION_TAKEN = 0x09,		/* exception taken */
	MRVL_PJ4B_EXCEPTION_RETURN = 0x0a,		/* exception return architecturally executed */
	MRVL_PJ4B_INSN_WR_CONTEXTIDR = 0x0b,		/* instruction that writes to the Context ID Register
							   architecturally executed */
	MRVL_PJ4B_SW_CHANGE_PC = 0x0c,			/* software change of PC, except by an exception,
							   architecturally executed */
	MRVL_PJ4B_BR_EXECUTED = 0x0d,			/* immediate branch architecturally executed, taken or not
							   taken */
	MRVL_PJ4B_PROCEDURE_RETURN = 0x0e,		/* procedure return architecturally executed */
	MRVL_PJ4B_UNALIGNED_ACCESS = 0x0f,		/* unaligned access architecturally executed */
	MRVL_PJ4B_BR_INST_MISS_PRED = 0x10,		/* branch mispredicted or not predicted */
	MRVL_PJ4B_CYCLE_COUNT = 0x11,			/* cycle count */
	MRVL_PJ4B_BR_PRED_TAKEN = 0x12,			/* branches or other change in the program flow that could
							   have been predicted by the branch prediction resources
							   of the processor */
	MRVL_PJ4B_DCACHE_READ_HIT = 0x40,		/* counts the number of Data Cache read hits */
	MRVL_PJ4B_DCACHE_READ_MISS = 0x41,		/* connts the number of Data Cache read misses */
	MRVL_PJ4B_DCACHE_WRITE_HIT = 0x42,		/* counts the number of Data Cache write hits */
	MRVL_PJ4B_DCACHE_WRITE_MISS = 0x43,		/* counts the number of Data Cache write misses */
	MRVL_PJ4B_MMU_BUS_REQUEST = 0x44,		/* counts the number of cycles of request to the MMU Bus */
	MRVL_PJ4B_ICACHE_BUS_REQUEST = 0x45,		/* counts the number of cycles the Instruction Cache requests
							   the bus until the data return */
	MRVL_PJ4B_WB_WRITE_LATENCY = 0x46,		/* counts the number of cycles the Write Buffer requests the
							   bus */
	MRVL_PJ4B_HOLD_LDM_STM = 0x47,			/* counts the number of cycles the pipeline is held because
							   of a load/store multiple instruction */
	MRVL_PJ4B_NO_DUAL_CFLAG = 0x48,			/* counts the number of cycles the processor cannot dual issue
							   because of a Carry flag dependency */
	MRVL_PJ4B_NO_DUAL_REGISTER_PLUS = 0x49,		/* counts the number of cycles the processor cannot dual issue
							   because the register file does not have enough read ports
							   and at least one other reason */
	MRVL_PJ4B_LDST_ROB0_ON_HOLD = 0x4a,		/* counts the number of cycles a load or store instruction
							   waits to retire from ROB0 */
	MRVL_PJ4B_LDST_ROB1_ON_HOLD = 0x4b,		/* counts the number of cycles a load or store instruction
							   waits to retire from ROB0=1 */
	MRVL_PJ4B_DATA_WRITE_ACCESS_COUNT = 0x4c, 	/* counts the number of any Data write access */
	MRVL_PJ4B_DATA_READ_ACCESS_COUNT = 0x4d, 	/* counts the number of any Data read access */
	MRVL_PJ4B_A2_STALL = 0x4e, 			/* counts the number of cycles ALU A2 is stalled */
	/*TODO: implement with fabric counters*/
	MRVL_PJ4B_L2C_WRITE_HIT = 0x4f, 		/* counts the number of write accesses to addresses already
							   in the L2C */
	MRVL_PJ4B_L2C_WRITE_MISS = 0x50,		/* counts the number of write accesses to addresses not in
							   the L2C */
	MRVL_PJ4B_L2C_READ_COUNT = 0x51,		/* counts the number of L2C cache-to-bus external read
							   request */
	/*TODO: end*/
	MRVL_PJ4B_ICACHE_READ_MISS = 0x60, 		/* counts the number of Instruction Cache read misses */
	MRVL_PJ4B_ITLB_MISS = 0x61, 			/* counts the number of instruction TLB miss */
	MRVL_PJ4B_SINGLE_ISSUE = 0x62, 			/* counts the number of cycles the processor single issues */
	MRVL_PJ4B_BR_RETIRED = 0x63, 			/* counts the number of times one branch retires */
	MRVL_PJ4B_ROB_FULL = 0x64, 			/* counts the number of cycles the Re-order Buffer (ROB) is
							   full */
	MRVL_PJ4B_MMU_READ_BEAT = 0x65, 		/* counts the number of times the bus returns RDY to the MMU */
	MRVL_PJ4B_WB_WRITE_BEAT = 0x66, 		/* counts the number times the bus returns ready to the Write
							   Buffer */
	MRVL_PJ4B_DUAL_ISSUE = 0x67, 			/* counts the number of cycles the processor dual issues */
	MRVL_PJ4B_NO_DUAL_RAW = 0x68, 			/* counts the number of cycles the processor cannot dual
							   issue because of a Read after Write hazard */
	MRVL_PJ4B_HOLD_IS = 0x69, 			/* counts the number of cycles the issue is held */
	/*TODO: implement with fabric counters*/
	MRVL_PJ4B_L2C_LATENCY = 0x6a, 			/* counts the latency for the most recent L2C read from the
							   external bus Counts cycles */
	/*TODO: end*/
	MRVL_PJ4B_DCACHE_ACCESS = 0x70, 		/* counts the number of times the Data cache is accessed */
	MRVL_PJ4B_DTLB_MISS = 0x71, 			/* counts the number of data TLB misses */
	MRVL_PJ4B_BR_PRED_MISS = 0x72, 			/* counts the number of mispredicted branches */
	MRVL_PJ4B_A1_STALL = 0x74, 			/* counts the number of cycles ALU A1 is stalled */
	MRVL_PJ4B_DCACHE_READ_LATENCY = 0x75, 		/* counts the number of cycles the Data cache requests the
							   bus for a read */
	MRVL_PJ4B_DCACHE_WRITE_LATENCY = 0x76, 		/* counts the number of cycles the Data cache requests the
							   bus for a write */
	MRVL_PJ4B_NO_DUAL_REGISTER_FILE = 0x77, 	/* counts the number of cycles the processor cannot dual
							   issue because the register file doesn't have enough read
							   ports */
	MRVL_PJ4B_BIU_SIMULTANEOUS_ACCESS = 0x78, 	/* BIU Simultaneous Access */
	MRVL_PJ4B_L2C_READ_HIT = 0x79, 			/* counts the number of L2C cache-to-bus external read
							   requests */
	MRVL_PJ4B_L2C_READ_MISS = 0x7a, 		/* counts the number of L2C read accesses that resulted in an
							   external read request */
	MRVL_PJ4B_L2C_EVICTION = 0x7b, 			/* counts the number of evictions (CastOUT) of a line from
							   the L2 cache */
	MRVL_PJ4B_TLB_MISS = 0x80, 			/* counts the number of instruction and data TLB misses */
	MRVL_PJ4B_BR_TAKEN = 0x81, 			/* counts the number of taken branches */
	MRVL_PJ4B_WB_FULL = 0x82, 			/* counts the number of cycles WB is full */
	MRVL_PJ4B_DCACHE_READ_BEAT = 0x83, 		/* counts the number of times the bus returns Data to the
							   Data cache during read request */
	MRVL_PJ4B_DCACHE_WRITE_BEAT = 0x84, 		/* counts the number of times the bus returns ready to the
							   Data cache during write request */
	MRVL_PJ4B_NO_DUAL_HW = 0x85, 			/* counts the number of cycles the processor cannot dual
							   issue because of hardware conflict */
	MRVL_PJ4B_NO_DUAL_MULTIPLE = 0x86, 		/* counts the number of cycles the processor cannot dual
							   issue because of multiple reasons */
	MRVL_PJ4B_BIU_ANY_ACCESS = 0x87, 		/* counts the number of cycles the BIU is accessed by any
							   unit */
	MRVL_PJ4B_MAIN_TLB_REFILL_BY_ICACHE = 0x88, 	/* counts the number of instruction fetch operations that
							   causes a Main TLB walk */
	MRVL_PJ4B_MAIN_TLB_REFILL_BY_DCACHE = 0x89, 	/* counts the number of Data read or write operations that
							   causes a Main TLB walk */
	MRVL_PJ4B_ICACHE_READ_BEAT = 0x8a, 		/* counts the number of times the bus returns RDY to the
							   instruction cache */
	MRVL_PJ4B_PMUEXT_IN0 = 0x90, 			/* counts any event from external input source PMUEXTIN[0] */
	MRVL_PJ4B_PMUEXT_IN1 = 0x91, 			/* counts any event from external input source PMUEXTIN[1] */
	MRVL_PJ4B_PMUEXT_IN0_IN1 = 0x92, 		/* counts any event from both external input sources
							   PMUEXTIN[0] and PMUEXTIN[1] */
	MRVL_PJ4B_WMMX2_STORE_FIFO_FULL = 0xc0, 	/* counts the number of cycles when the WMMX2 store FIFO is
							   full */
	MRVL_PJ4B_WMMX2_FINISH_FIFO_FULL = 0xc1, 	/* counts the number of cycles when the WMMX2 finish FIFO is
							   full */
	MRVL_PJ4B_WMMX2_INST_FIFO_FULL = 0xc2, 		/* counts the number of cycles when the WMMX2 instruction FIFO
							   is full */
	MRVL_PJ4B_WMMX2_INST_RETIRED = 0xc3, 		/* counts the number of retired WMMX2 instructions */
	MRVL_PJ4B_WMMX2_BUSY = 0xc4, 			/* counts the number of cycles when the WMMX2 is busy */
	MRVL_PJ4B_WMMX2_HOLD_MI = 0xc5, 		/* counts the number of cycles when WMMX2 holds the issue
							   stage */
	MRVL_PJ4B_WMMX2_HOLD_MW = 0xc6, 		/* counts the number of cycles when WMMX2 holds the write back
							   stage */
	/* EVT_CCNT is not hardware defined */
	MRVL_PJ4B_EVT_CCNT = 0xFE,			/* CPU_CYCLE */
	MRVL_PJ4B_EVT_UNUSED = 0xFF,
};

enum  pj4b_pmu_counters {
	MRVL_PJ4B_CCNT = 0,
	MRVL_PJ4B_PMN0,
	MRVL_PJ4B_PMN1,
	MRVL_PJ4B_PMN2,
	MRVL_PJ4B_PMN3,
	MRVL_PJ4B_PMN4,
	MRVL_PJ4B_PMN5,
	MRVL_PJ4B_MAX_COUNTERS
};


static INLINE MV_VOID mrvl_pj4b_pmu_intr_disable(MV_U32 val)
{
	MV_ASM("mcr p15, 0, %0, c9, c14, 2" : : "r"(val));
}

static INLINE MV_VOID mrvl_pj4b_pmu_cntr_disable(MV_U32 val)
{
	MV_ASM("mcr p15, 0, %0, c9, c12, 2" : : "r"(val));
}

static INLINE MV_VOID mrvl_pj4b_pmu_intr_enable(MV_U32 val)
{
	MV_ASM("mcr p15, 0, %0, c9, c14, 1" : : "r"(val));
}

static INLINE MV_VOID mrvl_pj4b_pmu_cntr_enable(MV_U32 val)
{
	MV_ASM("mcr p15, 0, %0, c9, c12, 1" : : "r"(val));
}

static INLINE MV_VOID mrvl_pj4b_pmu_select_event(MV_U32 cntr, MV_U32 evt)
{
	MV_ASM("mcr p15, 0, %0, c9, c12, 5" : : "r"(cntr));
	MV_ASM("mcr p15, 0, %0, c9, c13, 1" : : "r"(evt));
}


static INLINE MV_VOID mrvl_pj4b_write_pmnc(MV_U32 val)
{
	MV_ASM("mcr p15, 0, %0, c9, c12, 0" : : "r"(val));
}

static INLINE MV_U32 mrvl_pj4b_read_pmnc(MV_VOID)
{
	MV_U32 val;

	MV_ASM("mrc p15, 0, %0, c9, c12, 0" : "=r"(val));

	return val;
}

static INLINE MV_U64 mvCpuPmCntrRead(const int counter)
{
	MV_U32 val;

	MV_ASM("mcr p15, 0, %0, c9, c12, 5" : : "r"(counter));
	MV_ASM("mrc p15, 0, %0, c9, c13, 2" : "=r"(val));

	return (MV_U64)val;
}

static INLINE MV_U64 mvCpuCyclesCntrRead(MV_VOID)
{
	MV_U32 val;

	MV_ASM("mrc p15, 0, %0, c9, c13, 0" : "=r" (val));

	return (MV_U64)val;
}

/* Read counter register */
static INLINE MV_U64 mvCpuCntrsRead(const int counter)
{
	MV_U64 val = 0;

	switch (counter) {
	case MRVL_PJ4B_CCNT:
		val = mvCpuCyclesCntrRead();
		break;

	case MRVL_PJ4B_PMN0:
	case MRVL_PJ4B_PMN1:
	case MRVL_PJ4B_PMN2:
	case MRVL_PJ4B_PMN3:
	case MRVL_PJ4B_PMN4:
	case MRVL_PJ4B_PMN5:
		val = mvCpuPmCntrRead(counter - MRVL_PJ4B_PMN0);
		break;
	}
	return (MV_U64)val;
}

static INLINE MV_VOID mvCpuCntrsReset(MV_VOID)
{
	MV_U32	reg;

	reg = mrvl_pj4b_read_pmnc();
	reg |= MRVL_PJ4B_PMU_RESET;
	mrvl_pj4b_write_pmnc(reg);
}


#endif /* __mvPJ4Cntrs_h__ */
