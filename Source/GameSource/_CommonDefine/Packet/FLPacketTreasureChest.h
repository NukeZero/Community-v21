#ifndef __FLPACKETTREASURECHEST_H__
#define __FLPACKETTREASURECHEST_H__

#include "FLPacket.h"

//////////////////////////////////////////////////////////////////////////
// client -> world

class	FLPacketTreasureChestOpenByKeyReq : public FLPacket
{
public:

	FLPacketTreasureChestOpenByKeyReq();

	DWORD	dwChestItemObjID;
	DWORD	dwKeyItemObjID;


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// world -> client

class	FLSnapshotTreasureChestOpenNoti : public FLPacket
{
public:

	FLSnapshotTreasureChestOpenNoti();

	DWORD	dwChestItemObjID;


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

class	FLSnapshotTreasureChestOpenByKeyAck : public FLPacket
{
public:

	FLSnapshotTreasureChestOpenByKeyAck();

	T_TREASURE_ITEM arrTreasureItem[ MAX_TREASURE_ITEM_COUNT ];


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

//////////////////////////////////////////////////////////////////////////


#endif // __FLPACKETTREASURECHESTKEY_H__
