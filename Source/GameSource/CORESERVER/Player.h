#ifndef __PLAYER_H__
#define __PLAYER_H__

#pragma once

#include <memory/FLMemPooler.h>
#include <rtmessenger.h>

enum TAG_RESULT 
{
	TAG_NOTFRIEND, 
	TAG_BLOCKED, 
	TAG_OK
};

class CPlayer
{
public:
	CMclCritSec		m_AccessLock;
	DWORD			m_dwSerial;
	DPID			dpid;
	DPID			dpidCache;
	DPID			dpidUser;
	u_long			uKey;						// uPlayerId
	CHAR			lpszPlayer[MAX_PLAYER];		// name
	CHAR			lpAddr[16];					// ip
	CHAR			lpszAccount[MAX_ACCOUNT];	// account
	CRTMessenger	m_RTMessenger;
	u_long			m_uPartyId;	
	u_long			m_idGuild;
	u_long			m_idWar;
	u_long			m_uIdofMulti;
	CTime			m_tGuildMember;
	u_long			m_idSnoop;

public:
	CPlayer( u_long idPlayer, const CHAR* pszPlayer, const CHAR* pszAccount );
	virtual	~CPlayer();

	void		Lock()		{ m_AccessLock.Enter();	}
	void		Unlock()	{ m_AccessLock.Leave();	}
	TAG_RESULT	IsTagSendable( u_long idTo );

public:

	MEMPOOLER_DECLARE( CPlayer );

};


typedef std::map< u_long, CPlayer* >		ULONG2PTR;

class CPlayerMng
{
public:
	CMclCritSec				m_AddRemoveLock;
	std::map< DWORD, CPlayer*>	m_players;			// serial value key
	ULONG2PTR				m_ulong2;			// player id key
	std::set<u_long>				m_set;				// set of operators
	u_long					m_uCount;			

public:
	CPlayerMng();
	virtual	~CPlayerMng();

	void		Free( void );
	BOOL		AddCache( DPID dpidCache );
	BOOL		RemoveCache( DPID dpidCache );
	u_long		GetCache( DPID dpidCache );
	BOOL		AddPlayer( u_long idPlayer, const CHAR* lpszPlayer, const CHAR* lpszAccount );
	void		RemovePlayer( CPlayer* pPlayer, BOOL bNotify = TRUE );
	BOOL		RegisterPlayerInfo( CPlayer* pPlayer );
	BOOL		UnregisterPlayerInfo( CPlayer* pPlayer, BOOL bNotify );
	CPlayer*	GetPlayerBySerial( DWORD dwSerial );
	CPlayer*	GetPlayerBySocket( DPID dpidSocket );
	CPlayer*	GetPlayer( u_long uKey );
	u_long		GetCount( void );
	void		PackName( CAr & ar );
	void		AddItToSetofOperator( u_long uPlayerId );
	BOOL	IsOperator( u_long idPlayer );

	void	Logout( CPlayer* pPlayer );
};


#endif	// __PLAYER_H__