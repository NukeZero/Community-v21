
#ifndef __FLPACKETFLYFFPIECEEVENT_H__
#define __FLPACKETFLYFFPIECEEVENT_H__


#include "FLPacket.h"


//////////////////////////////////////////////////////////////////////////
// client -> world

class	FLPacketFlyffPieceExchangeListReq : public FLPacket
{
public:

	FLPacketFlyffPieceExchangeListReq();

	DWORD	dwMainCategory;
	DWORD	dwSubCategory;


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );
};

class	FLPacketExchangeFlyffPieceItemReq : public FLPacket
{
public:

	FLPacketExchangeFlyffPieceItemReq();

	DWORD	dwIndex;


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );
};

// client -> world
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// world -> client

class	FLSnapshotFlyffPieceExchangeListAck : public FLPacket
{
public:

	FLSnapshotFlyffPieceExchangeListAck();

	bool	bResult;
	std::vector< T_EXCHANGE_ITEM_DATA >	vecExchangeItemData;

private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );
};

class	FLSnapshotExchangeFlyffPieceItemAck : public FLPacket
{
public:

	FLSnapshotExchangeFlyffPieceItemAck();

	bool	bResult;


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );
};

// world -> client
//////////////////////////////////////////////////////////////////////////

#endif // __FLPACKETFLYFFPIECEEVENT_H__
