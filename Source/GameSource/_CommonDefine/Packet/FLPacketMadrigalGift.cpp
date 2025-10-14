
#include "StdAfx.h"
#include "FLPacketMadrigalGift.h"
#include "../../_Network/MsgHdr.h"


//////////////////////////////////////////////////////////////////////////
// client -> world

FLPacketMadrigalGiftItemReceiveReq::FLPacketMadrigalGiftItemReceiveReq() :
FLPacket( PACKETTYPE_MADRIGAL_GIFT_ITEM_RECV_REQ )
{
}

bool	FLPacketMadrigalGiftItemReceiveReq::BodySerialize( CAr& /* kAr */ )	const
{
	return true;
}

bool	FLPacketMadrigalGiftItemReceiveReq::BodyDeserialize( CAr& /* kAr */ )
{
	return true;
}

// client -> world
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// world -> client

FLSnapshotMadrigalGiftSystemActivationNoti::FLSnapshotMadrigalGiftSystemActivationNoti() :
FLPacket( SNAPSHOTTYPE_MADRIGAL_GIFT_SYSTEM_ACTIVATION_NOTI )
{
	bActivate			= false;
}

bool	FLSnapshotMadrigalGiftSystemActivationNoti::BodySerialize( CAr& kAr )	const
{
	kAr << bActivate;

	return true;
}

bool	FLSnapshotMadrigalGiftSystemActivationNoti::BodyDeserialize( CAr& kAr )
{
	kAr >> bActivate;

	return true;
}

FLSnapshotMadrigalGiftPointNoti::FLSnapshotMadrigalGiftPointNoti() :
FLPacket( SNAPSHOTTYPE_MADRIGAL_GIFT_POINT_NOTI )
{
	byCurrStep			= 0;
	nCurrPoint			= 0;
	nMaxPoint			= 0;
	nCurrRewardLevel	= 0;
	nNextRewardLevel	= 0;
	bActivate			= false;
}

bool	FLSnapshotMadrigalGiftPointNoti::BodySerialize( CAr& kAr )	const
{
	kAr << byCurrStep << nCurrPoint << nMaxPoint << nCurrRewardLevel << nNextRewardLevel << bActivate;

	return true;
}

bool	FLSnapshotMadrigalGiftPointNoti::BodyDeserialize( CAr& kAr )
{
	kAr >> byCurrStep >> nCurrPoint >> nMaxPoint >> nCurrRewardLevel >> nNextRewardLevel >> bActivate;

	return true;
}

FLSnapshotMadrigalGiftItemReceiveAck::FLSnapshotMadrigalGiftItemReceiveAck() :
FLPacket( SNAPSHOTTYPE_MADRIGAL_GIFT_ITEM_RECV_ACK )
{
	bActivate			= false;
}

bool	FLSnapshotMadrigalGiftItemReceiveAck::BodySerialize( CAr& kAr )	const
{
	kAr << bActivate;

	return true;
}

bool	FLSnapshotMadrigalGiftItemReceiveAck::BodyDeserialize( CAr& kAr )
{
	kAr >> bActivate;

	return true;
}

// world -> client
//////////////////////////////////////////////////////////////////////////
