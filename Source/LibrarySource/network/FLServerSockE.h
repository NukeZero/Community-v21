#ifndef __FLSERVERSOCKE_H__
#define __FLSERVERSOCKE_H__


#include "FLNetworkCommon.h"
#include "FLClientSockE.h"
#include "FLServerSockBase.h"

class CDPSock;
class CServerSockE : public FLServerSockBase< CClientSockE >
{
public:

	CDPSock*	m_pDPSock;

public:

	explicit CServerSockE( FLStreamProtocol* pProtocol );
	virtual	~CServerSockE();

	bool	StartServer( const char* lpAddr, WORD wPort, CDPSock* pDPSock );

	void	OnAccept( SOCKET hSocket, DWORD dwAddr );

};

#endif //__SERVERSOCKE_H__