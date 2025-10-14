
#ifndef	__FLPACKET_H__
#define	__FLPACKET_H__


#include <network/FLAr.h>


class	FLPacket
{
public:

	explicit FLPacket( int nPacketID );
	virtual ~FLPacket();

	bool	Serialize( CAr& kAr )	const;
	bool	Deserialize( CAr& kAr );

	int		GetPacketID()	const;

private:

	virtual bool	BodySerialize( CAr& kAr )	const	= 0;
	virtual bool	BodyDeserialize( CAr& kAr )			= 0;

private:

	int		m_nPacketID;


};





#endif