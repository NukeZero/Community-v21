
#ifndef	__FLPACKETTELEPORTMAP_H__
#define	__FLPACKETTELEPORTMAP_H__


#include "FLPacket.h"
#include "../FLTypes.h"
#include "../Spec/FLSpecStruct.h"


//////////////////////////////////////////////////////////////////////////
// 텔레포트 지도 관련
enum
{
	MAX_TELEPORT_MAP_USER_POINT			= 5,	// 유저 등록 지점 최대수
	MAX_TELEPORT_MAP_USER_POINT_NAME	= 17,	// 널 포함
};


// 텔레포트 요청
class	FLPacketTeleportMapToPointReq : public FLPacket
{
public:

	FLPacketTeleportMapToPointReq();

	DWORD	dwItemObjID;	// 텔레포트 지도 아이템

	BYTE	byType;			// 텔레포트 아이디 타입
	DWORD	dwTeleportID;	// 텔레포트 아이디

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};


// 텔레포트 지도 아이템 현재 위치 등록 요청
class	FLPacketTeleportMapAddMyPointReq : public FLPacket
{
public:

	FLPacketTeleportMapAddMyPointReq();

	DWORD	dwItemObjID;	// 텔레포트 지도 아이템

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};


// 텔레포트 지도 아이템 등록 위치 이름 수정 요청
class	FLPacketTeleportMapChgPointNameReq : public FLPacket
{
public:

	FLPacketTeleportMapChgPointNameReq();

	DWORD		dwItemObjID;									// 텔레포트 지도 아이템

	DWORD		dwUserPointID;									// 유저 등록 지점 인덱스
	TCHAR		szPointName[MAX_TELEPORT_MAP_USER_POINT_NAME];	// 유저 등록 지점 이름

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};


// 텔레포트 지도 아이템 등록 위치 삭제 요청
class	FLPacketTeleportMapDelPointReq : public FLPacket
{
public:

	FLPacketTeleportMapDelPointReq();

	DWORD	dwItemObjID;	// 텔레포트 지도 아이템
	DWORD	dwUserPointID;	// 유저 등록 지점 인덱스

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};


// 텔레포트 지도 아이템 등록 위치 모두 삭제 요청
class	FLPacketTeleportMapDelAllPointReq : public FLPacket
{
public:

	FLPacketTeleportMapDelAllPointReq();

	DWORD	dwItemObjID;	// 텔레포트 지도 아이템

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};


//////////////////////////////////////////////////////////////////////////
// client
//////////////////////////////////////////////////////////////////////////

// 텔레포트 지도 오픈
class	FLSnapshotTeleportMapOpenNoti : public FLPacket
{
public:

	FLSnapshotTeleportMapOpenNoti();

	DWORD								dwItemObjID;	// 아이템
	BYTE								byType;			// 텔레포트 타입

	std::vector< T_TELEPORTMAP_POINT >	vecUserPoints;	// 유저 등록 지점 데이터

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 텔레포트 요청 응답
class	FLSnapshotTeleportMapToPointAck : public FLPacket
{
public:

	FLSnapshotTeleportMapToPointAck();

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 텔레포트 지도 아이템 현재 위치 등록 응답
class	FLSnapshotTeleportMapAddMyPointAck : public FLPacket
{
public:

	FLSnapshotTeleportMapAddMyPointAck();

	T_TELEPORTMAP_POINT	tUserPoint;	// 등록된 지점

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 텔레포트 지도 아이템 등록 위치 이름 수정 응답
class	FLSnapshotTeleportMapChgPointNameAck : public FLPacket
{
public:

	FLSnapshotTeleportMapChgPointNameAck();

	T_TELEPORTMAP_POINT	tUserPoint;	// 변경된 지점

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};


// 텔레포트 지도 아이템 등록 위치 삭제 응답
class	FLSnapshotTeleportMapDelPointAck : public FLPacket
{
public:

	FLSnapshotTeleportMapDelPointAck();

	T_TELEPORTMAP_POINT	tUserPoint;	// 삭제된 지점

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};


// 텔레포트 지도 아이템 등록 위치 모두 삭제 응답
class	FLSnapShotTeleportMapDelAllPointAck : public FLPacket
{
public:

	FLSnapShotTeleportMapDelAllPointAck();

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};



//////////////////////////////////////////////////////////////////////////
// 서버간 통신
//////////////////////////////////////////////////////////////////////////

// 텔레포트 지도 유저 등록 리스트(DatabaseServer->WorldServer)
class	FLPacketTeleportMapPointListNoti : public FLPacket
{
public:

	FLPacketTeleportMapPointListNoti();

	OBJID									dwPlayerID;		// 유저 아이디
	// 유저 등록 지점 데이터
	std::vector< T_TELEPORTMAP_POINT >	vecUserPoints;

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// 텔레포트 지도 유저 등록 통보(WorldServer->DatabaseServer)
class	FLPacketTeleportMapAddPointNoti : public FLPacket
{
public:

	FLPacketTeleportMapAddPointNoti();

	OBJID				dwPlayerID;	// 유저 아이디
	T_TELEPORTMAP_POINT	tUserPoint;	// 삭제된 지점

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};


// 텔레포트 지도 유저 등록 위치 이름 수정 통보(WorldServer->DatabaseServer)
class	FLPacketTeleportMapChgPointNameNoti : public FLPacket
{
public:

	FLPacketTeleportMapChgPointNameNoti();

	OBJID				dwPlayerID;	// 유저 아이디
	T_TELEPORTMAP_POINT	tUserPoint;	// 삭제된 지점

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};


// 텔레포트 지도 유저 등록 위치 삭제 통보(WorldServer->DatabaseServer)
class	FLPacketTeleportMapDelPointNoti : public FLPacket
{
public:

	FLPacketTeleportMapDelPointNoti();

	OBJID				dwPlayerID;	// 유저 아이디
	T_TELEPORTMAP_POINT	tUserPoint;		// 삭제된 지점

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};





#endif