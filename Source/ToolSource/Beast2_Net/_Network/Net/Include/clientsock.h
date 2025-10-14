#ifndef __CLIENTSOCK_H__
#define __CLIENTSOCK_H__

#pragma once

#include "sock.h"
#include "buffer.h"

#ifdef __CRC
#include "crc.h"
//using	namespace	CryptoPP;
using	namespace	std;
#endif	// __CRC


class CClientSock : public CSock
{
public:
	CBuffer*	m_pRecvBuffer;
	WSAOVERLAPPED	m_ovRecv;
	WSAOVERLAPPED	m_ovSend;

#ifdef __SOCKLIB0516
	BOOL	m_bDoNotWrite;
	CMclCritSec	m_AccessLock;
	int		m_nReference;
#endif	// __SOCKLIB0516
#ifdef __INFO_SOCKLIB0516
	DWORD	m_dwDbgInfo;
#endif	// __INFO_SOCKLIB0516

#ifdef __INFO_SOCKLIB0102
	DWORD	m_dwReadBytes;
	DWORD	m_dwReadPacks;
	time_t	m_tRead;
#endif	// __INFO_SOCKLIB0102
	
#ifdef __PROTOCOL0910
	DWORD	m_dwProtocolId;
	void	SendProtocolId( void );
	virtual	void	SetProtocolId( DWORD dwProtocolId )	{	m_dwProtocolId	= dwProtocolId;	};
	BOOL	IsCrcWrite( void )	{	return (BOOL)m_lspSendBuffer.m_pcrc;	}
#endif	// __PROTOCOL0910
	
private:
	CBufferQueue	m_lspSendBuffer;
	WSABUF	m_lpBuffers[MAX_WSABUF];
	BUFFER_TYPE		m_nBufferType;
#ifdef __CRC
	CRC32	m_crcRead;
	CRC32	m_crcWrite;
	DWORD	m_dwReadHeaderSize;
#endif	// __CRC

public:
//	Constructions
#ifdef __CRC
	CClientSock( DWORD dwcrc );
#else	// __CRC
	CClientSock( BUFFER_TYPE type );
#endif	// __CRC
	virtual	~CClientSock();
//	Operations
	CBuffer* CreateBuffer();

	BOOL	Connect( char* lpAddr, u_short uPort );
	int		Recv( void );
	int		SendRemnant( DWORD dwBytesSent );
	CBuffer*	Fetch( DWORD dwBytes );
	
	virtual	void	Close( void );
	virtual	BOOL	CloseConnection( SOCKET hSocket );
	virtual	BOOL	Shutdown( SOCKET hSocket );
#ifdef __INFO_SOCKLIB0516
	virtual	DWORD	GetDebugInfo( SOCKET hSocket );
#endif	// __INFO_SOCKLIB0516
	virtual	void	Send( char* lpData, DWORD dwDataSize, DPID dpidTo );
	virtual	HRESULT	GetPeerAddr( DPID dpid, LPVOID lpAddr, LPDWORD lpdwSize );
	virtual	DWORD	GetPeerAddr( DPID dpid ) { return m_dwAddr; }
	DWORD	m_dwAddr;

public:
static	\
//CMemPool<CClientSock>*	m_pPool;
MemPooler<CClientSock>*	m_pPool;
public:
	void*	operator new( size_t nSize )	{	return CClientSock::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CClientSock::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CClientSock::m_pPool->Free( (CClientSock*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CClientSock::m_pPool->Free( (CClientSock*)lpMem );	}
};

#endif //__CLIENTSOCK_H__