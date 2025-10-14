#include "StdAfx.h"
#include "FLPacketCharacterServerTransform.h"
#include "../../_Network/MsgHdr.h"


//////////////////////////////////////////////////////////////////////////
// client -> world

FLPacketCharacterServerTransformReq::FLPacketCharacterServerTransformReq() :
FLPacket( PACKETTYPE_CHARACTER_SERVER_TRANSFORM_TO_OTHER_SERVER_REQ )
{
	dwItemObjID				= NULL_ID;
	dwTransformServerIndex	= NULL_ID;
}

bool	FLPacketCharacterServerTransformReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwItemObjID;
	kAr << dwTransformServerIndex;

	return true;
}

bool	FLPacketCharacterServerTransformReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwItemObjID;
	kAr >> dwTransformServerIndex;

	return true;
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// world -> client

FLSnapshotCharacterServerTransformNoti::FLSnapshotCharacterServerTransformNoti() :
FLPacket( SNAPSHOTTYPE_CHARACTER_SERVER_TRANSFORM_OPEN_NOTI )
{
	dwItemObjID		= NULL_ID;
//	vecServerInfo.clear();
}

bool	FLSnapshotCharacterServerTransformNoti::BodySerialize( CAr& kAr )	const
{
	kAr << dwItemObjID;
	
// 	BYTE byCount = 0;
// 	ptrdiff_t nCountPos = kAr.ReserveSpace( sizeof( byCount ) );
// 
// 	std::vector< T_SERVER_INFO >::const_iterator pos = vecServerInfo.begin();
// 	std::vector< T_SERVER_INFO >::const_iterator end = vecServerInfo.end();
// 
// 	for( ; pos != end; ++pos )
// 	{
// 		kAr << *pos;
// 		++byCount;
// 	}
// 
// 	kAr.WriteReservedSpace( nCountPos, &byCount, sizeof( byCount ) );

	return true;
}

bool	FLSnapshotCharacterServerTransformNoti::BodyDeserialize( CAr& kAr )
{
	kAr >> dwItemObjID;

// 	BYTE byCount = 0;
// 	kAr >> byCount;
// 
// 	for( BYTE byNth = 0; byNth < byCount; ++byNth )
// 	{
// 		T_SERVER_INFO kServerInfo;
// 		kAr >> kServerInfo;
// 		vecServerInfo.push_back( kServerInfo );
// 	}

	return true;
}

FLPacketCharacterServerTransformAck::FLPacketCharacterServerTransformAck() :
FLPacket( PACKETTYPE_CHARACTER_SERVER_TRANSFORM_TO_OTHSER_SERVER_ACK )
{
	memset( szMessage, 0, sizeof( szMessage ) );
}

bool	FLPacketCharacterServerTransformAck::BodySerialize( CAr& kAr )	const
{
	kAr.WriteString( szMessage );

	return true;
}

bool	FLPacketCharacterServerTransformAck::BodyDeserialize( CAr& kAr )
{
	kAr.ReadString( szMessage, sizeof( szMessage ) );

	return true;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

FLPacketCharacterServerTransformNoti::FLPacketCharacterServerTransformNoti() :
FLPacket( PACKETTYPE_CHARACTER_SERVER_TRANSFORM_TO_OTHSER_SERVER_NOTI )
{
	dwItemObjID				= NULL_ID;
	dwPlayerID				= NULL_PLAYER_ID;
	dwTransformServerIndex	= NULL_ID;
	memset( szAccount, 0, sizeof( szAccount ) );
	memset( szBankPassword, 0, sizeof( szBankPassword ) );
}

bool	FLPacketCharacterServerTransformNoti::BodySerialize( CAr& kAr )	const
{
	kAr << dwItemObjID;
	kAr << dwPlayerID;
	kAr << dwTransformServerIndex;
	kAr.WriteString( szAccount );
	kAr.WriteString( szBankPassword );

	return true;
}

bool	FLPacketCharacterServerTransformNoti::BodyDeserialize( CAr& kAr )
{
	kAr >> dwItemObjID;
	kAr >> dwPlayerID;
	kAr >> dwTransformServerIndex;
	kAr.ReadString( szAccount, sizeof( szAccount ) );
	kAr.ReadString( szBankPassword, sizeof( szBankPassword ) );

	return true;
}

//////////////////////////////////////////////////////////////////////////