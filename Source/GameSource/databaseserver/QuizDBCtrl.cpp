// QuizDBCtrl.cpp: implementation of the CQuizDBCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "QuizDBCtrl.h"

#include "dptrans.h"
#include <Quiz.h>
#include <post.h>
#include <definetext.h>
extern	APP_INFO	g_appInfo;

//////////////////////////////////////////////////////////////////////
// CQuizDBCtrl		Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuizDBCtrl::CQuizDBCtrl()
:m_pQuery( 0 ), m_nQuizEventId( 0 ), m_dwQuizSendTick( 0 )
{

}

CQuizDBCtrl::~CQuizDBCtrl()
{
	m_mapWorldData.clear();
	SAFE_DELETE( m_pQuery );
}

void CQuizDBCtrl::Handler( LPDB_OVERLAPPED_PLUS pov, ULONG_PTR dwCompletionKey )
{
	CAr ar( pov->lpBuf, pov->uBufSize );
	
	switch( pov->nQueryMode )
	{
	case QUERY_QUIZ_OPEN:
		{
			int nType;
			ar >> nType;
			if( nType == CQuiz::GetInstance()->GetType() )
			{
				if( CQuiz::GetInstance()->IsAutoMode() )
					CDPTrans::GetInstance()->SendQuizEventOpen( DPID_ALLPLAYERS );
				else
					CDPTrans::GetInstance()->SendQuizEventOpen( dwCompletionKey );
			}
		}
		break;

	case QUERY_QUIZ_STATE:
		{
			int nState, nChannel, nWinnerCount, nQuizCount;
			ar >> nState >> nChannel >> nWinnerCount >> nQuizCount;
			if( nState == CQuiz::QE_OPEN || nState == CQuiz::QE_WATCHINGZONE_OPEN )
			{
				InitWorldData( dwCompletionKey, nChannel, nState );
				SetState( dwCompletionKey, nState );
				CQuiz::GetInstance()->SetRun( TRUE );
				InsertQuizEvent( dwCompletionKey, nChannel );
				if( nState == CQuiz::QE_OPEN )
					LoadQuizList( dwCompletionKey );
			}
			else if( nState == CQuiz::QE_CLOSE || nState == CQuiz::QE_WINNER )
			{
				UpdateQuizEvent( dwCompletionKey, nChannel, nWinnerCount, nQuizCount );
				DeleteWorldData( dwCompletionKey, nState );
			}
			else
				SetState( dwCompletionKey, nState );
		}
		break;
		
	case QUERY_QUIZ_ENTRANCE:
		{
			u_long idPlayer;
			int nChannel;
			ar >> idPlayer >> nChannel;
			EntranceQuizEvent( dwCompletionKey, idPlayer, nChannel );
		}
		break;

	case QUERY_QUIZ_SELECT:
		{
			u_long idPlayer;
			int nChannel, nQuizId, nSelect, nAnswer;
			ar >> idPlayer >> nChannel >> nQuizId >> nSelect >> nAnswer;
			SelectQuizExample( dwCompletionKey, idPlayer, nChannel, nQuizId, nSelect, nAnswer );
		}
		break;
		
	case QUERY_QUIZ_POST_PRIZE:
		{
			u_long idPlayer;
			DWORD dwItemId;
			int nItemNum;
			ar >> idPlayer >> dwItemId >> nItemNum;
			PostPrizeItem( idPlayer, dwItemId, nItemNum );
		}
		break;
		
	case QUERY_QUIZ_CLOSE:
		{
			WORLDDATA* pWD = GetWorldData( dwCompletionKey );
			if( pWD )
			{
				int nChannel = pWD->nChannel;
				UpdateQuizEvent( dwCompletionKey, nChannel, 0, 0 );
				DeleteWorldData( dwCompletionKey, CQuiz::QE_CLOSE );
			}
		}
		break;
	}
}

void CQuizDBCtrl::OnTimer()
{
	if( !CQuiz::GetInstance()->IsRun() )
		return;

	DWORD dwTick = GetTickCount();
	
	for( std::map<DPID, WORLDDATA>::iterator it = m_mapWorldData.begin(); it != m_mapWorldData.end(); ++it )
	{
		// notice send
		if( dwTick > it->second.dwNoticeCycle && ( it->second.nState == CQuiz::QE_OPEN || it->second.nState == CQuiz::QE_WATCHINGZONE_OPEN ) )
		{
			CDPTrans::GetInstance()->SendQuizEventNotice( it->first );
			it->second.dwNoticeCycle = dwTick + MIN( 1 );
		}
		// Quiz list send
		if( it->second.bQuizSend && dwTick > it->second.dwQuizSendCycle && CQuiz::GetInstance()->m_nQuizSize != 0 )
		{
			CDPTrans::GetInstance()->SendQuizList( it->first, CQuiz::GetInstance()->m_vecQuizList[it->second.nCount], CQuiz::GetInstance()->m_nQuizSize );
			it->second.dwQuizSendCycle = dwTick + m_dwQuizSendTick;
			if( ++it->second.nCount >= CQuiz::GetInstance()->m_nQuizSize )
				it->second.bQuizSend = FALSE;
		}
	}
}

void CQuizDBCtrl::InitWorldData( DPID dpId, int nChannel, int nState )
{
	WORLDDATA WD;
	if( nState == CQuiz::QE_OPEN )
		WD.bQuizSend = TRUE;
	WD.nQuizEventId = SetQuizEventId();
	WD.nChannel = nChannel;
	m_mapWorldData.insert( std::map<DPID, WORLDDATA>::value_type( dpId, WD ) );
}

void CQuizDBCtrl::DeleteWorldData( DPID dpId, int nState )
{
	std::map<DPID, WORLDDATA>::iterator it = m_mapWorldData.find( dpId );
	if( it != m_mapWorldData.end() )
		m_mapWorldData.erase( it );
	if( !m_mapWorldData.size() && nState == CQuiz::QE_CLOSE )
	{
		CQuiz::GetInstance()->SetRun( FALSE );
		CQuiz::GetInstance()->m_vecQuizList.clear();
		CQuiz::GetInstance()->m_nQuizSize = 0;
		// 퀴즈 문제 사용 안함 업데이트
		CQuery* pQuery = GetQueryObject();
		char szQuery[MAX_QUERY_SIZE] = {0, };
		
		FLSPrintf( szQuery, _countof( szQuery ), "usp_Quiz_Update @serverindex = '%02d', @m_nQuizType = %d", g_appInfo.dwSys, CQuiz::GetInstance()->GetType() );
		if( pQuery->Exec( szQuery ) == FALSE )
		{ FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); return; }
	}
}

CQuizDBCtrl::WORLDDATA* CQuizDBCtrl::GetWorldData( DPID dpId )
{
	std::map<DPID, WORLDDATA>::iterator it = m_mapWorldData.find( dpId );
	if( it != m_mapWorldData.end() )
		return &it->second;
	return NULL;
}

void CQuizDBCtrl::SetState( DPID dpId, int nState)
{
	std::map<DPID, WORLDDATA>::iterator it = m_mapWorldData.find( dpId );
	if( it != m_mapWorldData.end() )
		it->second.nState = nState;
}

BOOL CQuizDBCtrl::LoadQuizEventId()
{
	char szQuery[MAX_QUERY_SIZE] = {0, };
	FLSPrintf( szQuery, _countof( szQuery ), "usp_QuizID_Select @serverindex = '%02d'", g_appInfo.dwSys );
	if( m_pQuery->Exec( szQuery ) == FALSE )
	{ FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); return FALSE; }
	if( m_pQuery->Fetch() )
	{
		InitQuizEventId( m_pQuery->GetInt( "QuizID" ) );
		return TRUE;
	}
	return FALSE;
}

int CQuizDBCtrl::GetQuizEventId( DPID dpId )
{
	std::map<DPID, WORLDDATA>::iterator it = m_mapWorldData.find( dpId );
	if( it != m_mapWorldData.end() )
		return it->second.nQuizEventId;

	return 0;
}

void CQuizDBCtrl::InsertQuizEvent( DPID dpId, int nChannel )
{
	if( !CQuiz::GetInstance()->IsRun() )
		return;
	
	int nId = GetQuizEventId( dpId );
	if( nId == 0 )
		return;

	char szQuery[MAX_QUERY_SIZE] = {0, };
	
	FLSPrintf( szQuery, _countof( szQuery ), "usp_QuizLog_Insert @idQuizEvent = %d, @serverindex = '%02d', @nChannel = %d, @nQuizType = %d", 
		nId, g_appInfo.dwSys, nChannel, CQuiz::GetInstance()->GetType() );
	if( m_pQuery->Exec( szQuery ) == FALSE )
	{ FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); return; }
}

void CQuizDBCtrl::UpdateQuizEvent( DPID dpId, int nChannel, int nWinnerCount, int nQuizCount )
{
	if( !CQuiz::GetInstance()->IsRun() )
		return;
	
	int nId = GetQuizEventId( dpId );
	if( nId == 0 )
		return;

	char szQuery[MAX_QUERY_SIZE] = {0, };
	
	FLSPrintf( szQuery, _countof( szQuery ), "usp_QuizLog_Update @idQuizEvent = %d, @serverindex = '%02d', @nChannel = %d, @nWinnerCount = %d, @nQuizCount = %d", 
		nId, g_appInfo.dwSys, nChannel, nWinnerCount, nQuizCount );
	if( m_pQuery->Exec( szQuery ) == FALSE )
	{ FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); return; }
}

void CQuizDBCtrl::LoadQuizList( DPID /*dpId*/ )
{
	if( CQuiz::GetInstance()->IsRun() && CQuiz::GetInstance()->m_vecQuizList.size() == 0 )
	{
		CQuery* pQuery = GetQueryObject();
		char szQuery[MAX_QUERY_SIZE] = {0, };
		
		FLSPrintf( szQuery, _countof( szQuery ), "usp_Quiz_Load @serverindex = '%02d', @m_nQuizType = %d", g_appInfo.dwSys, CQuiz::GetInstance()->GetType() );
		if( pQuery->Exec( szQuery ) == FALSE )
		{ FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); return; }

		CQuiz::QUIZLIST QL;
		char szQuestion[MAX_QUIZ_SIZE] = {0, };
		while( pQuery->Fetch() )
		{
			QL.nQuizId = pQuery->GetInt( "m_nIndex" );
			QL.nCorrect = pQuery->GetInt( "m_nAnswer" );
			pQuery->GetStr( "m_szQuestion", szQuestion, _countof( szQuestion ) );
			QL.strQuestion = szQuestion;
			QL.nItemId = pQuery->GetInt( "m_Item" );
			QL.nItemNum = pQuery->GetInt( "m_ItemCount" );
			CQuiz::GetInstance()->m_vecQuizList.push_back( QL );
		}
		if( CQuiz::GetInstance()->m_vecQuizList.size() > 0 )
		{
			CQuiz::GetInstance()->m_nQuizSize = CQuiz::GetInstance()->m_vecQuizList.size();
			m_dwQuizSendTick = ( CQuiz::GetInstance()->GetEntranceTime() / 2 ) / CQuiz::GetInstance()->m_nQuizSize;
			if( m_dwQuizSendTick < 1000 )
				m_dwQuizSendTick = 1000;
		}
	}
}

void CQuizDBCtrl::EntranceQuizEvent( DPID dpId, u_long idPlayer, int nChannel )
{
	if( !CQuiz::GetInstance()->IsRun() )
		return;
	
	int nId = GetQuizEventId( dpId );
	if( nId == 0 )
		return;

	char szQuery[MAX_QUERY_SIZE] = {0, };
	
	FLSPrintf( szQuery, _countof( szQuery ), "usp_QuizUserLog_Insert @m_idQuizEvent = %d, @m_idPlayer = '%07d', @serverindex = '%02d', @m_nChannel = %d", 
		nId, idPlayer, g_appInfo.dwSys, nChannel );
	if( m_pQuery->Exec( szQuery ) == FALSE )
	{ FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); return; }
}

void CQuizDBCtrl::SelectQuizExample( DPID dpId, u_long idPlayer, int nChannel, int nQuizId, int nSelect, int nAnswer )
{
	if( !CQuiz::GetInstance()->IsRun() )
		return;
	
	int nId = GetQuizEventId( dpId );
	if( nId == 0 )
		return;

	char szQuery[MAX_QUERY_SIZE] = {0, };
	
	FLSPrintf( szQuery, _countof( szQuery ), "usp_QuizAnswerLog_Insert @m_idQuizEvent = %d, @m_idPlayer = '%07d', @serverindex = '%02d', @m_nChannel = %d, @m_nQuizNum = %d, @m_nSelect = %d, @m_nAnswer = %d", 
		nId, idPlayer, g_appInfo.dwSys, nChannel, nQuizId, nSelect, nAnswer );
	if( m_pQuery->Exec( szQuery ) == FALSE )
	{ FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery ); return; }
}

void CQuizDBCtrl::PostPrizeItem( u_long idPlayer, DWORD dwItemId, int nItemNum )
{
	TCHAR szMailTitle[MAX_MAILTITLE]	= { 0, };
	TCHAR szMailText[MAX_MAILTEXT]		= { 0, };
	FLItemElem kItemElem;

	FLSPrintf( szMailTitle, _countof( szMailTitle ), prj.GetText( TID_GAME_QUIZ_PRIZEITEM_TITLE ) );
	FLSPrintf( szMailText, _countof( szMailText ), prj.GetText( TID_GAME_QUIZ_PRIZEITEM_TEXT ) );

	const PT_ITEM_SPEC pItemSpec = g_xSpecManager->GetSpecItem( dwItemId );
	if( pItemSpec != NULL && nItemNum > 0 )
	{
		kItemElem.m_dwItemId	= dwItemId;
		kItemElem.m_nItemNum	= nItemNum;
		kItemElem.SetSerialNumber();
	}

	CDbManager::PostMail( *GetQueryObject(), idPlayer, 0, kItemElem, 0, szMailTitle, szMailText );
}


//////////////////////////////////////////////////////////////////////
// CQuizDBMng		Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuizDBMng::CQuizDBMng()
{
	if( !m_QuizDBCtrl.CreateDbHandler( SEC(1) ) )
		FLERROR_LOG( PROGRAM_NAME, _T( "m_QuizDBCtrl.CreateDbHandler()" ) );
}

CQuizDBMng::~CQuizDBMng()
{
	m_QuizDBCtrl.CloseDbHandler();
}

CQuizDBMng* CQuizDBMng::GetInstance()
{
	static CQuizDBMng sQuizDBMng;
	return & sQuizDBMng;
}

CQuery* CQuizDBMng::CreateLogQuery()
{
	CQuery* pQuery = new CQuery;

#ifdef __DbStuff
	if (!pQuery->Connect(CQuery::SqlDatabase::LOGGING))
#else
	const char* pass	= CDbManager::GetInstance().DB_ADMIN_PASS_LOG;
	if( FALSE == pQuery->Connect( 3, DSN_NAME_LOG, DB_ADMIN_ID_LOG, pass ) )
#endif
	{
		::AfxMessageBox( "Can't connect db: CQuizDBCtrl.CreateLogQuery" );
		SAFE_DELETE( pQuery );
	}
	return pQuery;
}

BOOL CQuizDBMng::InitQuizEventId()
{
	m_QuizDBCtrl.m_pQuery = CreateLogQuery();
	if( !m_QuizDBCtrl.m_pQuery )
		return FALSE;
	if( !m_QuizDBCtrl.LoadQuizEventId() )
		return FALSE;
	return TRUE;
}
