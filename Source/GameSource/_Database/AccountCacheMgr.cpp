#include "stdafx.h"
#include "..\_network\objects\Obj.h"
#include "dbmanager.h"
#include "AccountCacheMgr.h"
#include <algorithm>

#ifdef __INTERNALSERVER
	const int MAX_CACHE_ALLOC = 100;
#else
	const int MAX_CACHE_ALLOC = 5000;
#endif


// TODO: list와 map을 두개 두고 하지 말고, 하나로 하는 것이 좋을 것 같다.
//       그리고, m_2AccountPtr도 제거하자. ( 찾은 것을 앞으로 두면 검색도 빠를 수 있을 것 같다. )
CAccountCacheMgr::CAccountCacheMgr()
{
	m_nAlloc = 0;
}

ACCOUNT_CACHE* CAccountCacheMgr::Find( LPCTSTR szAccount )
{
	std::map<std::string, ACCOUNT_CACHE*>::iterator it = m_2Account.find( szAccount );
	if( it != m_2Account.end() )
		return it->second;
	else
		return NULL;
}

void CAccountCacheMgr::Clear()
{
	ACCOUNT_CACHE* AccountCache;
	for( std::map<std::string, ACCOUNT_CACHE*>::iterator i = m_2Account.begin(); i != m_2Account.end(); ++i )
	{
		AccountCache = i->second;
		AccountCache->Clear();
		SAFE_DELETE( AccountCache );
	}
	m_2Account.clear();

	m_list.clear();
	m_nAlloc = 0;
}

void CAccountCacheMgr::AddMover( u_long idPlayer, LPCTSTR szAccount, int nSlot )
{
	ACCOUNT_CACHE* pCache = Find( szAccount );
	if( pCache )
		pCache->ClearMover( static_cast< BYTE >( nSlot ), idPlayer );
}


void CAccountCacheMgr::RemoveMover( u_long idPlayer, LPCTSTR szAccount )
{
	ACCOUNT_CACHE* AccountCache = Find( szAccount );
	if( AccountCache )
	{
		for( BYTE i = 0; i < 3; ++i )
		{
			if( AccountCache->m_idPlayerBank[i] == idPlayer )
			{
				AccountCache->ClearMover( i, 0 );
				break;
			}
		}
	}
}

void CAccountCacheMgr::AddAccount( u_long /*idPlayer*/, LPCTSTR szAccount, BOOL bCache, ACCOUNT_CACHE *pCache )
{
	if( !bCache )
		m_2Account.insert( std::map<std::string, ACCOUNT_CACHE*>::value_type( szAccount, pCache ) );
}

ACCOUNT_CACHE* CAccountCacheMgr::GetAccount( LPCTSTR szAccount, BOOL *pbCacheHit )
{
	ACCOUNT_CACHE* pCache = Find( szAccount );
	if( pCache )
	{
		*pbCacheHit = TRUE;

		std::list< ACCOUNT_CACHE* >::iterator wherr = std::find( m_list.begin(), m_list.end(), pCache );
		FLASSERT( wherr != m_list.end() );
		m_list.erase( wherr );
		m_list.push_front( pCache );					// 가장 최신에 사용한 것으로 설정 
		pCache->m_tmLastAccess	= ::time( NULL );
	}
	else
	{
		if( m_nAlloc++ < MAX_CACHE_ALLOC ) 
			pCache = new ACCOUNT_CACHE;
		else
		{	
			int cbList	= m_list.size();
			while( cbList > 0 )
			{
				pCache	= m_list.back();
				m_list.pop_back();
				if( pCache->IsReleaseable() )
				{
					m_2Account.erase( pCache->m_strAccount.c_str() );	
					pCache->Clear();
					break;
				}
				m_list.push_front( pCache );
				cbList--;
			}
			if( cbList == 0 )
				pCache	= new ACCOUNT_CACHE;
		}
		m_list.push_front( pCache );					// 가장 최신에 사용한 것으로 설정 

		pCache->m_strAccount = szAccount; 
		pCache->Init();									// 뱅크 메모리 해제
	}

	return pCache;
}

void CAccountCacheMgr::ChangeMultiServer( LPCTSTR szAccount, u_long uMultiServer )
{
	g_DbManager.m_AddRemoveLock.Enter();
	ACCOUNT_CACHE* pCache	= Find( szAccount );
	if( pCache )
		pCache->ChangeMultiServer( uMultiServer );
	g_DbManager.m_AddRemoveLock.Leave();
}

int CAccountCacheMgr::InitMultiServer( u_long uMultiServer )
{
	int cbPlayer	= 0;
	g_DbManager.m_AddRemoveLock.Enter();
	ACCOUNT_CACHE* pCache;
	for( std::map<std::string, ACCOUNT_CACHE*>::iterator i = m_2Account.begin(); i != m_2Account.end(); ++i )
	{
		pCache	= i->second;
		if( pCache->GetMultiServer() == uMultiServer )
		{
			pCache->ChangeMultiServer( 0 );
			cbPlayer++;
		}
	}
	g_DbManager.m_AddRemoveLock.Leave();
	return cbPlayer;
}
