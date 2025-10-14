#include "stdafx.h"
#include "couplehelper.h"
#include "dptrans.h"
#include <definetext.h>
#include <post.h>

extern	APP_INFO	g_appInfo;

CCoupleController::CCoupleController( CCoupleHelper* pHelper )
:
m_pHelper( pHelper ),
CDbController()
{
}

CCoupleController::~CCoupleController()
{
}

BOOL CCoupleController::Restore()
{
	CQuery* pQuery	= CreateQuery();
	if( !pQuery )
		return FALSE;
	if( !pQuery->Execute( "uspRestoreCouple %d", g_appInfo.dwSys ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "couldn't execute uspRestoreCouple" ) );
		return FALSE;
	}
	while( pQuery->Fetch() )
	{
		int nExperience	= pQuery->GetInt( "nExperience" );
		u_long idFirst	= pQuery->GetInt( "idFirst" );
		u_long idSecond	= pQuery->GetInt( "idSecond" );
		CCouple* pCouple	= new CCouple( idFirst, idSecond );
		m_pHelper->Couple( pCouple );
		pCouple->AddExperience( nExperience );
	}

	pQuery->Clear();
	if( !pQuery->Execute( "uspRestorePropose %d", g_appInfo.dwSys ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "couldn't execute uspRestorePropose" ) );
		return FALSE;
	}
	while( pQuery->Fetch() )
	{
		u_long idProposer	= pQuery->GetInt( "idProposer" );
		time_t tPropose	= pQuery->GetInt( "tPropose" );
		bool bResult	= m_pHelper->AddPropose( idProposer, tPropose );
		FLASSERT( bResult );
	}
	
	SAFE_DELETE( pQuery );
	return FALSE;
}

BOOL CCoupleController::Propose( u_long idProposer, time_t t )
{
	if( !GetQueryObject()->Execute( "uspAddPropose %d, %d, %d", g_appInfo.dwSys, idProposer, t ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "couldn't execute uspAddPropose" ) );
		return FALSE;
	}
	return TRUE;
}

BOOL CCoupleController::DeletePropose( time_t t )
{
	if( !GetQueryObject()->Execute( "uspDeletePropose %d, %d", g_appInfo.dwSys, t ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "couldn't execute uspDeletePropose" ) );
		return FALSE;
	}
	return TRUE;
}

BOOL CCoupleController::Couple( u_long idProposer, u_long idTarget )
{
	if( !GetQueryObject()->Execute( "uspAddCouple %d, %d, %d", g_appInfo.dwSys, idProposer, idTarget ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "couldn't execute uspAddCouple" ) );
		return FALSE;
	}
	return TRUE;
}

BOOL CCoupleController::Decouple( u_long idPlayer )
{
	if( !GetQueryObject()->Execute( "uspDeleteCouple %d, %d", g_appInfo.dwSys, idPlayer ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "couldn't execute uspDeleteCouple" ) );
		return FALSE;
	}
	return TRUE;
}

BOOL CCoupleController::AddExperience( u_long idPlayer, int nExperience )
{
	if( !GetQueryObject()->Execute( "uspAddCoupleExperience %d, %d, %d", g_appInfo.dwSys, idPlayer, nExperience ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "couldn't execute uspAddCoupleExperience" ) );
		return FALSE;
	}
	return TRUE;
}

void CCoupleController::Handler( LPDB_OVERLAPPED_PLUS pov, ULONG_PTR dwCompletionKey )
{
	m_pHelper->Handler( pov, dwCompletionKey );
}

void CCoupleController::OnTimer()
{
	m_pHelper->OnTimer();
}

////////////////////////////////////////////////////////////////////////////////
CCoupleHelper::CCoupleHelper()
{
}

CCoupleHelper::~CCoupleHelper()
{
	Clear();
}

CCoupleHelper* CCoupleHelper::Instance()
{
	static CCoupleHelper sCoupleHelper;
	return &sCoupleHelper;
}

void CCoupleHelper::Clear()
{
	if( m_pController )
		m_pController->CloseDbHandler();
	SAFE_DELETE( m_pController );
	SAFE_DELETE( m_pMgr );
}

void CCoupleHelper::Serialize( CAr & ar )
{
	FLASSERT( m_pMgr );
	if( m_pMgr != NULL )
	{
		m_pMgr->Serialize( ar );
	}
}

BOOL CCoupleHelper::Initialize()
{
	m_pController	= new CCoupleController( this );
	m_pController->CreateDbHandler( MIN( 1 ) );
	m_pMgr	= new CCoupleMgr;
	if( CCoupleProperty::Instance()->Initialize() )
		return m_pController->Restore();
	return FALSE;
}

void CCoupleHelper::Handler( LPDB_OVERLAPPED_PLUS pov, ULONG_PTR dwCompletionKey )
{
	CAr ar( pov->lpBuf, pov->uBufSize );
	switch( pov->nQueryMode )
	{
		case eTransfer:
			OnTransfer( dwCompletionKey );
			break;
		case ePropose:
			OnPropose( ar, dwCompletionKey );
			break;
		case eCouple:
			OnCouple( ar );
			break;
		case eDecouple:
			OnDecouple( ar );
			break;
		case eClearPropose:
			OnClearPropose();
			break;
		case eAddExperience:
			OnAddExperience( ar );
			break;
		default:
			break;
	}
}

void CCoupleHelper::OnAddExperience( CAr & ar )
{
	u_long idPlayer;
	int nExperience;
	ar >> idPlayer >> nExperience;
	FLTRACE_LOG( PROGRAM_NAME, _T( "T: CCoupleHelper.OnAddExperience: %d, %d" ), idPlayer, nExperience );
	CCouple* pCouple	= m_pMgr->GetCouple( idPlayer );
	if( pCouple )
	{
		if( m_pController->AddExperience( idPlayer, nExperience ) )
		{
			BOOL bLevelup	= pCouple->AddExperience( nExperience );
			if( bLevelup )
				PostItem( pCouple );
			CDPTrans::GetInstance()->SendAddCoupleExperience( idPlayer, nExperience );
		}
	}
}

void CCoupleHelper::PostItem( CCouple* pCouple )
{
	VCI& vItems	= CCoupleProperty::Instance()->GetItems( pCouple->GetLevel() );
	if( vItems.empty() )
		return;
	int nGenderFirst, nGenderSecond;
	nGenderFirst	= nGenderSecond	= SEX_SEXLESS;
	GetGender( pCouple, nGenderFirst, nGenderSecond );

	for( DWORD i = 0; i < vItems.size(); i++ )
	{
		if( vItems.at( i ).nSex == SEX_SEXLESS || vItems.at( i ).nSex == nGenderFirst )
			PostItem( pCouple->GetFirst(), vItems.at( i ), pCouple->GetLevel() );
		if( vItems.at( i ).nSex == SEX_SEXLESS || vItems.at( i ).nSex == nGenderSecond )
			PostItem( pCouple->GetSecond(), vItems.at( i ), pCouple->GetLevel() );
	}
}

void CCoupleHelper::GetGender( CCouple* pCouple, int & nGenderFirst, int & nGenderSecond )
{
	CMclAutoLock	Lock( CPlayerDataCenter::GetInstance()->m_Access );
	PlayerData* pData	= CPlayerDataCenter::GetInstance()->GetPlayerData( pCouple->GetFirst() );
	if( pData )
		nGenderFirst	= pData->data.nSex;
	pData	= CPlayerDataCenter::GetInstance()->GetPlayerData( pCouple->GetSecond() );
	if( pData )
		nGenderSecond	= pData->data.nSex;
}

void CCoupleHelper::PostItem( u_long idPlayer, const COUPLE_ITEM& ci, int nLevel )
{
	TCHAR szMailTitle[MAX_MAILTITLE]	= { 0, };
	TCHAR szMailText[MAX_MAILTEXT]		= { 0, };
	FLItemElem kItemElem;

	FLSPrintf( szMailTitle, _countof( szMailTitle ), prj.GetText( TID_GAME_COUPLE_ITEM_MAIL_TITLE ) );
	FLSPrintf( szMailText, _countof( szMailText ), prj.GetText( TID_GAME_COUPLE_ITEM_MAIL_TEXT ), nLevel );
	kItemElem.m_dwItemId	= ci.nItem;
	kItemElem.m_nItemNum	= ci.nNum;
	if( ci.nLife > 0 )
	{
		kItemElem.m_dwKeepTime	= ::time_null() + ci.nLife * 60;	// ci.nLife(단위: 분)
	}
	kItemElem.m_byFlag	= static_cast<BYTE>( ci.nFlags );
	kItemElem.SetSerialNumber();

	CDbManager::PostMail( *m_pController->GetQueryObject(), idPlayer, 0, kItemElem, 0, szMailTitle, szMailText );
}

void CCoupleHelper::OnTimer()
{
	FLTRACE_LOG( PROGRAM_NAME, _T( "T: CCoupleHelper.OnTimer" ) );
	time_t t	= time( NULL );
	for( MUT::iterator i = m_mapProposes.begin(); i != m_mapProposes.end(); )
	{
		if( t > i->second )
			m_mapProposes.erase( i++ );
		else
			++i;
	}
	m_pController->DeletePropose( t );
}

void CCoupleHelper::OnTransfer( DPID dpid )
{
	FLASSERT( m_pMgr );
	CDPTrans::GetInstance()->SendCouple( m_pMgr, dpid );
}

void CCoupleHelper::OnClearPropose()
{
	if( m_pController->DeletePropose( INT_MAX ) )
		m_mapProposes.clear();
}

void CCoupleHelper::OnPropose( CAr & ar, DPID dpid )
{
	int nResult		= 0;
	u_long idProposer, idTarget;
	ar >> idProposer >> idTarget;
	FLTRACE_LOG( PROGRAM_NAME, _T( "T: CCoupleHelper.OnPropose: %d, %d" ), idProposer, idTarget );
	time_t t	= GetPropose( idProposer );
	if( t > 0 )
	{
		nResult		= -1;
		t	-= time( NULL );
		if( t <= 0 )
			t	= 1;
	}
	if( nResult == 0 && m_pMgr->GetCouple( idProposer ) )
		nResult		= TID_GAME_COUPLE_E06;	// 커플이 아닌 캐릭터만 가능합니다.
	if( nResult == 0 && m_pMgr->GetCouple( idTarget ) )
		nResult		= TID_GAME_COUPLE_E01;	// 대상이 부적합 합니다. 커플이 아닌 이성 캐릭터에게만 가능합니다.
	if( nResult == 0 && !SetPropose( idProposer, time( NULL ) + 86400 ) )
		nResult		= TID_GAME_COUPLE_E10;	// 데이터베이스 오류입니다.
	CDPTrans::GetInstance()->SendProposeResult( idProposer, idTarget, nResult, t, dpid );
}

void CCoupleHelper::OnCouple( CAr & ar )
{
	int nResult	= 0;
	u_long idProposer, idTarget;
	ar >> idProposer >> idTarget;
	FLTRACE_LOG( PROGRAM_NAME, _T( "T: CCoupleHelper.OnCouple: %d, %d" ), idProposer, idTarget );
	if( m_pMgr->GetCouple( idProposer ) )
		nResult		= TID_GAME_COUPLE_E06;	// 커플이 아닌 캐릭터만 가능합니다.
	if( nResult == 0 && m_pMgr->GetCouple( idTarget ) )
		nResult		= TID_GAME_COUPLE_E01;	// 대상이 부적합 합니다. 커플이 아닌 이성 캐릭터에게만 가능합니다.
	if( nResult == 0 && !m_pController->Couple( idProposer, idTarget ) )
		nResult		= TID_GAME_COUPLE_E10;	// 데이터베이스 오류입니다.
	if( nResult == 0 )
		m_pMgr->Couple( idProposer, idTarget );
	CDPTrans::GetInstance()->SendCoupleResult( idProposer, idTarget, nResult );
}

void CCoupleHelper::OnDecouple( CAr & ar )
{
	int nResult		= 0;
	u_long idPlayer;
	ar >> idPlayer;

	FLTRACE_LOG( PROGRAM_NAME, _T( "T: CCoupleHelper.OnDecouple: %d" ), idPlayer );

	CCouple* pCouple	= m_pMgr->GetCouple( idPlayer );
	if( !pCouple )
		nResult		= TID_GAME_COUPLE_E05;
	if( nResult == 0 && !m_pController->Decouple( idPlayer ) )
		nResult		= TID_GAME_COUPLE_E10;	// 데이터베이스 오류입니다.
	if( nResult == 0 && !m_pMgr->Decouple( idPlayer ) )
		nResult		= TID_GAME_COUPLE_E11;
	CDPTrans::GetInstance()->SendDecoupleResult( idPlayer, nResult );
}

time_t CCoupleHelper::GetPropose( u_long idPlayer )
{
	MUT::iterator i = m_mapProposes.find( idPlayer );
	if( i != m_mapProposes.end() )
		return i->second;
	return 0;
}

BOOL CCoupleHelper::SetPropose( u_long idPlayer, time_t t )
{
	if( !m_pController->Propose( idPlayer, t ) )
		return FALSE;
	AddPropose( idPlayer, t );
	return TRUE;
}
