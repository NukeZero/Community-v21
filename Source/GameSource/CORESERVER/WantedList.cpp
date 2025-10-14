#include "stdafx.h"
#include "WantedList.h"
#include <algorithm>
#include "dpdatabaseclient.h"

extern	CDPDatabaseClient	g_dpDatabaseClient;

//////////////////////////////////////////////////////////////////////
// file global
//////////////////////////////////////////////////////////////////////

// 30일후의 날짜를 구한다. 
static long CalcEnd()
{
	CTime t = CTime::GetCurrentTime();
	CTimeSpan delta1( 0, t.GetHour(), t.GetMinute(), t.GetSecond() );
	t -= delta1;

	CTimeSpan delta2( 30, 23, 59, 30 );		
	t += delta2;

	return	( (long)( t.GetTime() ) );
}

struct wantedComparison 
{
   bool operator () (const WANTED_ENTRY * left, const WANTED_ENTRY * right) const
   { 
	   return left->nGold > right->nGold; 
   }
};

//////////////////////////////////////////////////////////////////////
// CWantedList
//////////////////////////////////////////////////////////////////////
CWantedList& CWantedList::GetInstance()
{
	static CWantedList wantedList;
	return wantedList;
}


CWantedList::CWantedList()
{
	m_nMinGold = 0;
}

CWantedList::~CWantedList()
{
	WANTED_ENTRY_LIST::iterator it;
	for( it = m_wantedList.begin(); it != m_wantedList.end(); ++it )
		SAFE_DELETE( it->second );
}

void CWantedList::Read( CAr& ar )
{
	CMclAutoLock Lock( m_AccessLock );

	m_wantedList.clear();

	int nCount;				

	ar >> nCount;						
	for( int i=0; i<nCount; i++)
	{
		WANTED_ENTRY *pEntry = new WANTED_ENTRY;
		u_long idPlayer;
		
		ar >> idPlayer;
		ar.ReadString( pEntry->szPlayer, _countof( pEntry->szPlayer ) );
		ar >> pEntry->nGold;
		ar >> pEntry->nEnd;
		ar.ReadString( pEntry->szMsg, _countof( pEntry->szMsg ) );

		m_wantedList.insert(std::make_pair(idPlayer, pEntry) );
	}
}

void CWantedList::Write( CAr& ar )
{
	CMclAutoLock Lock( m_AccessLock );

	std::vector<WANTED_ENTRY *> array;
	array.reserve( 100 );
	long lCurrent = time( NULL );

	WANTED_ENTRY_LIST::iterator it;
	for( it = m_wantedList.begin(); it != m_wantedList.end();  )
	{
		WANTED_ENTRY* pEntry = it->second;
		if( lCurrent > pEntry->nEnd )
		{
			SAFE_DELETE( pEntry );
			m_wantedList.erase( it++ );
		}
		else
		{
			array.push_back( pEntry );
			++it;
		}
	}

	std::sort( array.begin(), array.end(), wantedComparison() );
	int nCount = array.size();
	nCount = min( 100, nCount );

	ar << lCurrent;
	ar << nCount;
	for( int i=0; i<nCount; i++ )
	{
		ar.WriteString( array[i]->szPlayer );
		ar << array[i]->nGold;
		ar << array[i]->nEnd; 
		ar.WriteString( array[i]->szMsg );
	}

	if( nCount >= 1 )
	{
		m_nMinGold = array[nCount-1]->nGold;
		FLTRACE_LOG( PROGRAM_NAME, _T( "set m_nMinGold = %d" ), m_nMinGold );
	}
}

BOOL CWantedList::SetEntry( u_long idPlayer, LPCTSTR szPlayer, int nGold, LPCTSTR szMsg )
{
	CMclAutoLock Lock( m_AccessLock );

	__int64 nResult;
	long nEnd = CalcEnd();

	WANTED_ENTRY* pEntry = NULL; 
	WANTED_ENTRY_LIST::iterator it = m_wantedList.find( idPlayer );
	if( it != m_wantedList.end() )
	{
		pEntry = it->second; 
		pEntry->nEnd = nEnd;
		
		if( nGold >= (0.30f * pEntry->nGold) )	
			FLStrcpy( pEntry->szMsg, _countof( pEntry->szMsg ), szMsg );		// 총 누적 금액의 30%이상을 건 유저의 10자평이 남게 된다.
			
		pEntry->nGold += nGold;
	}
	else
	{
		pEntry = new WANTED_ENTRY;

		FLStrcpy( pEntry->szPlayer, _countof( pEntry->szPlayer ), szPlayer );
		pEntry->nGold = nGold;
		pEntry->nEnd = nEnd;
		FLStrcpy( pEntry->szMsg, _countof( pEntry->szMsg ), szMsg );
		
		m_wantedList.insert(std::make_pair(idPlayer, pEntry) );
	}
	nResult = pEntry->nGold;
	g_dpDatabaseClient.SendCTWanted( CT_WANTED_SET, idPlayer, nGold, nEnd, szMsg );	// DB처리 

	return IsUpdatable( nResult );
}

__int64	CWantedList::GetReward( u_long idPlayer, LPTSTR lpszReward, size_t cchReward, BOOL& bUpdatable )
{
	CMclAutoLock Lock( m_AccessLock );

	__int64 nGold = 0;

	WANTED_ENTRY_LIST::iterator it = m_wantedList.find( idPlayer );
	if( it != m_wantedList.end() )
	{
		WANTED_ENTRY* pEntry = it->second; 
		long lCurrent = time( NULL );
		if( pEntry->nEnd > lCurrent )		// 만료되지 않았으면, 해당 상금을 리턴 
		{
			nGold = pEntry->nGold;
			FLStrcpy( lpszReward, cchReward, pEntry->szPlayer );
		}

		SAFE_DELETE( pEntry );
		m_wantedList.erase( it );

		g_dpDatabaseClient.SendCTWanted( CT_WANTED_SET, idPlayer, 0, 0, "" );			// DB처리
	}

	bUpdatable = IsUpdatable( nGold );
	return nGold;
}


BOOL CWantedList::IsUpdatable( __int64 nGold )
{
	return nGold >= m_nMinGold;
}

void CWantedList::Rename( u_long idPlayer, const char* lpszPlayer )
{
	WANTED_ENTRY_LIST::iterator i	= m_wantedList.find( idPlayer );
	if( i != m_wantedList.end() )
	{
		WANTED_ENTRY* pEntry	= i->second;
		FLStrcpy( pEntry->szPlayer, _countof( pEntry->szPlayer ), lpszPlayer );
	}
}

