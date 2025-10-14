#ifndef	__DPACCOUNTCLIENT_H__
#define	__DPACCOUNTCLIENT_H__

#include <network/FLDPMng.h>
#include <msghdr.h>


class CDPAccountClient : public CDPMng
{
public:

//	Constructions
	CDPAccountClient();
	virtual	~CDPAccountClient();
//	Overrides
	virtual	void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual	void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
//	Operations
	void	SendAddIdofServer( DWORD dwIdofServer );
	void	SendRemoveAccount( char* lpString );
	void	SendRemoveAllAccounts( void );
	void	SendBuyingInfo( PBUYING_INFO2 pbi2, SERIALNUMBER iSerialNumber = 0 );

//	Handlers
	USES_PFNENTRIES( CDPAccountClient, ( CAr & ar, LPBYTE lpBuf, u_long uBufSize ) );
	void	OnGetPlayerList( CAr & ar, LPBYTE, u_long );
	void	OnRemovePlayer( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnJoin( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnPlayerCount( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnCloseExistingConnection( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnOneHourNotify( CAr & ar, LPBYTE lpBuf, u_long uBufSize );
	void	OnFail( CAr & ar, LPBYTE, u_long );
	void	OnBuyingInfo( CAr & ar, LPBYTE lpBuf, u_long uBufSize );	// 2

	void	OnServerList( CAr & ar, LPBYTE, u_long );

public:
	void	SendPacket( const FLPacket* pPacket );

};

#endif	// __DPACCOUNTCLIENT_H__