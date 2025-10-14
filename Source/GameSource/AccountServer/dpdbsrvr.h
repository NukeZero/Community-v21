#ifndef __DPDBSRVR_H__
#define	__DPDBSRVR_H__


#include <network/FLDPMng.h>
#include <msghdr.h>

class CAccount;

class CDPDBSrvr : public CDPMng
{
public:
//	Constructions
	CDPDBSrvr();
	virtual	~CDPDBSrvr();

//	Overrides
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId );

// Operations
	void	SendOneHourNotify( CAccount* pAccount );
	void	SendPlayerCount( void );
	void	OnRemoveConnection( DPID dpid );
	void	SendCloseExistingConnection( const char*lpszAccount, LONG lError );
	void	SendFail( long lError, const char* lpszAccount, DPID dpid );
	void	SendBuyingInfo( PBUYING_INFO2 pbi2 );

	void	SendPacket( DPID dwDPID, const FLPacket* pPacket );


	std::vector<CString>	m_vecstrChannelAccount;

//	Handlers
	USES_PFNENTRIES( CDPDBSrvr, ( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize ) );

	void	OnAddConnection( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnRemoveAccount( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnGetPlayerList( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnRemovePlayer( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnJoin( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnRemoveAllAccounts( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnBuyingInfo( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnServerEnable( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );



};

#endif	// __DPDBSRVR_H__