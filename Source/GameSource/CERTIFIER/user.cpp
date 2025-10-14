#include "stdafx.h"
#include "user.h"
#include <msghdr.h>
#ifdef _DEBUG
#include <logging/FLMyTrace.h>
extern	FLMyTrace	g_MyTrace;
#endif	// _DEBUG

//////////////////////////////////////////////////////////////////////////
CUser::CUser( DPID dpid ) :
	m_dpid( dpid ),
	m_dwTick( GetTickCount() ),
	m_bValid( TRUE ),
	nGPotatoNo ( 0 )
{
	m_pszAccount[0]	= '\0';
}

CUser::~CUser()
{
}

void CUser::SetAccount( const char* szAccount )
{
	FLStrncpy( m_pszAccount, _countof( m_pszAccount ), szAccount, MAX_ACCOUNT );
}

const char*	CUser::GetAccount()
{
	return m_pszAccount;
}
//////////////////////////////////////////////////////////////////////////


CUserMng::CUserMng()
{
}

CUserMng::~CUserMng()
{
	CMclAutoLock	Lock( m_AddRemoveLock );

	for( std::map<DPID, CUser*>::iterator i	= begin(); i != end(); ++i )
	{
		SAFE_DELETE( i->second );
	}

	clear();
}

BOOL CUserMng::AddUser( DPID dpid )
{
	CUser* pUser	= new CUser( dpid );

	CMclAutoLock	Lock( m_AddRemoveLock );

	bool bResult	= insert( std::map<DPID, CUser*>::value_type( dpid, pUser ) ).second;
	if( bResult == false )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "ADD//0" ) );
		SAFE_DELETE( pUser );
		return FALSE;
	}

#ifdef _DEBUG
	g_MyTrace.Add( FLMyTrace::Key( "count" ), FALSE, "// %04d", size() );
#endif	// _DEBUG
	return TRUE;
}

BOOL CUserMng::RemoveUser( DPID dpid )
{
	CMclAutoLock	Lock( m_AddRemoveLock );

	std::map<DPID, CUser*>::iterator i	= find( dpid );
	if( i != end() )
	{
		CUser* pRemoved		= i->second;
		SAFE_DELETE( pRemoved );
		erase( i );

#ifdef _DEBUG
	g_MyTrace.Add( FLMyTrace::Key( "count" ), FALSE, "// %04d", size() );
#endif	// _DEBUG
		return TRUE;
	}

	FLERROR_LOG( PROGRAM_NAME, _T( "REMOVE//0" ) );
	return FALSE;
}

CUser* CUserMng::GetUser( DPID dpid )
{
	std::map<DPID, CUser*>::iterator i	= find( dpid );
	if( i != end() )
	{
		return i->second;
	}

	return NULL;
}

void CUserMng::ClearDum( CDPMng* pdp )
{
	BEFORESEND( ar, PACKETTYPE_KEEP_ALIVE );

	DWORD dwTick	= GetTickCount() - SEC( 10 );

	CMclAutoLock	Lock( m_AddRemoveLock );

	for( std::map<DPID, CUser*>::iterator i = begin(); i != end(); ++i )
	{
		CUser* pUsertmp	= i->second;
		if( pUsertmp->m_dwTick < dwTick )
		{
			pdp->DestroyPlayer( pUsertmp->m_dpid );
		}
		else if( pUsertmp->m_bValid == FALSE )
		{
			pUsertmp->m_bValid	= TRUE;	// ¿ä°Ç ¿Ö?
			pdp->DestroyPlayer( pUsertmp->m_dpid );
		}
		else
		{
			pUsertmp->m_bValid	= FALSE;
			SEND( ar, pdp, pUsertmp->m_dpid );
		}
	}
}

CUserMng* CUserMng::GetInstance( void )
{
	static	CUserMng	sUserMng;
	return	&sUserMng;
}