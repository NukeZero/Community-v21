
#ifndef __FLPACKETMADRIGALGIFT_H__
#define __FLPACKETMADRIGALGIFT_H__


#include "FLPacket.h"

//////////////////////////////////////////////////////////////////////////
// client -> world

class	FLPacketMadrigalGiftItemReceiveReq : public FLPacket
{
public:

	FLPacketMadrigalGiftItemReceiveReq();


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// client -> world
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// world -> client

class	FLSnapshotMadrigalGiftSystemActivationNoti : public FLPacket
{
public:

	FLSnapshotMadrigalGiftSystemActivationNoti();

	bool	bActivate;


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

class	FLSnapshotMadrigalGiftPointNoti : public FLPacket
{
public:

	FLSnapshotMadrigalGiftPointNoti();

	BYTE	byCurrStep;
	int		nCurrPoint;
	int		nMaxPoint;
	int		nCurrRewardLevel;
	int		nNextRewardLevel;
	bool	bActivate;


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

class	FLSnapshotMadrigalGiftItemReceiveAck : public FLPacket
{
public:

	FLSnapshotMadrigalGiftItemReceiveAck();

	bool	bActivate;


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

// world -> client
//////////////////////////////////////////////////////////////////////////

#endif // __FLPACKETMADRIGALGIFT_H__
