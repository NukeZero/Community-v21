// AccountMgr.cpp: implementation of the CAccountMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AccountMgr.h"
#include <..\Resource\lang.h>

//////////////////////////////////////////////////////////////////////

UINT _HashKey( const char* key )
{
	UINT nHash = 0;
	while( *key )
		nHash = (nHash<<5) + nHash + *key++;
	return nHash;
}

//////////////////////////////////////////////////////////////////////

CAccountMgr::CAccountMgr()
{
	//////////////////////////////////////////////////////////////////////////
	//	BEGIN100616	Data Initialize
	m_cache.clear();
	//	END100616	Data Initialize
	//////////////////////////////////////////////////////////////////////////
}

CAccountMgr::~CAccountMgr()
{
	std::list< ACCOUNT_CACHE* >::iterator it;
	for( it = m_cache.begin(); it != m_cache.end(); ++it )
		SAFE_DELETE( *it );
}

//////////////////////////////////////////////////////////////////////

ACCOUNT_CHECK CAccountMgr::Check( const AUTH_INFO & kAuthInfo )
{
	time_t	tmCur = time( NULL );

	// 캐쉬에서 찾는다.
	std::list< ACCOUNT_CACHE* >::iterator it;
	for( it = m_cache.begin(); it != m_cache.end(); ++it )
	{
		ACCOUNT_CACHE* pInfo = *it;
		if( pInfo->m_dwIP == kAuthInfo.dwIP && _tcscmp( pInfo->szAccount, kAuthInfo.AccountInfo.szAccount ) == 0 )
		{
			ACCOUNT_CHECK result = CHECK_OK;

			if( pInfo->m_nError >= 3 )
			{
				time_t nSec = tmCur - pInfo->m_tmError;
				if( nSec <= (15 * 60) )
					result = CHECK_3TIMES_ERROR;
			}
			else if( pInfo->m_nError >= 1 && g_xFlyffConfig->GetMainLanguage() != LANG_KOR )
			{
				time_t nSec = tmCur - pInfo->m_tmError;
				if( nSec <= 15 )
					result = CHECK_1TIMES_ERROR;
			}

			m_cache.erase( it );
			m_cache.push_front( pInfo );		// 사용된 것은 앞으로 
			return result;
		}
	}
	
	// MAX보다 크면 - LRU를 재사용 
	//     or       - new로 넣는다. 
	ACCOUNT_CACHE* pInfo = NULL;
	if( m_cache.size() < 3 )
		pInfo = new ACCOUNT_CACHE;
	else
	{
		//////////////////////////////////////////////////////////////////////////
		// 15분이 지나지 않은 캐시를 재사용할 가능성이 있다.
		// 요구사항을 정확하게 만족하지 않는다. 의도적인지 잘모르겠다.
		pInfo = m_cache.back();					// 가장 덜 사용된 것은 맨 뒤에 남아 있다.
		m_cache.pop_back();
	}

	pInfo->m_nError  = 0;
	pInfo->m_dwIP    = kAuthInfo.dwIP;
	pInfo->m_tmError = tmCur;
	FLStrcpy( pInfo->szAccount, _countof( pInfo->szAccount ), kAuthInfo.AccountInfo.szAccount );

	m_cache.push_front( pInfo );

	return CHECK_OK;
}

void	CAccountMgr::IncErrorCount()
{
	if( g_xFlyffConfig->IsFWCCertifierIP() == true )
	{
		return;
	}
	// 에러를 낸 클라이언트가 m_cache의 맨 처음에 있다는 가정이 깔려있다.
	// 스레드당 한개의 AccountMgr이 존재하기 때문에 이상없다.
	if( m_cache.empty() == false )
	{
		ACCOUNT_CACHE* pInfo = m_cache.front();		// MRU

		++pInfo->m_nError;
		pInfo->m_tmError = time( NULL );
	}
}

void	CAccountMgr::ResetErrorCount()
{
	// 에러를 낸 클라이언트가 m_cache의 맨 처음에 있다는 가정이 깔려있다.
	// 스레드당 한개의 AccountMgr이 존재하기 때문에 이상없다.
	if( m_cache.empty() == false )
	{
		ACCOUNT_CACHE* pInfo = m_cache.front();		// MRU

		pInfo->m_nError = 0; 
		pInfo->m_tmError = time( NULL );
	}
}
