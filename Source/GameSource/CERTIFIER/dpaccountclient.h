#ifndef __DPACCOUNTCLIENT_H__
#define	__DPACCCOUNTCLIENT_H__

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

	void SendAddAccount( char* lpAddr, LPCTSTR lpszAccount, BYTE cbAccountFlag, DPID idFrom, int fCheck, const char* szCheck, const char* szBak, DWORD dwPCBangClass, const CTime tmBirthDate );

	void	SendRemoveAccount( DPID idFrom );
	void	SendRoute( DWORD dwIdofServer, u_long uIdofMulti, DPID idFrom );
	void	SendPing( DPID idFrom );
	void	SendCloseExistingConnection( const char* lpszAccount );
	void	SendPacket( DWORD dwDPID, const FLPacket* pPacket );

	USES_PFNENTRIES( CDPAccountClient, ( CAr & ar, DPID ) );
//	Handlers
	void	OnAddAccount( CAr & ar, DPID dpid );
	void	OnDestroyPlayer( CAr & ar, DPID dpid );
	void	OnServerList( CAr & ar, DPID dpid );
	void	OnPlayerCount( CAr & ar, DPID dpid );

	void	OnServerConnectionMode( CAr & ar, DPID dpid );

	void	OnLogoutAccount( CAr & ar, DPID dpid );
};

#endif	// __DPACCOUNTCLIENT_H__