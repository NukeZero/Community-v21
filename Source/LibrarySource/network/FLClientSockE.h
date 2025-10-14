#ifndef __FLCLIENTSOCKE_H__
#define __FLCLIENTSOCKE_H__


#include "FLNetworkCommon.h"
#include "FLSock.h"
#include "FLBuffer.h"
#include "FLStreamProtocol.h"
#include "../thread/FLCriticalSection.h"

class	CClientSockE : public CSock
{
public:
	CBuffer*			m_pRecvBuffer;

	WSAOVERLAPPED		m_ovRecv;
	WSAOVERLAPPED		m_ovSend;

	WSAEVENT			m_hRecv;
	WSAEVENT			m_hSend;
	HANDLE				m_hWorker;

	FLStreamProtocol*	m_pProtocol;

private:

	CMclCritSec			m_csSendBuffer;
	CBufferQueue		m_lspSendBuffer;

public:

	CClientSockE( FLStreamProtocol* pProtocol );
	virtual	~CClientSockE();

	bool	Connect( const char* lpAddr, WORD wPort );

	void	Attach( SOCKET hSocket );

	bool	Recv();
	bool	Fetch( DWORD dwBytes, CBufferQueue* pQueue );

	bool	Send( const char* lpData, u_long dwDataSize, DPID dpidTo );
	bool	SendRemnant( DWORD dwBytesSent );

	void	Close();
	bool	CloseConnection( SOCKET hSocket );

	WSAEVENT	GetRecvEvent();
	WSAEVENT	GetSendEvent();

	bool	StartProtocol();
	bool	ReadySyncProtocol();
	bool	WaitSyncProtocol( DWORD dwWaitTime );


private:

	bool	PostSend();

public:

	MEMPOOLER_DECLARE( CClientSockE );

};

#endif //__CLIENTSOCKE_H__