
#include "StdAfx.h"
#include "FLPacketItemExchange.h"
#include "../../_Network/MsgHdr.h"


//////////////////////////////////////////////////////////////////////////
// client -> world

FLPacketItemExchangeReq::FLPacketItemExchangeReq() :
FLPacket( PACKETTYPE_ITEM_EXCHANGE_REQ )
{
	dwMenuID			= NULL_ID;
	dwListNth			= NULL_ID;
	nExchangeCount		= 0;
	bMax				= false;
}

bool	FLPacketItemExchangeReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwMenuID << dwListNth << nExchangeCount << bMax;

	return true;
}

bool	FLPacketItemExchangeReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwMenuID >> dwListNth >> nExchangeCount >> bMax;

	return true;
}

// client -> world
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// world -> client

FLSnapshotItemExchangeAck::FLSnapshotItemExchangeAck() :
FLPacket( SNAPSHOTTYPE_ITEM_EXCHANGE_ACK )
{
	dwMenuID			= NULL_ID;
	dwListNth			= NULL_ID;
	bSuccess			= false;
}

bool	FLSnapshotItemExchangeAck::BodySerialize( CAr& kAr )	const
{
	kAr << dwMenuID << dwListNth << bSuccess;

	return true;
}

bool	FLSnapshotItemExchangeAck::BodyDeserialize( CAr& kAr )
{
	kAr >> dwMenuID >> dwListNth >> bSuccess;

	return true;
}

// world -> client
//////////////////////////////////////////////////////////////////////////
