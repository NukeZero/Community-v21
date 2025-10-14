#include "stdafx.h"
#include "account.h"
#include "dpsrvr.h"
#include "dpdbsrvr.h"
#include <MsgHdr.h>
#include "FLASDBManager.h"
#include "../_Common/FLServerDescription.h"

extern	CDPSrvr		g_dpSrvr;
extern	CDPDBSrvr	g_dpDbSrvr;

CAccountMng			g_AccountMng;
//----------------------------------------------------------------------------------------------------------------------------------------------------------------
MEMPOOLER_IMPLEMENT( CAccount, 1024 );

CAccount::CAccount( LPCTSTR lpszAccount, DWORD dpid1, DWORD dpid2, BYTE cbAccountFlag, int fCheck, const CTime & tmBirthDate, const TCHAR* pAddress )
: m_tmBirthDate( tmBirthDate )
{
	FLStrcpy( m_lpszAccount, _countof( m_lpszAccount ), lpszAccount );
	m_dpid1				= dpid1;
	m_dpid2				= dpid2;
	m_cbAccountFlag		= cbAccountFlag;
	m_fRoute			= FALSE;
	m_dwIdofServer		= 0;
	m_dwPing			= timeGetTime();
	m_uIdofMulti		= NULL_ID;
	m_dwAuthKey			= 0;
	m_cbRef				= 0;

	m_dwBillingClass	= 0;
	m_TimeOverDays		= 0;
	m_nStatus			= ACCOUNT_STATUS_INITIAL;
	m_fCheck			= fCheck;
	m_cbLastOneLogon	= 'N';

	// BEGIN110520
	m_bCertify			= FALSE;
	// END110520

	FLStrcpy( m_address, _countof( m_address ), pAddress );
}

CAccount::~CAccount()
{
}

BOOL CAccount::IsAdult()
{
	return ( m_cbAccountFlag & ACCOUNT_FLAG_18 );
}
//
//void CAccount::SendBillingResult( BILLING_INFO* pResult )
//{
//	BYTE cbResult;
//	switch( pResult->lResult )
//	{
//	case SUCCESS:
//		cbResult = ACCOUNT_CHECK_OK;
//		m_TimeOverDays	= *(pResult->pTimeOverDays);
//		m_cbLastOneLogon = pResult->cbLastOneLogon;
//		break;
//	case BILLING_INFO_FAILED:
//		cbResult = ACCOUNT_BILLING_INFO_FAILED;
//		break;
//	case DATABASE_ERROR:
//		cbResult = ACCOUNT_BILLING_DATABASE_ERROR;
//		break;
//	case TIME_OVER:
//		cbResult = ACCOUNT_BILLING_TIME_OVER;
//		break;
//	case OTHER_ERROR:
//	default:
//		cbResult = ACCOUNT_BILLING_OTHER_ERROR;
//		break;
//	}
//
//	if( m_nStatus == ACCOUNT_STATUS_SECONDQUERY )
//	{
//		if( pResult->lResult == SUCCESS )
//		{
//			m_nStatus = ACCOUNT_STATUS_INITIAL;								// GOTO BEGIN
//			FLTRACE_LOG( PROGRAM_NAME, _T( "%s ACCOUNT_STATUS_SECONDQUERY -> ACCOUNT_STATUS_INITIAL" ), pResult->szAccount );
//		}
//		else
//			g_dpSrvr.CloseExistingConnection( pResult->szAccount, ERROR_BILLING_TIME_OVER );	// END
//	}
//	else  // ACCOUNT_STATUS_INITIAL or ACCOUNT_STATUS_NOTIFIED
//	{
//		time_t lTimeSpan = 0;
//		if( pResult->lResult == SUCCESS )
//		{
//			// 결재가 1일 미만 남은경우, 유저가 로그인 할때 '몇시간 몇분 남았습니다' 메세지처리 
//			time_t tm = (time_t)( m_TimeOverDays.GetTime() );
//			time_t cur = time( NULL );
//			if( tm > cur )
//			{
//				time_t diff = tm - cur;
//				CTimeSpan span( diff );
//				if( span.GetTotalHours() < 24 )
//					lTimeSpan = diff;
//			}
//		}
//		
//		g_dpSrvr.OnAfterChecking( cbResult, pResult->szAccount, m_dpid1, m_dpid2, 
//								  m_dwAuthKey, m_cbAccountFlag, lTimeSpan, "", "" );
//	}
//}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

CAccountMng::CAccountMng()
{
	m_nSizeof	= 0;
	m_nCount	= 0;
	m_nOldHour  = 0;

	CTime tTime = CTime::GetCurrentTime();
	m_nYear = tTime.GetYear() - 1;
	m_nMonth = tTime.GetMonth();
	m_nDay = tTime.GetDay();
	m_nHour = tTime.GetHour();
	m_nMinute = tTime.GetMinute();
}

CAccountMng::~CAccountMng()
{
	Clear();
}

void CAccountMng::Clear( void )
{
	CMclAutoLock	Lock( m_AddRemoveLock );

	for( std::map<std::string, CAccount*>::iterator i	= m_stringToPAccount.begin(); i != m_stringToPAccount.end(); ++i )
		SAFE_DELETE( i->second );
	m_stringToPAccount.clear();

	for( int j = 0; j < m_nSizeof; j++ )
		m_adpidToPAccount[j].clear();
	m_dpidToIndex.clear();
}

int CAccountMng::GetIndex( DWORD dpid1 )
{
// locked
	std::map<DWORD, int>::iterator i		= m_dpidToIndex.find( dpid1 );
	if( i  != m_dpidToIndex.end() )
		return i->second;
	return( -1 );
}

int CAccountMng::GetIdofServer( DWORD dpid )
{
// locked
	std::map<DWORD, DWORD>::iterator i	= m_2IdofServer.find( dpid );
	if( i != m_2IdofServer.end() )
		return i->second;
	return( -1 );
}

BYTE CAccountMng::AddAccount( LPCTSTR lpszAccount, DWORD dpid1, DWORD dpid2, DWORD* pdwAuthKey, BYTE cbAccountFlag, int fCheck, const CTime & tmBirthDate, const TCHAR* pAddress )
{
	CMclAutoLock	Lock( m_AddRemoveLock );

	std::map<std::string, CAccount*>::iterator i1	= m_stringToPAccount.find( lpszAccount );
	if( i1 == m_stringToPAccount.end() )
	{
		int nIndex	= GetIndex( dpid1 );
		if( nIndex >= 0 && nIndex < MAX_CERTIFIER )
		{
			std::map<DWORD, CAccount*>::iterator i2	= m_adpidToPAccount[nIndex].find( dpid2 );
			if( i2 == m_adpidToPAccount[nIndex].end() )
			{
				CAccount* pAccount	= new CAccount( lpszAccount, dpid1, dpid2, cbAccountFlag, fCheck, tmBirthDate, pAddress );
				m_stringToPAccount.insert( std::map<std::string, CAccount*>::value_type( lpszAccount, pAccount ) );
				m_adpidToPAccount[nIndex].insert( std::map<DWORD, CAccount*>::value_type( dpid2, pAccount ) );
				m_nCount++;
				*pdwAuthKey	= pAccount->m_dwAuthKey		= xRandom( 0x00000001UL, ULONG_MAX );

				// BEGIN110520
				pAccount->m_bCertify					= TRUE;
				// END110520
				return ACCOUNT_CHECK_OK;
			}
			return ACCOUNT_DUPLIACTE;
		}
		return ACCOUNT_DUPLIACTE;
	}
	else
	{
		CAccount* pAccount	= GetAccount( lpszAccount );
		if( pAccount && FALSE == pAccount->m_fRoute )
		{
			if( timeGetTime() - pAccount->m_dwPing > MIN( 2 ) )
			{
				g_dpSrvr.DestroyPlayer( pAccount->m_dpid1, pAccount->m_dpid2 );
				RemoveAccount( pAccount->m_dpid1, pAccount->m_dpid2 );
				return AddAccount( lpszAccount, dpid1, dpid2, pdwAuthKey, cbAccountFlag, fCheck, tmBirthDate, pAddress );
			}
		}
		return ACCOUNT_DUPLIACTE;
	}
}

// 빌링서버에게 유저가 나갔음을 알리는 경우는 태국의 경우만 그러하다.
// 이는 빌링업체(페이레터)의 편의성을 봐주기 위해서이다.
// 게임서버가 능동적으로 게임 플레이 가능시간을 차감하지 않기 때문에, 
// 주기적 시간 차감을 함에도 불구하고, 로그아웃을  빌링서버에게 이를 알려줘야 하는 경우가 생겼다. 
//

// 예: 
// step1: 유저A가 로그인 ( 2분 플레이 시간이 남은 유저 )
// step2: 빌링서버에게 남은 시간을 물어봄 2분으로 응답 
// step3: 게임 플레이 
// step4: 게임 시간 만료, 끊어냄
// step5: 빌링서버에서 A유저가 플레이 하는 지 물어봄 
// step6: 플레이중이지 않음으로 응답 -> 빌링서버는 유저 A에 대해서 시간을 차감하지 않음 -> 플레이 시간은 그대로 2분 
// step7: 유저A가 로그인 -> 로그인 가능상태 

// 해결책A: 주기적으로 물어볼 때 남은 시간을 받아서 끊어냄. 
//		단점: 주기가 20분이라면 18분동안 무료 플레이를 할 수 있음  
// 해결책B: 게임서버가 시간을 차감하고, 빌링서버는 이를 받아서 저장만 한다.
//		단점: ?

// 페이레터는 해결책 A를 주로 사용했기에 이를 수용해서 로그아웃을 빌링서버에 알려줌. 

// 넵... 게임 시작에서 게임가능한 티켓이 없다고 요청이 갔는데 그다음에 바로 게임 종료 요청이 오거든요.
// 아.. 넵, 그때는 안보내 주실수는 없는지요? ^^; 별상관은 없지만 일단 빌링에서는 한번더 확인을 해야 하니까요.. 

void CAccountMng::RemoveAccount( LPCTSTR lpszAccount )
{
	CMclAutoLock	Lock( m_AddRemoveLock );

#ifdef __INTERNALSERVER
	FLINFO_LOG( PROGRAM_NAME, _T( "CAccountMng::RemoveAccount(%s)" ), lpszAccount );
#endif

	std::map<std::string, CAccount*>::iterator i	= m_stringToPAccount.find( lpszAccount );
	if( i != m_stringToPAccount.end() )
	{
		CAccount* pAccount = i->second;

		if( pAccount->m_fRoute && pAccount->m_cbRef == 2 )
		{
			long lConnectionCount = g_pServerDescription->DecreaseConnectionCount( pAccount->m_dwIdofServer, pAccount->m_uIdofMulti );

			g_dpSrvr.SendPlayerCount( pAccount->m_dwIdofServer, pAccount->m_uIdofMulti, lConnectionCount );
		}

		g_xASDBManager->UpdateTracking( FALSE, lpszAccount );	// 유저가 logoff했음을 디비에 쓴다.
		g_dpSrvr.SendAccountLogout( lpszAccount );				// 유저가 logoff했음을 Certifier에 알린다.

		SAFE_DELETE( pAccount );
		m_stringToPAccount.erase( i );
		m_nCount--;
	}
}

void CAccountMng::RemoveAccount( DWORD dpid1, DWORD dpid2 )
{
	CAccount* pAccount;
	int nIndex;
	CMclAutoLock	Lock( m_AddRemoveLock );

	nIndex	= GetIndex( dpid1 );
	if( nIndex >= 0 && nIndex < m_nSizeof )
	{
		std::map<DWORD, CAccount*>::iterator i	= m_adpidToPAccount[nIndex].find( dpid2 );
		if( i != m_adpidToPAccount[nIndex].end() )
		{
			pAccount	= i->second;
			m_adpidToPAccount[nIndex].erase( i );

			if( FALSE == pAccount->m_fRoute )
			{
				RemoveAccount( pAccount->m_lpszAccount );	// erase & delete
			}
		}
	}
}

CAccount* CAccountMng::GetAccount( LPCTSTR lpszAccount )
{
// locked
	std::map<std::string, CAccount*>::iterator i	= m_stringToPAccount.find( lpszAccount );
	if( i != m_stringToPAccount.end() )
		return i->second;
	return NULL;
}

CAccount* CAccountMng::GetAccount( DWORD dpid1, DWORD dpid2 )
{
// locked
	int nIndex	= GetIndex( dpid1 );
	if( nIndex >= 0 && nIndex < m_nSizeof )
	{
		std::map<DWORD, CAccount*>::iterator i	= m_adpidToPAccount[nIndex].find( dpid2 );
		if( i != m_adpidToPAccount[nIndex].end() )
		{
			return i->second;
		}
	}
	return NULL;
}

void CAccountMng::AddConnection( DWORD dpid1 )
{
	CMclAutoLock	Lock( m_AddRemoveLock );

	std::map<DWORD, int>::iterator i	= m_dpidToIndex.find( dpid1 );
	if( i == m_dpidToIndex.end() )
	{
		m_dpidToIndex.insert( std::map<DWORD, int>::value_type( dpid1, m_nSizeof ) );
		m_nSizeof++;
	}
//	else
//	{
//	}
}

void CAccountMng::RemoveConnection( DWORD dpid1 )
{
	CAccount* pAccount;
	int nIndex;

	CMclAutoLock	Lock( m_AddRemoveLock );
	nIndex	= GetIndex( dpid1 );
	if( nIndex >= 0 && nIndex < m_nSizeof )
	{
		std::map<DWORD, CAccount*>::iterator	i1	= m_adpidToPAccount[nIndex].begin();
		while( i1 != m_adpidToPAccount[nIndex].end() )
		{
			pAccount	= i1->second;
			std::map<DWORD, CAccount*>::iterator	i2	= i1;
			++i1;
			m_adpidToPAccount[nIndex].erase( i2 );
			RemoveAccount( pAccount->m_lpszAccount );	// erase & delete
		}
	}
}

void CAccountMng::AddIdofServer( DWORD dpid, DWORD dwIdofServer )
{
	int nIdofServer;
	CMclAutoLock	Lock( m_AddRemoveLock );

	nIdofServer		= GetIdofServer( dpid );
	if( nIdofServer < 0 )
		m_2IdofServer.insert( std::map<DPID, DWORD>::value_type( dpid, dwIdofServer ) );
}

void CAccountMng::RemoveIdofServer( DWORD dpid, BOOL bRemoveConnection )
{
	CAccount* pAccount;
	CMclAutoLock	Lock( m_AddRemoveLock );
	DWORD dwIdofServer	= GetIdofServer( dpid );

	std::map<std::string, CAccount*>::iterator i	= m_stringToPAccount.begin();
	while( i != m_stringToPAccount.end() )
	{
		pAccount	= i->second;
		++i;
		if( pAccount->m_dwIdofServer == dwIdofServer )
		{
			if( pAccount->m_fRoute )
				RemoveAccount( pAccount->m_lpszAccount );
			else
				RemoveAccount( pAccount->m_dpid1, pAccount->m_dpid2 );
		}
	}
	if( bRemoveConnection )
		m_2IdofServer.erase( dpid );
}



// 오후 10시에 미성년자와 미등록 성인들의 접속을 종료시킨다.
void CAccountMng::PreventExcess()
{
	CTime cur = CTime::GetCurrentTime();

	if( cur.GetHour() == 22 && m_nOldHour != 22 )
	{	
		CAccount* pAccount;
		CMclAutoLock	Lock( m_AddRemoveLock );
		std::map<std::string, CAccount*>::iterator i	= m_stringToPAccount.begin();
		while( i != m_stringToPAccount.end() )
		{
			pAccount	= i->second;
			++i;

			if( pAccount->IsAdult() )	// 성인이면 skip
				continue;

			g_dpSrvr.CloseExistingConnection( pAccount->m_lpszAccount, 0 );
		}	
	}

	m_nOldHour = cur.GetHour();
}

CAccount* CAccountMng::GetAccount( LPCTSTR lpszAccount, DWORD dwKey )
{
	CMclAutoLock	Lock( m_AddRemoveLock );

	CAccount* pAccount = GetAccount( lpszAccount );
	if( pAccount == NULL )
		return NULL;		// may be logout

	if( dwKey && pAccount->m_dwAuthKey != dwKey )		// dwKey가 0이면 검사하지 않는다.
		return NULL;		// may be different account
	
	return pAccount;
}

//// szAccount에게 빌링체크 결과(lResult)를 전송 
//void CAccountMng::SendBillingResult( BILLING_INFO* pResult )
//{
//	CMclAutoLock	Lock( m_AddRemoveLock );
//
//	CAccount* pAccount = GetAccount( pResult->szAccount, pResult->dwKey );
//	if( pAccount == NULL )
//		return;		// may be logout
//
//	pAccount->SendBillingResult( pResult );
//}

// 날짜를 체크해서 계정들을 풀건지 검사
BOOL CAccountMng::IsTimeCheckAddr()
{
	CTime tTime = CTime::GetCurrentTime();
	if( m_nYear == tTime.GetYear() && m_nMonth == tTime.GetMonth() && m_nDay == tTime.GetDay() &&
		m_nHour == tTime.GetHour() && m_nMinute == tTime.GetMinute() )
	{
		return TRUE;
	}
	return FALSE;
}

std::map<std::string, CAccount*> CAccountMng::GetMapAccount()
{
	return m_stringToPAccount;
}

DWORD CAccountMng::PopPCBangPlayer( DWORD dwAuthKey )
{
	std::map<DWORD, DWORD>::iterator it = m_mapPCBang.find( dwAuthKey );
	if( it != m_mapPCBang.end() )
	{
		DWORD dwReturn = it->second;
		m_mapPCBang.erase( it );
		return dwReturn;
	}

	return 0;
}