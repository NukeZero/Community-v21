#ifndef __SERVERSOCK_H__
#define __SERVERSOCK_H__

#pragma once

#include "clientsock.h"
#include "mymap.h"

#ifdef __NEWDPMNG
typedef struct _socklist : public OVERLAPPED
{
	SOCKET	sock;
	int		iIndex;
}	Socklist, *PSocklist;
#define	MAX_IO_PEND	128		// maximum pending I/0 requests
#define	PADDED_ADDR_SIZE	sizeof(SOCKADDR_IN)+16
#define	MAX_BYTES	2000
#endif	// __NEWDPMNG

class CServerSock : public CSock  
{
public:
#ifndef __NEWDPMNG
	HANDLE	m_hListenThread;
	HANDLE	m_hClose;
	WSAEVENT	m_hListen;
#else	// __NEWDPMNG
	HANDLE	m_hAcptThread;
	HANDLE	m_hIocpAcpt;
	Socklist	m_socklist[MAX_IO_PEND];
	BOOL	AcptRequest( int iIndex );
#endif	// __NEWDPMNG
	CMyMap<CClientSock*>	m_mapChild;
#ifdef __SOCKLIB0516
	HANDLE* m_phCompletionPort;
#else	// __SOCKLIB0516
	HANDLE	m_hCompletionPort;
#endif	// __SOCKLIB0516
	u_long	m_uIoWorker;
#ifdef __CRC
	DWORD	m_dwcrc;
#endif	// __CRC
	BUFFER_TYPE		m_nBufferType;

public:
//	Constructions
#ifdef __CRC
	CServerSock( DWORD dwcrc );
#else	// __CRC
	CServerSock( BUFFER_TYPE nBufferType );
#endif	// __CRC
	virtual	~CServerSock();
//	Operations
	BOOL	Listen( void );

	HANDLE	GetListenHandle( void );
	HANDLE	GetCloseHandle( void );

#ifdef __SOCKLIB0516
	HANDLE	GetCompletionPort( int iIoWorker );
	BOOL	StartServer( HANDLE* phCompletionPort, u_long uIoWorker );
#else	// __SOCKLIB0516
	HANDLE	GetCompletionPort( void );
	BOOL	StartServer( HANDLE hCompletionPort, u_long uIoWorker );
#endif	// __SOCKLIB0516
	int		GetIoWorkerCount( void );
	CClientSock*	AddChild( SOCKET hSocket );
	BOOL	CloseChild( SOCKET hSocket );
	void	CloseAllChild( void );
#ifdef __DOS1101
	void	ShutdownAddr( DWORD dwAddr );
#endif	// __DOS1101
	HRESULT	GetPeerAddr( SOCKET hSocket, u_long* paddr );

//	Override
	virtual	void	Close( void );
	virtual	BOOL	CloseConnection( SOCKET hSocket );
	virtual	BOOL	Shutdown( SOCKET hSocket );
#ifdef __INFO_SOCKLIB0516
	virtual	DWORD	GetDebugInfo( SOCKET hSocket );
#endif	// __INFO_SOCKLIB0516
	virtual	void	Send( char* lpData, DWORD dwDataSize, DPID dpidTo );
	virtual	HRESULT	GetPeerAddr( DPID dpid, LPVOID lpAddr, LPDWORD lpdwSize );
	virtual DWORD	GetPeerAddr( DPID dpid );
	virtual	CSock*	Get( SOCKET hSocket );
};

#ifndef __NEWDPMNG
inline HANDLE CServerSock::GetListenHandle( void )	{	return m_hListen; }
inline HANDLE CServerSock::GetCloseHandle( void )	{	return m_hClose;	}
#endif	// __NEWDPMNG

#ifdef __SOCKLIB0516
inline HANDLE CServerSock::GetCompletionPort( int iIoWorker )	{	ASSERT( m_phCompletionPort );	return m_phCompletionPort[iIoWorker];	}
#else	// __SOCKLIB0516
inline HANDLE CServerSock::GetCompletionPort( void )	{	return m_hCompletionPort;	}
#endif	// __SOCKLIB0516

inline int CServerSock::GetIoWorkerCount( void )	{	return m_uIoWorker;	}

inline CSock* CServerSock::Get( SOCKET hSocket )
{
	CClientSock* pChild;
	if( m_mapChild.Lookup( hSocket, pChild ) )
		return pChild;
	return NULL;
}

#endif //__SERVERSOCK_H__