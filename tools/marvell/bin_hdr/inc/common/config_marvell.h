/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
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
#ifndef __CONFIG_MARVELL_H
#define __CONFIG_MARVELL_H

/***********/
/* General */
/***********/

#define MV_DDR_64BIT
#define MV_MEM_FASTPATH

/**********************************/
/* defines for DB_88F6710   */
/**********************************/
#ifdef CONFIG_DB_88F6710_BP
#define DB_88F6710
#define MV88F6710
#define MV88F67XX

/**********************************/
/* defines for DB-RD_88F6710	  */
/**********************************/
#elif defined(CONFIG_RD_88F6710)
#define RD_88F6710
#define MV88F6710
#define MV88F67XX

/**********************************/
/* defines for DB-78x60-BP REV2   */
/**********************************/
#elif defined(CONFIG_DB_78x60_BP_REV2)
#define MV88F78X60
#define DB_88F78X60_REV2

#elif defined(CONFIG_DB_78X60_PCAC_REV2)

/**********************************/
/* defines for DB-78x60-PCAC REV2   */
/**********************************/
	#define DB_78X60_PCAC_REV2
	#define MV88F78X60
#elif defined(CONFIG_DB_78X60_AMC)

/**********************************/
/* defines for DB-78x60-AMC   */
/**********************************/
	#define DB_78X60_AMC
	#define MV88F78X60

#elif defined(CONFIG_RD_78460_NAS)

/**********************************/
/* defines for DB-78x60-NAS        */
/**********************************/
	#define RD_78460_NAS
	#define MV88F78X60

#elif defined(CONFIG_RD_78460_SERVER_REV2)

/**********************************/
/* defines for RD-78x60-SERVER    */
/**********************************/

	#define RD_78460_SERVER_REV2
	#define MV88F78X60

#elif defined(CONFIG_DB_784MP_GP)

/**********************************/
/* defines for DB-784MP-GP        */
/**********************************/

	#define DB_784MP_GP
	#define MV88F78X60

#elif defined(CONFIG_RD_78460_CUSTOMER)

/**********************************/
/* defines for DB-784MP-CUSTOMER  */
/**********************************/

	#define RD_78460_CUSTOMER
	#define MV88F78X60

#elif defined(CONFIG_ARMADA_375)

/**********************************/
/* defines for DB-6720  */
/**********************************/

	#define MV88F672X

/***************************************/
/* defines for Avanta LP - All boards  */
/***************************************/
#elif defined(CONFIG_AVANTA_LP)

	#define MV88F66XX

#elif defined(CONFIG_ARMADA_38X)

/**********************************/
/* defines for DB-6820  */
/**********************************/

	#define MV88F682X
	#define MV88F68XX

#elif defined(CONFIG_ARMADA_39X)
/**********************************/
/* defines for DB-6920  */
/**********************************/

	#define MV88F69XX

/*********************************/
/* defines for BOBCAT2    */
/*********************************/
#elif defined(CONFIG_DB_BOBCAT2)
	#define MV_MSYS_BC2
	#define DB_BOBCAT2
#elif defined(CONFIG_RD_BOBCAT2)
	#define MV_MSYS_BC2
	#define RD_BOBCAT2
#elif defined(CONFIG_BOBCAT2) /* general define for customer compilation */
	#define MV_MSYS_BC2
/*********************************/
/* defines for AlleyCat3        */
/*********************************/
#elif defined(CONFIG_ALLEYCAT3) /* general define for customer compilation */
	#define MV_MSYS_AC3
#endif

#endif  /* __CONFIG_MARVELL_H */
