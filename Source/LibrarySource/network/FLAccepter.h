
#ifndef	__FLACCEPTER_H__
#define	__FLACCEPTER_H__


#include "FLNetworkCommon.h"
#include "FLSock.h"


class	FLAcceptHandler
{
public:

	virtual void	OnAccept( SOCKET hSocket, DWORD dwAddr )	= 0;

};

class	FLAccepter
{
public:

	FLAccepter();
	~FLAccepter();

	bool	Create( CSock* pSock, const char* lpAddr, WORD wPort );
	void	Destroy();

	bool	Start( FLAcceptHandler* pHandler );
	void	Stop();

	void	DoAccept();
	void	OnAccept( SOCKET hSocket, DWORD dwAddr );

private:

	bool	Listen( const char* lpAddr, WORD wPort );

private:

	HANDLE			m_hListenThread;

	HANDLE			m_hClose;
	WSAEVENT		m_hListen;

	SOCKET			m_hListenSocket;

	FLAcceptHandler* m_pHandler;

};







#endif
