#ifndef __DPSRVR_H__
#define	__DPSRVR_H__

#include <network/FLDPMng.h>
#include <msghdr.h>
#include <misc.h>
#include <map>


#define MAX_IP 10240

typedef std::map<std::string, int>	STRING2INT;

class CDPSrvr : public CDPMng
{
public:
	bool	m_bCheckAddr;		// 접속하는 account의 address를 검사해야 하는가?
	int		m_nMaxConn;
	BOOL	m_bReloadPro;

// 	CMclCritSec		m_csAddrPmttd;
// 	char	m_sAddrPmttd[128][16];
// 	int		m_nSizeofAddrPmttd;
	

	CMclCritSec		m_csIPCut;
	STRING2INT m_sIPCut;
	int		m_nIPCut[MAX_IP][3];
	int		m_nSizeofIPCut;

public:
//	Constructions
	CDPSrvr();
	virtual	~CDPSrvr();
//	Overrides
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
// Operations
	void	CloseExistingConnection( LPCTSTR lpszAccount, LONG lError );
//	bool	LoadAddrPmttd( LPCTSTR lpszFileName );
	BOOL	LoadIPCut( LPCSTR lpszFileName );
	void	DestroyPlayer( DPID dpid1, DPID dpid2 );

	void	OnAddConnection( DPID dpid );
	void	OnRemoveConnection( DPID dpid );

	void	SendPlayerCount( DWORD dwParentID, DWORD dwMyID, long lConnectionCount );
	void	SendAccountLogout( const char* szAccount );

	void	RemoteServerConnectionMode( DWORD dwParentID, DWORD dwMyID, long lCanConnection );
	void	SendServerConnectionMode( DWORD dwParentID, DWORD dwMyID, long lCanConnection );

	void	OnAfterChecking( BYTE f, LPCTSTR lpszAccount, DPID dpid1, DPID dpid2, DWORD dwAuthKey, BYTE cbAccountFlag, time_t lTimeSpan, const char* szCheck, const char* szBak, int nPermitip);

	BOOL	IsABClass( LPCSTR lpszIP );
	void	GetABCClasstoString( LPCSTR lpszIP, char * lpABClass, int &nCClass );
	void	InitIPCut( void );

	void	SendPacket( DPID dwDPID, const FLPacket* pPacket );

//	Handlers
	USES_PFNENTRIES( CDPSrvr, ( CAr & ar, DPID, DPID ) );
	void	OnAddAccount( CAr & ar, DPID dpid1, DPID dpid2 );
	void	OnRemoveAccount( CAr & ar, DPID dpid1, DPID dpid2 );
	void	OnPing( CAr & ar, DPID dpid1, DPID dpid2 );
	void	OnCloseExistingConnection( CAr & ar, DPID dpid1, DPID dpid2 );
};

#endif	// __DPCRTFRSRVR_H__