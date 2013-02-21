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

#include "mvTypes.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "ctrlEnv/sys/mvCpuIf.h"
#include "cpu/mvCpu.h"
#include "mvIpc.h"
#include "mvOs.h"

//#define MV_IPC_DEBUG
#ifdef MV_IPC_DEBUG
#define mvIpcDbgPrintf mvOsPrintf
#else
#define mvIpcDbgPrintf(x...)
#endif

//#define mvIpcDbgWrite(x, y)  (x = y);
#define mvIpcDbgWrite(x, y)

#define mvIpcErrPrintf mvOsPrintf


//int axp_read_soc_clock(int timer_id);

#define  IPC_BASE_DOORBELL  12

unsigned int    myCpuId;
void   		    *queueBaseAddr;
MV_IPC_CHANNEL  ipcChannels[MAX_IPC_CHANNELS];


/***********************************************************************************
* mvIpcGetQueue
*
* DESCRIPTION:
*		This routine allocates an IPC queue from the shared memory space
*		Since the queue location must be at the same address for both parties
*		the exact location is determined by this function
*
* INPUT:
*		qId   - the id of the queue to allocate
*		isRx  - is it used to receive messages
*		qSize - size of individual queue
* OUTPUT:
*       None
* RETURN:
*		void * - return pointer to queue
*
************************************************************************************/
static void* mvIpcGetQueue(int qId, bool isRx, int qSize)
{
	void *ptr;

	ptr = (void *)((MV_U8*)queueBaseAddr + (qId * 2 * qSize) + (qSize * isRx));

	return ptr;
}

/***********************************************************************************
* mvIpcInit
*
* DESCRIPTION:
*		Initializes the IPC mechanism. reset all queues and sets global variables
*
* INPUT:
*		qBasePtr - base pointer to queue space
*		primary  - is this the primary initializer. help locating queue addresses
* OUTPUT:
*       None
* RETURN:
*		MV_OK or MV_ERROR
*
************************************************************************************/
MV_STATUS mvIpcInit(MV_VOID *qbasePtr, MV_BOOL primary)
{
	MV_U32 chnIdx;

	myCpuId        = whoAmI();
	queueBaseAddr  = qbasePtr;

	/* Initialize all channels */
	for(chnIdx = 0; chnIdx < MAX_IPC_CHANNELS; chnIdx++) {
		ipcChannels[chnIdx].state        = MV_CHN_CLOSED;
		ipcChannels[chnIdx].txEnable     = MV_FALSE;
		ipcChannels[chnIdx].rxEnable     = MV_FALSE;
		ipcChannels[chnIdx].queSizeInMsg = MV_IPC_QUEUE_SIZE;
		ipcChannels[chnIdx].nextRxMsgIdx = 1;
		ipcChannels[chnIdx].nextTxMsgIdx = 1;
		ipcChannels[chnIdx].rxMsgQueVa   = mvIpcGetQueue(chnIdx, (primary == MV_TRUE),  MV_IPC_QUEUE_SIZE * sizeof(MV_IPC_MSG));
		ipcChannels[chnIdx].txMsgQueVa   = mvIpcGetQueue(chnIdx, (primary == MV_FALSE), MV_IPC_QUEUE_SIZE * sizeof(MV_IPC_MSG));
		ipcChannels[chnIdx].rxCtrlMsg    = &ipcChannels[chnIdx].rxMsgQueVa[0];
		ipcChannels[chnIdx].txCtrlMsg    = &ipcChannels[chnIdx].txMsgQueVa[0];

		if(primary){
			mvOsMemset(ipcChannels[chnIdx].rxMsgQueVa, 0, MV_IPC_QUEUE_SIZE * sizeof(MV_IPC_MSG));
			mvOsMemset(ipcChannels[chnIdx].txMsgQueVa, 0, MV_IPC_QUEUE_SIZE * sizeof(MV_IPC_MSG));
		}

		mvIpcDbgPrintf("IPC HAL: Init channel %d with RxQ = 0x%08x; TxQ = 0x%08x\n",
			       chnIdx, (unsigned int)ipcChannels[chnIdx].rxMsgQueVa, (unsigned int)ipcChannels[chnIdx].txMsgQueVa);
	}

	mvIpcDbgPrintf("IPC HAL: Initialized interface as %s\n", (primary == MV_TRUE) ? "primary" : "secondary");

	return MV_OK;
}

/***********************************************************************************
* mvIpcClose
*
* DESCRIPTION:
*		Closes all IPC channels
*
* INPUT:
*		None
* OUTPUT:
*       None
* RETURN:
*		MV_OK or MV_ERROR
*
************************************************************************************/
MV_STATUS mvIpcClose(MV_VOID)
{
	MV_U32 chnIdx;

	/* De-activate all channels */
	for(chnIdx = 0; chnIdx < MAX_IPC_CHANNELS; chnIdx++) {

		if(ipcChannels[chnIdx].state == MV_CHN_ATTACHED)
			mvIpcDettachChannel(chnIdx);

		if(ipcChannels[chnIdx].state == MV_CHN_OPEN)
			mvIpcCloseChannel(chnIdx);
	}

	mvIpcDbgPrintf("IPC HAL: CLosed IPC interface\n");

	return MV_OK;
}
/***********************************************************************************
* mvIpcOpenChannel
*
* DESCRIPTION:
*		Opens a ipc channel and prepares it for receiving messages
*
* INPUT:
*		chnId - the channel ID to open
* OUTPUT:
*       None
* RETURN:
*		MV_OK or MV_ERROR
*
************************************************************************************/
MV_STATUS mvIpcOpenChannel(MV_U32 chnId)
{
	MV_IPC_CHANNEL *chn;
	MV_U32 msgId;

	/* Verify parameters */
	if(chnId > MAX_IPC_CHANNELS){
		mvIpcErrPrintf("IPC ERROR: Open channel: Invalid channel id %d\n", chnId);
		return MV_ERROR;
	}

	chn = &ipcChannels[chnId];

	if(chn->state != MV_CHN_CLOSED){
		mvIpcErrPrintf("IPC ERROR: Can't open channel %d. It is already open\n", chnId);
		return MV_ERROR;
	}

	/* Initialize the transmit queue */
	for(msgId = 0; msgId < chn->queSizeInMsg; msgId++)
		chn->txMsgQueVa[msgId].isUsed = MV_FALSE;

	/* Initialize channel members */
	chn->state	  	  = MV_CHN_OPEN;
	chn->nextRxMsgIdx = 1;
	chn->nextTxMsgIdx = 1;
	chn->rxEnable     = MV_TRUE;

	mvIpcDbgPrintf("IPC HAL: Opened channel %d successfully\n", chnId);

	return MV_OK;
}

/***********************************************************************************
* mvIpcAckAttach
*
* DESCRIPTION:
*		Acknowledges and Attach request from receiver.
*
* INPUT:
*		chnId - the channel ID
*		cpuId - the CPU ID to attach to
*		acknowledge - do i need to acknowledge the message
* OUTPUT:
*       None
* RETURN:
*		MV_OK or MV_ERROR
*
************************************************************************************/
static MV_STATUS mvIpcAckAttach(MV_U32 chnId, MV_U32 cpuId, MV_BOOL acknowledge)
{
	MV_IPC_CHANNEL *chn = &ipcChannels[chnId];
	MV_IPC_MSG attachMsg;
	MV_STATUS status;

	/* Cannot acknowledge remote attach until local attach was requested*/
	if((chn->state != MV_CHN_ATTACHED) && (chn->state != MV_CHN_LINKING)) {
		mvIpcDbgPrintf("IPC HAL: Can't acknowledge attach. channel in state %d\n", chn->state);
		return MV_ERROR;
	}

	if(acknowledge == MV_TRUE) {

		/* Check that channel is not already coupled to another CPU*/
		if(chn->remoteCpuId != cpuId) {
			mvIpcDbgPrintf("IPC HAL: Can't acknowledge attach. CPU %d != %d\n", chn->remoteCpuId, cpuId);
			return MV_ERROR;
		}

		mvIpcDbgPrintf("IPC HAL: Acknowledging attach from CPU %d\n", cpuId);

		/* Send the attach acknowledge message */
		attachMsg.type  = IPC_MSG_ATTACH_ACK;
		attachMsg.value = myCpuId;
		attachMsg.size  = 0;
		attachMsg.ptr   = 0;
		status = mvIpcTxCtrlMsg(chnId, &attachMsg);
		if(status != MV_OK) {
				mvIpcErrPrintf("IPC ERROR: Cannot Send attach acknowledge message\n");
				return MV_ERROR;
		}
	}

	/* Now change my own state to attached */
	chn->state = MV_CHN_ATTACHED;

	return MV_OK;
}

/***********************************************************************************
* mvIpcAckDetach
*
* DESCRIPTION:
*		Acknowledges detach request from receiver. this closes the channel for
*		transmission and resets the queues
*
* INPUT:
*		chnId - the channel ID
*		acknowledge - do i need to acknowledge the message
* OUTPUT:
*       None
* RETURN:
*		MV_OK or MV_ERROR
*
************************************************************************************/
static MV_STATUS mvIpcAckDetach(MV_U32 chnId, MV_BOOL acknowledge)
{
	MV_IPC_CHANNEL *chn = &ipcChannels[chnId];
	MV_IPC_MSG dettachMsg;
	MV_STATUS status;
	MV_U32 msgId;

	/* Cannot acknowledge remote detach until local attach was requested*/
	if((chn->state != MV_CHN_ATTACHED) && (chn->state != MV_CHN_UNLINKING)) {
		mvIpcDbgPrintf("IPC HAL: Can't acknowledge detach. channel in state %d\n", chn->state);
		return MV_ERROR;
	}

	if(acknowledge == MV_TRUE) {
		/* Send the attach acknowledge message */
		dettachMsg.type  = IPC_MSG_DETACH_ACK;
		dettachMsg.size  = 0;
		dettachMsg.ptr   = 0;
		dettachMsg.value = 0;

		status = mvIpcTxCtrlMsg(chnId, &dettachMsg);
		if(status != MV_OK) {
				mvIpcErrPrintf("IPC ERROR: Cannot Send dettach acknowledge message\n");
				return MV_ERROR;
		}
	}

	/* Now change my own state to attached */
	chn->state 		  = MV_CHN_OPEN;
	chn->txEnable     = MV_FALSE;
	chn->nextRxMsgIdx = 1;
	chn->nextTxMsgIdx = 1;

	/* Initialize the transmit queue */
	for(msgId = 1; msgId < chn->queSizeInMsg; msgId++)
		chn->txMsgQueVa[msgId].isUsed = MV_FALSE;

	return MV_OK;

	mvIpcDbgPrintf("IPC HAL: Acknowledging dettach message\n");
}

/***********************************************************************************
* mvIpcReqAttach
*
* DESCRIPTION:
*		Ask receiver to acknowledge attach request. To verify reception, message
*		transmission is possible only after receiver acknowledges the attach
*
* INPUT:
*		chn   - pointer to channel structure
*		chnId - the channel ID
* OUTPUT:
*       None
* RETURN:
*		MV_OK or MV_ERROR
*
************************************************************************************/
static MV_STATUS mvIpcReqAttach(MV_IPC_CHANNEL *chn, MV_U32 chnId)
{
	MV_IPC_MSG attachMsg;
	MV_STATUS status;
	int backoff = 10, timeout = 10;

	mvIpcDbgPrintf("IPC HAL: Requesting attach from cpu %d\n", chn->remoteCpuId);

	/* Send the attach message */
	attachMsg.type  = IPC_MSG_ATTACH_REQ;
	attachMsg.value = myCpuId;
	status = mvIpcTxCtrlMsg(chnId, &attachMsg);
	if(status != MV_OK) {
			mvIpcErrPrintf("IPC ERROR: Cannot Send attach req message\n");
			return MV_ERROR;
	}

	/* Give the receiver 10 seconds to reply */
	while ((chn->state != MV_CHN_ATTACHED) && timeout) {
		udelay(backoff);
		timeout--;
	}

	if(chn->state != MV_CHN_ATTACHED) {
		mvIpcDbgPrintf("IPC HAL: Cannot complete attach sequence. no reply from receiver after %d usec\n",
				  timeout * backoff);
		return MV_ERROR;
	}

	mvIpcDbgPrintf("IPC HAL: Attached channel %d\n", chnId);

	return MV_OK;
}
/***********************************************************************************
* mvIpcAttachChannel
*
* DESCRIPTION:
*		Attempts to attach the TX queue to a remote CPU by sending a ATTACH ACK
*		messages to receiver. if the message is acknowledged the the channel state
*		becomes attached and message transmission is enabled.
*
* INPUT:
*		chnId 		- The channel ID
*		remoteCpuId - CPU ID of receiver
* OUTPUT:
*		attached   - indicates if channel is attached
* RETURN:
*		MV_OK or MV_ERROR
*
************************************************************************************/
MV_STATUS mvIpcAttachChannel(MV_U32 chnId, MV_U32 remoteCpuId, MV_BOOL *attached)
{
	MV_IPC_CHANNEL *chn;
	MV_U32 msgId;
	MV_STATUS status;

	(*attached) = 0;

	if(chnId > MAX_IPC_CHANNELS){
		mvIpcErrPrintf("IPC ERROR: Attach channel: Invalid channel id %d\n", chnId);
		return MV_ERROR;
	}

	if(remoteCpuId > (NR_CPUS- 1)){
			mvIpcErrPrintf("IPC ERROR: Attach channel: Invalid target cpu id %d\n", remoteCpuId);
			return MV_ERROR;
	}

	chn = &ipcChannels[chnId];

	if(chn->state == MV_CHN_CLOSED){
		mvIpcErrPrintf("IPC ERROR: Can't attach channel %d. It is closed\n", chnId);
		return MV_ERROR;
	}

	if(chn->state == MV_CHN_ATTACHED){
		(*attached) = 1;
		return MV_OK;
	}

	chn->state 		  = MV_CHN_LINKING;
	chn->remoteCpuId  = remoteCpuId;
	chn->txEnable	  = MV_TRUE;

	/* Initialize the transmit queue */
	for(msgId = 1; msgId < chn->queSizeInMsg; msgId++)
		chn->txMsgQueVa[msgId].isUsed = MV_FALSE;

	/* Send req for attach to other side */
	status = mvIpcReqAttach(chn, chnId);
	if(status == MV_OK) {
		(*attached) = 1;
		mvIpcDbgPrintf("IPC HAL: Attached channel %d to CPU %d\n", chnId, remoteCpuId);
	}

	return MV_OK;
}

/***********************************************************************************
* mvIpcDettachChannel
*
* DESCRIPTION:
*		Detaches the channel from remote cpu. it notifies the remote cpu by sending
*		control message and waits for acknowledge. after calling this function
*		data messages cannot be sent anymore
*
* INPUT:
*		chnId 		- The channel ID
* OUTPUT:
*       None
* RETURN:
*		MV_OK or MV_ERROR
*
************************************************************************************/
MV_STATUS mvIpcDettachChannel(MV_U32 chnId)
{
	MV_IPC_CHANNEL *chn;
	MV_IPC_MSG msg;
	MV_STATUS status;

	if(chnId > MAX_IPC_CHANNELS){
		mvIpcErrPrintf("IPC ERROR: Detach channel: Invalid channel id %d\n", chnId);
		return MV_ERROR;
	}

	chn = &ipcChannels[chnId];

	if(chn->state != MV_CHN_ATTACHED){
		mvIpcErrPrintf("IPC ERROR: Detach: channel %d is not attached\n", chnId);
		return MV_ERROR;
	}

	msg.type  = IPC_MSG_DETACH_REQ;
	msg.size  = 0;
	msg.ptr   = 0;
	msg.value = 0;

	status = mvIpcTxCtrlMsg(chnId, &msg);
	if(status != MV_OK) {
			mvIpcErrPrintf("IPC ERROR: Cannot Send detach request message\n");
			return MV_ERROR;
	}

	chn->remoteCpuId  = 0;
	chn->state        = MV_CHN_UNLINKING;

	return MV_OK;
}

/***********************************************************************************
* mvIpcCloseChannel - CLose and IPC channel
*
* DESCRIPTION:
*		Closes the 	IPC channels. this disables the channels ability to receive messages
*
* INPUT:
*		chnId 		- The channel ID
* OUTPUT:
*       None
* RETURN:
*		MV_OK or MV_ERROR
*
************************************************************************************/
MV_STATUS mvIpcCloseChannel(MV_U32 chnId)
{
	if(chnId > MAX_IPC_CHANNELS){
		mvIpcErrPrintf("IPC ERROR: Close channel: Invalid channel id %d\n", chnId);
		return MV_ERROR;
	}
	if(ipcChannels[chnId].state == MV_CHN_CLOSED){
		mvIpcErrPrintf("IPC ERROR: Close channel: Channel %d is already closed\n", chnId);
		return MV_ERROR;
	}

	ipcChannels[chnId].state       = MV_CHN_CLOSED;
	ipcChannels[chnId].txEnable    = MV_FALSE;
	ipcChannels[chnId].rxEnable    = MV_FALSE;
	ipcChannels[chnId].remoteCpuId = 0;

	mvIpcDbgPrintf("IPC HAL: Closed channel %d successfully\n", chnId);

	return MV_OK;
}

/***********************************************************************************
* mvIpcIsTxReady
*
* DESCRIPTION:
*		Checks if the channel is ready to transmit
*
* INPUT:
*		chnId 		- The channel ID
* OUTPUT:
*       None
* RETURN:
*		MV_OK or MV_ERROR
*
************************************************************************************/
MV_BOOL mvIpcIsTxReady(MV_U32 chnId)
{
	MV_IPC_CHANNEL *chn;

	/* Some parameters verification */
	if(chnId > MAX_IPC_CHANNELS){
		mvIpcErrPrintf("IPC ERROR: Tx Test: Invalid channel id %d\n", chnId);
		return MV_FALSE;
	}
	if(ipcChannels[chnId].state != MV_CHN_ATTACHED){
		mvIpcErrPrintf("IPC ERROR: Tx Test: channel not attached, state is %d\n", ipcChannels[chnId].state);
		return MV_FALSE;
	}

	chn = &ipcChannels[chnId];

	/* Is next message still used by receiver, yes means full queue or bug */
	if(chn->txMsgQueVa[chn->nextTxMsgIdx].isUsed != MV_FALSE) {
		mvIpcDbgPrintf("IPC HAL: Tx Test: Can't send, Msg %d used flag = %d\n", chn->nextTxMsgIdx, chn->txMsgQueVa[chn->nextTxMsgIdx].isUsed);
		return MV_FALSE;
	}

	return MV_TRUE;
}
/***********************************************************************************
* mvIpcSendDoorbell
*
* DESCRIPTION:
* 		Send an IPC doorbell to target CPU
*
* INPUT:
*		cpuId - the id of the target CPU
*		chnId - The channel ID
* OUTPUT:
*       None
* RETURN:
*		MV_OK or MV_ERROR
*
************************************************************************************/
static INLINE MV_VOID mvIpcSendDoorbell(MV_U32 cpuId, MV_U32 chnId)
{
	MV_U32 cpuBitMask;
	MV_U32 doorbellNum;

	cpuBitMask   = (1 << cpuId);
	doorbellNum  = IPC_BASE_DOORBELL + chnId;

	/* Use private doorbell 15 for IPC */
	MV_REG_WRITE(CPU_SW_TRIG_IRQ, ((cpuBitMask << 8) | doorbellNum));

	return;
}

/***********************************************************************************
* mvIpcTxCtrlMsg
*
* DESCRIPTION:
*		Sends a control message to other side. these messages are not forwarded
*		to user
*
* INPUT:
*		chnId - The channel ID
*		inMsg - Pointer to message to send
* OUTPUT:
*       None
* RETURN:
*		MV_OK or MV_ERROR
*
************************************************************************************/
MV_STATUS mvIpcTxCtrlMsg(MV_U32 chnId, MV_IPC_MSG *inMsg)
{
	MV_IPC_CHANNEL *chn;

	chn = &ipcChannels[chnId];

	if(chn->txEnable == MV_FALSE)
	{
		mvIpcErrPrintf("IPC ERROR: Tx Ctrl msg: Tx not enabled\n");
		return MV_ERROR;
	}

	/* Write the message and pass */
	chn->txCtrlMsg->type  = inMsg->type;
	chn->txCtrlMsg->size  = inMsg->size;
	chn->txCtrlMsg->ptr   = inMsg->ptr;
	chn->txCtrlMsg->value = inMsg->value;

	/* Make sure the msg values are written before the used flag
	 * to ensure the polling receiver will get valid message once
	 * it detects isUsed == MV_TRUE.
	 */
	dmb();

	chn->txCtrlMsg->isUsed   = MV_TRUE;

	mvIpcDbgWrite(chn->txCtrlMsg->align[0], axp_read_soc_clock(0));
	mvIpcDbgWrite(chn->txCtrlMsg->align[1], 0);
	mvIpcDbgWrite(chn->txCtrlMsg->align[2], 0);

	mvIpcDbgPrintf("IPC HAL: Sent control message 0x%8x on channel %d to cpu %d\n", chn->txCtrlMsg, chnId, chn->remoteCpuId);

	mvIpcSendDoorbell(chn->remoteCpuId, chnId);

	return MV_OK;
}

/***********************************************************************************
* mvIpcTxMsg
*
* DESCRIPTION:
*		Main transmit function
*
* INPUT:
*		chnId - The channel ID
*		inMsg - Pointer to message to send
* OUTPUT:
*       None
* RETURN:
*		MV_OK or MV_ERROR
*
************************************************************************************/
MV_STATUS mvIpcTxMsg(MV_U32 chnId, MV_IPC_MSG *inMsg)
{
	MV_IPC_CHANNEL *chn;
	MV_IPC_MSG     *currMsg;

	if(mvIpcIsTxReady(chnId) == MV_FALSE){
		mvIpcDbgPrintf("IPC ERROR: Tx msg: Tx Queue not ready. Can't transmit at this time \n");
		return MV_ERROR;
	}

	chn = &ipcChannels[chnId];

	/* Write the message */
	currMsg  = &chn->txMsgQueVa[chn->nextTxMsgIdx];

	currMsg->type  = inMsg->type;
	currMsg->size  = inMsg->size;
	currMsg->ptr   = inMsg->ptr;
	currMsg->value = inMsg->value;

	/* Make sure the msg values are written before the used flag
	 * to ensure the polling receiver will get valid message once
	 * it detects isUsed == MV_TRUE.
	 */
	dmb();

	/* Pass ownership to remote cpu */
	currMsg->isUsed   = MV_TRUE;

	mvIpcDbgWrite(currMsg->align[0], axp_read_soc_clock(0));
	mvIpcDbgWrite(currMsg->align[1], 0);
	mvIpcDbgWrite(currMsg->align[2], 0);

	chn->nextTxMsgIdx++;
	if(chn->nextTxMsgIdx == chn->queSizeInMsg)
		chn->nextTxMsgIdx = 1;

	mvIpcDbgPrintf("IPC HAL: Sent message %d on channel %d to cpu %d\n", chn->nextTxMsgIdx - 1, chnId, chn->remoteCpuId);

	mvIpcSendDoorbell(chn->remoteCpuId, chnId);

	return MV_OK;
}

/***********************************************************************************
* mvIpcRecieveDoorbell
*
* DESCRIPTION:
*		Translates a received doorbell to the matching channel
*
* INPUT:
*		chnId   - The channel ID
*		drblNum - Number of doorbell received
* OUTPUT:
*       None
* RETURN:
*		MV_OK or MV_ERROR
*
************************************************************************************/
static MV_STATUS mvIpcRecieveDoorbell(int *chnId, MV_U32 drblNum)
{
	(*chnId) = (drblNum) - IPC_BASE_DOORBELL;
	return MV_OK;
}

/***********************************************************************************
* mvIpcRxCtrlMsg
*
* DESCRIPTION:
*		This routine initializes IPC channel: setup receive queue and enable data receiving
*		This routine receives IPC control structure (ipcCtrl) as input parameter.
*		The following ipcCtrl members must be initialized prior calling this function:
*
* INPUT:
*		chnId - The channel ID
*		msg   - Pointer to received control message
* OUTPUT:
*       None
* RETURN:
*		void
*
************************************************************************************/
static void mvIpcRxCtrlMsg(MV_U32 chnId, MV_IPC_MSG *msg)
{
	mvIpcDbgPrintf("IPC HAL: Processing control message %d \n", msg->type);

	switch(msg->type) {

	case IPC_MSG_ATTACH_REQ:
		mvIpcAckAttach(chnId, msg->value, MV_TRUE);
		break;

	case IPC_MSG_ATTACH_ACK:
		mvIpcAckAttach(chnId, msg->value, MV_FALSE);
		break;

	case IPC_MSG_DETACH_REQ:
		mvIpcAckDetach(chnId, MV_TRUE);
		break;

	case IPC_MSG_DETACH_ACK:
		mvIpcAckDetach(chnId, MV_FALSE);
		break;

	default:
		mvIpcDbgPrintf("IPC HAL: Unknown internal message type %d \n", msg->type);
	}

	mvIpcDbgWrite(msg->align[2], axp_read_soc_clock(0));

	mvIpcReleaseMsg(chnId, msg);
}
/***********************************************************************************
* isCtrlMsg
*
* DESCRIPTION:
*		Checks for control message
*
* INPUT:
*		chn - pointer to control channel
* OUTPUT:
*       None
* RETURN:
*		MV_TRUE or MV_FALSE
*
************************************************************************************/
static MV_BOOL isCtrlMsg(MV_IPC_CHANNEL *chn)
{
	if(chn->rxCtrlMsg->isUsed == MV_TRUE)
		return MV_TRUE;
	else
		return MV_FALSE;
}

/***********************************************************************************
* mvIpcDisableChnRx
*
* DESCRIPTION:
*		Masks the doorbell for the given channel
*
* INPUT:
*		irq - number of irq/doorbell to mask
* OUTPUT:
*       None
* RETURN:
*		MV_OK or MV_ERROR
*
************************************************************************************/
MV_VOID mvIpcDisableChnRx(MV_U32 irq)
{
	/* Reset the doorbell corresponding to channel */
	MV_REG_BIT_RESET(CPU_DOORBELL_IN_MASK_REG, (1 << irq));
}

/***********************************************************************************
* mvIpcEnableChnRx
*
* DESCRIPTION:
*		Unmasks the doorbell for the given channel
*
* INPUT:
*		irq - number of irq/doorbell to unmask
* OUTPUT:
*       None
* RETURN:
*		MV_OK or MV_ERROR
*
************************************************************************************/
MV_VOID mvIpcEnableChnRx(MV_U32 irq)
{
	/* Set the doorbell corresponding to channel */
	MV_REG_BIT_SET(CPU_DOORBELL_IN_MASK_REG, (1 << irq));
}

/***********************************************************************************
* mvIpcRxMsg
*
* DESCRIPTION:
*		Main Rx routine - should be called from interrupt routine
*
* INPUT:
*		drblNum  - number of doorbel received
* OUTPUT:
*       outChnId - the channel id that received a message
*       outMsg   - pointer to the message received
* RETURN:
*		MV_TRUE  - if a message was received
*		MV_FALSE - if no message exists
*
************************************************************************************/
MV_BOOL mvIpcRxMsg(MV_U32 *outChnId, MV_IPC_MSG **outMsg, MV_U32 drblNum)
{
	MV_IPC_CHANNEL *chn;
	MV_IPC_MSG     *currMsg;
	MV_U32 	  chnId;
	MV_STATUS status;

	status = mvIpcRecieveDoorbell(&chnId, drblNum);

	if(chnId > MAX_IPC_CHANNELS) {
		mvIpcErrPrintf("IPC ERROR: Rx msg: Bad channel id %d\n", chnId);
		return MV_FALSE;
	}

	chn = &ipcChannels[chnId];

	if (chn->state == MV_CHN_CLOSED)
		return MV_FALSE;

	/* First process control messages like attach, detach, close */
	if(isCtrlMsg(chn) == MV_TRUE){
		mvIpcRxCtrlMsg(chnId, chn->rxCtrlMsg);
	}

	currMsg = &chn->rxMsgQueVa[chn->nextRxMsgIdx];

	// Check for unread data messages in queue */
	if(currMsg->isUsed != MV_TRUE){
		return MV_FALSE;
	}

	/* Increment msg idx to keep in sync with sender */
	chn->nextRxMsgIdx++;
	if(chn->nextRxMsgIdx == chn->queSizeInMsg)
		chn->nextRxMsgIdx = 1;

	// Check if channel is ready to receive messages */
	if(chn->state < MV_CHN_OPEN){
		mvIpcErrPrintf("IPC ERROR: Rx msg: Channel not ready, state = %d\n", chn->state);
		return MV_FALSE;
	}

	mvIpcDbgWrite(currMsg->align[2], axp_read_soc_clock(0));

	/* Now process user messages */
	mvIpcDbgPrintf("IPC HAL: Received message %d on channel %d\n", chn->nextRxMsgIdx - 1, chnId);

	(*outMsg)   = currMsg;
	(*outChnId) = chnId;

	return MV_TRUE;
}

/***********************************************************************************
* mvIpcReleaseMsg
*
* DESCRIPTION:
*		Return ownership on message to transmitter
*
* INPUT:
*		chnId - The channel ID
*		msg   - Pointer to message to release
* OUTPUT:
*       None
* RETURN:
*		MV_OK or MV_ERROR
*
************************************************************************************/
MV_STATUS mvIpcReleaseMsg(MV_U32 chnId, MV_IPC_MSG *msg)
{
	MV_IPC_CHANNEL *chn;

	if((chnId > MAX_IPC_CHANNELS) || (ipcChannels[chnId].state == MV_CHN_CLOSED)) {
		mvIpcErrPrintf("IPC ERROR: Msg release: Bad or inactive channel id %d\n", chnId);
		return MV_ERROR;
	}

	chn = &ipcChannels[chnId];

	if(msg->isUsed == MV_FALSE) {
		mvIpcErrPrintf("IPC ERROR: Msg release: Msg %d owned by %d\n", chn->nextRxMsgIdx, msg->isUsed);
		return MV_ERROR;
	}

	msg->isUsed   = MV_FALSE;
	mvIpcDbgWrite(msg->align[1], axp_read_soc_clock(0));

	mvIpcDbgPrintf("IPC HAL: Released message 0x%8x on channel %d\n", msg, chnId);

	return MV_OK;
}
