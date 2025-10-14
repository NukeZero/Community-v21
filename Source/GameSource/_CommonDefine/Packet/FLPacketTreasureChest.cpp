#include "StdAfx.h"
#include "FLPacketTreasureChest.h"
#include "../../_Network/MsgHdr.h"


//////////////////////////////////////////////////////////////////////////
// client -> world

FLPacketTreasureChestOpenByKeyReq::FLPacketTreasureChestOpenByKeyReq() :
FLPacket( PACKETTYPE_TREASURE_CHEST_OPEN_BY_KEY_REQ )
{
	dwChestItemObjID	= NULL_ID;
	dwKeyItemObjID		= NULL_ID;
}

bool	FLPacketTreasureChestOpenByKeyReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwChestItemObjID;
	kAr << dwKeyItemObjID;

	return true;
}

bool	FLPacketTreasureChestOpenByKeyReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwChestItemObjID;
	kAr >> dwKeyItemObjID;

	return true;
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// world -> client

FLSnapshotTreasureChestOpenNoti::FLSnapshotTreasureChestOpenNoti() :
FLPacket( SNAPSHOTTYPE_TREASURE_CHEST_OPEN_NOTI )
{
	dwChestItemObjID	= NULL_ID;
}

bool	FLSnapshotTreasureChestOpenNoti::BodySerialize( CAr& kAr )	const
{
	kAr << dwChestItemObjID;

	return true;
}

bool	FLSnapshotTreasureChestOpenNoti::BodyDeserialize( CAr& kAr )
{
	kAr >> dwChestItemObjID;

	return true;
}

FLSnapshotTreasureChestOpenByKeyAck::FLSnapshotTreasureChestOpenByKeyAck() :
FLPacket( SNAPSHOTTYPE_TREASURE_CHEST_OPEN_BY_KEY_ACK )
{
	memset( arrTreasureItem, 0, sizeof( arrTreasureItem ) );
}

bool	FLSnapshotTreasureChestOpenByKeyAck::BodySerialize( CAr& kAr )	const
{
	kAr.Write( arrTreasureItem, sizeof( arrTreasureItem ) );

	return true;
}

bool	FLSnapshotTreasureChestOpenByKeyAck::BodyDeserialize( CAr& kAr )
{
	kAr.Read( arrTreasureItem, sizeof( arrTreasureItem ) );

	return true;
}

//////////////////////////////////////////////////////////////////////////