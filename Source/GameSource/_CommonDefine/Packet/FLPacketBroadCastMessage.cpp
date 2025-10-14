#include "StdAfx.h"
#include "FLPacketBroadCastMessage.h"
#include "../../_Network/MsgHdr.h"


//////////////////////////////////////////////////////////////////////////

FLPacketBroadCastMessageNoti::FLPacketBroadCastMessageNoti() :
FLPacket( PACKETTYPE_BROADCAST_MESSAGE )
{
	byBroadCastType	= BROADCAST_TYPE_NONE;
	memset( szMessage, 0, sizeof( szMessage ) );
}

bool	FLPacketBroadCastMessageNoti::BodySerialize( CAr& kAr )	const
{
	kAr << byBroadCastType;
	kAr.WriteString( szMessage );

	return true;
}

bool	FLPacketBroadCastMessageNoti::BodyDeserialize( CAr& kAr )
{
	kAr >> byBroadCastType;
	kAr.ReadString( szMessage, _countof( szMessage ) );

	return true;
}

//////////////////////////////////////////////////////////////////////////
