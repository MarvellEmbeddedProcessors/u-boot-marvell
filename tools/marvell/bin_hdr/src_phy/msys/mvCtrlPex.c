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

#include "config_marvell.h"     /* Required to identify SOC and Board */

#include "mv_os.h"
#include "mvBHboardEnvSpec.h"
#include "bin_hdr_twsi.h"
#include "mvUart.h"
#include "util.h"
#include "mvCtrlPex.h"
#include "ddr3_hws_hw_training_def.h"

#if defined(MV_MSYS_BC2)
#include "ddr3_msys_bc2.h"
#elif defined(MV_MSYS_AC3)
#include "ddr3_msys_ac3.h"
#else
#error "No MSYS device was defined"
#endif

#ifdef REGISTER_TRACE_DEBUG
static MV_U32 _MV_REG_READ(MV_U32 regAddr)
{
  putstring("\n >>>       MV_REG_READ.  regAddr=0x");
  putdata(INTER_REGS_BASE | (regAddr), 8);
  putstring(" regData=0x");
  MV_U32 regData = MV_MEMIO_LE32_READ((void *)(INTER_REGS_BASE | (regAddr)));
  putdata(regData, 8);

  return regData;
}

static MV_VOID _MV_REG_WRITE(MV_U32 regAddr, MV_U32 regData)
{
  putstring("\n >>>       MV_REG_WRITE. regAddr=0x");
  putdata(INTER_REGS_BASE | (regAddr), 8);
  putstring(" regData=0x");
  putdata(regData, 8);
  MV_MEMIO_LE32_WRITE((INTER_REGS_BASE | (regAddr)), (regData));
}

#define MV_REG_WRITE 	_MV_REG_WRITE
#define MV_REG_READ 	_MV_REG_READ
#endif /*REGISTER_TRACE_DEBUG*/

/************************ Local functions declarations ************************/

/**************************************************************************
* mvPexLocalBusNumSet -
*
* DESCRIPTION:          Sets PEX interface local bus number
*                       Note: In case the PEX interface is PEX-X,
*                       the information is read-only.
* INPUT:                pexIf           - PEX interface number.
*                       busNum          - Bus number.
* OUTPUT:               None.
* RETURNS:              MV_OK           -   for success
*                       MV_BAD_PARAM    -   when recieving a bad busNum
***************************************************************************/
MV_STATUS mvPexLocalBusNumSet
(
    MV_U32      pexIf,
    MV_U32      busNum
);

/**************************************************************************
* mvPexLocalBusNumSet -
*
* DESCRIPTION:          Set PEX interface local device number.
*                       Note: In case the PEX interface is PEX-X,
*                       the information is read-only.
* INPUT:                pexIf           -   PEX interface number.
*                       devNum          -   Device number.
* OUTPUT:               None.
* RETURNS:              MV_OK           -   for success
***************************************************************************/
MV_STATUS mvPexLocalDevNumSet
(
    MV_U32 pexIf,
    MV_U32 devNum
);

/*******************************************************************************
* mvPexConfigRead -
*
* DESCRIPTION:          Performs a 32 bit read from PEX configuration space.
*                       It supports both type 0 and type 1 of Configuration
*                       Transactions (local and over bridge).
*                       In order to read from local bus segment, uses
*                       bus number retrieved from mvPexLocalBusNumGet().
*                       Other bus numbers will result configuration
*                       transaction of type 1 (over bridge).*
* INPUT:                pexIf           -   PEX interface number.
*                       bus             -   PEX segment bus number.
*                       dev             -   PEX device number.
*                       func            -   Function number.
*                       regOffs         -   Register offset.
*                       regData         -   The data that will be returned as output
* RETURNS:               32bit register data, 0xffffffff on error
*******************************************************************************/
MV_U32 mvPexConfigRead
(
	MV_U32 pexIf,
	MV_U32 bus,
	MV_U32 dev,
	MV_U32 func,
	MV_U32 regOff
);

/*******************************************************************************
* mvCtrlModelGet -
*
* DESCRIPTION:      Returns 16bit describing the device model (ID) as defined
*                   in PCI Device and Vendor ID configuration register
*                   offset 0x0.
*
* INPUT:            None.
* OUTPUT:           None.
* RETURN:           16bit describing Marvell controller ID
*******************************************************************************/
MV_U16 mvCtrlModelGet(MV_VOID);

/************************** Functions implementation **************************/

/***************************************************************************/
MV_STATUS mvHwsPexConfig()
{
    MV_U32 pexIdx, tmp, next_busno, first_busno, tempPexReg, tempReg, addr, devId, ctrlMode;
    DEBUG_INIT_FULL_S("\n### mvHwsPexConfig ###\n");

	pexIdx = 0;
	tmp = MV_REG_READ(PEX_CAPABILITIES_REG(pexIdx));
	tmp &= ~(0xf<<20);
	tmp |= (0x4<<20);
	MV_REG_WRITE(PEX_CAPABILITIES_REG(pexIdx),tmp);

	tmp = MV_REG_READ(SOC_CTRL_REG);
	tmp &= ~(0x03);
	tmp |= 0x1<<PCIE0_ENABLE_OFFS;
	MV_REG_WRITE(SOC_CTRL_REG, tmp);

    /* support gen1/gen2 */
    DEBUG_INIT_FULL_S("Support gen1/gen2\n");
	next_busno = 0;

		DEBUG_INIT_FULL_S("\n");
		DEBUG_INIT_FULL_S(" pexIdx=0x");
		DEBUG_INIT_FULL_D(pexIdx, 8);
		DEBUG_INIT_FULL_S("\n");

        tmp = MV_REG_READ(PEX_DBG_STATUS_REG(pexIdx));

	    first_busno = next_busno;
        if((tmp & 0x7f) == 0x7E)
        {
		    next_busno++;
		    tempPexReg = MV_REG_READ((PEX_CFG_DIRECT_ACCESS(pexIdx, PEX_LINK_CAPABILITY_REG)));
		    tempPexReg &= (0xF);
		    if(tempPexReg == 0x2)
            {
			    tempReg = (MV_REG_READ(PEX_CFG_DIRECT_ACCESS(pexIdx, PEX_LINK_CTRL_STAT_REG)) & 0xF0000) >> 16;

				/* check if the link established is GEN1 */
                DEBUG_INIT_FULL_S("Checking if the link established is gen1\n");
				if (tempReg == 0x1)
                {
					mvPexLocalBusNumSet(pexIdx, first_busno);
					mvPexLocalDevNumSet(pexIdx, 1);
					DEBUG_INIT_FULL_S("PEX: pexIdx ");
					DEBUG_INIT_FULL_D(pexIdx, 1);


					DEBUG_INIT_S("** Link is Gen1, check the EP capability \n");
					/* link is Gen1, check the EP capability */
					addr = mvPexConfigRead(pexIdx, first_busno, 0, 0, 0x34) & 0xFF;
					DEBUG_INIT_FULL_C("mvPexConfigRead: return addr=0x%x", addr,4);
					if (addr == 0xff) {
						DEBUG_INIT_FULL_C("mvPexConfigRead: return 0xff -->PEX (%d): Detected No Link.", pexIdx,1);
						return MV_OK;
					}
					while ((mvPexConfigRead(pexIdx, first_busno, 0, 0, addr) & 0xFF) != 0x10) {
						addr = (mvPexConfigRead(pexIdx, first_busno, 0, 0, addr) & 0xFF00) >> 8;
					}
					if ((mvPexConfigRead(pexIdx, first_busno, 0, 0, addr + 0xC) & 0xF) >= 0x2) {
						tmp = MV_REG_READ(PEX_LINK_CTRL_STATUS2_REG(pexIdx));
						DEBUG_RD_REG(PEX_LINK_CTRL_STATUS2_REG(pexIdx),tmp );
						tmp &=~(BIT0 | BIT1);
						tmp |= BIT1;
						MV_REG_WRITE(PEX_LINK_CTRL_STATUS2_REG(pexIdx),tmp);
						DEBUG_WR_REG(PEX_LINK_CTRL_STATUS2_REG(pexIdx),tmp);

						tmp = MV_REG_READ(PEX_CTRL_REG(pexIdx));
						DEBUG_RD_REG(PEX_CTRL_REG(pexIdx), tmp );
						tmp |= BIT10;
						MV_REG_WRITE(PEX_CTRL_REG(pexIdx),tmp);
						DEBUG_WR_REG(PEX_CTRL_REG(pexIdx),tmp);
						mvOsUDelay(10000);/* We need to wait 10ms before reading the PEX_DBG_STATUS_REG in order not to read the status of the former state*/

						DEBUG_INIT_S("PEX: pexIdx ");
						DEBUG_INIT_D(pexIdx, 1);
						DEBUG_INIT_S(", Link upgraded to Gen2 based on client cpabilities \n");
					} else {
						DEBUG_INIT_S("PEX: pexIdx ");
						DEBUG_INIT_D(pexIdx, 1);
						DEBUG_INIT_S(", remains Gen1\n");
					}
				}
		    }
        }
        else
        {
            DEBUG_INIT_S("PEX: pexIdx ");
            DEBUG_INIT_D(pexIdx, 1);
            DEBUG_INIT_S(", detected no link\n");
        }

	/* update pex DEVICE ID*/
	ctrlMode = mvCtrlModelGet();

	devId = MV_REG_READ(PEX_CFG_DIRECT_ACCESS(pexIdx, PEX_DEVICE_AND_VENDOR_ID));
	devId &= 0xFFFF;
	devId |= ((ctrlMode << 16) & 0xFFFF0000);
	MV_REG_WRITE(PEX_CFG_DIRECT_ACCESS(pexIdx, PEX_DEVICE_AND_VENDOR_ID), devId);

	DEBUG_INIT_FULL_C("Update PEX Device ID ", ctrlMode, 4);

    return MV_OK;
}

/***************************************************************************/
MV_STATUS mvPexLocalBusNumSet(MV_U32 pexIf, MV_U32 busNum)
{
	MV_U32 pexStatus;

    DEBUG_INIT_FULL_S("\n### mvPexLocalBusNumSet ###\n");

	if (busNum >= MAX_PEX_BUSSES)
    {
		DEBUG_INIT_C("mvPexLocalBusNumSet: Illegal bus number %d\n", busNum, 4);
		return MV_BAD_PARAM;
	}

	pexStatus = MV_REG_READ(PEX_STATUS_REG(pexIf));
	pexStatus &= ~PXSR_PEX_BUS_NUM_MASK;
	pexStatus |= (busNum << PXSR_PEX_BUS_NUM_OFFS) & PXSR_PEX_BUS_NUM_MASK;
	MV_REG_WRITE(PEX_STATUS_REG(pexIf), pexStatus);

	return MV_OK;
}

/***************************************************************************/
MV_STATUS mvPexLocalDevNumSet
(
    MV_U32      pexIf,
    MV_U32      devNum
)
{
	MV_U32 pexStatus;

    DEBUG_INIT_FULL_S("\n### mvPexLocalDevNumSet ###\n");

	pexStatus = MV_REG_READ(PEX_STATUS_REG(pexIf));
	pexStatus &= ~PXSR_PEX_DEV_NUM_MASK;
	pexStatus |= (devNum << PXSR_PEX_DEV_NUM_OFFS) & PXSR_PEX_DEV_NUM_MASK;
	MV_REG_WRITE(PEX_STATUS_REG(pexIf), pexStatus);

	return MV_OK;
}

/*******************************************************************************
* mvPexConfigRead - Read from configuration space
*
* DESCRIPTION:
*       This function performs a 32 bit read from PEX configuration space.
*       It supports both type 0 and type 1 of Configuration Transactions
*       (local and over bridge). In order to read from local bus segment, use
*       bus number retrieved from mvPexLocalBusNumGet(). Other bus numbers
*       will result configuration transaction of type 1 (over bridge).
*
* INPUT:
*       pexIf   - PEX interface number.
*       bus     - PEX segment bus number.
*       dev     - PEX device number.
*       func    - Function number.
*       regOffs - Register offset.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit register data, 0xffffffff on error
*
*******************************************************************************/
MV_U32 mvPexConfigRead(MV_U32 pexIf, MV_U32 bus, MV_U32 dev, MV_U32 func, MV_U32 regOff)
{
  MV_U32 pexData = 0;
  MV_U32 localDev, localBus;
  MV_U32 pexStatus;

  pexStatus = MV_REG_READ(PEX_STATUS_REG(pexIf));

  localDev = ((pexStatus & PXSR_PEX_DEV_NUM_MASK) >> PXSR_PEX_DEV_NUM_OFFS);
  localBus = ((pexStatus & PXSR_PEX_BUS_NUM_MASK) >> PXSR_PEX_BUS_NUM_OFFS);


  /* in PCI Express we have only one device number */
  /* and this number is the first number we encounter
     else that the localDev */
  /* spec pex define return on config read/write on any device */
  if (bus == localBus) {
    if (localDev == 0) {
      /* if local dev is 0 then the first number we encounter
         after 0 is 1 */
      if ((dev != 1) && (dev != localDev))
        return MV_ERROR;
    } else {
      /* if local dev is not 0 then the first number we encounter
         is 0 */

      if ((dev != 0) && (dev != localDev))
        return MV_ERROR;
    }
  }
  /* Creating PEX address to be passed */
  pexData = (bus << PXCAR_BUS_NUM_OFFS);
  pexData |= (dev << PXCAR_DEVICE_NUM_OFFS);
  pexData |= (func << PXCAR_FUNC_NUM_OFFS);
  pexData |= (regOff & PXCAR_REG_NUM_MASK); /* lgacy register space */
  /* extended register space */
  pexData |= (((regOff & PXCAR_REAL_EXT_REG_NUM_MASK) >>
         PXCAR_REAL_EXT_REG_NUM_OFFS) << PXCAR_EXT_REG_NUM_OFFS);

  pexData |= PXCAR_CONFIG_EN;

  /* Write the address to the PEX configuration address register */
  MV_REG_WRITE(PEX_CFG_ADDR_REG(pexIf), pexData);

  /* In order to let the PEX controller absorbed the address of the read  */
  /* transaction we perform a validity check that the address was written */
  if (pexData != MV_REG_READ(PEX_CFG_ADDR_REG(pexIf)))
    return MV_ERROR;

  /* cleaning Master Abort */
  MV_REG_BIT_SET(PEX_CFG_DIRECT_ACCESS(pexIf, PEX_STATUS_AND_COMMAND), PXSAC_MABORT);
    /* Read the Data returned in the PEX Data register */
  pexData = MV_REG_READ(PEX_CFG_DATA_REG(pexIf));


  DEBUG_INIT_FULL_C(" --> ", pexData,4);

  return pexData;

}

/*******************************************************************************
* mvCtrlModelGet -
*
* DESCRIPTION:      Returns 16bit describing the device model (ID) as defined
*                   in PCI Device and Vendor ID configuration register
*                   offset 0x0.
*
* INPUT:            None.
* OUTPUT:           None.
* RETURN:           16bit describing Marvell controller ID
*******************************************************************************/
MV_U16 mvCtrlModelGet(MV_VOID)
{
	return 0xFC00 /*MV_BOBCAT2_DEV_ID*/;
}
