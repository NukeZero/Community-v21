#pragma once 

#pragma pack(push, 1)

//template < class _SuffixPacket, class _PrefixStruct >
//class FLPacketPrefix
//{
//private:
//	virtual bool			BodySerialize( CAr & kAr ) const
//	{
//		kAr << m_Prefix;
//		_SuffixPacket::BodySerialize( kAr );
//	}
//
//	virtual bool			BodyDeserialize( CAr & kAr );
//	{
//		kAr >> m_Prefix;
//		_SuffixPacket::BodyDeserialize( kAr );
//	}
//
//public:
//	_PrefixStruct			m_Prefix;
//};



template < class _SuffixPacket >
class FLPacketPrefix_PlayerID : public _SuffixPacket
{
public:
	FLPacketPrefix_PlayerID() : m_uPlayerID( NULL_PLAYER_ID )		{}
	//FLPacketPrefix_PlayerID & operator = ( FLPacketPrefix_PlayerID & spacket )
	//{
	//	*this				= spacket;
	//	return *this;
	//}

private:
	virtual bool			BodySerialize( CAr & kAr ) const
	{
		kAr << m_uPlayerID;
		return _SuffixPacket::BodySerialize( kAr );
	}

	virtual bool			BodyDeserialize( CAr & kAr )
	{
		kAr >> m_uPlayerID;
		return _SuffixPacket::BodyDeserialize( kAr );
	}

public:
	u_long					m_uPlayerID;
};


template < class _SuffixPacket >
class FLPacketPrefix_PlayerIDAndName : public _SuffixPacket
{
public:
	FLPacketPrefix_PlayerIDAndName() : m_uPlayerID( NULL_PLAYER_ID )		
	{ 
		m_szPlayerName[ 0 ]		= '\0';
	}

private:
	virtual bool			BodySerialize( CAr & kAr ) const
	{
		kAr << m_uPlayerID;
		kAr.WriteString( m_szPlayerName );
		return _SuffixPacket::BodySerialize( kAr );
	}

	virtual bool			BodyDeserialize( CAr & kAr )
	{
		kAr >> m_uPlayerID;
		kAr.ReadString( m_szPlayerName, _countof( m_szPlayerName ) );
		return _SuffixPacket::BodyDeserialize( kAr );
	}

public:
	u_long					m_uPlayerID;
	TCHAR					m_szPlayerName[MAX_NAME ];
};

#pragma pack(pop)