#ifndef __FLDPMNG_H__
#define __FLDPMNG_H__


#include "FLNetworkCommon.h"
#include "FLDPSock.h"
#include "FLStreamProtocol.h"

#include "FLAr.h"


extern	BOOL	InitializeNetLib();
extern	void	UninitializeNetLib();


#define BEFORESEND( ar, dw )	\
	CAr ar;	\
	u_long nBufSize;	\
	ar << dw;

#define	BEFORESENDSOLE( ar, dw, dpid_ )	\
	CAr ar;		\
	u_long nBufSize;	\
	ar << dpid_ << dw;

#define BEFORESENDDUAL( ar, dw, dpid_, dpid__ )	\
	CAr ar;		\
	u_long nBufSize;	\
	ar << dpid_ << dpid__ << dw;

#define	SEND( ar, pDPMng, idTo ) \
	LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );	\
	(pDPMng)->Send( lpBuf, nBufSize, idTo );

#define GETTYPE(ar)		\
	DWORD dw;	\
	ar >> dw;


#define	USES_PFNENTRIES( theClass, theParameters )	\
	private:	\
	typedef	theClass	thisClass;	\
	typedef	void (theClass::*PACKET_HANDLER_FUNC) theParameters ;	\
	std::map<DWORD, PACKET_HANDLER_FUNC >	m_pfnEntries;	\
	PACKET_HANDLER_FUNC theClass::GetHandler( DWORD dwType ) 	\
	{	\
		std::map< DWORD, PACKET_HANDLER_FUNC >::iterator i = m_pfnEntries.find( dwType );	\
		if( i != m_pfnEntries.end() )	\
			return i->second;	\
		return NULL;	\
	}

#define	ON_MSG( dwKey, hndlr )	\
	m_pfnEntries.insert( std::map< DWORD, PACKET_HANDLER_FUNC >::value_type( dwKey, &thisClass::hndlr ) );
//#define	ON_MSG2( dwKey, hndlr )	\
//	m_pfnEntries.insert( std::map< DWORD, PACKET_HANDLER_FUNC >::value_type( dwKey, &thisClass::hndlr ) );


class CDPMng
{
private:
	CDPSock*	m_pDPSock;
	HANDLE	m_hRecvThread;
	HANDLE	m_hClose;

	FLStreamProtocol*	m_pProtocol;

public:
//	Constructions
	CDPMng();
	virtual	~CDPMng();
//	Operations
	HANDLE	GetRecvHandle( void );
	HANDLE	GetCloseHandle( void );
	BOOL	CreateDPObject( FLStreamProtocol* pProtocol, BOOL bSingleThreaded = FALSE );
	BOOL	DeleteDPObject( void );
	void	SetConnectionInfo( const char* szIP, WORD wPort = 0 );
	BOOL	ReceiveMessage( void );

	BOOL	JoinSession( FLStreamProtocol* pProtocol, u_long uWaitingTime );
	BOOL	ConnectToServer( LPCSTR lpConnection, WORD wPort, FLStreamProtocol* pProtocol, BOOL bSingleThreaded = FALSE, u_long uWaitingTime = 10000 );

	BOOL	CreateSession( FLStreamProtocol* pProtocol );
	BOOL	StartServer( const char* szIP, WORD wPort, FLStreamProtocol* pProtocol, BOOL bSingleThreaded = FALSE );

	BOOL	Send( LPVOID lpData, DWORD dwDataSize, DPID dpidTo );
	BOOL	DestroyPlayer( DPID dpid );

	DWORD	GetDebugInfo( DPID dpid );

	void	GetHostAddr( LPSTR lpAddr, size_t cchAddr );
	void	GetPlayerAddr( DPID dpid, char* lpAddr, size_t cchAddr );
	DWORD	GetPlayerAddr( DPID dpid );


//	Override
	virtual	void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )		= 0;
	virtual	void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom )	= 0;
};

inline HANDLE CDPMng::GetRecvHandle( void )
{	
	return	m_pDPSock->GetRecvHandle();	
}

inline HANDLE CDPMng::GetCloseHandle( void )
{	
	return m_hClose;	
}

inline BOOL	CDPMng::ConnectToServer( LPCSTR lpConnection, WORD wPort, FLStreamProtocol* pProtocol, BOOL bSingleThreaded, u_long uWaitingTime )
{
	if( CreateDPObject( pProtocol, bSingleThreaded ) == TRUE )
	{
		SetConnectionInfo( lpConnection, wPort );
		if( JoinSession( pProtocol, uWaitingTime ) == TRUE )
		{
			return TRUE;
		}
	}

	return FALSE;
}

inline BOOL CDPMng::StartServer( const char* szIP, WORD wPort, FLStreamProtocol* pProtocol, BOOL bSingleThreaded )
{
	if( CreateDPObject( pProtocol, bSingleThreaded ) == TRUE )
	{
		SetConnectionInfo( szIP, wPort );
		if( CreateSession( pProtocol ) == TRUE )
		{
			return TRUE;
		}
	}

	return FALSE;
}

inline BOOL CDPMng::Send( LPVOID lpData, DWORD dwDataSize, DPID dpidTo )
{	
	if( !m_pDPSock )
		return FALSE;
	return m_pDPSock->Send( (char*)lpData, dwDataSize, dpidTo );
}

#endif //__DPMNG_H__