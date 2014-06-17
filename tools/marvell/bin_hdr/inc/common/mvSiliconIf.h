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
* siliconIf.h
*
* DESCRIPTION:
*       Application IF defintion.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 46 $
******************************************************************************/

#ifndef __siliconIf_H
#define __siliconIf_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mv_os.h"
#include "printf.h"
#include "soc_spec.h"


/* Global SysConf control library code for sysLog registration */
#ifdef CHECK_STATUS
#undef CHECK_STATUS
#endif

#define SERVER_WIN_ID					1
#define SWITCH_WIN_ID					5
#define SWITCH_REGS_BASE_ADDR_MASK		0xFC000000

static __inline MV_U32 SWITCH_WIN_BASE_ADDR_GET(MV_VOID)
{
	static MV_U32	baseAddr = 0;

	if (baseAddr == 0)
		baseAddr = MV_REG_READ(AHB_TO_MBUS_WIN_BASE_REG(SWITCH_WIN_ID));

	return baseAddr;
}

#define SWITCH_ADDR_COMPL_MSB_VAL(addr)	((addr >> 24) & 0xFF)
#define SWITCH_ADDR_COMPL_SHIFT(addr)	(((addr >> 24) & 0x3) << 3)
#define SWITCH_BUS_ADDR(addr)			((~SWITCH_REGS_BASE_ADDR_MASK & addr) |\
										(SWITCH_WIN_BASE_ADDR_GET() & SWITCH_REGS_BASE_ADDR_MASK))

static __inline MV_STATUS SWITCH_ADDR_COMPL_SET(MV_U32 addr)
{
	MV_U32	rVal;
	/* Configure address completion region REG using SERDES memory window */
	rVal  = MV_MEMIO_LE32_READ(SWITCH_WIN_BASE_ADDR_GET());
	rVal &= ~(0xFF << SWITCH_ADDR_COMPL_SHIFT(addr));
	rVal |= SWITCH_ADDR_COMPL_MSB_VAL(addr) << SWITCH_ADDR_COMPL_SHIFT(addr);
	MV_MEMIO_LE32_WRITE(SWITCH_WIN_BASE_ADDR_GET(), rVal);

	return MV_OK;
}

#define CHECK_STATUS(origFunc) \
do { \
	MV_STATUS mvStatus; \
	mvStatus = origFunc; \
	if (MV_OK != mvStatus) { \
		mvPrintf("Error %d in File:%s Line:%d\n",mvStatus, __FILE__, __LINE__);\
		return mvStatus; \
	} \
} while(0)

typedef enum
{
	INTERNAL_REG_UNIT,
	MG_UNIT,
	SERDES_UNIT,
	SERDES_PHY_UNIT,

	LAST_UNIT

} MV_HWS_UNITS_ID;

typedef struct
{
	MV_U32 baseAddr;
	MV_U32 regOffset;
} HWS_UNIT_INFO;


/*******************************************************************************
* mvUnitInfoGet
*
* DESCRIPTION:
*       Return silicon specific base address and index for specified unit based on
*       unit index.
*
* INPUTS:
*       unitId    - unit ID (MAC, PCS, SERDES)
*       unitNum   - unit number
*
* OUTPUTS:
*       baseAddr        - unit base address in device
*       unitIndexOffset - unit offset per each index
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
void  mvUnitInfoGet
(
	MV_HWS_UNITS_ID	unitId,
	MV_U8			unitNum,
	MV_U32			*baseAddr,
	MV_U32			*unitIndexOffset
);

/*******************************************************************************
* mvUnitInfoSet
*
* DESCRIPTION:
*       Init silicon specific base address and index for specified unit
*
* INPUTS:
*       unitId          - Unit ID
*       baseAddr        - base address of the unit
*       unitIndexOffset - Unit offset per each index
*
* OUTPUTS:
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
MV_STATUS  mvUnitInfoSet
(
	MV_HWS_UNITS_ID	unitId,
	MV_U32			baseAddr,
	MV_U32			unitIndexOffset
);

/*******************************************************************************
* mvGenUnitRegisterSet
*
* DESCRIPTION:
*       Implement write access to device registers.
*
* INPUTS:
*       unitId    - Unit ID
*       unitNum   - Unit number
*       regOffset - address to access
*       data      - data to write
*       mask      - mask for data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
MV_STATUS mvGenUnitRegisterSet
(
	MV_HWS_UNITS_ID unitId,
	MV_U8			unitNum,
	MV_U32			regOffset,
	MV_U32			data,
	MV_U32			mask
);

/*******************************************************************************
* mvGenUnitRegisterGet
*
* DESCRIPTION:
*       Read access to device registers.
*
* INPUTS:
*       unitId    - Unit ID
*       unitNum   - Unit number
*       regOffset - address to access
*       mask      - mask for data
*
* OUTPUTS:
*       data      - read data
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
MV_STATUS mvGenUnitRegisterGet
(
	MV_HWS_UNITS_ID unitId,
	MV_U8			unitNum,
	MV_U32			regOffset,
	MV_U32			*data,
	MV_U32			mask
);

/*******************************************************************************
* serverRegisterGet
*
* DESCRIPTION:
*       Read access to server registers.
*
* INPUTS:
*       regOffset    -
*       mask         -
*
* OUTPUTS:
*       data      - read data
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
MV_STATUS mvServerRegisterGet
(
	MV_U32			regOffset,
	MV_U32			*data,
	MV_U32			mask
);

/*******************************************************************************
* mvServerRegisterSet
*
* DESCRIPTION:
*       Read access to server registers.
*
* INPUTS:
*       regOffset    -
*       data         - write data
*       mask         -
*
* OUTPUTS:
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
MV_STATUS mvServerRegisterSet
(
	MV_U32			regOffset,
	MV_U32			data,
	MV_U32			mask
);

#ifdef __cplusplus
}
#endif

#endif /* __siliconIf_H */
