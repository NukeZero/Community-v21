
#ifndef __FLPACKETITEMEXCHANGE_H__
#define __FLPACKETITEMEXCHANGE_H__


#include "FLPacket.h"
#include "../_Common/FLItemElem.h"


//////////////////////////////////////////////////////////////////////////
// client -> world

class	FLPacketItemExchangeReq : public FLPacket
{
public:

	FLPacketItemExchangeReq();

	DWORD	dwMenuID;								// 메뉴 아이디
	DWORD	dwListNth;								// 교환 리스트 순번
	int		nExchangeCount;							// 총 교환 수
	bool	bMax;									// 되는데로 교환 할 것인가


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// client -> world
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// world -> client

class	FLSnapshotItemExchangeAck : public FLPacket
{
public:

	FLSnapshotItemExchangeAck();

	DWORD	dwMenuID;								// 메뉴 아이디
	DWORD	dwListNth;								// 교환 리스트 순번
	bool	bSuccess;


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// world -> client
//////////////////////////////////////////////////////////////////////////

#endif// __FLPACKETITEMEXCHANGE_H__
