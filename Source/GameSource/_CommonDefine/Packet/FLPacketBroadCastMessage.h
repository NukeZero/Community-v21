#ifndef __FLPACKETBROADCASTMESSAGE_H__
#define __FLPACKETBROADCASTMESSAGE_H__


#include "FLPacket.h"

//////////////////////////////////////////////////////////////////////////

class	FLPacketBroadCastMessageNoti : public FLPacket
{
public:

	FLPacketBroadCastMessageNoti();

	BYTE	byBroadCastType;
	TCHAR	szMessage[ 1024 ];


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

//////////////////////////////////////////////////////////////////////////

#endif // __FLPACKETBROADCASTMESSAGE_H__
