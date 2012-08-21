#include <Copyright.h>

/*******************************************************************************
* gtPTP.c
*
* DESCRIPTION:
*       API definitions for Precise Time Protocol logic
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <msApi.h>
#include <gtSem.h>
#include <gtHwCntl.h>
#include <gtDrvSwRegs.h>


#ifdef CONFIG_AVB_FPGA

#undef USE_SINGLE_READ

#define AVB_SMI_ADDR		0xC

#define QD_REG_PTP_INT_OFFSET		0
#define QD_REG_PTP_INTEN_OFFSET		1
#define QD_REG_PTP_FREQ_OFFSET		4
#define QD_REG_PTP_PHASE_OFFSET		6
#define QD_REG_PTP_CLK_CTRL_OFFSET	4
#define QD_REG_PTP_CYCLE_INTERVAL_OFFSET		5
#define QD_REG_PTP_CYCLE_ADJ_OFFSET				6
#define QD_REG_PTP_PLL_CTRL_OFFSET	7
#define QD_REG_PTP_CLK_SRC_OFFSET	0x9
#define QD_REG_PTP_P9_MODE_OFFSET	0xA
#define QD_REG_PTP_RESET_OFFSET		0xB

#define GT_PTP_MERGE_32BIT(_high16,_low16)	(((_high16)<<16)|(_low16))
#define GT_PTP_GET_HIGH16(_data)	((_data) >> 16) & 0xFFFF
#define GT_PTP_GET_LOW16(_data)		(_data) & 0xFFFF

#define AVB_FPGA_READ_REG	gsysReadMiiReg
#define AVB_FPGA_WRITE_REG	gsysWriteMiiReg

#endif

#if 0
#define GT_PTP_BUILD_TIME(_time1, _time2)	(((_time1) << 16) | (_time2))
#define GT_PTP_L16_TIME(_time1)	((_time1) & 0xFFFF)
#define GT_PTP_H16_TIME(_time1)	(((_time1) >> 16) & 0xFFFF)
#endif


/****************************************************************************/
/* PTP operation function declaration.                                    */
/****************************************************************************/
static GT_STATUS ptpOperationPerform
(
    IN   GT_QD_DEV 			*dev,
    IN   GT_PTP_OPERATION	ptpOp,
    INOUT GT_PTP_OP_DATA 	*opData
);


/*******************************************************************************
* gptpSetConfig
*
* DESCRIPTION:
*       This routine writes PTP configuration parameters.
*
* INPUTS:
*		ptpData  - PTP configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*		GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetConfig
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_PTP_CONFIG	*ptpData
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_LPORT			port;
	GT_PTP_PORT_CONFIG	ptpPortData;

	DBG_INFO(("gptpSetConfig Called.\n"));

    /* check if device supports this feature */
#ifndef CONFIG_AVB_FPGA
	if (!IS_IN_DEV_GROUP(dev,DEV_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

	opData.ptpBlock = 0x0;	/* PTP register space */

	opData.ptpPort = 0xF;	/* Global register */
	op = PTP_WRITE_DATA;

	/* setting PTPEType, offset 0 */
	opData.ptpAddr = 0;
	opData.ptpData = ptpData->ptpEType;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing PTPEType.\n"));
		return GT_FAIL;
	}

	/* setting MsgIDTSEn, offset 1 */
	opData.ptpAddr = 1;
	opData.ptpData = ptpData->msgIdTSEn;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing MsgIDTSEn.\n"));
		return GT_FAIL;
	}

	/* setting TSArrPtr, offset 2 */
	opData.ptpAddr = 2;
	opData.ptpData = ptpData->tsArrPtr;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing TSArrPtr.\n"));
		return GT_FAIL;
	}

#ifndef CONFIG_AVB_FPGA
	if (IS_IN_DEV_GROUP(dev,DEV_PTP_2))
#endif
    {
		ptpPortData.transSpec = ptpData->transSpec;
		ptpPortData.disTSpec = 1;	/* default value */
		ptpPortData.disTSOverwrite = ptpData->disTSOverwrite;
		ptpPortData.ipJump = 2;		/* default value */
		ptpPortData.etJump = 12;	/* default value */

		/* per port configuration */
		for(port=0; port<dev->numOfPorts; port++)
		{
			ptpPortData.ptpArrIntEn = (ptpData->ptpArrIntEn & (1 << port))? GT_TRUE : GT_FALSE;
			ptpPortData.ptpDepIntEn = (ptpData->ptpDepIntEn & (1 << port))? GT_TRUE : GT_FALSE;

			if((retVal = gptpSetPortConfig(dev, port, &ptpPortData)) != GT_OK)
			{
        		DBG_INFO(("Failed gptpSetPortConfig.\n"));
				return GT_FAIL;
			}
		}

		return GT_OK;
    }

	/* setting PTPArrIntEn, offset 3 */
	opData.ptpAddr = 3;
	opData.ptpData = GT_LPORTVEC_2_PORTVEC(ptpData->ptpArrIntEn);

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing PTPArrIntEn.\n"));
		return GT_FAIL;
	}

	/* setting PTPDepIntEn, offset 4 */
	opData.ptpAddr = 4;
	opData.ptpData = GT_LPORTVEC_2_PORTVEC(ptpData->ptpDepIntEn);

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing PTPDepIntEn.\n"));
		return GT_FAIL;
	}

	/* TransSpec, MsgIDStartBit, DisTSOverwrite bit, offset 5 */
	op = PTP_READ_DATA;
	opData.ptpAddr = 5;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	op = PTP_WRITE_DATA;
#ifdef CONFIG_AVB_FPGA
	opData.ptpData = ((ptpData->transSpec&0xF) << 12) | ((ptpData->msgIdStartBit&0x7) << 9) | 
					(opData.ptpData & 0x1) | ((ptpData->disTSOverwrite?1:0) << 1);
#else
	opData.ptpData = ((ptpData->transSpec&0xF) << 12) | (opData.ptpData & 0x1) | ((ptpData->disTSOverwrite?1:0) << 1);
#endif
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing MsgIDStartBit & DisTSOverwrite.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gptpGetConfig
*
* DESCRIPTION:
*       This routine reads PTP configuration parameters.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		ptpData  - PTP configuration parameters.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetConfig
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_PTP_CONFIG	*ptpData
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_LPORT			port;
	GT_PTP_PORT_CONFIG	ptpPortData;

	DBG_INFO(("gptpGetConfig Called.\n"));

    /* check if device supports this feature */
#ifndef CONFIG_AVB_FPGA
	if (!IS_IN_DEV_GROUP(dev,DEV_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

	opData.ptpBlock = 0x0;	/* PTP register space */

	opData.ptpPort = 0xF;	/* Global register */
	op = PTP_READ_DATA;

	/* getting PTPEType, offset 0 */
	opData.ptpAddr = 0;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading PTPEType.\n"));
		return GT_FAIL;
	}

	ptpData->ptpEType = opData.ptpData;

	/* getting MsgIDTSEn, offset 1 */
	opData.ptpAddr = 1;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading MsgIDTSEn.\n"));
		return GT_FAIL;
	}

	ptpData->msgIdTSEn = opData.ptpData;

	/* getting TSArrPtr, offset 2 */
	opData.ptpAddr = 2;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading TSArrPtr.\n"));
		return GT_FAIL;
	}

	ptpData->tsArrPtr = opData.ptpData;

#ifndef CONFIG_AVB_FPGA
	if (IS_IN_DEV_GROUP(dev,DEV_PTP_2))
#endif
    {
		ptpData->ptpArrIntEn = 0;
		ptpData->ptpDepIntEn = 0;

		/* per port configuration */
		for(port=0; port<dev->numOfPorts; port++)
		{
			if((retVal = gptpGetPortConfig(dev, port, &ptpPortData)) != GT_OK)
			{
        		DBG_INFO(("Failed gptpGetPortConfig.\n"));
				return GT_FAIL;
			}

			ptpData->ptpArrIntEn |= (ptpPortData.ptpArrIntEn ? (1 << port) : 0);
			ptpData->ptpDepIntEn |= (ptpPortData.ptpDepIntEn ? (1 << port) : 0);
		}

		ptpData->transSpec = ptpPortData.transSpec;
		ptpData->disTSOverwrite = ptpPortData.disTSOverwrite;

		ptpData->msgIdStartBit = 4;

		return GT_OK;
    }

	/* getting PTPArrIntEn, offset 3 */
	opData.ptpAddr = 3;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading PTPArrIntEn.\n"));
		return GT_FAIL;
	}
	opData.ptpData &= dev->validPortVec;
	ptpData->ptpArrIntEn = GT_PORTVEC_2_LPORTVEC(opData.ptpData);


	/* getting PTPDepIntEn, offset 4 */
	opData.ptpAddr = 4;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading PTPDepIntEn.\n"));
		return GT_FAIL;
	}

	opData.ptpData &= dev->validPortVec;
	ptpData->ptpDepIntEn = GT_PORTVEC_2_LPORTVEC(opData.ptpData);

	/* MsgIDStartBit, DisTSOverwrite bit, offset 5 */
	opData.ptpAddr = 5;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	ptpData->transSpec = (opData.ptpData >> 12) & 0xF;
#ifdef CONFIG_AVB_FPGA
	ptpData->msgIdStartBit = (opData.ptpData >> 9) & 0x7;
#else
	ptpData->msgIdStartBit = 0;
#endif
	ptpData->disTSOverwrite = ((opData.ptpData >> 1) & 0x1) ? GT_TRUE : GT_FALSE;

	DBG_INFO(("OK.\n"));
	return GT_OK;

}

/*******************************************************************************
* gptpSetGlobalConfig
*
* DESCRIPTION:
*       This routine writes PTP global configuration parameters.
*
* INPUTS:
*		ptpData  - PTP global configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*		GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetGlobalConfig
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_PTP_GLOBAL_CONFIG	*ptpData
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gptpSetGlobalConfig Called.\n"));

    /* check if device supports this feature */
#ifndef CONFIG_AVB_FPGA
	if (!IS_IN_DEV_GROUP(dev,DEV_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

	opData.ptpBlock = 0x0;	/* PTP register space */

	opData.ptpPort = 0xF;	/* Global register */
	op = PTP_WRITE_DATA;

	/* setting PTPEType, offset 0 */
	opData.ptpAddr = 0;
	opData.ptpData = ptpData->ptpEType;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing PTPEType.\n"));
		return GT_FAIL;
	}

	/* setting MsgIDTSEn, offset 1 */
	opData.ptpAddr = 1;
	opData.ptpData = ptpData->msgIdTSEn;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing MsgIDTSEn.\n"));
		return GT_FAIL;
	}

	/* setting TSArrPtr, offset 2 */
	opData.ptpAddr = 2;
	opData.ptpData = ptpData->tsArrPtr;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing TSArrPtr.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gptpGlobalGetConfig
*
* DESCRIPTION:
*       This routine reads PTP global configuration parameters.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		ptpData  - PTP global configuration parameters.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetGlobalConfig
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_PTP_GLOBAL_CONFIG	*ptpData
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gptpGetGlobalConfig Called.\n"));

    /* check if device supports this feature */
#ifndef CONFIG_AVB_FPGA
	if (!IS_IN_DEV_GROUP(dev,DEV_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

	opData.ptpBlock = 0x0;	/* PTP register space */

	opData.ptpPort = 0xF;	/* Global register */
	op = PTP_READ_DATA;

	/* getting PTPEType, offset 0 */
	opData.ptpAddr = 0;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading PTPEType.\n"));
		return GT_FAIL;
	}

	ptpData->ptpEType = opData.ptpData;

	/* getting MsgIDTSEn, offset 1 */
	opData.ptpAddr = 1;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading MsgIDTSEn.\n"));
		return GT_FAIL;
	}

	ptpData->msgIdTSEn = opData.ptpData;

	/* getting TSArrPtr, offset 2 */
	opData.ptpAddr = 2;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading TSArrPtr.\n"));
		return GT_FAIL;
	}

	ptpData->tsArrPtr = opData.ptpData;

	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gptpSetPortConfig
*
* DESCRIPTION:
*       This routine writes PTP port configuration parameters.
*
* INPUTS:
*		ptpData  - PTP port configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*		GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetPortConfig
(
	IN  GT_QD_DEV 	*dev,
	IN	GT_LPORT	port,
	IN  GT_PTP_PORT_CONFIG	*ptpData
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
    GT_U32          hwPort;         /* the physical port number     */

	DBG_INFO(("gptpSetPortConfig Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (GT_U32)GT_LPORT_2_PORT(port);

    /* check if device supports this feature */
#ifndef CONFIG_AVB_FPGA
	if (!IS_IN_DEV_GROUP(dev,DEV_PTP_2))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

	if (ptpData->transSpec > 0xF)	/* 4 bits */
		return GT_BAD_PARAM;

	if (ptpData->etJump > 0x1F)	/* 5 bits */
		return GT_BAD_PARAM;

	if (ptpData->ipJump > 0x3F)	/* 6 bits */
		return GT_BAD_PARAM;

	
	opData.ptpBlock = 0x0;	/* PTP register space */

	opData.ptpPort = hwPort;

	/* TransSpec, DisTSpecCheck, DisTSOverwrite bit, offset 0 */
	op = PTP_READ_DATA;
	opData.ptpAddr = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	op = PTP_WRITE_DATA;
	opData.ptpData = (ptpData->transSpec << 12) | (opData.ptpData & 0x1) |
					((ptpData->disTSpec?1:0) << 11) |
					((ptpData->disTSOverwrite?1:0) << 1);

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing TransSpec,DisTSpecCheck,DisTSOverwrite.\n"));
		return GT_FAIL;
	}

	/* setting etJump and ipJump, offset 1 */
	opData.ptpAddr = 1;
	opData.ptpData = (ptpData->ipJump << 8) | ptpData->etJump;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing MsgIDTSEn.\n"));
		return GT_FAIL;
	}

	/* setting Int, offset 2 */
	opData.ptpAddr = 2;
	opData.ptpData = (ptpData->ptpArrIntEn?1:0) |
					((ptpData->ptpDepIntEn?1:0) << 1);

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing TSArrPtr.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gptpGetPortConfig
*
* DESCRIPTION:
*       This routine reads PTP configuration parameters for a port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		ptpData  - PTP port configuration parameters.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetPortConfig
(
	IN  GT_QD_DEV 	*dev,
	IN	GT_LPORT	port,
	OUT GT_PTP_PORT_CONFIG	*ptpData
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
    GT_U32          hwPort;         /* the physical port number     */

	DBG_INFO(("gptpGetPortConfig Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (GT_U32)GT_LPORT_2_PORT(port);

    /* check if device supports this feature */
#ifndef CONFIG_AVB_FPGA
	if (!IS_IN_DEV_GROUP(dev,DEV_PTP_2))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

	opData.ptpBlock = 0x0;	/* PTP register space */

	opData.ptpPort = hwPort;
	op = PTP_READ_DATA;

	/* TransSpec, DisTSpecCheck, DisTSOverwrite bit, offset 0 */
	opData.ptpAddr = 0;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading PTPEType.\n"));
		return GT_FAIL;
	}

	ptpData->transSpec = opData.ptpData >> 12;
	ptpData->disTSpec = ((opData.ptpData >> 11) & 0x1) ? GT_TRUE : GT_FALSE;
	ptpData->disTSOverwrite = ((opData.ptpData >> 1) & 0x1) ? GT_TRUE : GT_FALSE;

	/* getting MsgIDTSEn, offset 1 */
	opData.ptpAddr = 1;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading MsgIDTSEn.\n"));
		return GT_FAIL;
	}

	ptpData->ipJump = (opData.ptpData >> 8) & 0x3F;
	ptpData->etJump = opData.ptpData & 0x1F;

	/* getting TSArrPtr, offset 2 */
	opData.ptpAddr = 2;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading TSArrPtr.\n"));
		return GT_FAIL;
	}

	ptpData->ptpDepIntEn = ((opData.ptpData >> 1) & 0x1) ? GT_TRUE : GT_FALSE;
	ptpData->ptpArrIntEn = (opData.ptpData & 0x1) ? GT_TRUE : GT_FALSE;

	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gptpSetPTPEn
*
* DESCRIPTION:
*       This routine enables or disables PTP.
*
* INPUTS:
*		en - GT_TRUE to enable PTP, GT_FALSE to disable PTP
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetPTPEn
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_BOOL		en
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_LPORT			port;

	DBG_INFO(("gptpSetPTPEn Called.\n"));

#ifndef CONFIG_AVB_FPGA
    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

#ifndef CONFIG_AVB_FPGA
	if (IS_IN_DEV_GROUP(dev,DEV_PTP_2))
#endif
    {
		/* per port configuration */
		for(port=0; port<dev->numOfPorts; port++)
		{
			if((retVal = gptpSetPortPTPEn(dev, port, en)) != GT_OK)
			{
        		DBG_INFO(("Failed gptpSetPortPTPEn.\n"));
				return GT_FAIL;
			}
		}

		return GT_OK;
    }

	opData.ptpBlock = 0x0;	/* PTP register space */

	opData.ptpPort = 0xF;	/* Global register */
	op = PTP_READ_DATA;
	opData.ptpAddr = 5;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	op = PTP_WRITE_DATA;
	opData.ptpData &= ~0x1;
	opData.ptpData |= (en ? 0 : 1);

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing MsgIDStartBit & DisTSOverwrite.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gptpGetPTPEn
*
* DESCRIPTION:
*       This routine checks if PTP is enabled.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		en - GT_TRUE if enabled, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetPTPEn
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_BOOL		*en
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gptpGetPTPEn Called.\n"));

#ifndef CONFIG_AVB_FPGA
    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

#ifndef CONFIG_AVB_FPGA
	if (IS_IN_DEV_GROUP(dev,DEV_PTP_2))
#endif
    {
		if((retVal = gptpGetPortPTPEn(dev, 0, en)) != GT_OK)
		{
       		DBG_INFO(("Failed gptpGetPortPTPEn.\n"));
			return GT_FAIL;
		}

		return GT_OK;
    }

	opData.ptpBlock = 0x0;	/* PTP register space */

	opData.ptpPort = 0xF;	/* Global register */
	op = PTP_READ_DATA;
	opData.ptpAddr = 5;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	*en = (opData.ptpData & 0x1) ? GT_FALSE : GT_TRUE;

	DBG_INFO(("OK.\n"));
	return GT_OK;

}

/*******************************************************************************
* gptpSetPortPTPEn
*
* DESCRIPTION:
*       This routine enables or disables PTP on a port.
*
* INPUTS:
*		en - GT_TRUE to enable PTP, GT_FALSE to disable PTP
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetPortPTPEn
(
	IN  GT_QD_DEV 	*dev,
	IN	GT_LPORT	port,
	IN  GT_BOOL		en
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U32			hwPort;

	DBG_INFO(("gptpSetPortPTPEn Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (GT_U32)GT_LPORT_2_PORT(port);

#ifndef CONFIG_AVB_FPGA
    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_PTP_2))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

	opData.ptpBlock = 0x0;	/* PTP register space */
	opData.ptpAddr = 0;

	opData.ptpPort = hwPort;

	op = PTP_READ_DATA;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	op = PTP_WRITE_DATA;

	if (en)
		opData.ptpData &= ~0x1;
	else
		opData.ptpData |= 0x1;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing TransSpec,DisTSpecCheck,DisTSOverwrite.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}

/*******************************************************************************
* gptpGetPortPTPEn
*
* DESCRIPTION:
*       This routine checks if PTP is enabled on a port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		en - GT_TRUE if enabled, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetPortPTPEn
(
	IN  GT_QD_DEV 	*dev,
	IN	GT_LPORT	port,
	OUT GT_BOOL		*en
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U32			hwPort;

	DBG_INFO(("gptpGetPortPTPEn Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (GT_U32)GT_LPORT_2_PORT(port);

#ifndef CONFIG_AVB_FPGA
    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_PTP_2))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

	opData.ptpBlock = 0x0;	/* PTP register space */

	opData.ptpAddr = 0;
	opData.ptpPort = hwPort;

	op = PTP_READ_DATA;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	*en = (opData.ptpData & 0x1) ? GT_FALSE : GT_TRUE;

	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gptpGetPTPInt
*
* DESCRIPTION:
*       This routine gets PTP interrupt status for each port.
*		The PTP Interrupt bit gets set for a given port when an incoming PTP 
*		frame is time stamped and PTPArrIntEn for that port is set to 0x1.
*		Similary PTP Interrupt bit gets set for a given port when an outgoing
*		PTP frame is time stamped and PTPDepIntEn for that port is set to 0x1.
*		This bit gets cleared upon software reading and clearing the corresponding
*		time counter valid bits that are valid for that port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		ptpInt 	- interrupt status for each port (bit 0 for port 0, bit 1 for port 1, etc.)
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetPTPInt
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_U32		*ptpInt
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gptpGetPTPInt Called.\n"));

#ifndef CONFIG_AVB_FPGA
    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

	opData.ptpBlock = 0x0;	/* PTP register space */
	opData.ptpPort = 0xF;	/* Global register */
	op = PTP_READ_DATA;
	opData.ptpAddr = 8;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	opData.ptpData &= (1 << dev->maxPorts) - 1;

	*ptpInt = GT_PORTVEC_2_LPORTVEC(opData.ptpData);

	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gptpGetPTPGlobalTime
*
* DESCRIPTION:
*       This routine gets the global timer value that is running off of the free
*		running switch core clock.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		ptpTime	- PTP global time
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetPTPGlobalTime
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_U32		*ptpTime
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gptpGetPTPGlobalTime Called.\n"));

#ifndef CONFIG_AVB_FPGA
    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

#ifndef USE_SINGLE_READ
	opData.ptpBlock = 0x0;	/* PTP register space */
	opData.ptpPort = IS_IN_DEV_GROUP(dev,DEV_TAI)?0xE:0xF;	/* Global register */
	op = PTP_READ_MULTIPLE_DATA;
	opData.ptpAddr = IS_IN_DEV_GROUP(dev,DEV_TAI)?0xE:9;
	opData.nData = 2;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	*ptpTime = GT_PTP_BUILD_TIME(opData.ptpMultiData[1],opData.ptpMultiData[0]);
#else
	{
	GT_U32 data[2];

	opData.ptpBlock = 0x0;	/* PTP register space */
	opData.ptpPort = IS_IN_DEV_GROUP(dev,DEV_TAI)?0xE:0xF;	/* Global register */
	op = PTP_READ_DATA;
	opData.ptpAddr = IS_IN_DEV_GROUP(dev,DEV_TAI)?0xE:9;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	data[0] = opData.ptpData;

	op = PTP_READ_DATA;
	opData.ptpAddr++;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	data[1] = opData.ptpData;

	*ptpTime = GT_PTP_BUILD_TIME(data[1],data[0]);

	}
#endif

	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gptpGetTimeStamped
*
* DESCRIPTION:
*		This routine retrieves the PTP port status that includes time stamp value 
*		and sequce Id that are captured by PTP logic for a PTP frame that needs 
*		to be time stamped.
*
* INPUTS:
*       port 		- logical port number.
*       timeToRead	- Arr0, Arr1, or Dep time (GT_PTP_TIME enum type)
*
* OUTPUTS:
*		ptpStatus	- PTP port status
*
* RETURNS:
*       GT_OK 		- on success
*       GT_FAIL 	- on error
*		GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetTimeStamped
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_LPORT	port,
	IN	GT_PTP_TIME	timeToRead,
	OUT GT_PTP_TS_STATUS	*ptpStatus
)
{
	GT_STATUS       	retVal;
	GT_U32				hwPort;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U32				baseReg;

	DBG_INFO(("gptpGetTimeStamped Called.\n"));

#ifndef CONFIG_AVB_FPGA
    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

    hwPort = (GT_U32)GT_LPORT_2_PORT(port);
	if (hwPort == GT_INVALID_PORT)
	{
        DBG_INFO(("Invalid port number\n"));
		return GT_BAD_PARAM;
	}

#ifndef CONFIG_AVB_FPGA
	if (IS_IN_DEV_GROUP(dev,DEV_PTP_2))
		baseReg = 8;
	else
		baseReg = 0;
#else
	baseReg = 8;
#endif

	switch (timeToRead)
	{
		case PTP_ARR0_TIME:
			opData.ptpAddr = baseReg + 0;
			break;
		case PTP_ARR1_TIME:
			opData.ptpAddr = baseReg + 4;
			break;
		case PTP_DEP_TIME:
			opData.ptpAddr = baseReg + 8;
			break;
		default:
        	DBG_INFO(("Invalid time to be read\n"));
			return GT_BAD_PARAM;
	}

	opData.ptpPort = hwPort;
	opData.ptpBlock = 0;

#ifndef USE_SINGLE_READ
	op = PTP_READ_TIMESTAMP_DATA;
	opData.nData = 4;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	ptpStatus->isValid = (opData.ptpMultiData[0] & 0x1) ? GT_TRUE : GT_FALSE;
	ptpStatus->status = (GT_PTP_INT_STATUS)((opData.ptpMultiData[0] >> 1) & 0x3);
	ptpStatus->timeStamped = GT_PTP_BUILD_TIME(opData.ptpMultiData[2],opData.ptpMultiData[1]);
	ptpStatus->ptpSeqId = opData.ptpMultiData[3];
#else
	{
	GT_U32 data[4], i;

	op = PTP_READ_DATA;

	for (i=0; i<4; i++)
	{
		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
        	DBG_INFO(("Failed reading DisPTP.\n"));
			return GT_FAIL;
		}
	
		data[i] = opData.ptpData;
		opData.ptpAddr++;
	}

	ptpStatus->isValid = (data[0] & 0x1) ? GT_TRUE : GT_FALSE;
	ptpStatus->status = (GT_PTP_INT_STATUS)((data[0] >> 1) & 0x3);
	ptpStatus->timeStamped = GT_PTP_BUILD_TIME(data[2],data[1]);
	ptpStatus->ptpSeqId = data[3];

	}
#endif

	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gptpResetTimeStamp
*
* DESCRIPTION:
*		This routine resets PTP Time valid bit so that PTP logic can time stamp
*		a next PTP frame that needs to be time stamped.
*
* INPUTS:
*       port 		- logical port number.
*       timeToReset	- Arr0, Arr1, or Dep time (GT_PTP_TIME enum type)
*
* OUTPUTS:
*		None.
*
* RETURNS:
*       GT_OK 		- on success
*       GT_FAIL 	- on error
*		GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpResetTimeStamp
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_LPORT	port,
	IN	GT_PTP_TIME	timeToReset
)
{
	GT_STATUS       	retVal;
	GT_U32				hwPort;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U32				baseReg;

	DBG_INFO(("gptpResetTimeStamp Called.\n"));

#ifndef CONFIG_AVB_FPGA
    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

    hwPort = (GT_U32)GT_LPORT_2_PORT(port);
	if (hwPort == GT_INVALID_PORT)
	{
        DBG_INFO(("Invalid port number\n"));
		return GT_BAD_PARAM;
	}

#ifndef CONFIG_AVB_FPGA
	if (IS_IN_DEV_GROUP(dev,DEV_PTP_2))
		baseReg = 8;
	else
		baseReg = 0;
#else
	baseReg = 8;
#endif

	switch (timeToReset)
	{
		case PTP_ARR0_TIME:
			opData.ptpAddr = baseReg + 0;
			break;
		case PTP_ARR1_TIME:
			opData.ptpAddr = baseReg + 4;
			break;
		case PTP_DEP_TIME:
			opData.ptpAddr = baseReg + 8;
			break;
		default:
        	DBG_INFO(("Invalid time to reset\n"));
			return GT_BAD_PARAM;
	}

	opData.ptpPort = hwPort;
	opData.ptpData = 0;
	opData.ptpBlock = 0;
	op = PTP_WRITE_DATA;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing Port Status.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gptpGetReg
*
* DESCRIPTION:
*       This routine reads PTP register.
*
* INPUTS:
*       port 		- logical port number.
*       regOffset	- register to read
*
* OUTPUTS:
*		data		- register data
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetReg
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_LPORT	port,
	IN  GT_U32		regOffset,
	OUT GT_U32		*data
)
{
	GT_STATUS       	retVal;
	GT_U32				hwPort;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gptpGetReg Called.\n"));

#ifndef CONFIG_AVB_FPGA
    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

    hwPort = (GT_U32)port;

	if (regOffset > 0x1F)
	{
        DBG_INFO(("Invalid reg offset\n"));
		return GT_BAD_PARAM;
	}

	op = PTP_READ_DATA;
	opData.ptpPort = hwPort;
	opData.ptpAddr = regOffset;
	opData.ptpBlock = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	*data = opData.ptpData;

	DBG_INFO(("OK.\n"));
	return GT_OK;

}

/*******************************************************************************
* gptpSetReg
*
* DESCRIPTION:
*       This routine writes data to PTP register.
*
* INPUTS:
*       port 		- logical port number
*       regOffset	- register to be written
*		data		- data to be written
*
* OUTPUTS:
*		None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetReg
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_LPORT	port,
	IN  GT_U32		regOffset,
	IN  GT_U32		data
)
{
	GT_STATUS       	retVal;
	GT_U32				hwPort;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gptpSetReg Called.\n"));

#ifndef CONFIG_AVB_FPGA
    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

    hwPort = (GT_U32)port;

	if ((regOffset > 0x1F) || (data > 0xFFFF))
	{
        DBG_INFO(("Invalid reg offset/data\n"));
		return GT_BAD_PARAM;
	}

	op = PTP_WRITE_DATA;
	opData.ptpPort = hwPort;
	opData.ptpAddr = regOffset;
	opData.ptpData = data;
	opData.ptpBlock = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}


	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gtaiSetEventConfig
*
* DESCRIPTION:
*       This routine sets TAI Event Capture configuration parameters.
*
* INPUTS:
*		eventData  - TAI Event Capture configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*		GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiSetEventConfig
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_TAI_EVENT_CONFIG	*eventData
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gtaiSetEventConfig Called.\n"));

    /* check if device supports this feature */
#ifndef CONFIG_AVB_FPGA
	if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

	opData.ptpBlock = 0x0;	/* PTP register space */

	opData.ptpPort = 0xE;	/* TAI register */
	op = PTP_READ_DATA;

	/* getting PTPEType, offset 0 */
	opData.ptpAddr = 0;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading PTPEType.\n"));
		return GT_FAIL;
	}

	opData.ptpData &= ~((3<<14)|(1<<8));
	if (eventData->intEn)
		opData.ptpData |= (1 << 8);
	if (eventData->eventOverwrite)
		opData.ptpData |= (1 << 15);
	if (eventData->eventCtrStart)
		opData.ptpData |= (1 << 14);

	op = PTP_WRITE_DATA;

	opData.ptpAddr = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing PTPEType.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gtaiGetEventConfig
*
* DESCRIPTION:
*       This routine gets TAI Event Capture configuration parameters.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		eventData  - TAI Event Capture configuration parameters.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiGetEventConfig
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_TAI_EVENT_CONFIG	*eventData
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gtaiGetEventConfig Called.\n"));

    /* check if device supports this feature */
#ifndef CONFIG_AVB_FPGA
	if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

	opData.ptpBlock = 0x0;	/* PTP register space */

	opData.ptpPort = 0xE;	/* TAI register */
	op = PTP_READ_DATA;

	/* getting PTPEType, offset 0 */
	opData.ptpAddr = 0;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading PTPEType.\n"));
		return GT_FAIL;
	}

	eventData->intEn = (opData.ptpData & (1<<8))?GT_TRUE:GT_FALSE;
	eventData->eventOverwrite = (opData.ptpData & (1<<15))?GT_TRUE:GT_FALSE;
	eventData->eventCtrStart = (opData.ptpData & (1<<14))?GT_TRUE:GT_FALSE;

	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gtaiGetEventStatus
*
* DESCRIPTION:
*       This routine gets TAI Event Capture status.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		eventData  - TAI Event Capture configuration parameters.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiGetEventStatus
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_TAI_EVENT_STATUS	*status
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U32 				data[2];

	DBG_INFO(("gtaiGetEventStatus Called.\n"));

    /* check if device supports this feature */
#ifndef CONFIG_AVB_FPGA
	if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

	opData.ptpBlock = 0x0;	/* PTP register space */

	opData.ptpPort = 0xE;	/* TAI register */
	op = PTP_READ_DATA;

	opData.ptpAddr = 9;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading PTPEType.\n"));
		return GT_FAIL;
	}

	status->isValid = (opData.ptpData & (1<<8))?GT_TRUE:GT_FALSE;
	status->eventCtr = opData.ptpData & 0xFF;
	status->eventErr = (opData.ptpData & (1<<9))?GT_TRUE:GT_FALSE;

	if (status->isValid == GT_FALSE)
	{
		return GT_OK;
	}

	opData.ptpAddr = 10;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading PTPEType.\n"));
		return GT_FAIL;
	}
	data[0] = opData.ptpData;

	opData.ptpAddr = 11;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading PTPEType.\n"));
		return GT_FAIL;
	}
	data[1] = opData.ptpData;

	status->eventTime = GT_PTP_BUILD_TIME(data[1],data[0]);

	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gtaiGetEventInt
*
* DESCRIPTION:
*       This routine gets TAI Event Capture Interrupt status.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		intStatus 	- interrupt status for TAI Event capture
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiGetEventInt
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_BOOL		*intStatus
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gtaiGetEventInt Called.\n"));

#ifndef CONFIG_AVB_FPGA
    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

	opData.ptpPort = 0xE;	/* Global register */
	op = PTP_READ_DATA;
	opData.ptpAddr = 9;
	opData.ptpBlock = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	*intStatus = (opData.ptpData & 0x8000)?GT_TRUE:GT_FALSE;

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gtaiGetTrigInt
*
* DESCRIPTION:
*       This routine gets TAI Trigger Interrupt status.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		intStatus 	- interrupt status for TAI Trigger
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiGetTrigInt
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_BOOL		*intStatus
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gtaiGetTrigInt Called.\n"));

#ifndef CONFIG_AVB_FPGA
    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

	opData.ptpPort = 0xE;	/* Global register */
	op = PTP_READ_DATA;
	opData.ptpAddr = 8;
	opData.ptpBlock = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	*intStatus = (opData.ptpData & 0x8000)?GT_TRUE:GT_FALSE;

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gtaiSetTrigConfig
*
* DESCRIPTION:
*       This routine sets TAI Trigger configuration parameters.
*
* INPUTS:
*		trigEn    - enable/disable TAI Trigger.
*		trigData  - TAI Trigger configuration parameters (valid only if trigEn is GT_TRUE).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*		GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiSetTrigConfig
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_BOOL 	trigEn,
	IN  GT_TAI_TRIGGER_CONFIG	*trigData
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gtaiSetTrigConfig Called.\n"));

    /* check if device supports this feature */
#ifndef CONFIG_AVB_FPGA
	if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

	opData.ptpBlock = 0x0;	/* PTP register space */

	opData.ptpPort = 0xE;	/* TAI register */
	op = PTP_READ_DATA;

	/* getting PTPEType, offset 0 */
	opData.ptpAddr = 0;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading PTPEType.\n"));
		return GT_FAIL;
	}

	opData.ptpData &= ~(3|(1<<9));

	if (trigEn == GT_FALSE)
	{
		op = PTP_WRITE_DATA;

		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
        	DBG_INFO(("Failed writing PTPEType.\n"));
			return GT_FAIL;
		}

		return GT_OK;
	}

	opData.ptpData |= 1;

	if (trigData->intEn)
		opData.ptpData |= (1 << 9);

	if (trigData->mode == GT_TAI_TRIG_ON_GIVEN_TIME)
		opData.ptpData |= (1 << 1);

	op = PTP_WRITE_DATA;

	opData.ptpAddr = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing PTPEType.\n"));
		return GT_FAIL;
	}

	opData.ptpAddr = 2;
	opData.ptpData = GT_PTP_L16_TIME(trigData->trigGenAmt);

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing PTPEType.\n"));
		return GT_FAIL;
	}

	opData.ptpAddr = 3;
	opData.ptpData = GT_PTP_H16_TIME(trigData->trigGenAmt);

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing PTPEType.\n"));
		return GT_FAIL;
	}

	if (trigData->mode == GT_TAI_TRIG_ON_GIVEN_TIME)
	{
		if ((trigData->pulseWidth >= 0) && (trigData->pulseWidth <= 0xF))
		{			
			op = PTP_READ_DATA;
			opData.ptpAddr = 5;		/* PulseWidth */

			if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
			{
				DBG_INFO(("Failed writing PTPEType.\n"));
				return GT_FAIL;
			}

			op = PTP_WRITE_DATA;
			opData.ptpAddr = 5;		/* PulseWidth */
			opData.ptpData &= (~0xF000);
			opData.ptpData |= (GT_U16)(trigData->pulseWidth << 12);

			if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
			{
				DBG_INFO(("Failed writing PTPEType.\n"));
				return GT_FAIL;
			}
		}
	}
	else
	{
		op = PTP_WRITE_DATA;
		opData.ptpAddr = 4;		/* TrigClkComp */
		opData.ptpData = (GT_U16)trigData->trigClkComp;

		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed writing PTPEType.\n"));
			return GT_FAIL;
		}
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;

}

/*******************************************************************************
* gtaiGetTrigConfig
*
* DESCRIPTION:
*       This routine gets TAI Trigger configuration parameters.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		trigEn    - enable/disable TAI Trigger.
*		trigData  - TAI Trigger configuration parameters (valid only if trigEn is GT_TRUE).
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*		GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiGetTrigConfig
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_BOOL 	*trigEn,
	OUT GT_TAI_TRIGGER_CONFIG	*trigData
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U32 				data[2];

	DBG_INFO(("gtaiGetTrigConfig Called.\n"));

    /* check if device supports this feature */
#ifndef CONFIG_AVB_FPGA
	if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

	opData.ptpBlock = 0x0;	/* PTP register space */

	opData.ptpPort = 0xE;	/* TAI register */
	op = PTP_READ_DATA;

	/* getting PTPEType, offset 0 */
	opData.ptpAddr = 0;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading PTPEType.\n"));
		return GT_FAIL;
	}

	if (!(opData.ptpData & 1))
	{
		*trigEn = GT_FALSE;
		return GT_OK;
	}

	if (trigData == NULL)
	{
		return GT_BAD_PARAM;
	}

	*trigEn = GT_TRUE;
	trigData->mode = (opData.ptpData >> 1) & 1;
	trigData->intEn = (opData.ptpData >> 9) & 1;

	opData.ptpAddr = 2;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading PTPEType.\n"));
		return GT_FAIL;
	}
	data[0] = opData.ptpData;

	opData.ptpAddr = 3;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading PTPEType.\n"));
		return GT_FAIL;
	}
	data[1] = opData.ptpData;

	trigData->trigGenAmt = GT_PTP_BUILD_TIME(data[1],data[0]);

	opData.ptpAddr = 5;		/* PulseWidth */
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed writing PTPEType.\n"));
		return GT_FAIL;
	}

	trigData->pulseWidth = (GT_U32)((opData.ptpData >> 12) & 0xF);

	/* getting TrigClkComp, offset 4 */
	opData.ptpAddr = 4;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading PTPEType.\n"));
		return GT_FAIL;
	}

	trigData->trigClkComp = (GT_U32)opData.ptpData;

	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gtaiGetTSClkPer
*
* DESCRIPTION:
* 		Time Stamping Clock Period in pico seconds.
*		This routine specifies the clock period for the time stamping clock supplied 
*		to the PTP hardware logic.
*		This is the clock that is used by the hardware logic to update the PTP 
*		Global Time Counter.
*
* INPUTS:
* 		None.
*
* OUTPUTS:
*		clk		- time stamping clock period
*
* RETURNS:
* 		GT_OK      - on success
* 		GT_FAIL    - on error
* 		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		None
*
*******************************************************************************/
GT_STATUS gtaiGetTSClkPer
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_U32		*clk
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gtaiGetTSClkPer Called.\n"));

#ifndef CONFIG_AVB_FPGA
    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

	opData.ptpBlock = 0x0;	/* PTP register space */

	opData.ptpPort = 0xE;	/* TAI register */
	op = PTP_READ_DATA;

	opData.ptpAddr = 1;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	*clk = (GT_U32)opData.ptpData;

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gtaiSetTSClkPer
*
* DESCRIPTION:
* 		Time Stamping Clock Period in pico seconds.
*		This routine specifies the clock period for the time stamping clock supplied 
*		to the PTP hardware logic.
*		This is the clock that is used by the hardware logic to update the PTP 
*		Global Time Counter.
*
* INPUTS:
*		clk		- time stamping clock period
*
* OUTPUTS:
* 		None.
*
* RETURNS:
* 		GT_OK      - on success
* 		GT_FAIL    - on error
* 		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		None
*
*******************************************************************************/
GT_STATUS gtaiSetTSClkPer
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_U32		clk
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gtaiSetTSClkPer Called.\n"));

#ifndef CONFIG_AVB_FPGA
    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

	opData.ptpBlock = 0x0;	/* PTP register space */

	opData.ptpPort = 0xE;	/* TAI register */
	op = PTP_WRITE_DATA;

	opData.ptpAddr = 1;

	opData.ptpData = (GT_U16)clk; 

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gtaiSetMultiPTPSync
*
* DESCRIPTION:
* 		This routine sets Multiple PTP device sync mode and sync time (TrigGenAmt).
*		When enabled, the hardware logic detects a low to high transition on the 
*		EventRequest(GPIO) and transfers the sync time into the PTP Global Time
*		register. The EventCapTime is also updated at that instant.
*
* INPUTS:
*		multiEn		- enable/disable Multiple PTP device sync mode
*		syncTime	- sync time (valid only if multiEn is GT_TRUE)
*
* OUTPUTS:
*		None.
*
* RETURNS:
*		GT_OK      - on success
*		GT_FAIL    - on error
*		GT_BAD_PARAM - if invalid parameter is given
*		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		When enabled, gtaiSetTrigConfig, gtaiSetEventConfig, gtaiSetTimeInc,
*		and gtaiSetTimeDec APIs are not operational.
*
*******************************************************************************/
GT_STATUS gtaiSetMultiPTPSync
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_BOOL 	multiEn,
	IN  GT_32		syncTime
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gtaiSetMultiPTPSync Called.\n"));

    /* check if device supports this feature */
#ifndef CONFIG_AVB_FPGA
	if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
		DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

	opData.ptpBlock = 0x0;	/* PTP register space */

	opData.ptpPort = 0xE;	/* TAI register */
	op = PTP_READ_DATA;

	/* getting PTPEType, offset 0 */
	opData.ptpAddr = 0;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading TAI register.\n"));
		return GT_FAIL;
	}

	opData.ptpData &= ~(1 << 2);

	if (multiEn == GT_FALSE)
	{
		op = PTP_WRITE_DATA;

		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed writing TAI register.\n"));
			return GT_FAIL;
		}

		return GT_OK;
	}

	opData.ptpData |= (1 << 2);

	op = PTP_WRITE_DATA;

	opData.ptpAddr = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed writing TAI register.\n"));
		return GT_FAIL;
	}

	opData.ptpAddr = 2;
	opData.ptpData = GT_PTP_L16_TIME(syncTime);

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed writing TAI register.\n"));
		return GT_FAIL;
	}

	opData.ptpAddr = 3;
	opData.ptpData = GT_PTP_H16_TIME(syncTime);

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed writing TAI register.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gtaiGetMultiPTPSync
*
* DESCRIPTION:
* 		This routine sets Multiple PTP device sync mode and sync time (TrigGenAmt).
*		When enabled, the hardware logic detects a low to high transition on the 
*		EventRequest(GPIO) and transfers the sync time into the PTP Global Time
*		register. The EventCapTime is also updated at that instant.
*
* INPUTS:
*		None.
*
* OUTPUTS:
*		multiEn		- enable/disable Multiple PTP device sync mode
*		syncTime	- sync time (valid only if multiEn is GT_TRUE)
*
* RETURNS:
*		GT_OK      - on success
*		GT_FAIL    - on error
*		GT_BAD_PARAM - if invalid parameter is given
*		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		When enabled, gtaiSetTrigConfig, gtaiSetEventConfig, gtaiSetTimeInc,
*		and gtaiSetTimeDec APIs are not operational.
*
*******************************************************************************/
GT_STATUS gtaiGetMultiPTPSync
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_BOOL 	*multiEn,
	OUT GT_32		*syncTime
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U32 				data[2];

	DBG_INFO(("gtaiGetMultiPTPSync Called.\n"));

    /* check if device supports this feature */
#ifndef CONFIG_AVB_FPGA
	if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
		DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

	opData.ptpBlock = 0x0;	/* PTP register space */

	opData.ptpPort = 0xE;	/* TAI register */
	op = PTP_READ_DATA;

	/* getting PTPEType, offset 0 */
	opData.ptpAddr = 0;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading TAI register.\n"));
		return GT_FAIL;
	}

	if(!(opData.ptpData & (1 << 2)))
	{
		*multiEn = GT_FALSE;
		*syncTime = 0;
		return GT_OK;
	}

	opData.ptpAddr = 2;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading TAI register.\n"));
		return GT_FAIL;
	}
	data[0] = opData.ptpData;

	opData.ptpAddr = 3;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading TAI register.\n"));
		return GT_FAIL;
	}
	data[1] = opData.ptpData;

	*syncTime = GT_PTP_BUILD_TIME(data[1],data[0]);

	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gtaiGetTimeIncDec
*
* DESCRIPTION:
* 		This routine retrieves Time increment/decrement setup.
*		This amount specifies the number of units of PTP Global Time that need to be 
*		incremented or decremented. This is used for adjusting the PTP Global Time 
*		counter value by a certain amount.
*
* INPUTS:
* 		None.
*
* OUTPUTS:
*		expired	- GT_TRUE if inc/dec occurred, GT_FALSE otherwise
*		inc		- GT_TRUE if increment, GT_FALSE if decrement
*		amount	- increment/decrement amount
*
* RETURNS:
* 		GT_OK      - on success
* 		GT_FAIL    - on error
* 		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		Time increment or decrement will be excuted once.
*
*******************************************************************************/
GT_STATUS gtaiGetTimeIncDec
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_BOOL		*expired,
	OUT GT_BOOL		*inc,
	OUT GT_U32		*amount
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gtaiGetTimeIncDec Called.\n"));

#ifndef CONFIG_AVB_FPGA
    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

	opData.ptpBlock = 0x0;	/* PTP register space */

	opData.ptpPort = 0xE;	/* TAI register */
	op = PTP_READ_DATA;

	opData.ptpAddr = 5;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	*inc = (opData.ptpData & 0x800)?GT_FALSE:GT_TRUE;
	*amount = (GT_U32)(opData.ptpData & 0x7FF);

	opData.ptpAddr = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	*expired = (opData.ptpData & 0x8)?GT_FALSE:GT_TRUE;

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gtaiSetTimeInc
*
* DESCRIPTION:
* 		This routine enables time increment by the specifed time increment amount.
*		The amount specifies the number of units of PTP Global Time that need to be 
*		incremented. This is used for adjusting the PTP Global Time counter value by
*		a certain amount.
*		Increment occurs just once.
*
* INPUTS:
*		amount	- time increment amount (0 ~ 0x7FF)
*
* OUTPUTS:
* 		None.
*
* RETURNS:
* 		GT_OK      - on success
* 		GT_FAIL    - on error
* 		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		None
*
*******************************************************************************/
GT_STATUS gtaiSetTimeInc
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_U32		amount
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gtaiSetTimeInc Called.\n"));

#ifndef CONFIG_AVB_FPGA
    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }
#endif

	/* set TimeIncAmt */
	opData.ptpBlock = 0x0;	/* PTP register space */

	opData.ptpPort = 0xE;	/* TAI register */
	op = PTP_READ_DATA;

	opData.ptpAddr = 5;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	opData.ptpData &= 0xF000;
	opData.ptpData |= amount;

	op = PTP_WRITE_DATA;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	/* set TimeIncEn */
	opData.ptpBlock = 0x0;	/* PTP register space */

	opData.ptpPort = 0xE;	/* TAI register */
	op = PTP_READ_DATA;

	opData.ptpAddr = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	opData.ptpData |= 0x8;

	op = PTP_WRITE_DATA;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gtaiSetTimeDec
*
* DESCRIPTION:
* 		This routine enables time decrement by the specifed time decrement amount.
*		The amount specifies the number of units of PTP Global Time that need to be 
*		decremented. This is used for adjusting the PTP Global Time counter value by
*		a certain amount.
*		Decrement occurs just once.
*
* INPUTS:
*		amount	- time decrement amount (0 ~ 0x7FF)
*
* OUTPUTS:
* 		None.
*
* RETURNS:
* 		GT_OK      - on success
* 		GT_FAIL    - on error
* 		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		None
*
*******************************************************************************/
GT_STATUS gtaiSetTimeDec
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_U32		amount
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gtaiSetTimeInc Called.\n"));

#ifndef CONFIG_AVB_FPGA
	/* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
	{
		DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
	}
#endif

	/* set TimeIncAmt */
	opData.ptpBlock = 0x0;	/* PTP register space */

	opData.ptpPort = 0xE;	/* TAI register */
	op = PTP_READ_DATA;

	opData.ptpAddr = 5;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading TAI register.\n"));
		return GT_FAIL;
	}

	opData.ptpData &= 0xF000;
	opData.ptpData |= amount;
	opData.ptpData |= 0x800;	/* decrement */

	op = PTP_WRITE_DATA;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed writing TAI register.\n"));
		return GT_FAIL;
	}

	/* set TimeIncEn */
	opData.ptpBlock = 0x0;	/* PTP register space */

	opData.ptpPort = 0xE;	/* TAI register */
	op = PTP_READ_DATA;

	opData.ptpAddr = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading TAI register.\n"));
		return GT_FAIL;
	}

	opData.ptpData |= 0x8;

	op = PTP_WRITE_DATA;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed writing TAI register.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gavbGetPriority
*
* DESCRIPTION:
*		Priority overwrite.
*		Supported priority type is defined as GT_AVB_PRI_TYPE.
*		Priority is either 3 bits or 2 bits depending on priority type.
*			GT_AVB_HI_FPRI		- priority is 0 ~ 7
*			GT_AVB_HI_QPRI		- priority is 0 ~ 3
*			GT_AVB_LO_FPRI		- priority is 0 ~ 7
*			GT_AVB_LO_QPRI		- priority is 0 ~ 3
*			GT_LEGACY_HI_FPRI	- priority is 0 ~ 7
*			GT_LEGACY_HI_QPRI	- priority is 0 ~ 3
*			GT_LEGACY_LO_FPRI	- priority is 0 ~ 7
*			GT_LEGACY_LO_QPRI	- priority is 0 ~ 3
*
* INPUTS:
* 		priType	- GT_AVB_PRI_TYPE
*
* OUTPUTS:
*		pri	- priority
*
* RETURNS:
* 		GT_OK      - on success
* 		GT_FAIL    - on error
* 		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		None
*
*******************************************************************************/
GT_STATUS gavbGetPriority
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_AVB_PRI_TYPE		priType,
	OUT GT_U32		*pri
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U16		mask, reg, bitPos;

	DBG_INFO(("gavbGetPriority Called.\n"));

#ifndef CONFIG_AVB_FPGA
	/* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_AVB_POLICY))
	{
		DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
	}
#endif

	switch (priType)
	{
		case GT_AVB_HI_FPRI:
			mask = 0x7;
			reg = 0;
			bitPos = 12;
			break;
		case GT_AVB_HI_QPRI:
			mask = 0x3;
			reg = 0;
			bitPos = 8;
			break;
		case GT_AVB_LO_FPRI:
			mask = 0x7;
			reg = 0;
			bitPos = 4;
			break;
		case GT_AVB_LO_QPRI:
			mask = 0x3;
			reg = 0;
			bitPos = 0;
			break;
		case GT_LEGACY_HI_FPRI:
			mask = 0x7;
			reg = 4;
			bitPos = 12;
			break;
		case GT_LEGACY_HI_QPRI:
			mask = 0x3;
			reg = 4;
			bitPos = 8;
			break;
		case GT_LEGACY_LO_FPRI:
			mask = 0x7;
			reg = 4;
			bitPos = 4;
			break;
		case GT_LEGACY_LO_QPRI:
			mask = 0x3;
			reg = 4;
			bitPos = 0;
			break;
		default:
			return GT_BAD_PARAM;
	}

	opData.ptpBlock = 0x1;	/* AVB Policy register space */

	opData.ptpPort = 0xF;	/* Global register */
	op = PTP_READ_DATA;

	opData.ptpAddr = reg;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading TAI register.\n"));
		return GT_FAIL;
	}

	*pri = (GT_U32)(opData.ptpData >> bitPos) & mask;

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gavbSetPriority
*
* DESCRIPTION:
*		Priority overwrite.
*		Supported priority type is defined as GT_AVB_PRI_TYPE.
*		Priority is either 3 bits or 2 bits depending on priority type.
*			GT_AVB_HI_FPRI		- priority is 0 ~ 7
*			GT_AVB_HI_QPRI		- priority is 0 ~ 3
*			GT_AVB_LO_FPRI		- priority is 0 ~ 7
*			GT_AVB_LO_QPRI		- priority is 0 ~ 3
*			GT_LEGACY_HI_FPRI	- priority is 0 ~ 7
*			GT_LEGACY_HI_QPRI	- priority is 0 ~ 3
*			GT_LEGACY_LO_FPRI	- priority is 0 ~ 7
*			GT_LEGACY_LO_QPRI	- priority is 0 ~ 3
*
* INPUTS:
* 		priType	- GT_AVB_PRI_TYPE
*		pri	- priority
*
* OUTPUTS:
*		None
*
* RETURNS:
* 		GT_OK      - on success
* 		GT_FAIL    - on error
* 		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		None
*
*******************************************************************************/
GT_STATUS gavbSetPriority
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_AVB_PRI_TYPE		priType,
	IN  GT_U32		pri
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U16		mask, reg, bitPos;

	DBG_INFO(("gavbSetPriority Called.\n"));

#ifndef CONFIG_AVB_FPGA
	/* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_AVB_POLICY))
	{
		DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
	}
#endif

	switch (priType)
	{
		case GT_AVB_HI_FPRI:
			mask = 0x7;
			reg = 0;
			bitPos = 12;
			break;
		case GT_AVB_HI_QPRI:
			mask = 0x3;
			reg = 0;
			bitPos = 8;
			break;
		case GT_AVB_LO_FPRI:
			mask = 0x7;
			reg = 0;
			bitPos = 4;
			break;
		case GT_AVB_LO_QPRI:
			mask = 0x3;
			reg = 0;
			bitPos = 0;
			break;
		case GT_LEGACY_HI_FPRI:
			mask = 0x7;
			reg = 4;
			bitPos = 12;
			break;
		case GT_LEGACY_HI_QPRI:
			mask = 0x3;
			reg = 4;
			bitPos = 8;
			break;
		case GT_LEGACY_LO_FPRI:
			mask = 0x7;
			reg = 4;
			bitPos = 4;
			break;
		case GT_LEGACY_LO_QPRI:
			mask = 0x3;
			reg = 4;
			bitPos = 0;
			break;
		default:
			return GT_BAD_PARAM;
	}

	if (pri & (~mask))
	{
		return GT_BAD_PARAM;
	}

	opData.ptpBlock = 0x1;	/* AVB Policy register space */

	opData.ptpPort = 0xF;	/* Global register */
	op = PTP_READ_DATA;

	opData.ptpAddr = reg;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading TAI register.\n"));
		return GT_FAIL;
	}

	opData.ptpData &= ~(mask << bitPos);
	opData.ptpData |= (pri << bitPos);

	op = PTP_WRITE_DATA;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed writing TAI register.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}



/*******************************************************************************
* gavbGetAVBHiLimit
*
* DESCRIPTION:
*		AVB Hi Frame Limit.
*		When these bits are zero, normal frame processing occurs. 
*		When it's non-zero, they are used to define the maximum frame size allowed
*		for AVB frames that can be placed into the GT_AVB_HI_QPRI queue. Frames
*		that are over this size limit are filtered. The only exception to this 
*		is non-AVB frames that get their QPriAvb assigned by the Priority Override 
*		Table
*
* INPUTS:
* 		None
*
* OUTPUTS:
*		limit	- Hi Frame Limit
*
* RETURNS:
* 		GT_OK      - on success
* 		GT_FAIL    - on error
* 		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		None
*
*******************************************************************************/
GT_STATUS gavbGetAVBHiLimit
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_U32		*limit
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gavbGetAVBHiLimit Called.\n"));

#ifndef CONFIG_AVB_FPGA
	/* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_AVB_POLICY))
	{
		DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
	}
#endif

	opData.ptpBlock = 0x1;	/* AVB Policy register space */

	opData.ptpPort = 0xF;	/* Global register */
	op = PTP_READ_DATA;

	opData.ptpAddr = 8;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	*limit = (GT_U32)(opData.ptpData & 0x7FF);

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gavbSetAVBHiLimit
*
* DESCRIPTION:
*		AVB Hi Frame Limit.
*		When these bits are zero, normal frame processing occurs. 
*		When it's non-zero, they are used to define the maximum frame size allowed
*		for AVB frames that can be placed into the GT_AVB_HI_QPRI queue. Frames
*		that are over this size limit are filtered. The only exception to this 
*		is non-AVB frames that get their QPriAvb assigned by the Priority Override 
*		Table
*
* INPUTS:
*		limit	- Hi Frame Limit
*
* OUTPUTS:
* 		None
*
* RETURNS:
* 		GT_OK      - on success
* 		GT_FAIL    - on error
* 		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		None
*
*******************************************************************************/
GT_STATUS gavbSetAVBHiLimit
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_U32		limit
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gavbSetAVBHiLimit Called.\n"));

#ifndef CONFIG_AVB_FPGA
	/* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_AVB_POLICY))
	{
		DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
	}
#endif

	opData.ptpBlock = 0x1;	/* AVB Policy register space */

	opData.ptpPort = 0xF;	/* Global register */
	op = PTP_WRITE_DATA;

	opData.ptpAddr = 8;
	opData.ptpData = (GT_U16)limit;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}


	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gavbGetPtpExtClk
*
* DESCRIPTION:
*		PTP external clock select.
*		When this bit is cleared to a zero, the PTP core gets its clock from 
*		an internal 125MHz clock based on the device's XTAL_IN input. 
*		When this bit is set to a one, the PTP core gets its clock from the device's
*		PTP_EXTCLK pin.
*
* INPUTS:
* 		None
*
* OUTPUTS:
*		extClk	- GT_TRUE if external clock is selected, GT_FALSE otherwise
*
* RETURNS:
* 		GT_OK      - on success
* 		GT_FAIL    - on error
* 		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		None
*
*******************************************************************************/
GT_STATUS gavbGetPtpExtClk
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_BOOL		*extClk
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gavbGetPtpExtClk Called.\n"));

#ifndef CONFIG_AVB_FPGA
	/* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_AVB_POLICY))
	{
		DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
	}
#endif

	opData.ptpBlock = 0x1;	/* AVB Policy register space */

	opData.ptpPort = 0xF;	/* Global register */
	op = PTP_READ_DATA;

	opData.ptpAddr = 0xB;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	*extClk = (GT_U32)(opData.ptpData >> 15) & 0x1;

	DBG_INFO(("OK.\n"));
	return GT_OK;
}



/*******************************************************************************
* gavbSetPtpExtClk
*
* DESCRIPTION:
*		PTP external clock select.
*		When this bit is cleared to a zero, the PTP core gets its clock from 
*		an internal 125MHz clock based on the device's XTAL_IN input. 
*		When this bit is set to a one, the PTP core gets its clock from the device's
*		PTP_EXTCLK pin.
*
* INPUTS:
*		extClk	- GT_TRUE if external clock is selected, GT_FALSE otherwise
*
* OUTPUTS:
* 		None
*
* RETURNS:
* 		GT_OK      - on success
* 		GT_FAIL    - on error
* 		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		None
*
*******************************************************************************/
GT_STATUS gavbSetPtpExtClk
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_BOOL		extClk
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gavbSetPtpExtClk Called.\n"));

#ifndef CONFIG_AVB_FPGA
	/* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_AVB_POLICY))
	{
		DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
	}
#endif

	opData.ptpBlock = 0x1;	/* AVB Policy register space */

	opData.ptpPort = 0xF;	/* Global register */
	op = PTP_READ_DATA;

	opData.ptpAddr = 0xB;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	if(extClk)
		opData.ptpData |= 0x8000;
	else
		opData.ptpData &= ~0x8000;

	op = PTP_WRITE_DATA;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gavbGetRecClkSel
*
* DESCRIPTION:
*		Synchronous Ethernet Recovered Clock Select.
*		This field indicate the internal PHY number whose recovered clock will
*		be presented on the SE_RCLK0 or SE_RCLK1 pin depending on the recClk selection.
*
* INPUTS:
*		recClk	- GT_AVB_RECOVERED_CLOCK type
*
* OUTPUTS:
*		clkSel	- recovered clock selection
*
* RETURNS:
* 		GT_OK      - on success
* 		GT_FAIL    - on error
* 		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		None
*
*******************************************************************************/
GT_STATUS gavbGetRecClkSel
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_AVB_RECOVERED_CLOCK	recClk,
	OUT GT_U32		*clkSel
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U16		bitPos;

	DBG_INFO(("gavbGetRecClkSel Called.\n"));

#ifndef CONFIG_AVB_FPGA
	/* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_AVB_POLICY))
	{
		DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
	}
#endif

	switch (recClk)
	{
		case GT_PRIMARY_RECOVERED_CLOCK:
			bitPos = 0;
			break;
		case GT_SECONDARY_RECOVERED_CLOCK:
			bitPos = 4;
			break;
		default:
			return GT_BAD_PARAM;
	}

	opData.ptpBlock = 0x1;	/* AVB Policy register space */

	opData.ptpPort = 0xF;	/* Global register */
	op = PTP_READ_DATA;

	opData.ptpAddr = 0xB;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	*clkSel = (GT_U32)(opData.ptpData >> bitPos) & 0x7;

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gavbSetRecClkSel
*
* DESCRIPTION:
*		Synchronous Ethernet Recovered Clock Select.
*		This field indicate the internal PHY number whose recovered clock will
*		be presented on the SE_RCLK0 or SE_RCLK1 pin depending on the recClk selection.
*
* INPUTS:
*		recClk	- GT_AVB_RECOVERED_CLOCK type
*		clkSel	- recovered clock selection (should be less than 8)
*
* OUTPUTS:
*		None
*
* RETURNS:
* 		GT_OK      - on success
* 		GT_FAIL    - on error
* 		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		None
*
*******************************************************************************/
GT_STATUS gavbSetRecClkSel
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_AVB_RECOVERED_CLOCK	recClk,
	IN  GT_U32		clkSel
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U16		bitPos;

	DBG_INFO(("gavbSetRecClkSel Called.\n"));

#ifndef CONFIG_AVB_FPGA
	/* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_AVB_POLICY))
	{
		DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
	}
#endif

	if (clkSel > 0x7)
		return GT_BAD_PARAM;

	switch (recClk)
	{
		case GT_PRIMARY_RECOVERED_CLOCK:
			bitPos = 0;
			break;
		case GT_SECONDARY_RECOVERED_CLOCK:
			bitPos = 4;
			break;
		default:
			return GT_BAD_PARAM;
	}

	opData.ptpBlock = 0x1;	/* AVB Policy register space */

	opData.ptpPort = 0xF;	/* Global register */
	op = PTP_READ_DATA;

	opData.ptpAddr = 0xB;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	opData.ptpData &= ~(0x7 << bitPos);
	opData.ptpData |= clkSel << bitPos;

	op = PTP_WRITE_DATA;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gavbGetAvbOuiBytes
*
* DESCRIPTION:
*		AVB OUI Limit Filter bytes(0 ~ 2).
*		When all three of the AvbOui Bytes are zero, normal frame processing occurs.
*		When any of the three AvbOui Bytes are non-zero, all AVB frames must have a
*		destination address whose 1st three bytes of the DA match these three 
*		AvbOui Bytes or the frame will be filtered.
*
* INPUTS:
* 		None
*
* OUTPUTS:
*		ouiBytes	- 3 bytes of OUI field in Ethernet address format
*
* RETURNS:
* 		GT_OK      - on success
* 		GT_FAIL    - on error
* 		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		None
*
*******************************************************************************/
GT_STATUS gavbGetAvbOuiBytes
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_U8		*obiBytes
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gavbGetAvbOuiBytes Called.\n"));

#ifndef CONFIG_AVB_FPGA
	/* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_AVB_POLICY))
	{
		DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
	}
#endif

	opData.ptpBlock = 0x1;	/* AVB Policy register space */

	opData.ptpPort = 0xF;	/* Global register */
	op = PTP_READ_DATA;

	opData.ptpAddr = 0xC;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	obiBytes[0] = (GT_U8)((opData.ptpData >> 8) & 0xFF);
	obiBytes[1] = (GT_U8)(opData.ptpData & 0xFF);

	opData.ptpAddr = 0xD;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	obiBytes[2] = (GT_U8)((opData.ptpData >> 8) & 0xFF);

	DBG_INFO(("OK.\n"));
	return GT_OK;
}

/*******************************************************************************
* gavbSetAvbOuiBytes
*
* DESCRIPTION:
*		AVB OUI Limit Filter bytes(0 ~ 2).
*		When all three of the AvbOui Bytes are zero, normal frame processing occurs.
*		When any of the three AvbOui Bytes are non-zero, all AVB frames must have a
*		destination address whose 1st three bytes of the DA match these three 
*		AvbOui Bytes or the frame will be filtered.
*
* INPUTS:
*		ouiBytes	- 3 bytes of OUI field in Ethernet address format
*
* OUTPUTS:
* 		None
*
* RETURNS:
* 		GT_OK      - on success
* 		GT_FAIL    - on error
* 		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		None
*
*******************************************************************************/
GT_STATUS gavbSetAvbOuiBytes
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_U8		*obiBytes
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gavbSetAvbOuiBytes Called.\n"));

#ifndef CONFIG_AVB_FPGA
	/* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_AVB_POLICY))
	{
		DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
	}
#endif

	opData.ptpBlock = 0x1;	/* AVB Policy register space */

	opData.ptpPort = 0xF;	/* Global register */
	op = PTP_WRITE_DATA;

	opData.ptpAddr = 0xC;

	opData.ptpData = (obiBytes[0] << 8) | obiBytes[1];

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	opData.ptpAddr = 0xD;
	opData.ptpData = (obiBytes[2] << 8);

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gavbGetAvbMode
*
* DESCRIPTION:
*		Port's AVB Mode.
*
* INPUTS:
*		port	- the logical port number
*
* OUTPUTS:
*		mode	- GT_AVB_MODE type
*
* RETURNS:
* 		GT_OK      - on success
* 		GT_FAIL    - on error
* 		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		None
*
*******************************************************************************/
GT_STATUS gavbGetAvbMode
(
	IN  GT_QD_DEV 	*dev,
	IN	GT_LPORT	port,
	OUT GT_AVB_MODE	*mode
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U8          hwPort;         /* the physical port number     */

	DBG_INFO(("gavbGetAvbMode Called.\n"));

#ifndef CONFIG_AVB_FPGA
	/* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_AVB_POLICY))
	{
		DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
	}
#endif

	/* translate LPORT to hardware port */
	hwPort = GT_LPORT_2_PORT(port);
	if (hwPort == GT_INVALID_PORT)
		return GT_BAD_PARAM;

	opData.ptpBlock = 0x1;	/* AVB Policy register space */

	opData.ptpPort = (GT_U16)hwPort;
	op = PTP_READ_DATA;

	opData.ptpAddr = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	*mode = (GT_AVB_MODE)((opData.ptpData >> 14) & 0x3);

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gavbSetAvbMode
*
* DESCRIPTION:
*		Port's AVB Mode.
*
* INPUTS:
*		port	- the logical port number
*		mode	- GT_AVB_MODE type
*
* OUTPUTS:
*		None
*
* RETURNS:
* 		GT_OK      - on success
* 		GT_FAIL    - on error
* 		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		None
*
*******************************************************************************/
GT_STATUS gavbSetAvbMode
(
	IN  GT_QD_DEV 	*dev,
	IN	GT_LPORT	port,
	IN  GT_AVB_MODE	mode
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U8          hwPort;         /* the physical port number     */

	DBG_INFO(("gavbSetAvbMode Called.\n"));

#ifndef CONFIG_AVB_FPGA
	/* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_AVB_POLICY))
	{
		DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
	}
#endif

	/* translate LPORT to hardware port */
	hwPort = GT_LPORT_2_PORT(port);
	if (hwPort == GT_INVALID_PORT)
		return GT_BAD_PARAM;

	opData.ptpBlock = 0x1;	/* AVB Policy register space */

	opData.ptpPort = (GT_U16)hwPort;
	op = PTP_READ_DATA;

	opData.ptpAddr = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	opData.ptpData &= ~(0x3 << 14);
	opData.ptpData |= (mode << 14);

	op = PTP_WRITE_DATA;

	opData.ptpAddr = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading DisPTP.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}

/*******************************************************************************
* gavbGetAvbOverride
*
* DESCRIPTION:
*		AVB Override.
*		When disabled, normal frame processing occurs.
*		When enabled, the egress portion of this port is considered AVB even if 
*		the ingress portion is not.
*
* INPUTS:
*		port	- the logical port number
*
* OUTPUTS:
*		en		- GT_TRUE if enabled, GT_FALSE otherwise
*
* RETURNS:
* 		GT_OK      - on success
* 		GT_FAIL    - on error
* 		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		None
*
*******************************************************************************/
GT_STATUS gavbGetAvbOverride
(
	IN  GT_QD_DEV 	*dev,
	IN	GT_LPORT	port,
	OUT GT_BOOL		*en
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U8          hwPort;         /* the physical port number     */

	DBG_INFO(("gavbGetAvbOverride Called.\n"));

#ifndef CONFIG_AVB_FPGA
	/* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_AVB_POLICY))
	{
		DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
	}
#endif

	/* translate LPORT to hardware port */
	hwPort = GT_LPORT_2_PORT(port);
	if (hwPort == GT_INVALID_PORT)
		return GT_BAD_PARAM;

	opData.ptpBlock = 0x1;	/* AVB Policy register space */

	opData.ptpPort = (GT_U16)hwPort;
	op = PTP_READ_DATA;

	opData.ptpAddr = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed AVB operation.\n"));
		return GT_FAIL;
	}

	*en = (opData.ptpData >> 13) & 0x1;

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gavbSetAvbOverride
*
* DESCRIPTION:
*		AVB Override.
*		When disabled, normal frame processing occurs.
*		When enabled, the egress portion of this port is considered AVB even if 
*		the ingress portion is not.
*
* INPUTS:
*		port	- the logical port number
*		en		- GT_TRUE to enable, GT_FALSE otherwise
*
* OUTPUTS:
*		None
*
* RETURNS:
* 		GT_OK      - on success
* 		GT_FAIL    - on error
* 		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		None
*
*******************************************************************************/
GT_STATUS gavbSetAvbOverride
(
	IN  GT_QD_DEV 	*dev,
	IN	GT_LPORT	port,
	IN  GT_BOOL		en
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U8          hwPort;         /* the physical port number     */

	DBG_INFO(("gavbSetAvbOverride Called.\n"));

#ifndef CONFIG_AVB_FPGA
	/* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_AVB_POLICY))
	{
		DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
	}
#endif

	/* translate LPORT to hardware port */
	hwPort = GT_LPORT_2_PORT(port);
	if (hwPort == GT_INVALID_PORT)
		return GT_BAD_PARAM;

	opData.ptpBlock = 0x1;	/* AVB Policy register space */

	opData.ptpPort = (GT_U16)hwPort;
	op = PTP_READ_DATA;

	opData.ptpAddr = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed AVB operation.\n"));
		return GT_FAIL;
	}

	if (en)
		opData.ptpData |= (0x1 << 13);
	else
		opData.ptpData &= ~(0x1 << 13);

	op = PTP_WRITE_DATA;

	opData.ptpAddr = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed AVB operation.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gavbGetFilterBadAvb
*
* DESCRIPTION:
*		Filter Bad AVB frames.
*		When disabled, normal frame processing occurs.
*		When enabled, frames that are considered Bad AVB frames are filtered.
*
* INPUTS:
*		port	- the logical port number
*
* OUTPUTS:
*		en		- GT_TRUE if enabled, GT_FALSE otherwise
*
* RETURNS:
* 		GT_OK      - on success
* 		GT_FAIL    - on error
* 		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		None
*
*******************************************************************************/
GT_STATUS gavbGetFilterBadAvb
(
	IN  GT_QD_DEV 	*dev,
	IN	GT_LPORT	port,
	OUT GT_BOOL		*en
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U8          hwPort;         /* the physical port number     */

	DBG_INFO(("gavbGetFilterBadAvb Called.\n"));

#ifndef CONFIG_AVB_FPGA
	/* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_AVB_POLICY))
	{
		DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
	}
#endif

	/* translate LPORT to hardware port */
	hwPort = GT_LPORT_2_PORT(port);
	if (hwPort == GT_INVALID_PORT)
		return GT_BAD_PARAM;

	opData.ptpBlock = 0x1;	/* AVB Policy register space */

	opData.ptpPort = (GT_U16)hwPort;
	op = PTP_READ_DATA;

	opData.ptpAddr = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed AVB operation.\n"));
		return GT_FAIL;
	}

	*en = (opData.ptpData >> 12) & 0x1;

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gavbSetFilterBadAvb
*
* DESCRIPTION:
*		Filter Bad AVB frames.
*		When disabled, normal frame processing occurs.
*		When enabled, frames that are considered Bad AVB frames are filtered.
*
* INPUTS:
*		port	- the logical port number
*		en		- GT_TRUE to enable, GT_FALSE otherwise
*
* OUTPUTS:
*		None
*
* RETURNS:
* 		GT_OK      - on success
* 		GT_FAIL    - on error
* 		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		None
*
*******************************************************************************/
GT_STATUS gavbSetFilterBadAvb
(
	IN  GT_QD_DEV 	*dev,
	IN	GT_LPORT	port,
	IN  GT_BOOL		en
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U8          hwPort;         /* the physical port number     */

	DBG_INFO(("gavbSetFilterBadAvb Called.\n"));

#ifndef CONFIG_AVB_FPGA
	/* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_AVB_POLICY))
	{
		DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
	}
#endif

	/* translate LPORT to hardware port */
	hwPort = GT_LPORT_2_PORT(port);
	if (hwPort == GT_INVALID_PORT)
		return GT_BAD_PARAM;

	opData.ptpBlock = 0x1;	/* AVB Policy register space */

	opData.ptpPort = (GT_U16)hwPort;
	op = PTP_READ_DATA;

	opData.ptpAddr = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed AVB operation.\n"));
		return GT_FAIL;
	}

	if (en)
		opData.ptpData |= (0x1 << 12);
	else
		opData.ptpData &= ~(0x1 << 12);

	op = PTP_WRITE_DATA;

	opData.ptpAddr = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed AVB operation.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gavbGetAvbTunnel
*
* DESCRIPTION:
*		AVB Tunnel.
*		When disabled, normal frame processing occurs.
*		When enabled, the port based VLAN Table masking, 802.1Q VLAN membership 
*		masking and the Trunk Masking is bypassed for any frame entering this port
*		that is considered AVB by DA. This includes unicast as well as multicast
*		frame
*
* INPUTS:
*		port	- the logical port number
*
* OUTPUTS:
*		en		- GT_TRUE if enabled, GT_FALSE otherwise
*
* RETURNS:
* 		GT_OK      - on success
* 		GT_FAIL    - on error
* 		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		None
*
*******************************************************************************/
GT_STATUS gavbGetAvbTunnel
(
	IN  GT_QD_DEV 	*dev,
	IN	GT_LPORT	port,
	OUT GT_BOOL		*en
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U8          hwPort;         /* the physical port number     */

	DBG_INFO(("gavbGetAvbTunnel Called.\n"));

#ifndef CONFIG_AVB_FPGA
	/* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_AVB_POLICY))
	{
		DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
	}
#endif

	/* translate LPORT to hardware port */
	hwPort = GT_LPORT_2_PORT(port);
	if (hwPort == GT_INVALID_PORT)
		return GT_BAD_PARAM;

	opData.ptpBlock = 0x1;	/* AVB Policy register space */

	opData.ptpPort = (GT_U16)hwPort;
	op = PTP_READ_DATA;

	opData.ptpAddr = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed AVB operation.\n"));
		return GT_FAIL;
	}

	*en = (opData.ptpData >> 11) & 0x1;

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gavbSetAvbTunnel
*
* DESCRIPTION:
*		AVB Tunnel.
*		When disabled, normal frame processing occurs.
*		When enabled, the port based VLAN Table masking, 802.1Q VLAN membership 
*		masking and the Trunk Masking is bypassed for any frame entering this port
*		that is considered AVB by DA. This includes unicast as well as multicast
*		frame
*
* INPUTS:
*		port	- the logical port number
*		en		- GT_TRUE to enable, GT_FALSE otherwise
*
* OUTPUTS:
*		None
*
* RETURNS:
* 		GT_OK      - on success
* 		GT_FAIL    - on error
* 		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		None
*
*******************************************************************************/
GT_STATUS gavbSetAvbTunnel
(
	IN  GT_QD_DEV 	*dev,
	IN	GT_LPORT	port,
	IN  GT_BOOL		en
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U8          hwPort;         /* the physical port number     */

	DBG_INFO(("GT_STATUS gavbGetAvbTunnel Called.\n"));

#ifndef CONFIG_AVB_FPGA
	/* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_AVB_POLICY))
	{
		DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
	}
#endif

	/* translate LPORT to hardware port */
	hwPort = GT_LPORT_2_PORT(port);
	if (hwPort == GT_INVALID_PORT)
		return GT_BAD_PARAM;

	opData.ptpBlock = 0x1;	/* AVB Policy register space */

	opData.ptpPort = (GT_U16)hwPort;
	op = PTP_READ_DATA;

	opData.ptpAddr = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed AVB operation.\n"));
		return GT_FAIL;
	}

	if (en)
		opData.ptpData |= (0x1 << 11);
	else
		opData.ptpData &= ~(0x1 << 11);

	op = PTP_WRITE_DATA;

	opData.ptpAddr = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed AVB operation.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gavbGetAvbFramePolicy
*
* DESCRIPTION:
*		AVB Hi or Lo frame policy mapping.
*		Supported policies are defined in GT_AVB_FRAME_POLICY.
*			
* INPUTS:
*		port	- the logical port number
*		fType	- GT_AVB_FRAME_TYPE 
*
* OUTPUTS:
*		policy	- GT_AVB_FRAME_POLICY
*
* RETURNS:
* 		GT_OK      - on success
* 		GT_FAIL    - on error
* 		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		None
*
*******************************************************************************/
GT_STATUS gavbGetAvbFramePolicy
(
	IN  GT_QD_DEV 	*dev,
	IN	GT_LPORT	port,
	IN	GT_AVB_FRAME_TYPE	fType,
	OUT GT_AVB_FRAME_POLICY		*policy
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U8 		hwPort;         /* the physical port number     */
	GT_U16		bitPos;

	DBG_INFO(("gavbGetAvbFramePolicy Called.\n"));

#ifndef CONFIG_AVB_FPGA
	/* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_AVB_POLICY))
	{
		DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
	}
#endif

	/* translate LPORT to hardware port */
	hwPort = GT_LPORT_2_PORT(port);
	if (hwPort == GT_INVALID_PORT)
		return GT_BAD_PARAM;

	switch (fType)
	{
		case AVB_HI_FRAME:
			bitPos = 2;
			break;
		case AVB_LO_FRAME:
			bitPos = 0;
			break;
		default:
			return GT_BAD_PARAM;
	}

	opData.ptpBlock = 0x1;	/* AVB Policy register space */

	opData.ptpPort = (GT_U16)hwPort;
	op = PTP_READ_DATA;

	opData.ptpAddr = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed AVB operation.\n"));
		return GT_FAIL;
	}

	*policy = (opData.ptpData >> bitPos) & 0x3;

	DBG_INFO(("OK.\n"));
	return GT_OK;
}

/*******************************************************************************
* gavbSetAvbFramePolicy
*
* DESCRIPTION:
*		AVB Hi or Lo frame policy mapping.
*		Supported policies are defined in GT_AVB_FRAME_POLICY.
*
* INPUTS:
*		port	- the logical port number
*		fType	- GT_AVB_FRAME_TYPE 
*		policy	- GT_AVB_FRAME_POLICY
*
* OUTPUTS:
*		None
*
* RETURNS:
* 		GT_OK      - on success
* 		GT_FAIL    - on error
* 		GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
* 		None
*
*******************************************************************************/
GT_STATUS gavbSetAvbFramePolicy
(
	IN  GT_QD_DEV 	*dev,
	IN	GT_LPORT	port,
	IN	GT_AVB_FRAME_TYPE	fType,
	IN  GT_AVB_FRAME_POLICY		policy
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U8 		hwPort;         /* the physical port number     */
	GT_U16		bitPos;

	DBG_INFO(("gavbSetAvbFramePolicy Called.\n"));

#ifndef CONFIG_AVB_FPGA
	/* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_AVB_POLICY))
	{
		DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
	}
#endif

	/* translate LPORT to hardware port */
	hwPort = GT_LPORT_2_PORT(port);
	if (hwPort == GT_INVALID_PORT)
		return GT_BAD_PARAM;

	switch (fType)
	{
		case AVB_HI_FRAME:
			bitPos = 2;
			break;
		case AVB_LO_FRAME:
			bitPos = 0;
			break;
		default:
			return GT_BAD_PARAM;
	}

	opData.ptpBlock = 0x1;	/* AVB Policy register space */

	opData.ptpPort = (GT_U16)hwPort;
	op = PTP_READ_DATA;

	opData.ptpAddr = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed AVB operation.\n"));
		return GT_FAIL;
	}

	opData.ptpData &= ~(0x3 << bitPos);
	opData.ptpData |= (policy & 0x3) << bitPos;

	op = PTP_WRITE_DATA;

	opData.ptpAddr = 0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed AVB operation.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}



/****************************************************************************/
/* Internal functions.                                                  */
/****************************************************************************/

/*******************************************************************************
* ptpOperationPerform
*
* DESCRIPTION:
*       This function accesses PTP Command Register and Data Register.
*
* INPUTS:
*       ptpOp      - The stats operation bits to be written into the stats
*                    operation register.
*
* OUTPUTS:
*       ptpData    - points to the data storage that the operation requires.
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS ptpOperationPerform
(
    IN    GT_QD_DEV 			*dev,
    IN    GT_PTP_OPERATION		ptpOp,
    INOUT GT_PTP_OP_DATA		*opData
)
{
    GT_STATUS       retVal;	/* Functions return value */
    GT_U16          data; 	/* temporary Data storage */
	GT_U32 			i;

#ifdef CONFIG_AVB_FPGA
	GT_U32 			tmpData;
#endif

    gtSemTake(dev,dev->ptpRegsSem,OS_WAIT_FOREVER);

    /* Wait until the ptp in ready. */
    data = 1;
    while(data == 1)
    {
#ifndef CONFIG_AVB_FPGA
        retVal = hwGetGlobal2RegField(dev,QD_REG_PTP_COMMAND,15,1,&data);
#else
        retVal = AVB_FPGA_READ_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_COMMAND,&tmpData);
		data = (GT_U16)tmpData;
		data = (data >> 15) & 0x1;
#endif
        if(retVal != GT_OK)
        {
            gtSemGive(dev,dev->ptpRegsSem);
            return retVal;
        }
    }

    /* Set the PTP Operation register */
	switch (ptpOp)
	{
		case PTP_WRITE_DATA:
			data = (GT_U16)opData->ptpData;
#ifndef CONFIG_AVB_FPGA
			retVal = hwWriteGlobal2Reg(dev,QD_REG_PTP_DATA,data);
#else
			tmpData = (GT_U32)data;
	        retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_DATA,tmpData);
#endif
	        if(retVal != GT_OK)
    	    {
        	    gtSemGive(dev,dev->ptpRegsSem);
            	return retVal;
	        }

			data = (GT_U16)((1 << 15) | (PTP_WRITE_DATA << 12) | 
					(opData->ptpPort << 8)	|
					(opData->ptpBlock << 5)	|
					(opData->ptpAddr & 0x1F));
#ifndef CONFIG_AVB_FPGA
			retVal = hwWriteGlobal2Reg(dev,QD_REG_PTP_COMMAND,data);
#else
			tmpData = (GT_U32)data;
	        retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_COMMAND,tmpData);
#endif
	        if(retVal != GT_OK)
    	    {
        	    gtSemGive(dev,dev->ptpRegsSem);
            	return retVal;
	        }
			break;

		case PTP_READ_DATA:
			data = (GT_U16)((1 << 15) | (PTP_READ_DATA << 12) | 
					(opData->ptpPort << 8)	|
					(opData->ptpBlock << 5)	|
					(opData->ptpAddr & 0x1F));
#ifndef CONFIG_AVB_FPGA
			retVal = hwWriteGlobal2Reg(dev,QD_REG_PTP_COMMAND,data);
#else
			tmpData = (GT_U32)data;
	        retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_COMMAND,tmpData);
#endif
	        if(retVal != GT_OK)
    	    {
        	    gtSemGive(dev,dev->ptpRegsSem);
            	return retVal;
	        }

		    data = 1;
		    while(data == 1)
		    {
#ifndef CONFIG_AVB_FPGA
		        retVal = hwGetGlobal2RegField(dev,QD_REG_PTP_COMMAND,15,1,&data);
#else
        		retVal = AVB_FPGA_READ_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_COMMAND,&tmpData);
				data = (GT_U32)tmpData;
				data = (data >> 15) & 0x1;
#endif
		        if(retVal != GT_OK)
		        {
		            gtSemGive(dev,dev->ptpRegsSem);
		            return retVal;
        		}
		    }

#ifndef CONFIG_AVB_FPGA
			retVal = hwReadGlobal2Reg(dev,QD_REG_PTP_DATA,&data);
#else
	        retVal = AVB_FPGA_READ_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_DATA,&tmpData);
			data = (GT_U32)tmpData;
#endif
			opData->ptpData = (GT_U32)data;
		    gtSemGive(dev,dev->ptpRegsSem);
		    return retVal;

		case PTP_READ_MULTIPLE_DATA:
			data = (GT_U16)((1 << 15) | (PTP_READ_MULTIPLE_DATA << 12) | 
					(opData->ptpPort << 8)	|
					(opData->ptpBlock << 5)	|
					(opData->ptpAddr & 0x1F));
#ifndef CONFIG_AVB_FPGA
			retVal = hwWriteGlobal2Reg(dev,QD_REG_PTP_COMMAND,data);
#else
			tmpData = (GT_U32)data;
	        retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_COMMAND,tmpData);
#endif
	        if(retVal != GT_OK)
    	    {
        	    gtSemGive(dev,dev->ptpRegsSem);
            	return retVal;
	        }

		    data = 1;
		    while(data == 1)
		    {
#ifndef CONFIG_AVB_FPGA
		        retVal = hwGetGlobal2RegField(dev,QD_REG_PTP_COMMAND,15,1,&data);
#else
        		retVal = AVB_FPGA_READ_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_COMMAND,&tmpData);
				data = (GT_U32)tmpData;
				data = (data >> 15) & 0x1;
#endif
		        if(retVal != GT_OK)
		        {
		            gtSemGive(dev,dev->ptpRegsSem);
		            return retVal;
        		}
		    }

			for(i=0; i<opData->nData; i++)
			{
#ifndef CONFIG_AVB_FPGA
				retVal = hwReadGlobal2Reg(dev,QD_REG_PTP_DATA,&data);
#else
		        retVal = AVB_FPGA_READ_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_DATA,&tmpData);
				data = (GT_U32)tmpData;
#endif
				opData->ptpMultiData[i] = (GT_U32)data;
	    	    if(retVal != GT_OK)
    	    	{
        	    	gtSemGive(dev,dev->ptpRegsSem);
	            	return retVal;
		        }
			}

		    gtSemGive(dev,dev->ptpRegsSem);
		    return retVal;

		case PTP_READ_TIMESTAMP_DATA:
			data = (GT_U16)((1 << 15) | (PTP_READ_MULTIPLE_DATA << 12) | 
					(opData->ptpPort << 8)	|
					(opData->ptpBlock << 5)	|
					(opData->ptpAddr & 0x1F));
#ifndef CONFIG_AVB_FPGA
			retVal = hwWriteGlobal2Reg(dev,QD_REG_PTP_COMMAND,data);
#else
			tmpData = (GT_U32)data;
	        retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_COMMAND,tmpData);
#endif
	        if(retVal != GT_OK)
    	    {
        	    gtSemGive(dev,dev->ptpRegsSem);
            	return retVal;
	        }

		    data = 1;
		    while(data == 1)
		    {
#ifndef CONFIG_AVB_FPGA
		        retVal = hwGetGlobal2RegField(dev,QD_REG_PTP_COMMAND,15,1,&data);
#else
        		retVal = AVB_FPGA_READ_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_COMMAND,&tmpData);
				data = (GT_U32)tmpData;
				data = (data >> 15) & 0x1;
#endif
		        if(retVal != GT_OK)
		        {
		            gtSemGive(dev,dev->ptpRegsSem);
		            return retVal;
        		}
		    }

#ifndef CONFIG_AVB_FPGA
			retVal = hwReadGlobal2Reg(dev,QD_REG_PTP_DATA,&data);
#else
	        retVal = AVB_FPGA_READ_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_DATA,&tmpData);
			data = (GT_U32)tmpData;
#endif
			opData->ptpMultiData[0] = (GT_U32)data;
    	    if(retVal != GT_OK)
   	    	{
       	    	gtSemGive(dev,dev->ptpRegsSem);
            	return retVal;
	        }

			if (!(data & 0x1))
			{
				/* valid bit is not set */
       	    	gtSemGive(dev,dev->ptpRegsSem);
            	return retVal;
			}

			for(i=1; i<opData->nData; i++)
			{
#ifndef CONFIG_AVB_FPGA
				retVal = hwReadGlobal2Reg(dev,QD_REG_PTP_DATA,&data);
#else
		        retVal = AVB_FPGA_READ_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_DATA,&tmpData);
				data = (GT_U32)tmpData;
#endif
				opData->ptpMultiData[i] = (GT_U32)data;
	    	    if(retVal != GT_OK)
    	    	{
        	    	gtSemGive(dev,dev->ptpRegsSem);
	            	return retVal;
		        }
			}

#ifndef CONFIG_AVB_FPGA
			retVal = hwWriteGlobal2Reg(dev,QD_REG_PTP_DATA,0);
#else
	        retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_DATA,0);
#endif
	        if(retVal != GT_OK)
    	    {
        	    gtSemGive(dev,dev->ptpRegsSem);
            	return retVal;
	        }

			data = (GT_U16)((1 << 15) | (PTP_WRITE_DATA << 12) | 
					(opData->ptpPort << 8)	|
					(opData->ptpBlock << 5)	|
					(opData->ptpAddr & 0x1F));
#ifndef CONFIG_AVB_FPGA
			retVal = hwWriteGlobal2Reg(dev,QD_REG_PTP_COMMAND,data);
#else
			tmpData = (GT_U32)data;
	        retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_COMMAND,tmpData);
#endif
	        if(retVal != GT_OK)
    	    {
        	    gtSemGive(dev,dev->ptpRegsSem);
            	return retVal;
	        }


		    gtSemGive(dev,dev->ptpRegsSem);
			break;

		default:
			
			gtSemGive(dev,dev->ptpRegsSem);
			return GT_FAIL;
	}

    /* Wait until the ptp is ready. */
    data = 1;
    while(data == 1)
    {
#ifndef CONFIG_AVB_FPGA
        retVal = hwGetGlobal2RegField(dev,QD_REG_PTP_COMMAND,15,1,&data);
#else
        retVal = AVB_FPGA_READ_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_COMMAND,&tmpData);
		data = (GT_U16)tmpData;
		data = (data >> 15) & 0x1;
#endif
        if(retVal != GT_OK)
        {
            gtSemGive(dev,dev->ptpRegsSem);
            return retVal;
        }
    }

    gtSemGive(dev,dev->ptpRegsSem);
    return retVal;
}


#ifdef CONFIG_AVB_FPGA


/*******************************************************************************
* gptpGetFPGAIntStatus
*
* DESCRIPTION:
*       This routine gets interrupt status of PTP logic.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		ptpInt	- PTP Int Status
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetFPGAIntStatus
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_U32		*ptpInt
)
{
	GT_STATUS       	retVal;
	GT_U32				data;

	DBG_INFO(("gptpGetPTPIntStatus Called.\n"));

	retVal = AVB_FPGA_READ_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_INT_OFFSET,&data);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	*ptpInt = (GT_U32)data & 0x1;

	DBG_INFO(("OK.\n"));
	return GT_OK;

}

/*******************************************************************************
* gptpSetFPGAIntStatus
*
* DESCRIPTION:
*       This routine sets interrupt status of PTP logic.
*
* INPUTS:
*	ptpInt	- PTP Int Status
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetFPGAIntStatus
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_U32	ptpInt
)
{
	GT_STATUS       	retVal;
	GT_U32				data;

	DBG_INFO(("gptpSetPTPIntStatus Called.\n"));

	data = ptpInt?1:0;

	retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_INT_OFFSET,ptpInt);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}

/*******************************************************************************
* gptpSetFPGAIntEn
*
* DESCRIPTION:
*       This routine enables PTP interrupt.
*
* INPUTS:
*		ptpInt	- PTP Int Status (1 to enable, 0 to disable)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetFPGAIntEn
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_U32		ptpInt
)
{
	GT_STATUS       	retVal;
	GT_U32				data;

	DBG_INFO(("gptpGetPTPIntEn Called.\n"));

	data = (ptpInt == 0)?0:1;

	retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_INTEN_OFFSET,data);
	if(retVal != GT_OK)
	{
		return retVal;
	}


	DBG_INFO(("OK.\n"));
	return GT_OK;

}

/*******************************************************************************
* gptpGetClockSource
*
* DESCRIPTION:
*       This routine gets PTP Clock source setup.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		clkSrc	- PTP clock source (A/D Device or FPGA)
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetClockSource
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_PTP_CLOCK_SRC 	*clkSrc
)
{
	GT_STATUS       	retVal;
	GT_U32				data;

	DBG_INFO(("gptpGetClockSource Called.\n"));

	retVal = AVB_FPGA_READ_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_CLK_SRC_OFFSET,&data);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	*clkSrc = (GT_PTP_CLOCK_SRC)(data & 0x1);

	DBG_INFO(("OK.\n"));
	return GT_OK;

}

/*******************************************************************************
* gptpSetClockSource
*
* DESCRIPTION:
*       This routine sets PTP Clock source setup.
*
* INPUTS:
*		clkSrc	- PTP clock source (A/D Device or FPGA)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetClockSource
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_PTP_CLOCK_SRC 	clkSrc
)
{
	GT_STATUS       	retVal;
	GT_U32				data;

	DBG_INFO(("gptpSetClockSource Called.\n"));

	data = (GT_U32)clkSrc;

	retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_CLK_SRC_OFFSET,data);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;

}

/*******************************************************************************
* gptpGetP9Mode
*
* DESCRIPTION:
*       This routine gets Port 9 Mode.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		mode - Port 9 mode (GT_PTP_P9_MODE enum type)
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetP9Mode
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_PTP_P9_MODE 	*mode
)
{
	GT_STATUS       	retVal;
	GT_U32				data;

	DBG_INFO(("gptpGetP9Mode Called.\n"));

	retVal = AVB_FPGA_READ_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_P9_MODE_OFFSET,&data);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	if (data & 0x1)
	{
		switch (data & 0x6)
		{
			case 0:
				*mode = PTP_P9_MODE_GMII;
				break;
			case 2:
				*mode = PTP_P9_MODE_MII;
				break;
			case 4:
				*mode = PTP_P9_MODE_MII_CONNECTOR;
				break;
			default:
				return GT_BAD_PARAM;
		}
	}
	else
	{
		*mode = PTP_P9_MODE_JUMPER;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gptpSetP9Mode
*
* DESCRIPTION:
*       This routine sets Port 9 Mode.
*
* INPUTS:
*		mode - Port 9 mode (GT_PTP_P9_MODE enum type)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetP9Mode
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_PTP_P9_MODE 	mode
)
{
	GT_STATUS       	retVal;
	GT_U32				data;

	DBG_INFO(("gptpSetP9Mode Called.\n"));

	switch (mode)
	{
		case PTP_P9_MODE_GMII:
			data = 1;
			break;
		case PTP_P9_MODE_MII:
			data = 3;
			break;
		case PTP_P9_MODE_MII_CONNECTOR:
			data = 5;
			break;
		case PTP_P9_MODE_JUMPER:
			data = 0;
			break;
		default:
			return GT_BAD_PARAM;
	}

	retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_P9_MODE_OFFSET,data);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;

}

/*******************************************************************************
* gptpReset
*
* DESCRIPTION:
*       This routine performs software reset for PTP logic.
*
* INPUTS:
*		None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpReset
(
	IN  GT_QD_DEV 	*dev
)
{
	GT_STATUS       	retVal;
	GT_U32				data;

	DBG_INFO(("gptpReset Called.\n"));

	data = 1;

	retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_RESET_OFFSET,data);
	if(retVal != GT_OK)
	{
		return retVal;
	}


	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gptpGetCycleAdjustEn
*
* DESCRIPTION:
*       This routine checks if PTP Duty Cycle Adjustment is enabled.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		adjEn	- GT_TRUE if enabled, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetCycleAdjustEn
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_BOOL		*adjEn
)
{
	GT_STATUS       	retVal;
	GT_U32				data;

	DBG_INFO(("gptpGetCycleAdjustEn Called.\n"));

	retVal = AVB_FPGA_READ_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_CLK_CTRL_OFFSET,&data);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	*adjEn = (data & 0x2)?GT_TRUE:GT_FALSE;

	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gptpSetCycleAdjustEn
*
* DESCRIPTION:
*       This routine enables/disables PTP Duty Cycle Adjustment.
*
* INPUTS:
*		adjEn	- GT_TRUE to enable, GT_FALSE to disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetCycleAdjustEn
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_BOOL		adjEn
)
{
	GT_STATUS       	retVal;
	GT_U32				data;

	DBG_INFO(("gptpGetCycleAdjustEn Called.\n"));

	retVal = AVB_FPGA_READ_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_CLK_CTRL_OFFSET,&data);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	if (adjEn == GT_FALSE)
		data &= ~0x3;	/* clear both Enable bit and Valid bit */
	else
		data |= 0x2;

	retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_CLK_CTRL_OFFSET,data);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gptpGetCycleAdjust
*
* DESCRIPTION:
*       This routine gets clock duty cycle adjustment value.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		adj	- adjustment value (GT_PTP_CLOCK_ADJUSTMENT structure)
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetCycleAdjust
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_PTP_CLOCK_ADJUSTMENT	*adj
)
{
	GT_STATUS       	retVal;
	GT_U32				data;

	DBG_INFO(("gptpGetCycleAdjust Called.\n"));

	retVal = AVB_FPGA_READ_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_CLK_CTRL_OFFSET,&data);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	adj->adjSign = (data & 0x4)?GT_PTP_SIGN_PLUS:GT_PTP_SIGN_NEGATIVE;
	adj->cycleStep = (data >> 3) & 0x7;

	retVal = AVB_FPGA_READ_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_CYCLE_INTERVAL_OFFSET,&data);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	adj->cycleInterval = data;

	retVal = AVB_FPGA_READ_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_CYCLE_ADJ_OFFSET,&data);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	adj->cycleAdjust = data;

	DBG_INFO(("OK.\n"));
	return GT_OK;

}

/*******************************************************************************
* gptpSetCycleAdjust
*
* DESCRIPTION:
*       This routine sets clock duty cycle adjustment value.
*
* INPUTS:
*		adj	- adjustment value (GT_PTP_CLOCK_ADJUSTMENT structure)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetCycleAdjust
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_PTP_CLOCK_ADJUSTMENT	*adj
)
{
	GT_STATUS       	retVal;
	GT_U32				data, data1;

	DBG_INFO(("gptpSetCycleAdjust Called.\n"));

	retVal = AVB_FPGA_READ_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_CLK_CTRL_OFFSET,&data);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	data &= ~0x1;	/* clear Valid bit */
	retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_CLK_CTRL_OFFSET,data);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	/* Setup the Cycle Interval */
	data1 = adj->cycleInterval & 0xFFFF;

	retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_CYCLE_INTERVAL_OFFSET,data1);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	/* Setup the Cycle Adjustment */
	data1 = adj->cycleAdjust & 0xFFFF;

	retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_CYCLE_ADJ_OFFSET,data1);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	/* clear Sign bit and Cycle Step bits on QD_REG_PTP_CLK_CTRL_OFFSET value */
	data &= ~0x3C;

	switch (adj->adjSign)
	{
		case GT_PTP_SIGN_PLUS:
			data |= 0x4;
			break;
			
		case GT_PTP_SIGN_NEGATIVE:
			break;

		default:
			return GT_BAD_PARAM;
	}

	data |= ((adj->cycleStep & 0x7) << 3);	/* setup Step bits */
	data |= 0x1;							/* set Valid bit */

	retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_CLK_CTRL_OFFSET,data);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gptpGetPLLEn
*
* DESCRIPTION:
*       This routine checks if PLL is enabled.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		en		- GT_TRUE if enabled, GT_FALSE otherwise
*		freqSel	- PLL Frequency Selection (default 0x3 - 22.368MHz)
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       PLL Frequence selection is based on the Clock Recovery PLL device.
*		IDT MK1575-01 is the default PLL device.
*
*******************************************************************************/
GT_STATUS gptpGetPLLEn
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_BOOL		*en,
	OUT GT_U32		*freqSel
)
{
	GT_STATUS       	retVal;
	GT_U32				data;

	DBG_INFO(("gptpGetPLLEn Called.\n"));

	retVal = AVB_FPGA_READ_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_PLL_CTRL_OFFSET,&data);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	*en = (data & 0x1)?GT_TRUE:GT_FALSE;

	*freqSel = (data >> 1) & 0x7;

	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gptpSetPLLEn
*
* DESCRIPTION:
*       This routine enables/disables PLL device.
*
* INPUTS:
*		en		- GT_TRUE to enable, GT_FALSE to disable
*		freqSel	- PLL Frequency Selection (default 0x3 - 22.368MHz)
*				  Meaningful only when enabling PLL device
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       PLL Frequence selection is based on the Clock Recovery PLL device.
*		IDT MK1575-01 is the default PLL device.
*
*******************************************************************************/
GT_STATUS gptpSetPLLEn
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_BOOL		en,
	IN  GT_U32		freqSel
)
{
	GT_STATUS       	retVal;
	GT_U32				data;

	DBG_INFO(("gptpSetPPLEn Called.\n"));

	retVal = AVB_FPGA_READ_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_PLL_CTRL_OFFSET,&data);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	if(en == GT_FALSE)
	{
		data |= 0x1;
	}
	else
	{
		data &= ~0x1;
		data |= (freqSel & 0x7) << 1;
	}

	retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,QD_REG_PTP_PLL_CTRL_OFFSET,data);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gptpGetDDSReg
*
* DESCRIPTION:
*       This routine gets DDS register data.
*
* INPUTS:
*	ddsReg	- DDS Register
*
* OUTPUTS:
*	ddsData	- register data
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetDDSReg
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_U32	ddsReg,
	OUT GT_U32	*ddsData
)
{
	GT_STATUS       	retVal;
	GT_U32			data;
	GT_U32			timeout = 0x100000;

	DBG_INFO(("gptpGetDDSReg Called.\n"));

	if (ddsReg > 0x3f)
		return GT_BAD_PARAM;
	do 
	{
		retVal = AVB_FPGA_READ_REG(dev,AVB_SMI_ADDR,12,&data);
		if (retVal != GT_OK)
			return retVal;
		timeout--;
		if (timeout == 0)
			return GT_FAIL;
	} while (data & 0x8000);

	data = 0x8000 | 0x4000 | (ddsReg << 8);
	retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,12,data);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	retVal = AVB_FPGA_READ_REG(dev,AVB_SMI_ADDR,12,&data);
	if (retVal != GT_OK)
		return retVal;

	*ddsData = data & 0xFF;

	retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,12,0);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}

/*******************************************************************************
* gptpSetDDSReg
*
* DESCRIPTION:
*       This routine sets DDS register data.
*	DDS register data written by this API are not affected until gptpUpdateDDSReg API is called.
*
* INPUTS:
*	ddsReg	- DDS Register
*	ddsData	- register data
*
* OUTPUTS:
*	none
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetDDSReg
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_U32	ddsReg,
	IN  GT_U32	ddsData
)
{
	GT_STATUS       	retVal;
	GT_U32			data;
	GT_U32			timeout = 0x100000;

	DBG_INFO(("gptpSetDDSReg Called.\n"));

	if ((ddsReg > 0x3f) || (ddsData > 0xff))
		return GT_BAD_PARAM;

	do 
	{
		retVal = AVB_FPGA_READ_REG(dev,AVB_SMI_ADDR,12,&data);
		if (retVal != GT_OK)
			return retVal;
		timeout--;
		if (timeout == 0)
			return GT_FAIL;
	} while (data & 0x8000);

	data = 0x8000 | (ddsReg << 8) | (ddsData);
	retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,12,data);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,12,0);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}

/*******************************************************************************
* gptpUpdateDDSReg
*
* DESCRIPTION:
*       This routine updates DDS register data.
*	DDS register data written by gptpSetDDSReg are not affected until this API is called.
*
* INPUTS:
*	none
*
* OUTPUTS:
*	none
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpUpdateDDSReg
(
	IN  GT_QD_DEV 	*dev
)
{
	GT_STATUS       	retVal;

	DBG_INFO(("gptpUpdateDDSReg Called.\n"));

	retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,13,0x0);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,13,0x1);
	if(retVal != GT_OK)
	{
		return retVal;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}

/*******************************************************************************
* gptpSetADFReg
*
* DESCRIPTION:
*       This routine sets ADF4156 register data.
*
* INPUTS:
*	adfData	- register data
*
* OUTPUTS:
*	none
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetADFReg
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_U32	adfData
)
{
	GT_STATUS       	retVal;

	DBG_INFO(("gptpSetADFReg Called.\n"));

	retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,14,(adfData & 0xFFFF));
	if(retVal != GT_OK)
	{
		return retVal;
	}

	retVal = AVB_FPGA_WRITE_REG(dev,AVB_SMI_ADDR,15,((adfData>>16) & 0xFFFF));
	if(retVal != GT_OK)
	{
		return retVal;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}

#endif


/******************************************************************************
*
*
*******************************************************************************/
/* Amber QAV API */

/*******************************************************************************
* gqavSetPortQpriXQTSToken
*
* DESCRIPTION:
*       This routine set Priority Queue 0-3 time slot tokens on a port.
*		The setting value is number of tokens that need to be subtracted at each 
*		QTS interval boundary.
*
* INPUTS:
*		queue	 - 0 - 3
*		qtsToken - number of tokens.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_BAD_PARAM - if input parameters are beyond range.
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavSetPortQpriXQTSToken
(
	IN  GT_QD_DEV 	*dev,
	IN	GT_LPORT	port,
	IN  GT_U8		queue,
	IN  GT_U16		qtsToken
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U32			hwPort;

	DBG_INFO(("gqavSetPortQpriXQTSToken Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (GT_U32)GT_LPORT_2_PORT(port);

    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_QAV))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }

    /* check if queue is beyond range */
	if (queue>0x3)
    {
        DBG_INFO(("GT_BAD_PARAM\n"));
		return GT_BAD_PARAM;
    }

    /* check if qtsToken is beyond range */
	if (qtsToken>0x7fff)
    {
        DBG_INFO(("GT_BAD_PARAM\n"));
		return GT_BAD_PARAM;
    }

	opData.ptpBlock = 0x2;	/* QAV register space */
	opData.ptpAddr = queue*2;

	opData.ptpPort = hwPort;

	op = PTP_WRITE_DATA;

	opData.ptpData = qtsToken&0x7fff;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing QTS token for port %d queue %d.\n", port, queue));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}

/*******************************************************************************
* gqavGetPortQpriXQTSToken
*
* DESCRIPTION:
*       This routine get Priority Queue 0-3 time slot tokens on a port.
*		The setting value is number of tokens that need to be subtracted at each 
*		QTS interval boundary.
*
* INPUTS:
*		queue - 0 - 3
*
* OUTPUTS:
*		qtsToken - number of tokens
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_BAD_PARAM - if input parameters are beyond range.
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavGetPortQpriXQTSToken
(
	IN  GT_QD_DEV 	*dev,
	IN	GT_LPORT	port,
	IN  GT_U8		queue,
	OUT GT_U16		*qtsToken
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U32			hwPort;

	DBG_INFO(("gqavGetPortQpriXQTSToken Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (GT_U32)GT_LPORT_2_PORT(port);

    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_QAV))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }

    /* check if queue is beyond range */
	if (queue>0x3)
    {
        DBG_INFO(("GT_BAD_PARAM\n"));
		return GT_BAD_PARAM;
    }

	opData.ptpBlock = 0x2;	/* QAV register space */

	opData.ptpAddr = queue*2;
	opData.ptpPort = hwPort;

	op = PTP_READ_DATA;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading QTS token for port %d queue %d.\n", port, queue));
		return GT_FAIL;
	}

	*qtsToken =	(GT_U16)(opData.ptpData&0x7fff);

	DBG_INFO(("OK.\n"));
	return GT_OK;

}


/*******************************************************************************
* gqavSetPortQpriXBurstBytes
*
* DESCRIPTION:
*       This routine set Priority Queue 0-3 Burst Bytes on a port.
*		This value specifies the number of credits in bytes that can be 
*		accumulated when the queue is blocked from sending out a frame due to 
*		higher priority queue frames being sent out.
*
* INPUTS:
*		queue - 0 - 3
*		burst - number of credits in bytes .
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_BAD_PARAM - if input parameters are beyond range.
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavSetPortQpriXBurstBytes
(
	IN  GT_QD_DEV 	*dev,
	IN	GT_LPORT	port,
	IN  GT_U8		queue,
	IN  GT_U16		burst
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U32			hwPort;

	DBG_INFO(("gqavSetPortQpriXBurstBytes Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (GT_U32)GT_LPORT_2_PORT(port);

    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_QAV))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }

    /* check if queue is beyond range */
	if (queue>0x3)
    {
        DBG_INFO(("GT_BAD_PARAM\n"));
		return GT_BAD_PARAM;
    }

    /* check if burst is beyond range */
	if (burst>0x7fff)
    {
        DBG_INFO(("GT_BAD_PARAM\n"));
		return GT_BAD_PARAM;
    }

	opData.ptpBlock = 0x2;	/* QAV register space */
	opData.ptpAddr = queue*2+1;

	opData.ptpPort = hwPort;

	op = PTP_WRITE_DATA;

	opData.ptpData = burst&0x7fff;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing Burst bytes for port %d queue %d.\n", port, queue));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}

/*******************************************************************************
* gqavGetPortQpriXBurstBytes
*
* DESCRIPTION:
*       This routine get Priority Queue 0-3 Burst Bytes on a port.
*		This value specifies the number of credits in bytes that can be 
*		accumulated when the queue is blocked from sending out a frame due to 
*		higher priority queue frames being sent out.
*
* INPUTS:
*		queue - 0 - 3
*
* OUTPUTS:
*		burst - number of credits in bytes .
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_BAD_PARAM - if input parameters are beyond range.
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavGetPortQpriXBurstBytes
(
	IN  GT_QD_DEV 	*dev,
	IN	GT_LPORT	port,
	IN  GT_U8		queue,
	OUT GT_U16		*burst
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U32			hwPort;

	DBG_INFO(("gqavgetPortQpriXBurstBytes Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (GT_U32)GT_LPORT_2_PORT(port);

    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_QAV))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }

    /* check if queue is beyond range */
	if (queue>0x3)
    {
        DBG_INFO(("GT_BAD_PARAM\n"));
		return GT_BAD_PARAM;
    }

	opData.ptpBlock = 0x2;	/* QAV register space */

	opData.ptpAddr = queue*2+1;
	opData.ptpPort = hwPort;

	op = PTP_READ_DATA;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading Burst bytes for port %d queue %d.\n", port, queue));
		return GT_FAIL;
	}

	*burst = (GT_U16)opData.ptpData&0x7fff;

	DBG_INFO(("OK.\n"));
	return GT_OK;

}

/*******************************************************************************
* gqavSetPortQavEnable
*
* DESCRIPTION:
*       This routine set QAV enable status on a port.
*
* INPUTS:
*		en - GT_TRUE: QAV enable, GT_FALSE: QAV disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_BAD_PARAM - if input parameters are beyond range.
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavSetPortQavEnable
(
	IN  GT_QD_DEV 	*dev,
	IN	GT_LPORT	port,
	IN  GT_BOOL		en
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U32			hwPort;

	DBG_INFO(("gqavSetPortQavEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (GT_U32)GT_LPORT_2_PORT(port);

    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_QAV))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }

	opData.ptpBlock = 0x2;	/* QAV register space */
	opData.ptpAddr = 8;

	opData.ptpPort = hwPort;

	op = PTP_WRITE_DATA;

	opData.ptpData = (en==GT_TRUE)?0x8000:0;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing QAV enable for port %d.\n", port));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}

/*******************************************************************************
* gqavGetPortQavEnable
*
* DESCRIPTION:
*       This routine get QAV enable status on a port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		en - GT_TRUE: QAV enable, GT_FALSE: QAV disable
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_BAD_PARAM - if input parameters are beyond range.
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavGetPortQavEnable
(
	IN  GT_QD_DEV 	*dev,
	IN	GT_LPORT	port,
	OUT GT_BOOL		*en
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;
	GT_U32			hwPort;

	DBG_INFO(("gqavGetPortQavEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (GT_U32)GT_LPORT_2_PORT(port);

    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_QAV))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }

	opData.ptpBlock = 0x2;	/* QAV register space */
	opData.ptpAddr = 8;

	opData.ptpPort = hwPort;

	op = PTP_READ_DATA;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed reading QAV enable for port %d.\n", port));
		return GT_FAIL;
	}

	*en = ((opData.ptpData&0x8000)==0)?GT_FALSE:GT_TRUE;

	DBG_INFO(("OK.\n"));
	return GT_OK;
}

/*******************************************************************************/
/* QAV Global resters processing */
/*******************************************************************************
* gqavSetGlobalAdminMGMT
*
* DESCRIPTION:
*       This routine set to accept Admit Management Frames always.
*
* INPUTS:
*		en - GT_TRUE to set MGMT frame accepted always, 
*			 GT_FALSE do not set MGMT frame accepted always
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_BAD_PARAM - if input parameters are beyond range.
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavSetGlobalAdminMGMT
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_BOOL		en
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gqavSetGlobalAdminMGMT Called.\n"));

    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_QAV))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }

	opData.ptpBlock = 0x2;	/* QAV register space */
	opData.ptpAddr = 0;

	opData.ptpPort = 0xF;

	op = PTP_READ_DATA;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading QAV global config admin MGMT.\n"));
		return GT_FAIL;
	}

	op = PTP_WRITE_DATA;

	opData.ptpData &= ~0x8000;
	if (en)
		opData.ptpData |= 0x8000;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing QAV global config admin MGMT.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}

/*******************************************************************************
* gqavGetGlobalAdminMGMT
*
* DESCRIPTION:
*       This routine get setting of Admit Management Frames always.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		en - GT_TRUE to set MGMT frame accepted always, 
*			 GT_FALSE do not set MGMT frame accepted always
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_BAD_PARAM - if input parameters are beyond range.
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavGetGlobalAdminMGMT
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_BOOL		*en
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gqavGetGlobalAdminMGMT Called.\n"));

    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_QAV))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }

	opData.ptpBlock = 0x2;	/* QAV register space */
	opData.ptpAddr = 0;

	opData.ptpPort = 0xF;

	op = PTP_READ_DATA;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading QAV global config admin MGMT.\n"));
		return GT_FAIL;
	}

	if (opData.ptpData&0x8000)
	  *en = GT_TRUE;
	else
	  *en = GT_FALSE;

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gqavSetGlobalIsoPtrThreshold
*
* DESCRIPTION:
*       This routine set Global Isochronous Queue Pointer Threshold.
*		This field indicates the total number of isochronous pointers 
*		that are reserved for isochronous streams. The value is expected to be 
*		computed in SRP software and programmed into hardware based on the total 
*		aggregate isochronous streams configured to go through this device..
*
* INPUTS:
*		isoPtrs -  total number of isochronous pointers 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_BAD_PARAM - if input parameters are beyond range.
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavSetGlobalIsoPtrThreshold
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_U16		isoPtrs
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gqavSetGlobalIsoPtrThreshold Called.\n"));

    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_QAV))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }

    /* check if isoPtrs is beyond range */
	if (isoPtrs>0x3ff)
    {
        DBG_INFO(("GT_BAD_PARAM\n"));
		return GT_BAD_PARAM;
    }

	opData.ptpBlock = 0x2;	/* QAV register space */
	opData.ptpAddr = 0;

	opData.ptpPort = 0xF;

	op = PTP_READ_DATA;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading QAV global config Isochronous Queue Pointer Threshold.\n"));
		return GT_FAIL;
	}

	op = PTP_WRITE_DATA;

	opData.ptpData &= ~0x3ff;
	opData.ptpData |= isoPtrs;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing QAV global config Isochronous Queue Pointer Threshold.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gqavGetGlobalIsoPtrThreshold
*
* DESCRIPTION:
*       This routine get Global Isochronous Queue Pointer Threshold.
*		This field indicates the total number of isochronous pointers 
*		that are reserved for isochronous streams. The value is expected to be 
*		computed in SRP software and programmed into hardware based on the total 
*		aggregate isochronous streams configured to go through this device..
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		isoPtrs -  total number of isochronous pointers 
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_BAD_PARAM - if input parameters are beyond range.
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavGetGlobalIsoPtrThreshold
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_U16		*isoPtrs
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gqavGetGlobalIsoPtrThreshold Called.\n"));

    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_QAV))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }

	opData.ptpBlock = 0x2;	/* QAV register space */
	opData.ptpAddr = 0;

	opData.ptpPort = 0xF;

	op = PTP_READ_DATA;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading Isochronous Queue Pointer Threshold.\n"));
		return GT_FAIL;
	}

	*isoPtrs = (GT_U16)opData.ptpData&0x3ff;

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gqavSetGlobalDisQSD4MGMT
*
* DESCRIPTION:
*       This routine set Disable Queue Scheduler Delays for Management frames..
*
* INPUTS:
*		en - GT_TRUE, it indicates to the Queue Controller to disable applying Queue
*		Scheduler Delays and the corresponding rate regulator does not account
*		for MGMT frames through this queue.
*			 GT_FALSE, the MGMT frames follow similar rate regulation and delay
*		regulation envelope as specified for the isochronous queue that the
*		MGMT frames are sharing with.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavSetGlobalDisQSD4MGMT
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_BOOL		en
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gqavSetGlobalDisQSD4MGMT Called.\n"));

    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_QAV))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }

	opData.ptpBlock = 0x2;	/* QAV register space */
	opData.ptpAddr = 3;

	opData.ptpPort = 0xF;

	op = PTP_READ_DATA;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading Disable Queue Scheduler Delay for MGMT frames.\n"));
		return GT_FAIL;
	}

	op = PTP_WRITE_DATA;

	opData.ptpData &= ~0x4000;
	if (en==GT_TRUE)
		opData.ptpData |= 0x4000;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing Disable Queue Scheduler Delay for MGMT frames.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}

/*******************************************************************************
* gqavGetGlobalDisQSD4MGMT
*
* DESCRIPTION:
*       This routine Get Disable Queue Scheduler Delays for Management frames..
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		en - GT_TRUE, it indicates to the Queue Controller to disable applying Queue
*		Scheduler Delays and the corresponding rate regulator does not account
*		for MGMT frames through this queue.
*			 GT_FALSE, the MGMT frames follow similar rate regulation and delay
*		regulation envelope as specified for the isochronous queue that the
*		MGMT frames are sharing with.
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavGetGlobalDisQSD4MGMT
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_BOOL		*en
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gqavGetGlobalDisQSD4MGMT Called.\n"));

    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_QAV))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }

	opData.ptpBlock = 0x2;	/* QAV register space */
	opData.ptpAddr = 3;

	opData.ptpPort = 0xF;

	op = PTP_READ_DATA;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading Disable Queue Scheduler Delay for MGMT frames.\n"));
		return GT_FAIL;
	}

	if (opData.ptpData&0x4000)
	  *en = GT_TRUE;
	else
	  *en = GT_FALSE;

	DBG_INFO(("OK.\n"));
	return GT_OK;
}



/*******************************************************************************
* gqavSetGlobalInterrupt
*
* DESCRIPTION:
*       This routine set QAV interrupt enable, The QAV interrypts include:
*		 [GT_QAV_INT_ENABLE_ENQ_LMT_BIT]      # EnQ Limit Interrupt Enable 
*		 [GT_QAV_INT_ENABLE_ISO_DEL_BIT]      # Iso Delay Interrupt Enable   
*		 [GT_QAV_INT_ENABLE_ISO_DIS_BIT]      # Iso Discard Interrupt Enable 
*		 [GT_QAV_INT_ENABLE_ISO_LIMIT_EX_BIT]   # Iso Packet Memory Exceeded 
*												  Interrupt Enable 
*
* INPUTS:
*		intEn - [GT_QAV_INT_ENABLE_ENQ_LMT_BIT] OR  
*				[GT_QAV_INT_ENABLE_ISO_DEL_BIT] OR    
*				[GT_QAV_INT_ENABLE_ISO_DIS_BIT] OR  
*				[GT_QAV_INT_ENABLE_ISO_LIMIT_EX_BIT]   
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_BAD_PARAM - if input parameters are beyond range.
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavSetGlobalInterrupt
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_U16		intEn
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gqavSetGlobalInterrupt Called.\n"));

    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_QAV))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }

	opData.ptpBlock = 0x2;	/* QAV register space */
	opData.ptpAddr = 8;

	opData.ptpPort = 0xF;

	op = PTP_READ_DATA;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading Interrupt enable status.\n"));
		return GT_FAIL;
	}

	op = PTP_WRITE_DATA;

	opData.ptpData &= ~0xff;
	opData.ptpData |= (intEn&0xff);

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing Interrupt enable status.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}

/*******************************************************************************
* gqavGetGlobalInterrupt
*
* DESCRIPTION:
*       This routine get QAV interrupt enable, The QAV interrypts include:
*		 [GT_QAV_INT_STATUS_ISO_DEL_BIT]      # Iso Delay Interrupt Status   
*		 [GT_QAV_INT_STATUS_ISO_DIS_BIT]      # Iso Discard Interrupt Status 
*		 [GT_QAV_INT_STATUS_ISO_LIMIT_EX_BIT]   # Iso Packet Memory Exceeded 
*												  Interrupt Status 
*		 [GT_QAV_INT_ENABLE_ENQ_LMT_BIT]      # EnQ Limit Interrupt Enable 
*		 [GT_QAV_INT_ENABLE_ISO_DEL_BIT]      # Iso Delay Interrupt Enable   
*		 [GT_QAV_INT_ENABLE_ISO_DIS_BIT]      # Iso Discard Interrupt Enable 
*		 [GT_QAV_INT_ENABLE_ISO_LIMIT_EX_BIT]   # Iso Packet Memory Exceeded 
*												  Interrupt Enable 
*
* INPUTS:
*       None.
*
* OUTPUTS:
* INPUTS:
*		intEn - [GT_QAV_INT_STATUS_ISO_DEL_BIT] OR     
*				[GT_QAV_INT_STATUS_ISO_DIS_BIT] OR   
*				[GT_QAV_INT_STATUS_ISO_LIMIT_EX_BIT] OR
*				[GT_QAV_INT_ENABLE_ENQ_LMT_BIT] OR  
*				[GT_QAV_INT_ENABLE_ISO_DEL_BIT] OR    
*				[GT_QAV_INT_ENABLE_ISO_DIS_BIT] OR  
*				[GT_QAV_INT_ENABLE_ISO_LIMIT_EX_BIT]   
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_BAD_PARAM - if input parameters are beyond range.
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavGetGlobalInterrupt
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_U16		*intSt
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gqavGetGlobalInterrupt Called.\n"));

    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_QAV))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }

	opData.ptpBlock = 0x2;	/* QAV register space */
	opData.ptpAddr = 8;

	opData.ptpPort = 0xF;

	op = PTP_READ_DATA;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading Interrupt status.\n"));
		return GT_FAIL;
	}

	*intSt = (GT_U16)opData.ptpData;

	DBG_INFO(("OK.\n"));
	return GT_OK;
}

/*******************************************************************************
* gqavGetGlobalIsoInterruptPort
*
* DESCRIPTION:
*       This routine get Isochronous interrupt port.
*		This field indicates the port number for IsoDisInt or IsoLimitExInt 
*		bits. Only one such interrupt condition can be detected by hardware at one 
*		time. Once an interrupt bit has been set along with the IsoIntPort, the 
*		software would have to come and clear the bits before hardware records 
*		another interrupt event.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		port - port number for IsoDisInt or IsoLimitExInt bits.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_BAD_PARAM - if input parameters are beyond range.
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavGetGlobalIsoInterruptPort
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_U8		*port
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gqavGetGlobalIsoInterruptPort Called.\n"));

    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_QAV))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }

	opData.ptpBlock = 0x2;	/* QAV register space */
	opData.ptpAddr = 9;

	opData.ptpPort = 0xF;

	op = PTP_READ_DATA;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading Isochronous interrupt port..\n"));
		return GT_FAIL;
	}

	*port = (GT_U8)opData.ptpData&0xf;

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gqavSetGlobalIsoDelayLmt
*
* DESCRIPTION:
*       This routine set Isochronous queue delay Limit
*		This field represents a per-port isochronous delay limit that 
*		will be checked by the queue controller logic to ensure no isochronous 
*		packets suffer more than this delay w.r.t to their eligibility time slot.
*		This represents the number of Queue Time Slots. The interval for the QTS 
*		can be configured using the register in Qav Global Configuration, Offset 0x2.
*
* INPUTS:
*		limit - per-port isochronous delay limit.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_BAD_PARAM - if input parameters are beyond range.
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavSetGlobalIsoDelayLmt
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_U8		limit
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gqavSetGlobalIsoDelayLmt Called.\n"));

    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_QAV))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }

	opData.ptpBlock = 0x2;	/* QAV register space */
	opData.ptpAddr = 10;

	opData.ptpPort = 0xF;

	op = PTP_READ_DATA;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading Isochronous queue delay Limit.\n"));
		return GT_FAIL;
	}

	op = PTP_WRITE_DATA;

	opData.ptpData &= ~0xff;
	opData.ptpData |= (limit&0xff);

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing Isochronous queue delay Limit.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}

/*******************************************************************************
* gqavGetGlobalIsoDelayLmt
*
* DESCRIPTION:
*       This routine get Isochronous queue delay Limit
*		This field represents a per-port isochronous delay limit that 
*		will be checked by the queue controller logic to ensure no isochronous 
*		packets suffer more than this delay w.r.t to their eligibility time slot.
*		This represents the number of Queue Time Slots. The interval for the QTS 
*		can be configured using the register in Qav Global Configuration, Offset 0x2.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		limit - per-port isochronous delay limit.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_BAD_PARAM - if input parameters are beyond range.
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavGetGlobalIsoDelayLmt
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_U8		*limit
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gqavGetGlobalIsoDelayLmt Called.\n"));

    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_QAV))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }

	opData.ptpBlock = 0x2;	/* QAV register space */
	opData.ptpAddr = 10;

	opData.ptpPort = 0xF;

	op = PTP_READ_DATA;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading Isochronous queue delay Limit.\n"));
		return GT_FAIL;
	}

	*limit = (GT_U8)(opData.ptpData)&0xff;

	DBG_INFO(("OK.\n"));
	return GT_OK;
}

/*******************************************************************************
* gqavSetGlobalIsoMonEn
*
* DESCRIPTION:
*       This routine set Isochronous monitor enable
*		Set 1: this bit enables the statistics gathering capabilities stated
*		in PTP Global Status Registers Offset 0xD, 0xE and 0xF. Once enabled, the
*		software is expected to program the IsoMonPort (PTP Global Status Offset
*		0xD) indicating which port of the device does the software wants to monitor.
*		Upon setting this bit, the hardware collects IsoHiDisCtr, IsoLoDisCtr and
*		IsoSchMissCtr values for the port indicated by IsoMonPort till this bit is 
*		set to a zero.
*		Set 0: this bit disables the statistics gathering capabilities.
*
* INPUTS:
*		en - [1]/[0].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_BAD_PARAM - if input parameters are beyond range.
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavSetGlobalIsoMonEn
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_BOOL		en
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gqavSetGlobalIsoMonEn Called.\n"));

    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_QAV))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }

	opData.ptpBlock = 0x2;	/* QAV register space */
	opData.ptpAddr = 12;

	opData.ptpPort = 0xF;

	op = PTP_READ_DATA;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading Isochronous monitor enable.\n"));
		return GT_FAIL;
	}

	op = PTP_WRITE_DATA;

	opData.ptpData &= ~0x8000;
	if (en)
		opData.ptpData |= 0x8000;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing Isochronous monitor enable.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}

/*******************************************************************************
* gqavGetGlobalIsoMonEn
*
* DESCRIPTION:
*       This routine get Isochronous monitor enable
*		Set 1: this bit enables the statistics gathering capabilities stated
*		in PTP Global Status Registers Offset 0xD, 0xE and 0xF. Once enabled, the
*		software is expected to program the IsoMonPort (PTP Global Status Offset
*		0xD) indicating which port of the device does the software wants to monitor.
*		Upon setting this bit, the hardware collects IsoHiDisCtr, IsoLoDisCtr and
*		IsoSchMissCtr values for the port indicated by IsoMonPort till this bit is 
*		set to a zero.
*		Set 0: this bit disables the statistics gathering capabilities.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		en - [1]/[0].

*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_BAD_PARAM - if input parameters are beyond range.
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavGetGlobalIsoMonEn
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_BOOL		*en
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gqavGetGlobalIsoMonEn Called.\n"));

    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_QAV))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }

	opData.ptpBlock = 0x2;	/* QAV register space */
	opData.ptpAddr = 12;

	opData.ptpPort = 0xF;

	op = PTP_READ_DATA;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading Isochronous monitor enable.\n"));
		return GT_FAIL;
	}

	if (opData.ptpData&0x8000)
	  *en = 1;
	else
	  *en = 0;

	DBG_INFO(("OK.\n"));
	return GT_OK;
}

/*******************************************************************************
* gqavSetGlobalIsoMonPort
*
* DESCRIPTION:
*       This routine set Isochronous monitoring port.
*		This field is updated by software along with Iso Mon En bit 
*		(Qav Global Status, offset 0xD) and it indicates the port number that 
*		the software wants the hardware to start monitoring i.e., start updating 
*		IsoHiDisCtr, IsoLoDisCtr and IsoSchMissCtr. The queue controller clears 
*		the above stats when IsoMonPort is changed..
*
* INPUTS:
*		port -  port number .
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_BAD_PARAM - if input parameters are beyond range.
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavSetGlobalIsoMonPort
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_U16		port
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gqavSetGlobalIsoMonPort Called.\n"));

    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_QAV))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }

    /* check if port is beyond range */
	if (port>0xf)
    {
        DBG_INFO(("GT_BAD_PARAM\n"));
		return GT_BAD_PARAM;
    }

	opData.ptpBlock = 0x2;	/* QAV register space */
	opData.ptpAddr = 12;

	opData.ptpPort = 0xF;

	op = PTP_READ_DATA;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading Isochronous monitoring port.\n"));
		return GT_FAIL;
	}

	op = PTP_WRITE_DATA;

	opData.ptpData &= ~0xf;
	opData.ptpData |= port;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing Isochronous monitoring port.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gqavGetGlobalIsoMonPort
*
* DESCRIPTION:
*       This routine get Isochronous monitoring port.
*		This field is updated by software along with Iso Mon En bit 
*		(Qav Global Status, offset 0xD) and it indicates the port number that 
*		the software wants the hardware to start monitoring i.e., start updating 
*		IsoHiDisCtr, IsoLoDisCtr and IsoSchMissCtr. The queue controller clears 
*		the above stats when IsoMonPort is changed..
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		port -  port number .
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_BAD_PARAM - if input parameters are beyond range.
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavGetGlobalIsoMonPort
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_U16		*port
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gqavGetGlobalIsoMonPort Called.\n"));

    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_QAV))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }

	opData.ptpBlock = 0x2;	/* QAV register space */
	opData.ptpAddr = 12;

	opData.ptpPort = 0xF;

	op = PTP_READ_DATA;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading Isochronous monitoring port.\n"));
		return GT_FAIL;
	}

	*port = (GT_U16)opData.ptpData&0xf;

	DBG_INFO(("OK.\n"));
	return GT_OK;
}

/*******************************************************************************
* gqavSetGlobalIsoHiDisCtr
*
* DESCRIPTION:
*       This routine set Isochronous hi queue discard counter.
*		This field is updated by hardware when instructed to do so by 
*		enabling the IsoMonEn bit in Qav Global Status Register Offset 0xD. 
*		This is an upcounter of number of isochronous hi packets discarded 
*		by Queue Controller.
*
* INPUTS:
*		disCtr - upcounter of number of isochronous hi packets discarded 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_BAD_PARAM - if input parameters are beyond range.
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavSetGlobalIsoHiDisCtr
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_U8		disCtr
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gqavSetGlobalIsoHiDisCtr Called.\n"));

    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_QAV))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }

	opData.ptpBlock = 0x2;	/* QAV register space */
	opData.ptpAddr = 13;

	opData.ptpPort = 0xF;

	op = PTP_READ_DATA;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading Isochronous hi queue discard counter..\n"));
		return GT_FAIL;
	}

	op = PTP_WRITE_DATA;

	opData.ptpData &= ~0xff00;
	if (disCtr)
		opData.ptpData |= (disCtr<<8);

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing Isochronous hi queue discard counter..\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}

/*******************************************************************************
* gqavGetGlobalIsoHiDisCtr
*
* DESCRIPTION:
*       This routine get Isochronous hi queue discard counter.
*		This field is updated by hardware when instructed to do so by 
*		enabling the IsoMonEn bit in Qav Global Status Register Offset 0xD. 
*		This is an upcounter of number of isochronous hi packets discarded 
*		by Queue Controller.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*		disCtr - upcounter of number of isochronous hi packets discarded 
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_BAD_PARAM - if input parameters are beyond range.
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavGetGlobalIsoHiDisCtr
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_U8		*disCtr
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gqavGetGlobalIsoHiDisCtr Called.\n"));

    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_QAV))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }

	opData.ptpBlock = 0x2;	/* QAV register space */
	opData.ptpAddr = 13;

	opData.ptpPort = 0xF;

	op = PTP_READ_DATA;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading Isochronous hi queue discard counter.\n"));
		return GT_FAIL;
	}

	*disCtr = (GT_U8)(opData.ptpData>>8)&0xff;

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


/*******************************************************************************
* gqavSetGlobalIsoLoDisCtr
*
* DESCRIPTION:
*       This routine set Isochronous Lo queue discard counter.
*		This field is updated by hardware when instructed to do so by 
*		enabling the IsoMonEn bit in Qav Global Status Register Offset 0xD. 
*		This is an upcounter of number of isochronous lo packets discarded 
*		by Queue Controller.
*
* INPUTS:
*		disCtr - upcounter of number of isochronous lo packets discarded 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_BAD_PARAM - if input parameters are beyond range.
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavSetGlobalIsoLoDisCtr
(
	IN  GT_QD_DEV 	*dev,
	IN  GT_U8		disCtr
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gqavSetGlobalIsoLoDisCtr Called.\n"));

    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_QAV))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }

	opData.ptpBlock = 0x2;	/* QAV register space */
	opData.ptpAddr = 13;

	opData.ptpPort = 0xF;

	op = PTP_READ_DATA;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading Isochronous lo queue discard counter.\n"));
		return GT_FAIL;
	}

	op = PTP_WRITE_DATA;

	opData.ptpData &= ~0xff;
	opData.ptpData |= (disCtr&0xff);

	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
        DBG_INFO(("Failed writing Isochronous lo queue discard counter.\n"));
		return GT_FAIL;
	}

	DBG_INFO(("OK.\n"));
	return GT_OK;
}

/*******************************************************************************
* gqavGetGlobalIsoLoDisCtr
*
* DESCRIPTION:
*       This routine set Isochronous Lo queue discard counter.
*		This field is updated by hardware when instructed to do so by 
*		enabling the IsoMonEn bit in Qav Global Status Register Offset 0xD. 
*		This is an upcounter of number of isochronous lo packets discarded 
*		by Queue Controller.
*
* INPUTS:
*		None
*
* OUTPUTS:
*		disCtr - upcounter of number of isochronous lo packets discarded 
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_BAD_PARAM - if input parameters are beyond range.
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gqavGetGlobalIsoLoDisCtr
(
	IN  GT_QD_DEV 	*dev,
	OUT GT_U8		*disCtr
)
{
	GT_STATUS       	retVal;
	GT_PTP_OPERATION	op;
	GT_PTP_OP_DATA		opData;

	DBG_INFO(("gqavGetGlobalIsoLoDisCtr Called.\n"));

    /* check if device supports this feature */
	if (!IS_IN_DEV_GROUP(dev,DEV_QAV))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
		return GT_NOT_SUPPORTED;
    }

	opData.ptpBlock = 0x2;	/* QAV register space */
	opData.ptpAddr = 13;

	opData.ptpPort = 0xF;

	op = PTP_READ_DATA;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	{
		DBG_INFO(("Failed reading Isochronous lo queue discard counter.\n"));
		return GT_FAIL;
	}

	*disCtr = (GT_U8)(opData.ptpData)&0xff;

	DBG_INFO(("OK.\n"));
	return GT_OK;
}


