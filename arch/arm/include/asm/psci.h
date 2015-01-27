/*
 * Copyright (C) 2013 - ARM Ltd
 * Author: Marc Zyngier <marc.zyngier@arm.com>
 *
 * Copyright (C) 2014 - Freescale Semiconductor Ltd
 * Author: Arnab Basu <arnab.basu@freescale.com>
 *	   updated file for PSCI v0.2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __ARM_PSCI_H__
#define __ARM_PSCI_H__

/* PSCI v0.1 interface */
#define ARM_PSCI_FN_BASE		0x95c1ba5e
#define ARM_PSCI_FN(n)			(ARM_PSCI_FN_BASE + (n))

#define ARM_PSCI_FN_CPU_SUSPEND		ARM_PSCI_FN(0)
#define ARM_PSCI_FN_CPU_OFF		ARM_PSCI_FN(1)
#define ARM_PSCI_FN_CPU_ON		ARM_PSCI_FN(2)
#define ARM_PSCI_FN_MIGRATE		ARM_PSCI_FN(3)

/* PSCI v0.2 interface */
#define PSCI_0_2_FN_BASE		0x84000000
#define PSCI_0_2_FN(n)			(PSCI_0_2_FN_BASE + (n))
#define PSCI_0_2_64BIT			0x40000000
#define PSCI_0_2_FN64_BASE		\
				(PSCI_0_2_FN_BASE + PSCI_0_2_64BIT)
#define PSCI_0_2_FN64(n)		(PSCI_0_2_FN64_BASE + (n))

#define PSCI_0_2_FN_PSCI_VERSION	PSCI_0_2_FN(0)
#define PSCI_0_2_FN_CPU_SUSPEND		PSCI_0_2_FN(1)
#define PSCI_0_2_FN_CPU_OFF		PSCI_0_2_FN(2)
#define PSCI_0_2_FN_CPU_ON		PSCI_0_2_FN(3)
#define PSCI_0_2_FN_AFFINITY_INFO	PSCI_0_2_FN(4)
#define PSCI_0_2_FN_MIGRATE		PSCI_0_2_FN(5)
#define PSCI_0_2_FN_MIGRATE_INFO_TYPE	PSCI_0_2_FN(6)
#define PSCI_0_2_FN_MIGRATE_INFO_UP_CPU	PSCI_0_2_FN(7)
#define PSCI_0_2_FN_SYSTEM_OFF		PSCI_0_2_FN(8)
#define PSCI_0_2_FN_SYSTEM_RESET	PSCI_0_2_FN(9)

#define PSCI_0_2_FN64_CPU_SUSPEND		PSCI_0_2_FN64(1)
#define PSCI_0_2_FN64_CPU_ON			PSCI_0_2_FN64(3)
#define PSCI_0_2_FN64_AFFINITY_INFO		PSCI_0_2_FN64(4)
#define PSCI_0_2_FN64_MIGRATE			PSCI_0_2_FN64(5)
#define PSCI_0_2_FN64_MIGRATE_INFO_UP_CPU	PSCI_0_2_FN64(7)


/*
 * Only PSCI return values such as: SUCCESS, NOT_SUPPORTED,
 * INVALID_PARAMS, and DENIED defined below are applicable
 * to PSCI v0.1.
 */
#define ARM_PSCI_RET_SUCCESS		0
#define ARM_PSCI_RET_NI			(-1)
#define ARM_PSCI_RET_INVAL		(-2)
#define ARM_PSCI_RET_DENIED		(-3)
#define PSCI_RET_ALREADY_ON		(-4)
#define PSCI_RET_ON_PENDING		(-5)
#define PSCI_RET_INTERNAL_FAILURE	(-6)
#define PSCI_RET_NOT_PRESENT		(-7)
#define PSCI_RET_DISABLED		(-8)

#endif /* __ARM_PSCI_H__ */
