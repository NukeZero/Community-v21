#ifndef	__FLPACKETCHARACTERSERVERTRANSFORM_H__
#define	__FLPACKETCHARACTERSERVERTRANSFORM_H__

#include "FLPacket.h"

//////////////////////////////////////////////////////////////////////////
// client -> world

class	FLPacketCharacterServerTransformReq : public FLPacket
{
public:

	FLPacketCharacterServerTransformReq();

	DWORD	dwItemObjID;
	DWORD	dwTransformServerIndex;


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// world -> client

class	FLSnapshotCharacterServerTransformNoti : public FLPacket
{
public:

	FLSnapshotCharacterServerTransformNoti();

	DWORD	dwItemObjID;
	
//	std::vector< T_SERVER_INFO >	vecServerInfo;


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

class	FLPacketCharacterServerTransformAck : public FLPacket
{
public:

	FLPacketCharacterServerTransformAck();

	TCHAR	szMessage[512];


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );
};

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// world <-> trans

class	FLPacketCharacterServerTransformNoti : public FLPacket
{
public:

	FLPacketCharacterServerTransformNoti();

	DWORD	dwItemObjID;
	DWORD	dwPlayerID;
	DWORD	dwTransformServerIndex;
	TCHAR	szAccount[MAX_ACCOUNT];
	TCHAR	szBankPassword[5];


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );
};

//////////////////////////////////////////////////////////////////////////

#endif // __FLPACKETCHARACTERSERVERTRANSFORM_H__
