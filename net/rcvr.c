/*
 *	This file impliments the TFTP server with the Distress Beacon UDP packet that will be send
 * 	as a notification for a system recovery process.
 */

#include "rcvr.h"

/* #define	DEBUG_RCVR */

#ifdef DEBUG_RCVR
#define debug_rcvr(fmt,args...)	printf (fmt ,##args)
#else
#define debug_rcvr(fmt,args...)
#endif

#if defined(CONFIG_CMD_RCVR)

/* Globals */
rcvr_state_t rcvr_state = RCVR_INIT;
ulong myTID = 0;			/* Transfer ID used my me as a TFPT server */
ulong peerTID = 0;			/* Transfer ID received frm the peer */
ulong packetNum = 0;			/* Packet number expected */
uchar * imagePtr = NULL;		/* Pointer to the location to copy the uImage file */

static void
BeaconSend (void)
{
	DistressBeaconPacketStruct * bconpkt;
	char *s;

	bconpkt = (DistressBeaconPacketStruct*) (NetTxPacket + NetEthHdrSize() + IP_HDR_SIZE);

	/* clear the whole packet to zeros */
	memset(bconpkt, 0x0, sizeof(bconpkt));

	/* Fill the packet with the information needed */
	bconpkt->Header.PacketType =  RECOVERY_MODE;
	bconpkt->Header.Version = BEACON_VERSION;
	bconpkt->Payload.State = (((RECOVERY_STATE_FIRST & 0xFF) << 8) | ((RECOVERY_STATE_FIRST & 0xFF00) >> 8));
	NetCopyIP(&bconpkt->Payload.IPAddr.IPAddress, &NetOurIP);
	memcpy (bconpkt->Payload.LinkAddr.LinkLevelAddr, NetOurEther, LL_ADDR_LEN);

	if ((s = getenv("deviceName")) != NULL)
	{
		ulong strln = strlen(s);
		if (strln >= MAX_NAME_LEN)
			strncpy(bconpkt->Payload.Name, s, MAX_NAME_LEN-1);	/* keep space for null termination */
		else
			strcpy(bconpkt->Payload.Name, s);
	}
	else
	{
		printf("Warning: Missing environment variable \"deviceName\". Assuming default!\n");
		strcpy(bconpkt->Payload.Name, "Unknown S/N");
	}

	if ((s = getenv("modelNumber")) != NULL)
	{
		ulong strln = strlen(s);
		if (strln >= MAX_MODEL_NUMBER_LEN)
			strncpy(bconpkt->Payload.ModelNumber, s, MAX_MODEL_NUMBER_LEN-1); /* keep space for null termination */
		else
			strcpy(bconpkt->Payload.ModelNumber, s);
	}
	else
	{
		printf("Warning: Missing environment variable \"modelNumber\". Assuming default!\n");
		strcpy(bconpkt->Payload.ModelNumber, "Unknown Model # of NAS system");
	}

	printf("Broadcasting Distress Beacon Packet.\n");
	debug_rcvr("Sending Beacon packet with IP = 0x%08x and MAC = %02x:%02x:%02x:%02x:%02x:%02x\n", bconpkt->Payload.IPAddr.IPAddress,
	bconpkt->Payload.LinkAddr.LinkLevelAddr[0], bconpkt->Payload.LinkAddr.LinkLevelAddr[1], bconpkt->Payload.LinkAddr.LinkLevelAddr[2],
	bconpkt->Payload.LinkAddr.LinkLevelAddr[3], bconpkt->Payload.LinkAddr.LinkLevelAddr[4], bconpkt->Payload.LinkAddr.LinkLevelAddr[5]);

	NetSendUDPPacket(NetServerEther, 0, BEACON_UDP_PORT, BEACON_UDP_PORT, sizeof(DistressBeaconPacketStruct));
}


/*
 *	Send an Error Reply.
 */
static void
SendTftpError(ushort error, unsigned src, unsigned dst, char * errStr)
{
	ushort * fld;
	char *s;

	/* write first the opcode */
	fld = (ushort*) (NetTxPacket + NetEthHdrSize() + IP_HDR_SIZE);
	*fld = (((TFTP_OPCODE_ERR & 0xFF) << 8) | ((TFTP_OPCODE_ERR & 0xFF00) >> 8));

	/* Write the error code */
	fld++;
	*fld = (((error & 0xFF) << 8) | ((error & 0xFF00) >> 8));

	/* Add a string to explain */
	s = (char *) ++fld;
	sprintf(s, "%s", errStr);

	/* Transmit the error message */
	debug_rcvr("Sendin ERR packet (PeerTID = %d, MyTID = %d).\n", src, dst);
	NetSendUDPPacket(NetServerEther, NetServerIP, src, dst, (strlen(s) + 5));
}

/*
 *	Send an Error Reply.
 */
static void
SendTftpAck(ushort block)
{
	ushort * fld;

	/* write first the opcode */
	fld = (ushort*) (NetTxPacket + NetEthHdrSize() + IP_HDR_SIZE);
	*fld = (((TFTP_OPCODE_ACK & 0xFF) << 8) | ((TFTP_OPCODE_ACK & 0xFF00) >> 8));

	/* Write the error code */
	fld++;
	*fld = (((block & 0xFF) << 8) | ((block & 0xFF00) >> 8));

	/* Transmit the error message */
	debug_rcvr("Sendin ACK packet (PeerTID = %d, MyTID = %d, Block = %d).\n", peerTID, myTID, block);
	NetSendUDPPacket(NetServerEther, NetServerIP, peerTID, myTID, 4);
}


/*
 *	Timeout on Distress Beacon Recovery request.
 */
static void
RecoverTimeout(void)
{
	switch (rcvr_state)
	{
	/* 5 seconds between Distress Beacon */
	case RCVR_WAIT_4_CNCT:
	    NetSetTimeout (RCVR_BEACON_TIMEOUT * CONFIG_SYS_HZ, RecoverTimeout);
	    BeaconSend();
	    break;

	/* Timeout between data packets or between uImage and RamDisk files */
	case RCVR_IMAGE_DWNLD:
	    debug_rcvr("Timeout, Failing the recovery process!\n");
	    SendTftpError(TFTP_ERROR_UNDEFINED, peerTID, myTID, "Data Packet TimeOut!");
	    NetSetTimeout(0, (thand_f *)0);
	    net_state = NETLOOP_FAIL;
	    rcvr_state = RCVR_INIT;
	    break;

	/* Client finished successfully no retransmit requested */
	case RCVR_FINISHED:
	    debug_rcvr("Finished successfully.\n");
	    NetSetTimeout(0, (thand_f *)0);
	    net_state = NETLOOP_SUCCESS;
	    rcvr_state = RCVR_INIT;
	    break;

	default:
	    debug_rcvr("Invalid state received in the Timeout routine~\n");
	}
}


/*
 *	Handle Recovery received packets.
 */
static void
RecoveryHandler(uchar * pkt, unsigned dest, unsigned src, unsigned len)
{
	ushort opcode;
	ushort * fld;
//	char * tftpfile = NULL;
//	uchar * tftpmode = NULL;
    ushort rxPacketNumber;
	unsigned dlen;

	fld = (ushort*) pkt;
	opcode = (((*fld & 0xFF) << 8) | ((*fld & 0xFF00) >> 8));

	switch (rcvr_state)
	{
	case RCVR_INIT:

		break;

	case RCVR_WAIT_4_CNCT:
		switch (opcode)
		{
			case TFTP_OPCODE_RRQ:
			/* check that the destination port is correct */
			if (dest != TFTP_SERVER_PORT)
			return;

			/* Copy the server IP and MAC address */
			memcpy (NetServerEther, (uchar*) &NetRxPacket[6], 6);
			NetServerIP = NetRxPacket[26];
			NetServerIP |= (NetRxPacket[27] << 8);
			NetServerIP |= (NetRxPacket[28] << 16);
			NetServerIP |= (NetRxPacket[29] << 24);
			debug_rcvr("New Peer Info (MAC %02x:%02x:%02x:%02x:%02x:%02x, IP %08x\n", NetServerEther[0], NetServerEther[1],
				NetServerEther[2], NetServerEther[3], NetServerEther[4], NetServerEther[5], NetServerIP);

			printf("Unsupported TFTP GET request received from %d.%d.%d.%d (MAC %02x:%02x:%02x:%02x:%02x:%02x)\n",
				(NetServerIP & 0xFF), ((NetServerIP >> 8) & 0xFF), ((NetServerIP >> 16) & 0xFF), ((NetServerIP >> 24) & 0xFF),
				NetServerEther[0], NetServerEther[1], NetServerEther[2], NetServerEther[3], NetServerEther[4],
				NetServerEther[5]);

			SendTftpError(TFTP_ERROR_ILLEGAL_OPERATION, src, TFTP_SERVER_PORT, "Request Not Supported");
			break;

		case TFTP_OPCODE_WRQ:

			/* check that the destination port is correct */
			if (dest != TFTP_SERVER_PORT)
			return;

			/* Copy the server IP and MAC address */
			memcpy (NetServerEther, (uchar*) &NetRxPacket[6], 6);
			NetServerIP = NetRxPacket[26];
			NetServerIP |= (NetRxPacket[27] << 8);
			NetServerIP |= (NetRxPacket[28] << 16);
			NetServerIP |= (NetRxPacket[29] << 24);
			debug_rcvr("New Peer Info (MAC %02x:%02x:%02x:%02x:%02x:%02x, IP %08x\n", NetServerEther[0], NetServerEther[1],
				NetServerEther[2], NetServerEther[3], NetServerEther[4], NetServerEther[5], NetServerIP);

			printf("New TFTP PUT request received from %d.%d.%d.%d (MAC %02x:%02x:%02x:%02x:%02x:%02x)\n",
				(NetServerIP & 0xFF), ((NetServerIP >> 8) & 0xFF), ((NetServerIP >> 16) & 0xFF), ((NetServerIP >> 24) & 0xFF),
				NetServerEther[0], NetServerEther[1], NetServerEther[2], NetServerEther[3], NetServerEther[4],
				NetServerEther[5]);

//			tftpfile = (char *)pkt +2;
//			tftpmode = pkt + 3 + strlen (tftpfile);
			debug_rcvr("TFTP WRQ received (Dest = %d, Src = %d, Len = %d, Opcode = %d, File = %s, PeerTID = %d\n",
								dest, src, len, opcode, tftpfile, src);

			/* save the peer port # as the peerTID */
			peerTID = src;
			packetNum = 1; /* reset the packet numbering */
			debug_rcvr("Saving the PeerTID to used throughout the session (PeerTID = %d).\n", peerTID);

			/* received the request successfully */
			rcvr_state = RCVR_IMAGE_DWNLD;
			NetSetTimeout (RCVR_DATA_TIMEOUT * CONFIG_SYS_HZ, RecoverTimeout);

			/* Send the ACK */
			SendTftpAck(0);
			break;

			case TFTP_OPCODE_DATA:
		case TFTP_OPCODE_ACK:
		case TFTP_OPCODE_ERR:
			debug_rcvr("ERROR: Invalid TFTP request while in WAIT_4_CNCT (opcode = %d)!\n",opcode);
			break;

		default:
			debug_rcvr("ERROR: Invalid TFTP opcode!\n");
		}
		break;

	case RCVR_IMAGE_DWNLD:
		switch (opcode)
		{
		case TFTP_OPCODE_RRQ:
			debug_rcvr("TFTP GET requestes are not supported. Sendin Error!\n");
			break;

		case TFTP_OPCODE_WRQ:

			/* check if the WRQ ACK was not received so we are having it again */
			if (packetNum == 1) /* we did not yeat receive any DATA packet */
			{
				/* check that the destination port is correct */
			if (dest != TFTP_SERVER_PORT)
				return;

			/* Copy the server IP and MAC address */
			memcpy (NetServerEther, (uchar*) &NetRxPacket[6], 6);
			NetServerIP = NetRxPacket[26];
			NetServerIP |= (NetRxPacket[27] << 8);
			NetServerIP |= (NetRxPacket[28] << 16);
			NetServerIP |= (NetRxPacket[29] << 24);
			debug_rcvr("New Peer Info (MAC %02x:%02x:%02x:%02x:%02x:%02x, IP %08x\n", NetServerEther[0], NetServerEther[1],
				NetServerEther[2], NetServerEther[3], NetServerEther[4], NetServerEther[5], NetServerIP);

			printf("TFTP PUT request received again from %d.%d.%d.%d (MAC %02x:%02x:%02x:%02x:%02x:%02x)\n",
				(NetServerIP & 0xFF), ((NetServerIP >> 8) & 0xFF), ((NetServerIP >> 16) & 0xFF), ((NetServerIP >> 24) & 0xFF),
				NetServerEther[0], NetServerEther[1], NetServerEther[2], NetServerEther[3], NetServerEther[4],
				NetServerEther[5]);

//			tftpfile = (char *)pkt +2;
//			tftpmode = pkt + 3 + strlen (tftpfile);
			debug_rcvr("TFTP WRQ received again (Dest = %d, Src = %d, Len = %d, Opcode = %d, File = %s, PeerTID = %d\n",
								dest, src, len, opcode, tftpfile,  src);

			/* save the peer port # as the peerTID */
			peerTID = src;
			debug_rcvr("Saving the PeerTID to used throughout the session (PeerTID = %d).\n", peerTID);

			/* Send the ACK */
			SendTftpAck(0);
			}
			else
			debug_rcvr("ERROR: Invalid WRQ request while data transfer!\n");

			break;

			case TFTP_OPCODE_DATA:

			/* check that the destination port is correct */
			if (dest != myTID)
			{
			debug_rcvr("ERROR: TFTP data packet not to my TID port (port = %d)!\n", dest);
			return;
			}

			fld = (ushort*) (pkt+2);
			rxPacketNumber = (((*fld & 0xFF) << 8) | ((*fld & 0xFF00) >> 8));
			if (rxPacketNumber == (ushort)(packetNum & 0xffff))
			{
			/* check the length of data */
			if (len == (TFTP_MAX_DATA_LEN + 4))
			{
				/* print a progress message */
				if ((packetNum % 500) == 0)
				printf("%luKB\r", (packetNum / 2));

				dlen = TFTP_MAX_DATA_LEN;
				NetSetTimeout (RCVR_DATA_TIMEOUT * CONFIG_SYS_HZ, RecoverTimeout);
			}
			else if (len <  (TFTP_MAX_DATA_LEN + 4))
			{
				dlen = (len - 4);
				rcvr_state = RCVR_FINISHED;
				NetSetTimeout (RCVR_FINISH_TIMEOUT * CONFIG_SYS_HZ, RecoverTimeout);
				//debug_rcvr("Received the last packet in the uImage file, changing state to WAIT_4_RAMDISK.\n");
				printf("Recovery Image received (%lu bytes).\n",((packetNum * TFTP_MAX_DATA_LEN) + dlen));
			}
			else /* Fatal Error */
			{
				debug_rcvr("ERROR: TFTP data packet larger that 512!\n");
				NetSetTimeout(0, (thand_f *)0);
				net_state = NETLOOP_FAIL;
				rcvr_state = RCVR_INIT;
				return;
			}

			debug_rcvr("Received uImage Packet #%d (length = %d). Sending Ack.\n", rxPacketNumber, dlen);

			/* copy the data to the RAM */
			memcpy(imagePtr, (pkt+4), dlen);
			imagePtr += dlen;
			NetBootFileXferSize += dlen;

			/* Send the Ack for the new packet */
			SendTftpAck(packetNum);

			/* increment the packet number */
			++packetNum;
			}
			else if (rxPacketNumber == ((ushort)(packetNum & 0xffff)-1))
			{
			debug_rcvr("Received Packet #%d AGAIN. Sending Ack.\n");

			/* Seems that my last ACK was not delivered SO Send the Ack again */
			SendTftpAck(rxPacketNumber);
			}
			else
			debug_rcvr("Invalid Packet #%d received (expecting %d)!\n", rxPacketNumber, packetNum);

			break;

		case TFTP_OPCODE_ACK:
		case TFTP_OPCODE_ERR:
			debug_rcvr("ERROR: Invalid TFTP request while in IMAGE_DWNLD (opcode = %d)!\n",opcode);
			break;

		default:
			debug_rcvr("ERROR: Invalid TFTP opcode!\n");
		}
		break;

	case RCVR_FINISHED:
		switch (opcode)
		{
		case TFTP_OPCODE_DATA:
			/* check that the destination port is correct */
			if (dest != myTID)
			{
			debug_rcvr("ERROR: TFTP data packet not to my TID port (port = %d)!\n", dest);
			return;
			}

			fld = (ushort*) (pkt+2);
			rxPacketNumber = (((*fld & 0xFF) << 8) | ((*fld & 0xFF00) >> 8));

			/* check if the last ACK was not received; so retransmit it */
			if (rxPacketNumber == ((ushort)(packetNum & 0xffff)-1))
			{
			debug_rcvr("Received Packet #%d AGAIN. Sending Ack.\n");

			/* Seems that my last ACK was not delivered SO Send the Ack again */
			SendTftpAck(rxPacketNumber);
			}
			else
			debug_rcvr("Invalid Packet #%d deceived (expecting %d)!\n", rxPacketNumber, packetNum);

			break;

		case TFTP_OPCODE_RRQ:
		case TFTP_OPCODE_WRQ:
		case TFTP_OPCODE_ACK:
		case TFTP_OPCODE_ERR:
			debug_rcvr("ERROR: Invalid TFTP request while in RCVR_FINISHED (opcode = %d)!\n",opcode);
			break;

		default:
			debug_rcvr("ERROR: Invalid TFTP opcode!\n");
		}
		break;

	default:
		debug_rcvr("ERROR: Invalid Recovery status!\n");
	}

	return;
}


/*
 *	Start a recovery process - Using Distress Beacon and TFTP server
 */
void RecoverRequest(void)
{
	uchar * s;

	/* get the uImage locations */
	if ((s = (uchar *)getenv("loadaddr")) != NULL)
	{
	imagePtr = (uchar *)simple_strtoul((char *)s, NULL, 16);
	printf("uImage load address 0x%08x\n", (unsigned int)imagePtr);
	}
	else
	{
	printf("ERROR: Missing environment variable for \"loadaddr\"!\n");
	net_state = NETLOOP_FAIL;
	return;
	}


	/* Caculate the TID to be used */
	myTID = ((NetOurEther[4] << 8) | (NetOurEther[5]));
	peerTID = 0;    /* reset the peer TID */

	/* Change the state for waiting to connect */
	rcvr_state = RCVR_WAIT_4_CNCT;

	/* Set the handler to the TFTP server */
/*	NetSetHandler(RecoveryHandler); */
/* omriii - replaced old handler with udp and arp handlers*/
	net_set_udp_handler((rxhand_f *)RecoveryHandler);
	net_set_arp_handler((rxhand_f *)RecoveryHandler);

	/* Set the Timeout */
	NetSetTimeout(RCVR_BEACON_TIMEOUT * CONFIG_SYS_HZ, RecoverTimeout);

	/* Transmit the First Distress Beacon packet */
	BeaconSend();
}

#endif /* (CONFIG_CMD_RCVR) */
