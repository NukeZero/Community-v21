#ifndef __FLDPSOCK_H__
#define __FLDPSOCK_H__


#include "FLNetworkCommon.h"
#include "FLServerSock.h"
#include "FLClientSock.h"
#include "FLServerSockE.h"
#include "FLClientSockE.h"

#define	NEWSOCKETMSG	(DWORD)-3
#define	CLOSEIOWORKERMSG	(DWORD)-2
#define	INVALIDSOCKMSG	(DWORD)-4

class CDPSock
{
private:

	CSock*				m_pSock;

	char				m_lpAddr[32];
	WORD				m_wPort;

	u_long				m_uIoWorker;
	std::list<HANDLE>	m_listthread;
	CMclCritSec			m_locklistthread;

	HANDLE*				m_phCompletionPort;

	WSAEVENT			m_hClose;
	HANDLE				m_hRecv;

	CSock*				m_pSockThreaded;

public:

	CMclCritSec			m_csRecvBuffer;
	CBufferQueue		m_lspRecvBuffer;

	BOOL				m_fServer;
	long				m_lActiveIoWorker;

	FLStreamProtocol*	m_pProtocol;

private:
//	Operations
	BOOL	CreateIoWorker( u_long uIoWorker );
	BOOL	CloseIoWorker();
public:
//	Constructions
	CDPSock();
	virtual	~CDPSock();
//	Operations
	void	Close();
	void	CloseConnection( DPID dpid );
	BOOL	Shutdown( DPID dpid );
	HANDLE	GetCompletionPort( int iIoWorker );

	DWORD	GetDebugInfo( DPID dpid );

	HANDLE	GetCloseHandle();
	HANDLE	GetRecvHandle();
	CSock*	GetSockThreaded();
	int		GetIoWorkerCount();

	BOOL	CreateEventWorker( CSock* pSock );
	void	RemoveThread( HANDLE hThread );
	CClientSock*	Get( SOCKET hSocket );
	
	void	SetConnectionInfo( const char* szIP, WORD wPort );

	BOOL	JoinToServer( FLStreamProtocol* pProtocol, u_long uWaitingTime = 10000 );
	BOOL	JoinToServerE( FLStreamProtocol* pProtocol, u_long uWaitingTime = 10000 );
	BOOL	CreateServer( FLStreamProtocol* pProtocol );
	BOOL	CreateServerE( FLStreamProtocol* pProtocol );

	BOOL	Send( char* lpData, DWORD dwDataSize, DPID dpidTo );

	HRESULT	GetPlayerAddr( DPID dpid, char* lpAddr, size_t cchAddr );
	DWORD	GetPlayerAddr( DPID dpid );

	bool	GetHostAddr( char* lpAddr, size_t cchAddr );
	void	Receive( CBufferQueue* pQueue );

	void	AddCreatePlayerOrGroupMsg( DPID dpid );
	void	AddDestroyPlayerOrGroupMsg( DPID dpid );

	CSock*	GetServerSock( void )		{	return	( m_fServer? m_pSock: NULL );	}
};

inline HANDLE CDPSock::GetCompletionPort( int iIoWorker )
	{	return m_phCompletionPort[iIoWorker];	}
inline HANDLE CDPSock::GetCloseHandle( void )
	{	return m_hClose;	}
inline HANDLE CDPSock::GetRecvHandle( void )
	{	return m_hRecv;	}
inline int	CDPSock::GetIoWorkerCount( void )
	{	return (int)m_uIoWorker;	}
inline CSock* CDPSock::GetSockThreaded( void )
	{	return m_pSockThreaded;	}
inline CClientSock* CDPSock::Get( SOCKET hSocket )
	{	return ( m_pSock? (CClientSock*)m_pSock->Get( hSocket ): NULL );	}

inline bool	CDPSock::GetHostAddr( char* lpAddr, size_t cchAddr )
	{	return CSock::GetHostAddr( lpAddr, cchAddr );	}

inline HRESULT CDPSock::GetPlayerAddr( DPID dpid, char* lpAddr, size_t cchAddr )
	{	return m_pSock->GetPeerAddr( dpid, lpAddr, cchAddr );	}
inline DWORD CDPSock::GetPlayerAddr( DPID dpid )
	{	return m_pSock->GetPeerAddr( dpid );	}

#endif //__DPSOCK_H__