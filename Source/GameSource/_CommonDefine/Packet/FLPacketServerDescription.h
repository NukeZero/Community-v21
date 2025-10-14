#ifndef __FLPACKETSERVERDESCRIPTION_H__
#define __FLPACKETSERVERDESCRIPTION_H__


#include "FLPacket.h"

//////////////////////////////////////////////////////////////////////////

class	FLPacketServerDescriptionNoti : public FLPacket
{
public:

	FLPacketServerDescriptionNoti();

	DWORD	dwServerIndex;

	std::vector< T_SERVER_DESC >	vecServerDesc;


private:

	bool	BodySerialize( CAr& kAr )	const;
	bool	BodyDeserialize( CAr& kAr );

};

#endif // __FLPACKETSERVERDESCRIPTION_H__
