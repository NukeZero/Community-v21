
#include "StdAfx.h"
#include "FLPacketFlyffPieceEvent.h"
#include "../../_Network/MsgHdr.h"


//////////////////////////////////////////////////////////////////////////
// client -> world

FLPacketFlyffPieceExchangeListReq::FLPacketFlyffPieceExchangeListReq() :
FLPacket( PACKETTYPE_FLYFF_PIECE_EXCHANGE_LIST_REQ )
{
	dwMainCategory	= TYPE1_NONE;
	dwSubCategory	= TYPE2_NONE;
}

bool	FLPacketFlyffPieceExchangeListReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwMainCategory;
	kAr << dwSubCategory;

	return true;
}

bool	FLPacketFlyffPieceExchangeListReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwMainCategory;
	kAr >> dwSubCategory;

	return true;
}

FLPacketExchangeFlyffPieceItemReq::FLPacketExchangeFlyffPieceItemReq() :
FLPacket( PACKETTYPE_EXCHANGE_FLYFF_PIECE_ITEM_REQ )
{
	dwIndex		= 0;
}

bool	FLPacketExchangeFlyffPieceItemReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwIndex;

	return true;
}

bool	FLPacketExchangeFlyffPieceItemReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwIndex;

	return true;
}

// client -> world
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// world -> client

FLSnapshotFlyffPieceExchangeListAck::FLSnapshotFlyffPieceExchangeListAck() :
FLPacket( SNAPSHOTTYPE_FLYFF_PIECE_EXCHANGE_LIST_ACK )
{
	bResult		= false;
	vecExchangeItemData.clear();
}

bool	FLSnapshotFlyffPieceExchangeListAck::BodySerialize( CAr& kAr )	const
{
	kAr << bResult;

	DWORD dwCount = 0;
	ptrdiff_t nCountPos = kAr.ReserveSpace( sizeof( dwCount ) );

	for( std::vector< T_EXCHANGE_ITEM_DATA >::const_iterator pos = vecExchangeItemData.begin(); pos != vecExchangeItemData.end(); ++pos )
	{
		kAr << *pos;

		++dwCount;
	}

	kAr.WriteReservedSpace( nCountPos, &dwCount, sizeof( dwCount ) );

	return true;
}

bool	FLSnapshotFlyffPieceExchangeListAck::BodyDeserialize( CAr& kAr )
{
	kAr >> bResult;

	DWORD dwCount = 0;
	kAr >> dwCount;

	for( DWORD Nth = 0; Nth < dwCount; ++Nth )
	{
		T_EXCHANGE_ITEM_DATA	kExchangeItemData;

		kAr >> kExchangeItemData;

		vecExchangeItemData.push_back( kExchangeItemData );
	}

	return true;
}

FLSnapshotExchangeFlyffPieceItemAck::FLSnapshotExchangeFlyffPieceItemAck() :
FLPacket( SNAPSHOTTYPE_EXCHANGE_FLYFF_PIECE_ITEM_ACK )
{
	bResult		= false;
}

bool	FLSnapshotExchangeFlyffPieceItemAck::BodySerialize( CAr& kAr )	const
{
	kAr << bResult;

	return true;
}

bool	FLSnapshotExchangeFlyffPieceItemAck::BodyDeserialize( CAr& kAr )
{
	kAr >> bResult;

	return true;
}

// world -> client
//////////////////////////////////////////////////////////////////////////
