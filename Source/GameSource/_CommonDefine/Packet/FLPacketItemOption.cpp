
#include "StdAfx.h"
#include "FLPacketItemOption.h"
#include "../../_Network/MsgHdr.h"


//////////////////////////////////////////////////////////////////////////
// client -> world

FLPacketItemOptionRandomOptionSyncReq::FLPacketItemOptionRandomOptionSyncReq() :
FLPacket( PACKETTYPE_ITEM_UPGRADE_RANDOM_OPTION_SYNC_REQ )
{
	dwItemObjID				= NULL_ID;
}

bool	FLPacketItemOptionRandomOptionSyncReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwItemObjID;

	return true;
}

bool	FLPacketItemOptionRandomOptionSyncReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwItemObjID;

	return true;
}

// client -> world
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// world -> client

FLSnapshotItemUpgradeRandomOptionSyncAck::FLSnapshotItemUpgradeRandomOptionSyncAck() :
FLPacket( SNAPSHOTTYPE_ITEM_UPGRADE_RANDOM_OPTION_SYNC_ACK )
{
	dwItemObjID			= NULL_ID;
	bSuccess			= false;
}

bool	FLSnapshotItemUpgradeRandomOptionSyncAck::BodySerialize( CAr& kAr )	const
{
	kAr << dwItemObjID;
	kAr << bSuccess;

	return true;
}

bool	FLSnapshotItemUpgradeRandomOptionSyncAck::BodyDeserialize( CAr& kAr )
{
	kAr >> dwItemObjID;
	kAr >> bSuccess;

	return true;
}

FLSnapshotItemUpgradeRandomOptionAck::FLSnapshotItemUpgradeRandomOptionAck() :
FLPacket( SNAPSHOTTYPE_ITEM_UPGRADE_RANDOM_OPTION_ACK )
{
	dwItemObjID				= NULL_ID;
	dwSerialNumber			= NULL_ID;

	kRandomOption.Clear();
}

bool	FLSnapshotItemUpgradeRandomOptionAck::BodySerialize( CAr& kAr )	const
{
	kAr << dwItemObjID;
	kAr << dwSerialNumber;

	kRandomOption.SerializeAllRandomOptionExtension( kAr );

	return true;
}

bool	FLSnapshotItemUpgradeRandomOptionAck::BodyDeserialize( CAr& kAr )
{
	kAr >> dwItemObjID;
	kAr >> dwSerialNumber;

	kRandomOption.DeserializeAllRandomOptionExtension( kAr );

	return true;
}

// world -> client
//////////////////////////////////////////////////////////////////////////
