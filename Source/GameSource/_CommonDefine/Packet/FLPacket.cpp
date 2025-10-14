
#include "stdafx.h"
#include "FLPacket.h"


FLPacket::FLPacket( int nPacketID ) :
	m_nPacketID( nPacketID )
{
}

FLPacket::~FLPacket()
{
	m_nPacketID = 0;
}

bool	FLPacket::Serialize( CAr& kAr )	const
{
	FLASSERT( kAr.IsStoring() );
	kAr << m_nPacketID;

	if( BodySerialize( kAr ) == false )
	{
		return false;
	}

	return true;
}

bool	FLPacket::Deserialize( CAr& kAr )
{
	FLASSERT( kAr.IsLoading() );

	//Packet Id´Â ÀÌ¹Ì »°´Ù.

	if( BodyDeserialize( kAr ) == false )
	{
		return false;
	}

	return true;
}

int		FLPacket::GetPacketID()	const
{
	return m_nPacketID;
}
