#include "StdAfx.h"
#include "FLPacketServerDescription.h"
#include "../../_Network/MsgHdr.h"


FLPacketServerDescriptionNoti::FLPacketServerDescriptionNoti() :
FLPacket( PACKETTYPE_SERVER_LIST )
{
	dwServerIndex	= NULL_ID;
	vecServerDesc.clear();
}

bool	FLPacketServerDescriptionNoti::BodySerialize( CAr& kAr )	const
{
	kAr << dwServerIndex;

	BYTE byCount = 0;
	ptrdiff_t nCountPos = kAr.ReserveSpace( sizeof( byCount ) );

	std::vector< T_SERVER_DESC >::const_iterator pos = vecServerDesc.begin();
	std::vector< T_SERVER_DESC >::const_iterator end = vecServerDesc.end();

	for( ; pos != end; ++pos )
	{
		kAr << *pos;
		++byCount;
	}

	kAr.WriteReservedSpace( nCountPos, &byCount, sizeof( byCount ) );
	return true;
}

bool	FLPacketServerDescriptionNoti::BodyDeserialize( CAr& kAr )
{
	kAr >> dwServerIndex;

	BYTE byCount = 0;
	kAr >> byCount;

	vecServerDesc.reserve( byCount );

	for( int i = 0; i < byCount; ++i )
	{
		T_SERVER_DESC kServerDesc;
		kAr >> kServerDesc;

		vecServerDesc.push_back( kServerDesc );
	}

	return true;
}