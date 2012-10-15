#include <Copyright.h>

/*******************************************************************************
* gtTCAM.c
*
* DESCRIPTION:
*       API definitions for control of Ternary Content Addressable Memory
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <msApi.h>
#include <gtSem.h>
#include <gtHwCntl.h>
#include <gtDrvSwRegs.h>

/****************************************************************************/
/* TCAM operation function declaration.                                    */
/****************************************************************************/
static GT_STATUS tcamOperationPerform
(
    IN   GT_QD_DEV             *dev,
    IN   GT_TCAM_OPERATION    tcamOp,
    INOUT GT_TCAM_OP_DATA    *opData
);

/*******************************************************************************
* gtcamFlushAll
*
* DESCRIPTION:
*       This routine is to flush all entries. A Flush All command will initialize 
*       TCAM Pages 0 and 1, offsets 0x02 to 0x1B to 0x0000, and TCAM Page 2 offset
*       0x02 to 0x05 to 0x0000 for all TCAM entries with the exception that TCAM 
*       Page 0 offset 0x02 will be initialized to 0x00FF.
*
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtcamFlushAll
(
    IN  GT_QD_DEV     *dev
)
{
    GT_STATUS           retVal;
    GT_TCAM_OPERATION    op;
    GT_TCAM_OP_DATA     tcamOpData;

    DBG_INFO(("gtcamFlushAll Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TCAM))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Program Tuning register */
    op = TCAM_FLUSH_ALL;
    tcamOpData.tcamEntry = 0xFF;
    retVal = tcamOperationPerform(dev,op, &tcamOpData);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed (tcamOperationPerform returned GT_FAIL).\n"));
        return retVal;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;

}

/*******************************************************************************
* gtcamFlushEntry
*
* DESCRIPTION:
*       This routine is to flush a single entry. A Flush a single TCAM entry command 
*       will write the same values to a TCAM entry as a Flush All command, but it is
*       done to the selected single TCAM entry only.
*
*
* INPUTS:
*        tcamPointer - pointer to the desired entry of TCAM (0 ~ 254)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtcamFlushEntry
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32        tcamPointer
)
{
    GT_STATUS           retVal;
    GT_TCAM_OPERATION    op;
    GT_TCAM_OP_DATA     tcamOpData;

    DBG_INFO(("gtcamFlushEntry Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TCAM))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* check if the given pointer is valid */
    if (tcamPointer > 0xFE)
    {
        DBG_INFO(("GT_BAD_PARAM\n"));
        return GT_BAD_PARAM;
    }

    /* Program Tuning register */
    op = TCAM_FLUSH_ALL;
    tcamOpData.tcamEntry = tcamPointer;
    retVal = tcamOperationPerform(dev,op, &tcamOpData);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed (tcamOperationPerform returned GT_FAIL).\n"));
        return retVal;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;

}

/*******************************************************************************
* gtcamLoadEntry
*
* DESCRIPTION:
*       This routine loads a TCAM entry.
*    	The load sequence of TCAM entry is critical. Each TCAM entry is made up of
*       3 pages of data. All 3 pages need to loaded in a particular order for the TCAM 
*       to operate correctly while frames are flowing through the switch. 
*       If the entry is currently valid, it must first be flushed. Then page 2 needs 
*       to be loaded first, followed by page 1 and then finally page 0. 
*       Each page load requires its own write TCAMOp with these TCAM page bits set 
*       accordingly. 
*
* INPUTS:
*        tcamPointer - pointer to the desired entry of TCAM (0 ~ 254)
*        tcamData    - Tcam entry Data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtcamLoadEntry
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32        tcamPointer,
    IN  GT_TCAM_DATA        *tcamData
)
{
    GT_STATUS           retVal;
    GT_TCAM_OPERATION    op;
    GT_TCAM_OP_DATA     tcamOpData;

    DBG_INFO(("gtcamLoadEntry Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TCAM))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* check if the given pointer is valid */
    if ((tcamPointer > 0xFE)||(tcamData==NULL))
    {
        DBG_INFO(("GT_BAD_PARAM\n"));
        return GT_BAD_PARAM;
    }

    /* Program Tuning register */
    op = TCAM_LOAD_ENTRY;
    tcamOpData.tcamPage = 0; /* useless */
    tcamOpData.tcamEntry = tcamPointer;
    tcamOpData.tcamDataP = tcamData;
    retVal = tcamOperationPerform(dev,op, &tcamOpData);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed (tcamOperationPerform returned GT_FAIL).\n"));
        return retVal;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;

}

/*******************************************************************************
* gtcamPurgyEntry
*
* DESCRIPTION:
*       This routine Purgy a TCAM entry.
*
* INPUTS:
*        tcamPointer - pointer to the desired entry of TCAM (0 ~ 254)
*        tcamData    - Tcam entry Data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtcamPurgyEntry
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32        tcamPointer,
    IN  GT_TCAM_DATA        *tcamData
)
{
    GT_STATUS           retVal;
    GT_TCAM_OPERATION    op;
    GT_TCAM_OP_DATA     tcamOpData;

    DBG_INFO(("gtcamPurgyEntry Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TCAM))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* check if the given pointer is valid */
    if ((tcamPointer > 0xFE)||(tcamData==NULL))
    {
        DBG_INFO(("GT_BAD_PARAM\n"));
        return GT_BAD_PARAM;
    }

    /* Program Tuning register */
    op = TCAM_LOAD_ENTRY;
    tcamOpData.tcamPage = 0; /* useless */
    tcamOpData.tcamEntry = tcamPointer;
    retVal = tcamOperationPerform(dev,op, &tcamOpData);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed (tcamOperationPerform returned GT_FAIL).\n"));
        return retVal;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;

}


/*******************************************************************************
* gtcamReadTCAMData
*
* DESCRIPTION:
*       This routine loads the global 3 offsets 0x02 to 0x1B registers with 
*       the data found in the TCAM entry and its TCAM page pointed to by the TCAM
*       entry and TCAM page bits of this register (bits 7:0 and 11:10 respectively.
*
*
* INPUTS:
*        tcamPointer - pointer to the desired entry of TCAM (0 ~ 254)
*
* OUTPUTS:
*        tcamData    - Tcam entry Data
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtcamReadTCAMData
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32        tcamPointer,
    OUT GT_TCAM_DATA        *tcamData
)
{
    GT_STATUS           retVal;
    GT_TCAM_OPERATION    op;
    GT_TCAM_OP_DATA     tcamOpData;

    DBG_INFO(("gtcamReadTCAMData Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TCAM))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* check if the given pointer is valid */
    if ((tcamPointer > 0xFE)||(tcamData==NULL))
    {
        DBG_INFO(("GT_BAD_PARAM\n"));
        return GT_BAD_PARAM;
    }

    /* Program Tuning register */
    op = TCAM_READ_ENTRY;
    tcamOpData.tcamPage = 0; /* useless */
    tcamOpData.tcamEntry = tcamPointer;
    tcamOpData.tcamDataP = tcamData;
    retVal = tcamOperationPerform(dev,op, &tcamOpData);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed (tcamOperationPerform returned GT_FAIL).\n"));
        return retVal;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;

}

/*******************************************************************************
* gtcamGetNextTCAMData
*
* DESCRIPTION:
*       This routine  finds the next higher TCAM Entry number that is valid (i.e.,
*       any entry whose Page 0 offset 0x02 is not equal to 0x00FF). The TCAM Entry 
*       register (bits 7:0) is used as the TCAM entry to start from. To find 
*       the lowest number TCAM Entry that is valid, start the Get Next operation 
*       with TCAM Entry set to 0xFF.
*
*
* INPUTS:
*        tcamPointer - pointer to the desired entry of TCAM (0 ~ 255)
*
* OUTPUTS:
*        tcamData    - Tcam entry Data
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtcamGetNextTCAMData
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32        tcamPointer,
    OUT GT_TCAM_DATA        *tcamData
)
{
    GT_STATUS           retVal;
    GT_TCAM_OPERATION    op;
    GT_TCAM_OP_DATA     tcamOpData;

    DBG_INFO(("gtcamGetNextTCAMData Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TCAM))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* check if the given pointer is valid */
    if ((tcamPointer > 0xFF)||(tcamData==NULL))
    {
        DBG_INFO(("GT_BAD_PARAM\n"));
        return GT_BAD_PARAM;
    }

    /* Program Tuning register */
    op = TCAM_GET_NEXT_ENTRY;
    tcamOpData.tcamPage = 0; /* useless */
    tcamOpData.tcamEntry = tcamPointer;
    tcamOpData.tcamDataP = tcamData;
    retVal = tcamOperationPerform(dev,op, &tcamOpData);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed (tcamOperationPerform returned GT_FAIL).\n"));
        return retVal;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;

}


/****************************************************************************/
/* Internal functions.                                                  */
/****************************************************************************/
static GT_STATUS tcamSetPage0Data(GT_QD_DEV *dev, GT_TCAM_DATA *tcamDataP)
{
  GT_STATUS       retVal;    /* Functions return value */
  GT_U16          data;     /* temporary Data storage */
  int i;

  data = ((tcamDataP->paraFrm.frameTypeMask<<14) | (tcamDataP->paraFrm.frameType<<6) );
  retVal = hwWriteGlobal3Reg(dev,QD_REG_TCAM_P0_KEYS_1,data);
  if(retVal != GT_OK)
  {
     return retVal;
  }
  data = ((tcamDataP->paraFrm.spvMask<<8) | (tcamDataP->paraFrm.spv<<0)  );
  retVal = hwWriteGlobal3Reg(dev,QD_REG_TCAM_P0_KEYS_2,data);
  if(retVal != GT_OK)
  {
     return retVal;
  }
  data = ((tcamDataP->paraFrm.ppriMask<<12) | (tcamDataP->paraFrm.ppri<<4));
  data = (((tcamDataP->paraFrm.pvidMask&0x0f00)&0x000f) | ((tcamDataP->paraFrm.pvid>>8)&0x000f));
  retVal = hwWriteGlobal3Reg(dev,QD_REG_TCAM_P0_KEYS_3,data);
  if(retVal != GT_OK)
  {
     return retVal;
  }
  data = ((tcamDataP->paraFrm.pvidMask&0x00ff)<<8 | (tcamDataP->paraFrm.pvid&0x00ff));
  retVal = hwWriteGlobal3Reg(dev,QD_REG_TCAM_P0_KEYS_4,data);
  if(retVal != GT_OK)
  {
     return retVal;
  }
  for(i=0; i<22; i++)
  {
    data = ((tcamDataP->paraFrm.frameOctetMask[i])<<8 | (tcamDataP->paraFrm.frameOctet[i]));
    retVal = hwWriteGlobal3Reg(dev,QD_REG_TCAM_P0_MATCH_DATA_1+i,data);
    if(retVal != GT_OK)
	{
     return retVal;
	}
  }

  return GT_OK;
}
static GT_STATUS tcamSetPage1Data(GT_QD_DEV *dev, GT_TCAM_DATA *tcamDataP)
{
  GT_STATUS       retVal;    /* Functions return value */
  GT_U16          data;     /* temporary Data storage */
  int i;

  for(i=0; i<25; i++)
  {
    data = ((tcamDataP->paraFrm.frameOctetMask[i+23])<<8 | (tcamDataP->paraFrm.frameOctet[i+23]));
    retVal = hwWriteGlobal3Reg(dev,QD_REG_TCAM_P1_MATCH_DATA_23+i,data);
    if(retVal != GT_OK)
	{
     return retVal;
	}
  }

  return GT_OK;
}

static GT_STATUS tcamSetPage2Data(GT_QD_DEV *dev, GT_TCAM_DATA *tcamDataP)
{
  GT_STATUS       retVal;    /* Functions return value */
  GT_U16          data;     /* temporary Data storage */

  data = ((tcamDataP->paraFrm.continu<<15) | (tcamDataP->paraFrm.interrupt<<14) |
	  (tcamDataP->paraFrm.IncTcamCtr<<13) | (tcamDataP->paraFrm.vidOverride<<12) |
	  (tcamDataP->paraFrm.vidData&0x07ff));
  retVal = hwWriteGlobal3Reg(dev,QD_REG_TCAM_P2_ACTION_1,data);
  if(retVal != GT_OK)
  {
     return retVal;
  }
  data = ((tcamDataP->paraFrm.nextId<<8) | (tcamDataP->paraFrm.qpriOverride<<7) |
	  (tcamDataP->paraFrm.qpriData<<4) | (tcamDataP->paraFrm.fpriOverride<<3) |
	  (tcamDataP->paraFrm.fpriData));
  retVal = hwWriteGlobal3Reg(dev,QD_REG_TCAM_P2_ACTION_2,data);
  if(retVal != GT_OK)
  {
     return retVal;
  }
  data = ((tcamDataP->paraFrm.qpriAvbOverride<<15) | (tcamDataP->paraFrm.qpriAvbData<<12) |
	  (tcamDataP->paraFrm.dpvOverride<<11) | (tcamDataP->paraFrm.dpvData) );
  retVal = hwWriteGlobal3Reg(dev,QD_REG_TCAM_P2_ACTION_3,data);
  if(retVal != GT_OK)
  {
     return retVal;
  }
  data = ((tcamDataP->paraFrm.factionOverride<<15) | (tcamDataP->paraFrm.factionData<<4) |
	  (tcamDataP->paraFrm.ldBalanceOverride<<3) | (tcamDataP->paraFrm.ldBalanceData) );
  retVal = hwWriteGlobal3Reg(dev,QD_REG_TCAM_P2_ACTION_4,data);
  if(retVal != GT_OK)
  {
     return retVal;
  }
  data = (tcamDataP->paraFrm.debugPort );
  retVal = hwWriteGlobal3Reg(dev,QD_REG_TCAM_P2_DEBUG_PORT,data);
  if(retVal != GT_OK)
  {
     return retVal;
  }
  data = ((tcamDataP->paraFrm.highHit<<8) | (tcamDataP->paraFrm.lowHit<<0)  );
  retVal = hwWriteGlobal3Reg(dev,QD_REG_TCAM_P2_ALL_HIT,data);
  if(retVal != GT_OK)
  {
     return retVal;
  }
  return GT_OK;
}


static GT_STATUS tcamGetPage0Data(GT_QD_DEV *dev, GT_TCAM_DATA *tcamDataP)
{
  GT_STATUS       retVal;    /* Functions return value */
  GT_U16          data;     /* temporary Data storage */
  int i;

  retVal = hwReadGlobal3Reg(dev,QD_REG_TCAM_P0_KEYS_1,&data);
  if(retVal != GT_OK)
  {
     return retVal;
  }
  tcamDataP->paraFrm.frameTypeMask = (data >>14)&0xff;
  tcamDataP->paraFrm.frameType = (data >> 6)&0xff;
  retVal = hwReadGlobal3Reg(dev,QD_REG_TCAM_P0_KEYS_2,&data);
  if(retVal != GT_OK)
  {
     return retVal;
  }
  tcamDataP->paraFrm.spvMask = (data >>8)&0xff;
  tcamDataP->paraFrm.spv = data&0xff;
  retVal = hwReadGlobal3Reg(dev,QD_REG_TCAM_P0_KEYS_3,&data);
  if(retVal != GT_OK)
  {
     return retVal;
  }
  tcamDataP->paraFrm.ppriMask = (data >>12)&0xff;
  tcamDataP->paraFrm.ppri = (data>>4)&0xff;
  tcamDataP->paraFrm.pvidMask = data &0x0f00;
  tcamDataP->paraFrm.pvid = data <<8;
  retVal = hwReadGlobal3Reg(dev,QD_REG_TCAM_P0_KEYS_4,&data);
  if(retVal != GT_OK)
  {
     return retVal;
  }
  tcamDataP->paraFrm.pvidMask = (data>>8)&0xff ;
  tcamDataP->paraFrm.pvid = data&0x00ff;
  for(i=0; i<22; i++)
  {
    retVal = hwReadGlobal3Reg(dev,QD_REG_TCAM_P0_MATCH_DATA_1+i,&data);
    if(retVal != GT_OK)
	{
     return retVal;
	}
    tcamDataP->paraFrm.frameOctetMask[i] = data>>8;
    tcamDataP->paraFrm.frameOctet[i] = data&0xff;
  }

  return GT_OK;
}
static GT_STATUS tcamGetPage1Data(GT_QD_DEV *dev, GT_TCAM_DATA *tcamDataP)
{
  GT_STATUS       retVal;    /* Functions return value */
  GT_U16          data;     /* temporary Data storage */
  int i;

  for(i=0; i<25; i++)
  {
    retVal = hwReadGlobal3Reg(dev,QD_REG_TCAM_P1_MATCH_DATA_23+i,&data);
    if(retVal != GT_OK)
	{
     return retVal;
	}
    tcamDataP->paraFrm.frameOctetMask[i+23] = data>>8;
    tcamDataP->paraFrm.frameOctet[i+23] = data&0xff;
  }

  return GT_OK;
}

static GT_STATUS tcamGetPage2Data(GT_QD_DEV *dev, GT_TCAM_DATA *tcamDataP)
{
  GT_STATUS       retVal;    /* Functions return value */
  GT_U16          data;     /* temporary Data storage */

  retVal = hwReadGlobal3Reg(dev,QD_REG_TCAM_P2_ACTION_1,&data);
  if(retVal != GT_OK)
  {
     return retVal;
  }
  tcamDataP->paraFrm.continu = data >>15;
  tcamDataP->paraFrm.interrupt = (data >>14)&1;
  tcamDataP->paraFrm.IncTcamCtr = (data>>13)&1;
  tcamDataP->paraFrm.vidOverride = (data<<12)&1;
  tcamDataP->paraFrm.vidData = data&0x07ff;
  retVal = hwReadGlobal3Reg(dev,QD_REG_TCAM_P2_ACTION_2,&data);
  if(retVal != GT_OK)
  {
     return retVal;
  }
  tcamDataP->paraFrm.nextId = (data>>8)&0xff;
  tcamDataP->paraFrm.qpriOverride = (data>>7)&1;
  tcamDataP->paraFrm.qpriData = (data>>4)&3;
  tcamDataP->paraFrm.fpriOverride = (data>>3)&1;
  tcamDataP->paraFrm.fpriData = data&7;
  retVal = hwReadGlobal3Reg(dev,QD_REG_TCAM_P2_ACTION_3,&data);
  if(retVal != GT_OK)
  {
     return retVal;
  }
  tcamDataP->paraFrm.qpriAvbOverride = (data>>15)&1;
  tcamDataP->paraFrm.qpriAvbData = (data>>12)&3;
  tcamDataP->paraFrm.dpvOverride  = (data>>11)&1;
  tcamDataP->paraFrm.dpvData = data&0x7F;
  retVal = hwReadGlobal3Reg(dev,QD_REG_TCAM_P2_ACTION_4,&data);
  if(retVal != GT_OK)
  {
     return retVal;
  }
  tcamDataP->paraFrm.factionOverride = (data>>15)&1;
  tcamDataP->paraFrm.factionData = (data>>4)&0x7ff;
  tcamDataP->paraFrm.ldBalanceOverride = (data>>3)&1;
  tcamDataP->paraFrm.ldBalanceData = data&7;
  retVal = hwReadGlobal3Reg(dev,QD_REG_TCAM_P2_DEBUG_PORT,&data);
  if(retVal != GT_OK)
  {
     return retVal;
  }
  tcamDataP->paraFrm.debugPort = (data)&0xf;
  retVal = hwReadGlobal3Reg(dev,QD_REG_TCAM_P2_ALL_HIT,&data);
  if(retVal != GT_OK)
  {
     return retVal;
  }
  tcamDataP->paraFrm.highHit = (data>>8)&0xff;
  tcamDataP->paraFrm.lowHit = data&0xff;
  return GT_OK;
}

static GT_STATUS waitTcamReady(GT_QD_DEV           *dev)
{
    GT_STATUS       retVal;    /* Functions return value */
#ifdef GT_RMGMT_ACCESS
    {
      HW_DEV_REG_ACCESS regAccess;

      regAccess.entries = 1;
  
      regAccess.rw_reg_list[0].cmd = HW_REG_WAIT_TILL_0;
      regAccess.rw_reg_list[0].addr = CALC_SMI_DEV_ADDR(dev, 0, GLOBAL3_REG_ACCESS);
      regAccess.rw_reg_list[0].reg = QD_REG_TCAM_OPERATION;
      regAccess.rw_reg_list[0].data = 15;
      retVal = hwAccessMultiRegs(dev, &regAccess);
      if(retVal != GT_OK)
      {
        gtSemGive(dev,dev->tblRegsSem);
        return retVal;
      }
    }
#else
    GT_U16          data;     /* temporary Data storage */
    data = 1;
    while(data == 1)
    {
        retVal = hwGetGlobal3RegField(dev,QD_REG_TCAM_OPERATION,15,1,&data);
        if(retVal != GT_OK)
        {
            gtSemGive(dev,dev->tblRegsSem);
            return retVal;
        }
    }
#endif
    return GT_OK;
}


/*******************************************************************************
* tcamOperationPerform
*
* DESCRIPTION:
*       This function accesses TCAM Table
*
* INPUTS:
*       tcamOp   - The tcam operation
*       tcamData - address and data to be written into TCAM
*
* OUTPUTS:
*       tcamData - data read from TCAM pointed by address
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS tcamOperationPerform
(
    IN    GT_QD_DEV           *dev,
    IN    GT_TCAM_OPERATION   tcamOp,
    INOUT GT_TCAM_OP_DATA     *opData
)
{
    GT_STATUS       retVal;    /* Functions return value */
    GT_U16          data;     /* temporary Data storage */

    gtSemTake(dev,dev->tblRegsSem,OS_WAIT_FOREVER);

    /* Wait until the tcam in ready. */
    retVal = waitTcamReady(dev);
    if(retVal != GT_OK)
    {
      gtSemGive(dev,dev->tblRegsSem);
      return retVal;
    }

    /* Set the TCAM Operation register */
    switch (tcamOp)
    {
        case TCAM_FLUSH_ALL:
            data = (1 << 15) | (tcamOp << 12);
            retVal = hwWriteGlobal3Reg(dev,QD_REG_TCAM_OPERATION,data);
            if(retVal != GT_OK)
            {
                gtSemGive(dev,dev->tblRegsSem);
                return retVal;
            }
            break;

        case TCAM_FLUSH_ENTRY:
            data = (1 << 15) | (tcamOp << 12) | (opData->tcamPage << 10) | (opData->tcamEntry) ;
            retVal = hwWriteGlobal3Reg(dev,QD_REG_TCAM_OPERATION,data);
            if(retVal != GT_OK)
            {
                gtSemGive(dev,dev->tblRegsSem);
                return retVal;
            }
            break;

        case TCAM_LOAD_ENTRY:
           data = (1 << 15) | (tcamOp << 12) | (2 << 10) | (opData->tcamEntry) ;
            retVal = hwWriteGlobal3Reg(dev,QD_REG_TCAM_OPERATION,data);
            if(retVal != GT_OK)
            {
                gtSemGive(dev,dev->tblRegsSem);
                return retVal;
            }
            retVal = tcamSetPage2Data(dev, opData->tcamDataP);
            {
                gtSemGive(dev,dev->tblRegsSem);
                return retVal;
            }

            retVal = waitTcamReady(dev);
            if(retVal != GT_OK)
			{
              gtSemGive(dev,dev->tblRegsSem);
              return retVal;
			}

            data = (1 << 15) | (tcamOp << 12) | (1 << 10) | (opData->tcamEntry) ;
            retVal = hwWriteGlobal3Reg(dev,QD_REG_TCAM_OPERATION,data);
            if(retVal != GT_OK)
            {
                gtSemGive(dev,dev->tblRegsSem);
                return retVal;
            }
            retVal = tcamSetPage1Data(dev, opData->tcamDataP);
             {
                gtSemGive(dev,dev->tblRegsSem);
                return retVal;
            }

            retVal = waitTcamReady(dev);
            if(retVal != GT_OK)
			{
              gtSemGive(dev,dev->tblRegsSem);
              return retVal;
			}

            data = (1 << 15) | (tcamOp << 12) | (0 << 10) | (opData->tcamEntry) ;
            retVal = hwWriteGlobal3Reg(dev,QD_REG_TCAM_OPERATION,data);
            if(retVal != GT_OK)
            {
                gtSemGive(dev,dev->tblRegsSem);
                return retVal;
            }
            retVal = tcamSetPage0Data(dev,  opData->tcamDataP);
            {
                gtSemGive(dev,dev->tblRegsSem);
                return retVal;
            }

            break;

        case TCAM_PURGE_ENTRY:
            data = (1 << 15) | (tcamOp << 12) | ((opData->tcamEntry)<< 10) | (opData->tcamEntry) ;
            retVal = hwWriteGlobal3Reg(dev,QD_REG_TCAM_OPERATION,data);
            if(retVal != GT_OK)
            {
                gtSemGive(dev,dev->tblRegsSem);
                return retVal;
            }
            data = 0x00ff ;
            retVal = hwWriteGlobal3Reg(dev,QD_REG_TCAM_P0_KEYS_1,data);
            if(retVal != GT_OK)
            {
                gtSemGive(dev,dev->tblRegsSem);
                return retVal;
            }
            break;

        case TCAM_GET_NEXT_ENTRY:
			{
            data = (1 << 15) | (tcamOp << 12) | (0 << 10) | (opData->tcamEntry) ;
            retVal = hwWriteGlobal3Reg(dev,QD_REG_TCAM_OPERATION,data);
            if(retVal != GT_OK)
            {
                gtSemGive(dev,dev->tblRegsSem);
                return retVal;
            }
            /* Wait until the tcam in ready. */
            retVal = waitTcamReady(dev);
            if(retVal != GT_OK)
			{
              gtSemGive(dev,dev->tblRegsSem);
              return retVal;
			}

            retVal = hwReadGlobal3Reg(dev,QD_REG_TCAM_OPERATION, &data);
            if(retVal != GT_OK)
            {
                gtSemGive(dev,dev->tblRegsSem);
                return retVal;
            }

			if(opData->tcamEntry == 0xff)  /* If ask to find the lowest entry*/
			{
			  if ((data&0xff)==0xff)
			  {
                retVal = hwReadGlobal3Reg(dev,QD_REG_TCAM_P0_KEYS_1, &data);
                if(retVal != GT_OK)
				{
                  gtSemGive(dev,dev->tblRegsSem);
                  return retVal;
				}
	            if(data==0x00ff)
				{
					/* No higher valid TCAM entry */
					return GT_OK;
				}
				else
				{
					/* The highest valid TCAM entry found*/
				}
			  }
			}

            /* Get next entry and read the entry */
            opData->tcamEntry = data&0xff;   
           }
        case TCAM_READ_ENTRY:
			/* Read page 0 */
            data = (1 << 15) | (tcamOp << 12) | (0 << 10) | (opData->tcamEntry) ;
            retVal = hwWriteGlobal3Reg(dev,QD_REG_TCAM_OPERATION,data);
            if(retVal != GT_OK)
            {
                gtSemGive(dev,dev->tblRegsSem);
                return retVal;
            }
            /* Wait until the tcam in ready. */
            retVal = waitTcamReady(dev);
            if(retVal != GT_OK)
			{
              gtSemGive(dev,dev->tblRegsSem);
              return retVal;
			}

            retVal = tcamGetPage0Data(dev, opData->tcamDataP);
            {
                gtSemGive(dev,dev->tblRegsSem);
                return retVal;
            }

			/* Read page 1 */
            data = (1 << 15) | (tcamOp << 12) | (1 << 10) | (opData->tcamEntry) ;
            retVal = hwWriteGlobal3Reg(dev,QD_REG_TCAM_OPERATION,data);
            if(retVal != GT_OK)
            {
                gtSemGive(dev,dev->tblRegsSem);
                return retVal;
            }
            /* Wait until the tcam in ready. */
            retVal = waitTcamReady(dev);
            if(retVal != GT_OK)
			{
              gtSemGive(dev,dev->tblRegsSem);
              return retVal;
			}

            retVal = tcamGetPage1Data(dev, opData->tcamDataP);
            {
                gtSemGive(dev,dev->tblRegsSem);
                return retVal;
            }

			/* Read page 2 */
            data = (1 << 15) | (tcamOp << 12) | (2 << 10) | (opData->tcamEntry) ;
            retVal = hwWriteGlobal3Reg(dev,QD_REG_TCAM_OPERATION,data);
            if(retVal != GT_OK)
            {
                gtSemGive(dev,dev->tblRegsSem);
                return retVal;
            }
            /* Wait until the tcam in ready. */
            retVal = waitTcamReady(dev);
            if(retVal != GT_OK)
			{
              gtSemGive(dev,dev->tblRegsSem);
              return retVal;
			}

            retVal = tcamGetPage2Data(dev, opData->tcamDataP);
            {
                gtSemGive(dev,dev->tblRegsSem);
                return retVal;
            }
            break;

        default:
            
            gtSemGive(dev,dev->tblRegsSem);
            return GT_FAIL;
    }

    gtSemGive(dev,dev->tblRegsSem);
    return retVal;
}

