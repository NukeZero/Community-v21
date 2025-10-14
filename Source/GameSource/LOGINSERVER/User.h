#ifndef __USER_H__
#define __USER_H__

#pragma once

#include <map>
#include <string>

#include <memory/FLMemPooler.h>
#include <network/FLDPMng.h>

//	mulcom	BEGIN100409	NPROTECT
//#include "ggsrv.h"
#ifdef __gameguard
#include <gameguard/ggsrv30.h>
#endif
//	mulcom	END100409	NPROTECT
#ifdef _xtrap
#include "../_Common/FLXTrap_Server.h"
#endif

#define	AUTH_PERIOD		300		// 5 min

class CUser
{
	//////////////////////////////////////////////////////////////////////////
	//	BEGIN100616	WARNINGCODE
	private:
		BYTE	m_byConnectLevel;		//	( ref. _CONNECT_LEVEL )

	public:
		BYTE	SetConnectLevel( BYTE byConnectLevel );
		BYTE	GetConnectLevel();
	//	END100616	WARNINGCODE
	//////////////////////////////////////////////////////////////////////////
public:
	DPID		m_dpid;
	DWORD		m_dwTime;
	int			m_nIndexOfCache;
	char		m_pKey[MAX_ACCOUNT];
	DWORD		m_idPlayer;
	DWORD		m_tPingRecvd;
	DWORD		m_dwAuthKey;
	BOOL		m_bIllegal;
#ifdef _gameguard
	CCSAuth3	m_csa;
#endif
	time_t	m_tAuth;
	int			m_nWrite;
	int			m_nRead;
	BOOL		m_bAuth;

#ifdef __xtrap
	FLXTrap_Server::FLSession	m_XTrapSession;
#endif

public:
	// Constructions
	CUser( DPID dpid );
	virtual	~CUser();

	void	SetExtra( const char* pKey, DWORD dwAuthKey );
	void	SetAuth( BOOL bAuth ) { m_bAuth = bAuth; }
	BOOL	GetAuth() { return m_bAuth; }

public:
	MEMPOOLER_DECLARE( CUser );
};

typedef std::map<DPID, CUser*>	C2User;

class CUserMng
{
private:
	C2User	m_dpid2User;
	std::map<std::string, CUser*>	m_ac2User;
	u_long	m_uCount;

public:
	CMclCritSec		m_AddRemoveLock;
	
public:
//	Constructions
	CUserMng();
	virtual	~CUserMng();
//	Operations

	void	Free( void );
	BOOL	AddUser( DPID dpid, CUser* pUser );
	BOOL	AddUser( const char* pKey, CUser* pUser );
	BOOL	RemoveUser( DPID dpid );
	CUser*	GetUser( const char* pKey );
	CUser*	GetUser( DPID dpid );
	u_long	GetCount( void );

	void	DestroyAbnormalPlayer( void );
	void	CheckAll();

	void	XTrapUpdate();
};

inline u_long CUserMng::GetCount( void )
{	
	CMclAutoLock	Lock( m_AddRemoveLock );
	return m_uCount;
}

#endif	// __USER_H__