
#include "StdAfx.h"
#include "FLPacketUserAuth.h"

#include "../../_Network/MsgHdr.h"

//////////////////////////////////////////////////////////////////////////
FLSnapshotSetAuthorizationNoti::FLSnapshotSetAuthorizationNoti() :
	FLPacket( SNAPSHOTTYPE_SET_AUTHORIZATION_NOTI )
{
	dwAuthorization = AUTH_GENERAL;
}

bool	FLSnapshotSetAuthorizationNoti::BodySerialize( CAr& kAr )	const
{
	kAr << dwAuthorization;
	return true;
}

bool	FLSnapshotSetAuthorizationNoti::BodyDeserialize( CAr& kAr )
{
	kAr >> dwAuthorization;
	return true;
}
//////////////////////////////////////////////////////////////////////////

