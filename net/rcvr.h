/*
 *	This file provides the typedefs and constants for the TFTP server and Distress
 *	Beacn implimentation
 */

#ifndef __RCVR_H__
#define __RCVR_H__

#include <common.h>
#include <command.h>
#include <net.h>

#if defined(CONFIG_CMD_RCVR)

/********************************************************************************************************************/
/*						DISTRESS BEACON DEFINITIONS					    */
/********************************************************************************************************************/

#define BEACON_UDP_PORT			3583		/* Destination UDP Port to broadcast the distress beacon to */
#define	RECOVERY_MODE			0xDE		/* packet_type values/signatures */
#define RECOVERY_STATE_FIRST		0x00001		/* First stage for loading the initrd */
#define RECOVERY_STATE_SECOND		0x00002		/* Second stage for loading the firmware.bin - not used in UBoot */
#define BEACON_VERSION			1		/* Current version of the Beacon Packet */
#define LL_ADDR_LEN			6		/* Length of a link level address (in bytes) */
#define IP_ADDR_LEN			4		/* Length of IP address in bytes */
#define MAX_NAME_LEN			16		/* Length of name (NetBIOS) */
#define MAX_MODEL_NUMBER_LEN		50		/* Length of model Number */

#pragma pack (1)

/* LinkLevelAddrStruct - Defines the structure of a link level address */
typedef struct
{
    uchar	LinkLevelAddr[LL_ADDR_LEN];
} LinkLevelAddrStruct;

/* IPAddrStruct - Defines the structure of an IP address */
typedef struct
{
    ulong	IPAddress;
} IPAddressStruct;

/* PacketHeaderStruct - Defines the structure of the beacon packet header
 * 	- 16-bit value that specifies teh type of packet, (RECOVERY_MODE)
 * 	- 8-bit version of packet for future extension
 */
typedef struct
{
    uchar	PacketType;
    uchar	Version;
}PacketHeaderStruct;

/* DistressBeaconPayloadStruct - Defines the structure of the beacon packet payload
 *	- 4 bytes IP address of the NAS in distress
 *	- 6 bytes LL address of the NAS in distress
 *	- 16 bytes string with the name of the NAS in distress
 *	- 50 bytes string with the NAS Model number
 */
typedef struct
{
    ushort		    State;
    IPAddressStruct	    IPAddr;
    LinkLevelAddrStruct	    LinkAddr;
    char		    Name[MAX_NAME_LEN];
    char		    ModelNumber[MAX_MODEL_NUMBER_LEN];
}DistressBeaconPayloadStruct;

/* DistressBeaconPacketStruct  - The structure with both the header and payload */
typedef struct
{
    PacketHeaderStruct		    Header;
    DistressBeaconPayloadStruct	    Payload;
}DistressBeaconPacketStruct;

#pragma pack ()

/********************************************************************************************************************/
/*						RECOVERY DEFINITIONS						    */
/********************************************************************************************************************/

#define	    RCVR_BEACON_TIMEOUT		5   /* timeout in seconds between Distress Beacon */
#define	    RCVR_DATA_TIMEOUT		6   /* timeout in seconds to receive a data packet */
#define	    RCVR_FINISH_TIMEOUT		2   /* timeout in seconds to verify that the client received the last ACK */

/* Recovery States */
typedef enum
{
    RCVR_INIT,
    RCVR_WAIT_4_CNCT,
    RCVR_IMAGE_DWNLD,
    RCVR_FINISHED
}rcvr_state_t;

/* Function Prototypes */
void RecoverRequest(void);

/********************************************************************************************************************/
/*						TFTP SERVER DEFINITIONS						    */
/********************************************************************************************************************/

/* TFTP ports */
#define		TFTP_SERVER_PORT		69

/* Length of the opcode field */
#define		TFTP_OPCODE_LEN			2

/* All possible TFTP opcodes */
#define		TFTP_OPCODE_RRQ			0x0001
#define		TFTP_OPCODE_WRQ			0x0002
#define		TFTP_OPCODE_DATA		0x0003
#define		TFTP_OPCODE_ACK			0x0004
#define		TFTP_OPCODE_ERR			0x0005

/* TFTP error codes supported */
#define		TFTP_ERROR_UNDEFINED		0
#define		TFTP_ERROR_ILLEGAL_OPERATION	4

/* MAX size of TFTP DATA */
#define		TFTP_MAX_DATA_LEN		512



#endif /* #if defined(CONFIG_CMD_RCVR) */

#endif /* __RCVR_H__ */
