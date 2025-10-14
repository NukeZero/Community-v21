#include "StdAfx.h"
#include "FLPacketChargeZoneTicket.h"
#include "../../_Network/MsgHdr.h"


//////////////////////////////////////////////////////////////////////////
// client -> world

FLPacketChargeZoneTicketEnteranceReq::FLPacketChargeZoneTicketEnteranceReq() :
FLPacket( PACKETTYPE_CHARGE_ZONE_TICKET_ENTERANCE_REQ )
{
	dwTicketObjID	= NULL_ID;
	dwWorldID		= NULL_ID;
	byChannel		= 0;
}

bool	FLPacketChargeZoneTicketEnteranceReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwTicketObjID;
	kAr << dwWorldID;
	kAr << byChannel;

	return true;
}

bool	FLPacketChargeZoneTicketEnteranceReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwTicketObjID;
	kAr >> dwWorldID;
	kAr >> byChannel;

	return true;
}

FLPacketChargeZoneTicketLeaveReq::FLPacketChargeZoneTicketLeaveReq() :
FLPacket( PACKETTYPE_CHARGE_ZONE_TICKET_LEAVE_REQ )
{
	dwTicketObjID	= NULL_ID;
}

bool	FLPacketChargeZoneTicketLeaveReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwTicketObjID;

	return true;
}

bool	FLPacketChargeZoneTicketLeaveReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwTicketObjID;

	return true;
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// world -> client

FLSnapshotChargeZoneTicketEnteranceNoti::FLSnapshotChargeZoneTicketEnteranceNoti() :
FLPacket( SNAPSHOTTYPE_CHARGE_ZONE_TICKET_ENTERANCE_NOTI )
{
	dwTicketObjID	= NULL_ID;
}

bool	FLSnapshotChargeZoneTicketEnteranceNoti::BodySerialize( CAr& kAr )	const
{
	kAr << dwTicketObjID;

	return true;
}

bool	FLSnapshotChargeZoneTicketEnteranceNoti::BodyDeserialize( CAr& kAr )
{
	kAr >> dwTicketObjID;

	return true;
}

FLSnapshotChargeZoneTicketLeaveNoti::FLSnapshotChargeZoneTicketLeaveNoti() :
FLPacket( SNAPSHOTTYPE_CHARGE_ZONE_TICKET_LEAVE_NOTI )
{
	dwTicketObjID	= NULL_ID;
}

bool	FLSnapshotChargeZoneTicketLeaveNoti::BodySerialize( CAr& kAr )	const
{
	kAr << dwTicketObjID;

	return true;
}

bool	FLSnapshotChargeZoneTicketLeaveNoti::BodyDeserialize( CAr& kAr )
{
	kAr >> dwTicketObjID;

	return true;
}

//////////////////////////////////////////////////////////////////////////