#ifndef __FLSERVERSOCK_H__
#define __FLSERVERSOCK_H__

#include "FLNetworkCommon.h"
#include "FLClientSock.h"
#include "FLServerSockBase.h"

class	CServerSock : public FLServerSockBase< CClientSock >
{
private:

	HANDLE*	m_phCompletionPort;
	u_long	m_uIoWorker;

public:

	explicit CServerSock( FLStreamProtocol* pProtocol );
	virtual	~CServerSock();

	bool	StartServer( const char* lpAddr, WORD wPort, HANDLE* phCompletionPort, u_long uIoWorker );

	void	OnAccept( SOCKET hSocket, DWORD dwAddr );
	bool	OnNewChild( SOCKET hSocket, DWORD dwAddr );

	HANDLE	GetCompletionPort( u_long iIoWorker )	const;
	int		GetIoWorkerCount()	const;

};


#endif //__SERVERSOCK_H__