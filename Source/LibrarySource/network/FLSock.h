#ifndef __FLSOCK_H__
#define __FLSOCK_H__



#include "FLNetworkCommon.h"


class CSock
{
protected:

	SOCKET	m_hSocket;

private:

	DPID	m_dpid;
	DPID	m_dpidpeer;

	DWORD	m_dwDbgInfo;

public:

	static	bool	GetHostAddr( char* lpAddr, size_t cchAddr );

	static	SOCKET	CreateSocket( int type = SOCK_STREAM );
	static	void	CloseSocket( SOCKET hSocket );

	CSock();
	virtual	~CSock();

	SOCKET	GetHandle();
	void	Clear();

	void	SetID( DPID dpid );
	DPID	GetID();
	void	SetPeerID( DPID dpid );
	DPID	GetPeerID();

	virtual	void	Attach(	SOCKET hSocket );
	virtual	void	Detach();

	virtual	void	Close()	= 0;
	virtual	bool	CloseConnection( SOCKET hSocket )	= 0;
	virtual	bool	Shutdown( SOCKET /*hSocket*/ ) { return false; }

	virtual	bool	GetPeerAddr( DPID dpid, char* lpAddr, size_t cchAddr );
	virtual	DWORD	GetPeerAddr( DPID dpid );

	virtual	bool	Send( const char* lpData, u_long dwDataSize, DPID dpidTo )	= 0;

	virtual	CSock*	Get( SOCKET hSocket );

	void	SetDebugInfo( DWORD dwError );
	DWORD	GetDebugInfo( SOCKET hSocket );

};


#endif //__SOCK_H__