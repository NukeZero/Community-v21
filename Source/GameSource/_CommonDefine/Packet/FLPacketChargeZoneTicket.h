#ifndef	__FLPACKETCHARGEZONETICKET_H__
#define	__FLPACKETCHARGEZONETICKET_H__


#include "FLPacket.h"


//////////////////////////////////////////////////////////////////////////
// client -> world

class	FLPacketChargeZoneTicketEnteranceReq : public FLPacket
{
public:

	FLPacketChargeZoneTicketEnteranceReq();

	DWORD	dwTicketObjID;
	DWORD	dwWorldID;
	BYTE	byChannel;


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

class	FLPacketChargeZoneTicketLeaveReq : public FLPacket
{
public:

	FLPacketChargeZoneTicketLeaveReq();

	DWORD	dwTicketObjID;


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// world -> client

class	FLSnapshotChargeZoneTicketEnteranceNoti : public FLPacket
{
public:

	FLSnapshotChargeZoneTicketEnteranceNoti();

	DWORD	dwTicketObjID;


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

class	FLSnapshotChargeZoneTicketLeaveNoti : public FLPacket
{
public:

	FLSnapshotChargeZoneTicketLeaveNoti();

	DWORD	dwTicketObjID;


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

//////////////////////////////////////////////////////////////////////////


#endif // __FLPACKETCHARGEZONETICKET_H__
