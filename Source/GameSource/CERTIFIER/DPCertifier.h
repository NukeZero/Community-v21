#ifndef __DPCERTIFIER_H__
#define	__DPCERTIFIER_H__

#include <network/FLDPMng.h>
#include <msghdr.h>
#include <misc.h>
#include <map>


// 초당 접속자를 제한한다.
class CLoginLimit
{
private:
	int		m_nLimitation;
	DWORD	m_dwTick;
	int		m_nRequestPerSecond;

public:
	CLoginLimit();
	bool	Check();
	void	SetLimitation( int nLimitation )
	{
		m_nLimitation = nLimitation;
	}
};

class CDPCertifier : public CDPMng
{
public:
	CLoginLimit		m_limit;

public:
//	Constructions
	CDPCertifier();
	virtual	~CDPCertifier();
// Operations
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid );
	void	SendCertifyAck( DPID dpid, DWORD dwAuthKey, BYTE cbAccountFlag, long lTimeSpan, const char* szGPotatoNo, const char* szCheck, const char* szBak, const int nGPotatoNo );

	void	SendError( LONG lError, DPID dpid );
	void	SendErrorString( const char* szError, DPID dpid );
	void	SendErrorCodeString( const LONG errorCode, const char* szError, DPID dpid );
	void	SendHdr( DWORD dwHdr, DPID dpid );
	void	SetLimitation( int nLimit );

	void	SendPacket( DWORD dwDPID, const FLPacket* pPacket );

	USES_PFNENTRIES( CDPCertifier, ( CAr & ar, DPID, LPBYTE, u_long ) );

	// Handlers
	void	OnAddConnection( DPID dpid );
	void	OnRemoveConnection( DPID dpid );
	void	OnCertify( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnPing( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnCloseExistingConnection( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnKeepAlive( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnError( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
};

inline void CDPCertifier::SendHdr( DWORD dwHdr, DPID dpid )
{
	BEFORESEND( ar, dwHdr );
	SEND( ar, this, dpid );
}


#endif	// __DPCERTIFIER_H__