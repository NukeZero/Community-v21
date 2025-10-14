
#include "StdAfx.h"
#include "FLPacketTeleportMap.h"

#include "../../_Network/MsgHdr.h"


//////////////////////////////////////////////////////////////////////////
FLPacketTeleportMapToPointReq::FLPacketTeleportMapToPointReq() :
	FLPacket( PACKETTYPE_TELEPORT_MAP_TO_POINT_REQ )
{
	dwItemObjID	= 0;	// 텔레포트 지도 아이템

	byType	= TELEPORT_MAP_TYPE_SPEC;			// 텔레포트 아이디 타입
	dwTeleportID = 0;	// 텔레포트 아이디
}

bool	FLPacketTeleportMapToPointReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwItemObjID;
	kAr << byType;
	kAr << dwTeleportID;
	return true;
}

bool	FLPacketTeleportMapToPointReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwItemObjID;
	kAr >> byType;
	kAr >> dwTeleportID;
	return true;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
FLPacketTeleportMapAddMyPointReq::FLPacketTeleportMapAddMyPointReq() :
	FLPacket( PACKETTYPE_TELEPORT_MAP_ADD_MY_POINT_REQ )
{
	dwItemObjID = 0;
}

bool	FLPacketTeleportMapAddMyPointReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwItemObjID;
	return true;
}

bool	FLPacketTeleportMapAddMyPointReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwItemObjID;
	return true;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
FLPacketTeleportMapChgPointNameReq::FLPacketTeleportMapChgPointNameReq() :
	FLPacket( PACKETTYPE_TELEPORT_MAP_CHG_POINT_NAME_REQ )
{
	dwItemObjID = 0;
	dwUserPointID = 0;
	::memset( szPointName, 0, sizeof( szPointName ) );
}

bool	FLPacketTeleportMapChgPointNameReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwItemObjID;
	kAr << dwUserPointID;
	kAr.WriteString( szPointName );
	return true;
}

bool	FLPacketTeleportMapChgPointNameReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwItemObjID;
	kAr >> dwUserPointID;
	kAr.ReadString( szPointName, _countof( szPointName ) );
	return true;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
FLPacketTeleportMapDelPointReq::FLPacketTeleportMapDelPointReq() :
	FLPacket( PACKETTYPE_TELEPORT_MAP_DEL_POINT_REQ )
{
	dwItemObjID = 0;
	dwUserPointID = 0;
}

bool	FLPacketTeleportMapDelPointReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwItemObjID;
	kAr << dwUserPointID;
	return true;
}

bool	FLPacketTeleportMapDelPointReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwItemObjID;
	kAr >> dwUserPointID;
	return true;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
FLPacketTeleportMapDelAllPointReq::FLPacketTeleportMapDelAllPointReq() :
	FLPacket( PACKETTYPE_TELEPORT_MAP_DEL_ALL_POINT_REQ )
{
	dwItemObjID = 0;
}

bool	FLPacketTeleportMapDelAllPointReq::BodySerialize( CAr& kAr )	const
{
	kAr << dwItemObjID;
	return true;
}

bool	FLPacketTeleportMapDelAllPointReq::BodyDeserialize( CAr& kAr )
{
	kAr >> dwItemObjID;
	return true;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
FLSnapshotTeleportMapOpenNoti::FLSnapshotTeleportMapOpenNoti() :
	FLPacket( SNAPSHOTTYPE_TELEPORT_MAP_OPEN_NOTI )
{
	dwItemObjID = 0;
	byType = TELEPORT_MAP_TYPE_SPEC;
}

bool	FLSnapshotTeleportMapOpenNoti::BodySerialize( CAr& kAr )	const
{
	kAr << dwItemObjID;
	kAr << byType;

	BYTE byCount = 0;
	ptrdiff_t nCountPos = kAr.ReserveSpace( sizeof( byCount ) );

	std::vector< T_TELEPORTMAP_POINT >::const_iterator pos = vecUserPoints.begin();
	std::vector< T_TELEPORTMAP_POINT >::const_iterator end = vecUserPoints.end();

	for( ; pos != end; ++pos )
	{
		kAr << *pos;
		++byCount;
	}

	kAr.WriteReservedSpace( nCountPos, &byCount, sizeof( byCount ) );
	return true;
}

bool	FLSnapshotTeleportMapOpenNoti::BodyDeserialize( CAr& kAr )
{
	kAr >> dwItemObjID;
	kAr >> byType;

	BYTE byCount = 0;
	kAr >> byCount;

	for( int i = 0; i < byCount; ++i )
	{
		T_TELEPORTMAP_POINT tUserPoint;
		kAr >> tUserPoint;
		vecUserPoints.push_back( tUserPoint );
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
FLSnapshotTeleportMapToPointAck::FLSnapshotTeleportMapToPointAck() :
	FLPacket( SNAPSHOTTYPE_TELEPORT_MAP_TO_POINT_ACK )
{
}

bool	FLSnapshotTeleportMapToPointAck::BodySerialize( CAr& /*kAr*/ )	const
{
	return true;
}

bool	FLSnapshotTeleportMapToPointAck::BodyDeserialize( CAr& /*kAr*/ )
{
	return true;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
FLSnapshotTeleportMapAddMyPointAck::FLSnapshotTeleportMapAddMyPointAck() :
	FLPacket( SNAPSHOTTYPE_TELEPORT_MAP_ADD_MY_POINT_ACK )
{
	::memset( &tUserPoint, 0, sizeof( tUserPoint ) );
}

bool	FLSnapshotTeleportMapAddMyPointAck::BodySerialize( CAr& kAr )	const
{
	kAr << tUserPoint;
	return true;
}

bool	FLSnapshotTeleportMapAddMyPointAck::BodyDeserialize( CAr& kAr )
{
	kAr >> tUserPoint;
	return true;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
FLSnapshotTeleportMapChgPointNameAck::FLSnapshotTeleportMapChgPointNameAck() :
	FLPacket( SNAPSHOTTYPE_TELEPORT_MAP_CHG_POINT_NAME_ACK )
{
	::memset( &tUserPoint, 0, sizeof( tUserPoint ) );
}

bool	FLSnapshotTeleportMapChgPointNameAck::BodySerialize( CAr& kAr )	const
{
	kAr << tUserPoint;
	return true;
}

bool	FLSnapshotTeleportMapChgPointNameAck::BodyDeserialize( CAr& kAr )
{
	kAr >> tUserPoint;
	return true;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
FLSnapshotTeleportMapDelPointAck::FLSnapshotTeleportMapDelPointAck() :
	FLPacket( SNAPSHOTTYPE_TELEPORT_MAP_DEL_POINT_ACK )
{
	::memset( &tUserPoint, 0, sizeof( tUserPoint ) );
}

bool	FLSnapshotTeleportMapDelPointAck::BodySerialize( CAr& kAr )	const
{
	kAr << tUserPoint;
	return true;
}

bool	FLSnapshotTeleportMapDelPointAck::BodyDeserialize( CAr& kAr )
{
	kAr >> tUserPoint;
	return true;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
FLSnapShotTeleportMapDelAllPointAck::FLSnapShotTeleportMapDelAllPointAck() :
	FLPacket( SNAPSHOTTYPE_TELEPORT_MAP_DEL_ALL_POINT_ACK )
{
}

bool	FLSnapShotTeleportMapDelAllPointAck::BodySerialize( CAr& /*kAr*/ )	const
{
	return true;
}

bool	FLSnapShotTeleportMapDelAllPointAck::BodyDeserialize( CAr& /*kAr*/ )
{
	return true;
}
//////////////////////////////////////////////////////////////////////////





// 텔레포트 지도 유저 등록 리스트(DatabaseServer->WorldServer)
FLPacketTeleportMapPointListNoti::FLPacketTeleportMapPointListNoti() :
	FLPacket( PACKETTYPE_TELEPORT_MAP_POINT_LIST_NOTI )
{
	dwPlayerID = 0;
}

bool	FLPacketTeleportMapPointListNoti::BodySerialize( CAr& kAr )	const
{
	kAr << dwPlayerID;

	BYTE byCount = 0;
	ptrdiff_t nCountPos = kAr.ReserveSpace( sizeof( byCount ) );

	std::vector< T_TELEPORTMAP_POINT >::const_iterator pos = vecUserPoints.begin();
	std::vector< T_TELEPORTMAP_POINT >::const_iterator end = vecUserPoints.end();

	for( ; pos != end; ++pos )
	{
		kAr << *pos;
		++byCount;
	}

	kAr.WriteReservedSpace( nCountPos, &byCount, sizeof( byCount ) );
	return true;
}

bool	FLPacketTeleportMapPointListNoti::BodyDeserialize( CAr& kAr )
{
	kAr >> dwPlayerID;

	BYTE byCount = 0;
	kAr >> byCount;

	for( int i = 0; i < byCount; ++i )
	{
		T_TELEPORTMAP_POINT tUserPoint;
		kAr >> tUserPoint;
		vecUserPoints.push_back( tUserPoint );
	}

	return true;
}


// 텔레포트 지도 유저 등록 통보(WorldServer->DatabaseServer)
FLPacketTeleportMapAddPointNoti::FLPacketTeleportMapAddPointNoti() :
	FLPacket( PACKETTYPE_TELEPORT_MAP_ADD_POINT_NOTI )
{
	dwPlayerID = 0;
	::memset( &tUserPoint, 0, sizeof( tUserPoint ) );
}

bool	FLPacketTeleportMapAddPointNoti::BodySerialize( CAr& kAr )	const
{
	kAr << dwPlayerID;
	kAr << tUserPoint;
	return true;
}

bool	FLPacketTeleportMapAddPointNoti::BodyDeserialize( CAr& kAr )
{
	kAr >> dwPlayerID;
	kAr >> tUserPoint;
	return true;
}


// 텔레포트 지도 유저 등록 위치 이름 수정 통보(WorldServer->DatabaseServer)
FLPacketTeleportMapChgPointNameNoti::FLPacketTeleportMapChgPointNameNoti() :
	FLPacket( PACKETTYPE_TELEPORT_MAP_CHG_POINT_NAME_NOTI )
{
	dwPlayerID = 0;
	::memset( &tUserPoint, 0, sizeof( tUserPoint ) );
}

bool	FLPacketTeleportMapChgPointNameNoti::BodySerialize( CAr& kAr )	const
{
	kAr << dwPlayerID;
	kAr << tUserPoint;
	return true;
}

bool	FLPacketTeleportMapChgPointNameNoti::BodyDeserialize( CAr& kAr )
{
	kAr >> dwPlayerID;
	kAr >> tUserPoint;
	return true;
}


// 텔레포트 지도 유저 등록 위치 삭제 통보(WorldServer->DatabaseServer)
FLPacketTeleportMapDelPointNoti::FLPacketTeleportMapDelPointNoti() :
	FLPacket( PACKETTYPE_TELEPORT_MAP_DEL_POINT_NOTI )
{
	dwPlayerID = 0;
	::memset( &tUserPoint, 0, sizeof( tUserPoint ) );
}

bool	FLPacketTeleportMapDelPointNoti::BodySerialize( CAr& kAr )	const
{
	kAr << dwPlayerID;
	kAr << tUserPoint;
	return true;
}

bool	FLPacketTeleportMapDelPointNoti::BodyDeserialize( CAr& kAr )
{
	kAr >> dwPlayerID;
	kAr >> tUserPoint;
	return true;
}

