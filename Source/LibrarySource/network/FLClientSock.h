#ifndef __FLCLIENTSOCK_H__
#define __FLCLIENTSOCK_H__


#include "FLNetworkCommon.h"
#include "FLSock.h"
#include "FLBuffer.h"
#include "FLStreamProtocol.h"

#include "../thread/FLCriticalSection.h"


class	CClientSock : public CSock
{
public:

	CBuffer*			m_pRecvBuffer;
	WSAOVERLAPPED		m_ovRecv;
	WSAOVERLAPPED		m_ovSend;

	FLStreamProtocol*	m_pProtocol;

	CMclCritSec			m_AccessLock;
	BOOL				m_bDoNotWrite;
	int					m_nReference;

private:

	CMclCritSec			m_csSendBuffer;
	CBufferQueue		m_lspSendBuffer;

public:

	explicit CClientSock( FLStreamProtocol* pProtocol );
	virtual	~CClientSock();

	bool	Connect( const char* lpAddr, WORD wPort );

	bool	Recv();
	bool	Fetch( DWORD dwBytes, CBufferQueue* pQueue );

	bool	Send( const char* lpData, u_long dwDataSize, DPID dpidTo );
	bool	SendRemnant( DWORD dwBytesSent );

	void	Close();
	bool	CloseConnection( SOCKET hSocket );

	bool	StartProtocol();
	bool	ReadySyncProtocol();
	bool	WaitSyncProtocol( DWORD dwWaitTime );

private:

	bool	PostSend();

public:

	MEMPOOLER_DECLARE( CClientSock );

};

#endif //__CLIENTSOCK_H__