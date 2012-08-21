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

#ifndef __mvIpc_h
#define __mvIpc_h

#define IPC_TRANSMITTER		0
#define IPC_RECIEVER		1

typedef enum
{
	MV_CHN_CLOSED =  0,
	MV_CHN_OPEN,
	MV_CHN_LINKING,
	MV_CHN_UNLINKING,
	MV_CHN_ATTACHED

}MV_IPC_CHN_STATE;

typedef struct __ipc_message_struct
{
	MV_U32	 type;
	MV_U32 	 size; 			/*buffer size*/
	MV_VOID* ptr;   		/*buffer virtual address for Rx side*/
	MV_U32	 value;			/*User data*/
	MV_U32   isUsed;		/*CPU Id and optional oob message*/
	MV_U32	 align[3];		/* Align message size to cache line */
} MV_IPC_MSG;

typedef struct __ipc_channel_struct
{
	MV_IPC_MSG* rxMsgQueVa;   /*buffer virtual address for Rx side*/
	MV_IPC_MSG* txMsgQueVa;   /*buffer virtual address for Tx side*/
	MV_IPC_MSG* rxCtrlMsg;    /*buffer virtual address for Rx side*/
	MV_IPC_MSG* txCtrlMsg;    /*buffer virtual address for Tx side*/
	MV_U32	 nextRxMsgIdx;
	MV_U32	 nextTxMsgIdx;
	MV_U32	 queSizeInMsg;
	MV_U32   remoteCpuId;
	MV_BOOL	 txEnable;
	MV_BOOL	 rxEnable;
	MV_IPC_CHN_STATE state;

} MV_IPC_CHANNEL;

#define MAX_IPC_CHANNELS     4
#define MV_IPC_QUEUE_SIZE    256
#define MV_IPC_QUEUE_MEM     (MV_IPC_QUEUE_SIZE * 2 * sizeof(MV_IPC_MSG) * MAX_IPC_CHANNELS)
#define MAX_USER_MSG_TYPE	 (1 << 16)

typedef enum
{
	IPC_MSG_ATTACH_REQ = 0,
	IPC_MSG_ATTACH_ACK,
	IPC_MSG_DETACH_REQ,
	IPC_MSG_DETACH_ACK
}MV_IPC_CTRL_MSG_TYPE;


MV_STATUS mvIpcInit(MV_VOID *qbasePtr, MV_BOOL primary);
MV_STATUS mvIpcClose(MV_VOID);
MV_STATUS mvIpcOpenChannel(MV_U32 chnId);
MV_STATUS mvIpcCloseChannel(MV_U32 chnId);
MV_STATUS mvIpcAttachChannel(MV_U32 chnId, MV_U32 remoteCpuId, MV_BOOL *attached);
MV_STATUS mvIpcDettachChannel(MV_U32 chnId);
MV_BOOL   mvIpcIsTxReady(MV_U32 chnId);
MV_STATUS mvIpcTxMsg(MV_U32 chnId, MV_IPC_MSG *inMsg);
MV_STATUS mvIpcTxCtrlMsg(MV_U32 chnId, MV_IPC_MSG *inMsg);
MV_STATUS mvIpcRxMsg(MV_U32 *outChnId, MV_IPC_MSG **outMsg, MV_U32 drblNum);
MV_STATUS mvIpcReleaseMsg(MV_U32 chnId, MV_IPC_MSG *msg);
MV_VOID   mvIpcDisableChnRx(MV_U32 irq);
MV_VOID   mvIpcEnableChnRx(MV_U32 irq);


#endif /*__mvIpc_h */
