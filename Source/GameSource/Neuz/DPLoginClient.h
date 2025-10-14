#ifndef __DPLOGINCLIENT_H__
#define __DPLOGINCLIENT_H__

#include <network/FLDPMng.h>
#include "MsgHdr.h"

class CDPLoginClient : public CDPMng
{
private:
	BOOL	m_fDestroy; 
	BOOL	m_fConn;
	CTimer	m_timer;
	BOOL	m_bShowDisconnectMsg;
	LONG	m_lError;
	u_long	m_idNumberPad;
public:
	int		m_nSlot;

//	Constructions
	CDPLoginClient();
	virtual	~CDPLoginClient();
//	Override
	virtual	void SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );

//	Operations
	LONG	GetErrorCode() { return m_lError; }
	LONG	GetNetError();
	void	Destroy( BOOL fDestroy );
	BOOL	IsDestroyed( void );
	BOOL	IsConnected( void );
	void	Ping( void );
	void	QueryTickCount( void );
	void	SendGetPlayerList( DWORD dwID, LPCSTR lpszAccount, LPCSTR lpszpw, BYTE bySelectPage = 0 );
	void	SendCreatePlayer( BYTE nSlot, LPCSTR lpszPlayer/*, LPDWORD adwEquipment*/, BYTE nFace, BYTE nCostume, BYTE nSkinSet, BYTE nHairMesh, DWORD dwHairColor, BYTE nSex, BYTE nJob, BYTE nHeadMesh, int nBankPW, BYTE bySelectPage = 0 );
	void	SendDeletePlayer( BYTE nSlot, LPCTSTR szNo, BYTE bySelectPage = 0 );
	void	SendPreJoin( const TCHAR* lpszAccount, u_long idPlayer, const TCHAR* lpszPlayer, int nSlot, int nSecretNum = 0 );
	void	OnPreJoin( CAr & ar );

	void	RemoteShowDisconnectMsg( BOOL bShow );
#ifdef PASSWORD_RESET_2ND
	void	SendChangeBankPass( const char* szName, const char *szNewPass, u_long uidPlayer );
#endif // PASSWORD_RESET_2ND
//	Handlers
	USES_PFNENTRIES( CDPLoginClient, ( CAr & ar ) );
	void	OnPlayerList( CAr & ar );
	void	OnError( CAr & ar );
	void	OnCacheAddr( CAr & ar );
	void	OnQueryTickCount( CAr & ar );
	void	OnOneHourNotify( CAr & ar );

	void	OnPing( CAr & ar );
	void	OnAuthQuery( CAr & ar );
#ifdef __gameguard
	void	OnAuthQueryVer30( CAr & ar );
#endif

	void	OnLoginNumPad( CAr & ar );
	void	OnLoginProtect( CAr & ar );

#ifdef _USE_XTRAP
	void	OnXTrapCommandRequest( CAr & ar );
#endif //_USE_XTRAP

public:
	u_long	GetNumberPad( void ) const;
};

inline void CDPLoginClient::Destroy( BOOL fDestroy )
	{	m_fDestroy	= fDestroy;	}
inline BOOL CDPLoginClient::IsDestroyed( void )
	{	return( m_fDestroy );	}
inline BOOL CDPLoginClient::IsConnected( void )
	{	return( m_fConn );	}

inline void CDPLoginClient::Ping( void )
{
	if( m_timer.IsTimeOut() ) {
		m_timer.Reset();
		BEFORESENDSOLE( ar, PACKETTYPE_PING, DPID_UNKNOWN );
		DWORD dwPingTime = GetCurrentTime();
		ar << dwPingTime;
		++g_Neuz.m_nPingCount;
		if( g_Neuz.m_nPingCount > 1)
		{
			g_Neuz.m_dwPingTime = 0;
		}
		SEND( ar, this, DPID_SERVERPLAYER );
	}

	if( m_fDestroy ) {
		DeleteDPObject();
		m_fDestroy	= FALSE;
	}
}
#endif	// __DPLOGINCLIENT_H__