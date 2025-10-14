
#include "stdafx.h"
#include "FLAuthWait.h"


FLAuthWait::FLAuthWait()
{
}

FLAuthWait::~FLAuthWait()
{
}

bool	FLAuthWait::Register( AUTH_INFO* pAuthInfo )
{
	if( IsValid( pAuthInfo ) == false )
	{
		return false;
	}

	T_AUTH_WAIT tWait;
	tWait.dwRegisterTick = ::GetTickCount();
	tWait.AuthInfo = *pAuthInfo;
	m_listWait.push_back( tWait );
	return true;
}

bool	FLAuthWait::Unregister( const char* szAccount, AUTH_INFO* pAuthInfo )
{
	AUTH_WAIT_LIST::iterator pos = m_listWait.begin();
	AUTH_WAIT_LIST::iterator end = m_listWait.end();

	for( ; pos != end; ++pos )
	{
		T_AUTH_WAIT& wait = (*pos);
		if( _stricmp( wait.AuthInfo.AccountInfo.szAccount, szAccount ) == 0 )
		{
			*pAuthInfo = wait.AuthInfo;
			m_listWait.erase( pos );
			return true;
		}
	}

	return false;
}

bool	FLAuthWait::IsValid( AUTH_INFO* pAuthInfo )
{
	AUTH_WAIT_LIST::iterator pos = m_listWait.begin();
	AUTH_WAIT_LIST::iterator end = m_listWait.end();

	for( ; pos != end; ++pos )
	{
		T_AUTH_WAIT& wait = (*pos);
		if( _stricmp( wait.AuthInfo.AccountInfo.szAccount, pAuthInfo->AccountInfo.szAccount ) == 0 )
		{
			return false;
		}
	}

	return true;
}

void	FLAuthWait::TimeoutProcess( TIMEOUT_HANDLER pFunction, void* pParam )
{
	DWORD dwNowTick = ::GetTickCount();

	AUTH_WAIT_LIST::iterator pos = m_listWait.begin();
	AUTH_WAIT_LIST::iterator end = m_listWait.end();

	for( ; pos != end; )
	{
		T_AUTH_WAIT& wait = (*pos);
		if( (dwNowTick - wait.dwRegisterTick) >= SEC( TIMEOUT_SEC ) )
		{
			pFunction( pParam, &wait.AuthInfo );
			m_listWait.erase( pos++ );
		}
		else
		{
			++pos;
		}
	}
}
