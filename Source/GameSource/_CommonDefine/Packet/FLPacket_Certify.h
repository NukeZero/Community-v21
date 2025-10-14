#pragma once

#pragma pack(push, 1)


struct FLAccount_Common
{
	FLAccount_Common() : dwAuthKey( 0 ), bySlot( 0 ), uPlayerID( 0 )		{	szAccount[ 0 ] = '\0';			szPassword[ 0 ] = '\0';	} 

public:
	DWORD					dwAuthKey;
	BYTE					bySlot;
	u_long					uPlayerID;

	char					szAccount[ MAX_ACCOUNT ];
	char					szPassword[ MAX_ENCRYPT_PWD ];
};

struct FLDpids
{
	FLDpids() : dpid( DPID_UNKNOWN ), dpidCache( DPID_UNKNOWN ), dpidUser( DPID_UNKNOWN ) {}

public:

	DPID					dpid;
	DPID					dpidCache;
	DPID					dpidUser;
};


enum E_LOGIN_RESULT
{
	E_LOGIN_SUCCESS			= 0,
	E_LOGIN_FAILURE, 
	E_LOGIN_SHUTDOWN_RULE,
};


//-------------------------------------------------------------------------------------------------------//

class FLPacketWtoDB_ChannelJoinReq : public FLPacket
{
public:
	FLPacketWtoDB_ChannelJoinReq() : FLPacket( PACKETTYPE_JOIN ) {}

protected:
	virtual bool			BodySerialize( CAr & kAr ) const {		kAr << m_kAccount;	return true;	}
	virtual bool			BodyDeserialize( CAr & kAr )	{		kAr >> m_kAccount;	return true;	}

public:
	FLAccount_Common		m_kAccount;
};


class FLPacketDBtoA_ChannelJoinReq : public FLPacket
{
public:
	FLPacketDBtoA_ChannelJoinReq() : FLPacket( PACKETTYPE_JOIN ) {}

protected:
	virtual bool			BodySerialize( CAr & kAr ) const {		kAr << m_kAccount << m_kDpids;	return true;	}
	virtual bool			BodyDeserialize( CAr & kAr )	{		kAr >> m_kAccount >> m_kDpids;	return true;	}

public:
	FLAccount_Common		m_kAccount;
	FLDpids					m_kDpids;
};

class FLPacketAtoDB_ChannelJoinAck : public FLPacket
{
public:
	FLPacketAtoDB_ChannelJoinAck() : FLPacket( PACKETTYPE_JOIN ), m_eResult( E_LOGIN_FAILURE ), m_dwPCBangClass( 0 ), m_permitedIP( false )  {}

protected:
	virtual bool			BodySerialize( CAr & kAr ) const {		kAr << m_eResult << m_kAccount << m_kDpids << m_dwPCBangClass << m_tmBirthDate << m_permitedIP;	return true;	}
	virtual bool			BodyDeserialize( CAr & kAr )	{		kAr >> m_eResult >> m_kAccount >> m_kDpids >> m_dwPCBangClass >> m_tmBirthDate >> m_permitedIP;	return true;	}

public:
	E_LOGIN_RESULT			m_eResult;
	DWORD					m_dwPCBangClass;
	bool					m_permitedIP;

	FLAccount_Common		m_kAccount;
	FLDpids					m_kDpids;
	CTime					m_tmBirthDate;
};




//class FLSnapshot_LoginAck : public FLPacket
//{
//public:
//	FLSnapshot_LoginAck() : FLPacket( SNAPSHOTTYPE_JOIN ), m_eResult( E_LOGIN_FAILURE ) {}
//
//protected:
//	virtual bool			BodySerialize( CAr & kAr ) const {		kAr << m_eResult;	return true;	}
//	virtual bool			BodyDeserialize( CAr & kAr )	{		kAr >> m_eResult;	return true;	}
//
//public:
//	E_LOGIN_RESULT			m_eResult;
//};

#pragma pack(pop)