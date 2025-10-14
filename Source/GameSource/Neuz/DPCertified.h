#ifndef __DPCERTIFIED_H__
#define	__DPCERTIFIED_H__

#include <network/FLDPMng.h>
#include "msghdr.h"
#include "misc.h"


class CDPCertified : public CDPMng
{
private:
	BOOL	m_fConn;
	CTimer	m_timer;
	LONG	m_lError;					// protocol error code 

public:
//	Constructions
	CDPCertified();
	virtual	~CDPCertified();

//	Overrides
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId );

//	Operations
	LONG	GetNetError();
	LONG	GetErrorCode()	{ return m_lError; }
//	void	Destroy( BOOL bDestroy );
	BOOL	IsDestroyed( void );
	BOOL	IsConnected( void );
	void	Ping( void );
	void	SendNewAccount( LPCSTR lpszAccount, LPCSTR lpszpw );
	void	SendHdr( DWORD dwHdr );
	void	SendCloseExistingConnection();
	BOOL	CheckNofityDisconnected();
	void	SendCertify();

	void	SendPacket( const FLPacket* pPacket );

	USES_PFNENTRIES( CDPCertified, ( CAr & ar, DPID ) );

	// Handlers
	void	OnCertifyAck( CAr & ar, DPID );
	void	OnServerList( CAr & ar, DPID );
	void	OnError( CAr & ar, DPID dpid );
	void	OnErrorString( CAr & ar, DPID dpid );
	void	OnKeepAlive( CAr & ar, DPID );
	void	OnErrorCodeString( CAr & ar, DPID dpid );
};

inline void CDPCertified::SendHdr( DWORD dwHdr )
{
	BEFORESEND( ar, dwHdr );
	SEND( ar, this, DPID_SERVERPLAYER );
}
inline void CDPCertified::Ping( void )
{
	if( m_timer.IsTimeOut() ) 
	{
		m_timer.Reset();
		SendHdr( PACKETTYPE_PING );
	}
}

inline BOOL CDPCertified::IsConnected( void )
{	
	return m_fConn;	
}

#endif	// __DPCERTIFIED_H__