#include "stdafx.h"

//sun: 13, 레인보우 레이스

#include "RainbowRace.h"
#ifdef __CLIENT
#include "../Neuz/DPClient.h"
extern CDPClient g_DPlay;
#endif // __CLIENT

#ifdef __WORLDSERVER
#include "../WorldServer/MiniGameGawibawibo.h"
#include "../WorldServer/MiniGameDiceplay.h"
#include "../WorldServer/MiniGameArithmetic.h"
#include "../WorldServer/MiniGameStopwatch.h"
#include "../WorldServer/MiniGameTyping.h"
#include "../WorldServer/MiniGamePairgame.h"
#include "../WorldServer/MiniGameLadder.h"

#include "worldmng.h"
extern CWorldMng g_WorldMng;
#include "../WorldServer/User.h"

#include "post.h"
#endif // __WORLDSERVER


//////////////////////////////////////////////////////////////////
// CRainbowRace //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
CRainbowRace::CRainbowRace()
: m_wGameState(0x0000), m_wNowGame(0x0000)
{
#ifdef __CLIENT
	m_dwRemainTime = 0;
#endif // __CLIENT
#ifdef __WORLDSERVER
	m_bFinished = FALSE;
	m_pMiniGame = NULL;
	SetNextMiniGame();
#endif // __WORLDSERVER
}
CRainbowRace::~CRainbowRace()
{ 
	DestroyGame();	
}

void CRainbowRace::DestroyGame()
{
#ifdef __WORLDSERVER
	SAFE_DELETE( m_pMiniGame );
#endif // __WORLDSERVER
}


void CRainbowRace::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << m_wNowGame;
		ar << m_wGameState;
#ifdef __WORLDSERVER
		if( IsFinished() || (CRainbowRaceMng::GetInstance()->GetState() == CRainbowRaceMng::RR_CLOSED) )
			ar << (DWORD)0;
		else
			ar << CRainbowRaceMng::GetInstance()->GetNextTime() - GetTickCount();
#endif // __WORLDSERVER
	}
	else
	{
		ar >> m_wNowGame;
		ar >> m_wGameState;
#ifdef __CLIENT
		ar >> m_dwRemainTime;
		if( m_dwRemainTime > 0 )
			m_dwRemainTime += GetTickCount();
#endif // __CLIENT
	}
}

BOOL CRainbowRace::IsCompletedGame( int nGameNum )
{
#ifdef __WORLDSERVER
	if( nGameNum > CRainbowRaceMng::GetInstance()->GetMiniGameSize() )
		return FALSE;
#endif // __WORLDSERVER

	return ( m_wGameState & ( 0x0001 << nGameNum ) );
}

#ifdef __CLIENT
CRainbowRace* CRainbowRace::GetInstance()
{
	static CRainbowRace sRainbowRace;
	return & sRainbowRace;
}
int	CRainbowRace::GetGameKey()
{
	for( int i=0; i<RMG_MAX; i++ )
	{
		if( ( 0x0001 << i ) & m_wNowGame )
			return i;
	}

	return -1;
}

void CRainbowRace::SendMinigamePacket( int nGameKey, int nState, int nParam1, int nParam2 )
{
	__MINIGAME_PACKET MiniGamePacket( m_wNowGame, nState, nParam1, nParam2 );
	if( nGameKey != RMG_MAX )
		MiniGamePacket.wNowGame = GetGameKeyToNowGame( nGameKey );
    g_DPlay.SendRainbowRaceMiniGameState( MiniGamePacket );
}

void CRainbowRace::SendMinigameExtPacket( std::vector<std::string> & vecszData, int nGameKey, int nState, int nParam1, int nParam2 )
{
	__MINIGAME_EXT_PACKET MiniGameExtPacket( m_wNowGame, nState, nParam1, nParam2 );
	MiniGameExtPacket.vecszData.assign( vecszData.begin(), vecszData.end() );
	
	if( nGameKey != RMG_MAX )
		MiniGameExtPacket.wNowGame = GetGameKeyToNowGame( nGameKey );
	g_DPlay.SendRainbowRaceMiniGameExtState( MiniGameExtPacket );
}
#endif // __CLIENT

#ifdef __WORLDSERVER
void CRainbowRace::SetNextMiniGame()
{
	DestroyGame();
	if( IsAllCompleted() )
		return;
	
	int nGame = xRandom( CRainbowRaceMng::GetInstance()->GetMiniGameSize() );
	while( IsCompletedGame( nGame ) )
	{
		nGame--;
		if( nGame < 0 )
			nGame = CRainbowRaceMng::GetInstance()->GetMiniGameSize() - 1;
	}

	SetMiniGame( nGame );
}

void CRainbowRace::SetMiniGame( int nGameNum )
{
	m_wNowGame = static_cast< WORD >( 0x0001 << nGameNum );
	CMiniGameBase* pMiniGame = CRainbowRaceMng::GetInstance()->GetMiniGame( nGameNum );
	switch( nGameNum )
	{
		case RMG_GAWIBAWIBO:	m_pMiniGame = new CMiniGameGawibawibo( pMiniGame ); break;
		case RMG_DICEPLAY:		m_pMiniGame = new CMiniGameDiceplay( pMiniGame );	break;
		case RMG_ARITHMATIC:	m_pMiniGame = new CMiniGameArithmetic( pMiniGame );		break;
		case RMG_STOPWATCH:		m_pMiniGame = new CMiniGameStopwatch( pMiniGame );		break;
		case RMG_TYPING:		m_pMiniGame = new CMiniGameTyping( pMiniGame );		break;
		case RMG_PAIRGAME:		m_pMiniGame = new CMiniGamePairgame( pMiniGame );		break;
		case RMG_LADDER:		m_pMiniGame = new CMiniGameLadder( pMiniGame );		break;
	}
}

void CRainbowRace::OnMiniGamePacket( FLWSUser* pUser, __MINIGAME_PACKET* pMiniGamePacket )
{
	if( m_wNowGame != pMiniGamePacket->wNowGame )	// 클라로 부터 받은 패킷에 지정된 미니게임이 서버와 다르면..
	{
		pUser->AddDefinedText( TID_GAME_RAINBOWRACE_ISNOTNOWMINIGAME );
		return;
	}

	if( m_pMiniGame && m_pMiniGame->Excute( pUser, pMiniGamePacket ) )	// 이번 미니게임이 완료되었으면...
	{
		SetNowGameComplete( pUser );	// 이번 게임을 완료상태로 만들고 다음 게임을 셋팅한다.
	}
}

void CRainbowRace::SetNowGameComplete( FLWSUser* pUser )
{
	m_wGameState |= m_wNowGame;
	m_wNowGame = 0x0000;

	if( IsAllCompleted() )
		DestroyGame();
	else
		SetNextMiniGame();			// 다음 게임 세팅

	pUser->AddRainbowRaceState( this );	// 클라에 알려준다.
	CRainbowRaceMng::GetInstance()->SetMiniGamePrizeList( pUser->m_idPlayer, GetCompletedGameNum(), GetTickCount() );	// 미니게임 상품지급
}

int	CRainbowRace::GetCompletedGameNum()
{
	int nCount = 0;
	for( int i=0; i<CRainbowRaceMng::GetInstance()->GetMiniGameSize(); i++ )
	{
		if( IsCompletedGame( i ) )
			nCount++;
	}

	return nCount;
}
#endif // __WORLDSERVER

//////////////////////////////////////////////////////////////////
// CRainbowRaceMng ///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
#ifdef __WORLDSERVER
CRainbowRaceMng::CRainbowRaceMng(void)
: m_Day( DAY_NONE ), m_Activation( false )
{
	m_vecMiniGame.resize( RMG_MAX, NULL );
	m_vecMiniGame[RMG_GAWIBAWIBO]	= new CMiniGameGawibawibo;
	m_vecMiniGame[RMG_DICEPLAY]		= new CMiniGameDiceplay;
	m_vecMiniGame[RMG_ARITHMATIC]	= new CMiniGameArithmetic;
	m_vecMiniGame[RMG_STOPWATCH]	= new CMiniGameStopwatch;
	m_vecMiniGame[RMG_TYPING]		= new CMiniGameTyping;
	m_vecMiniGame[RMG_PAIRGAME]		= new CMiniGamePairgame;
	m_vecMiniGame[RMG_LADDER]		= new CMiniGameLadder;

	Init();
}

CRainbowRaceMng::~CRainbowRaceMng(void)
{
	Clear();

	for( DWORD i=0; i<m_vecMiniGame.size(); i++ )
		SAFE_DELETE( m_vecMiniGame[i] );
	
	m_vecMiniGame.clear();
}

BOOL CRainbowRaceMng::Init()
{
	Clear();

	return TRUE;
}

void CRainbowRaceMng::Clear()
{
	SetNextTime(0);
	SetState( RR_CLOSED );
	
	for( MAPRR::iterator it=m_mapRainbowRace.begin(); it!=m_mapRainbowRace.end(); it++ )
	{
		CRainbowRace* pRainbowRace = it->second;
		pRainbowRace->m_wNowGame = 0x0000;
		pRainbowRace->m_wGameState = 0x0000;
		FLWSUser* pUser = static_cast<FLWSUser*>( prj.GetUserByID( it->first ) );
		if( IsValidObj( pUser ) && !pRainbowRace->IsFinished() )
			pUser->AddRainbowRaceState( pRainbowRace );
		SAFE_DELETE( it->second );
	}
	m_mapRainbowRace.clear();
	m_vecdwRankingId.clear();
	m_vecMiniGamePrizeList.clear();
}

CRainbowRaceMng* CRainbowRaceMng::GetInstance()
{
	static CRainbowRaceMng sRainbowRace;
	return & sRainbowRace;
}

// BOOL CRainbowRaceMng::LoadScript()
// {
// 	const TCHAR szFileName[]	= _T( "RainbowRace.lua" );
// 
// 	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
// 	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), szFileName );
// 
// 	if( m_Lua.RunScript( szFullPath ) != 0 )
// 	{
// 		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
// 		exit(0);
// 	}
// 	m_Lua.Pop( 0 );
// 
// 	return TRUE;
// }

void CRainbowRaceMng::Process()
{
	if( IsActivate() == false )
	{
		return;
	}

	DWORD dwTick = GetTickCount();

	switch( GetState() )		// 현재 State에서 다음 State로 넘어갈때 처리, m_nState는 현재 State
	{
		case RR_CLOSED:
		{
			if( IsApplicationTime() )
			{
				g_DPCoreClient.SendCaption( GETTEXT( TID_GAME_RAINBOWRACE_CAPTION_APPLICATION ) );
				SetState( RR_APPLICATION );
				break;
			}
			
			if( IsOpenTime() )
			{
				SetState( RR_OPEN );
				break;
			}
			
			break;
		}

		case RR_APPLICATION:
		{
			if( !IsApplicationTime() )
			{
				//g_DPCoreClient.SendCaption( "레인보우 신청 시간이 끝났어!!" );	
				
				SetNextTime( 0 );
				SetState( RR_CLOSED );
			}
			break;
		}

		case RR_OPEN:
		{
			if( GetApplicantNum() > 0 )
			{
//				SetNextTime( dwTick + static_cast<DWORD>( m_Lua.GetGlobalNumber( "Wait_Time" ) ) );

				const int Day				= GetCurrentDay();
				const DWORD WaitingTick		= GetWaitingTick( Day );
				SetNextTime( dwTick + WaitingTick );
				g_DPCoreClient.SendCaption( GETTEXT( TID_GAME_RAINBOWRACE_CAPTION_WAIT ) );
				SetState( RR_WAIT );
			}
			else
			{
				SetNextTime( 0 );
				SetState( RR_CLOSED );
			}
			break;
		}

		case RR_WAIT:
		{
			if( GetNextTime() < dwTick )
			{
//				SetNextTime( dwTick + static_cast<DWORD>( m_Lua.GetGlobalNumber( "Ready_Time" ) ) );

				const int Day				= GetCurrentDay();
				const DWORD ReadyTick		= GetReadyTick( Day );
				SetNextTime( dwTick + ReadyTick );

				g_DPCoreClient.SendCaption( GETTEXT( TID_GAME_RAINBOWRACE_CAPTION_READY01 ) );
				g_DPCoreClient.SendCaption( GETTEXT( TID_GAME_RAINBOWRACE_CAPTION_READY02 ) );
				SetState( RR_READY );
			}
			break;
		}

		case RR_READY:
		{
			if( GetNextTime() < dwTick )
			{
//				SetNPC();	// 진행 NPC를 생성
				if( SetNPC() == false )
				{
					RemoveNPC();
					SetNextTime( 0 );
					SetState( RR_CLOSED );
					// TODO : 텍스트 대체. 시스템상 문제로 인하여 레인보우 레이스가 종료되는 상황.
					g_DPCoreClient.SendCaption( GETTEXT( TID_GAME_RAINBOWRACE_CAPTION_FINISH01 /*레인보우 레이스가 종료되었습니다.*/ ) );
					break;
				}

				g_DPCoreClient.SendCaption( GETTEXT( TID_GAME_RAINBOWRACE_CAPTION_START ) );
//				SetNextTime( dwTick + static_cast<DWORD>( m_Lua.GetGlobalNumber( "Progress_Time" ) ) );

				const int Day				= GetCurrentDay();
				const DWORD ProgressTick	= GetProgressTick( Day );
				SetNextTime( dwTick + ProgressTick );

				SetState( RR_PROGRESS );
				BeginRainbowRace();
			}
			break;
		}

		case RR_PROGRESS:
		{
			if( GetNextTime() < dwTick )
			{
				RemoveNPC();
				SetMiniGamePrize( 0 );	// 미니게임 상품지급 예약 된거 먼저 지급
				g_DPCoreClient.SendCaption( GETTEXT( TID_GAME_RAINBOWRACE_CAPTION_FINISH01 ) );
				if( m_vecdwRankingId.size() > 0 ) // 우승자 출력
				{
					char szFinish[256] = {0,};
					FLSPrintf( szFinish, _countof( szFinish ), GETTEXT( TID_GAME_RAINBOWRACE_CAPTION_FINISH02 ), 
							CPlayerDataCenter::GetInstance()->GetPlayerString( m_vecdwRankingId[0] ) ); // 우승자 이름
					g_DPCoreClient.SendCaption( szFinish );
				}
				SetPrize();
				g_dpDBClient.SendRainbowRaceRanking( NULL_ID, 0 ); // 레인보우 레이스 종료 - 완주자 랭킹 및 패배자들을 DB에 저장한다.
				SetPrevRanking( m_vecdwRankingId );
				Clear();
			}
			else
				SetMiniGamePrize( dwTick );
			
			break;
		}
	}
}

BOOL CRainbowRaceMng::IsApplicationTime()
{
// 	BOOL bIsAppTime = FALSE;
// 	if( m_Lua.GetLuaFunction( "IsApplicationTime" ) )
// 	{
// 		m_Lua.CallLuaFunction( 0, 1 );
// 		bIsAppTime = static_cast<BOOL>( m_Lua.ToBool( -1 ) );
// 	}
// 
// 	m_Lua.Pop( 0 );
// 
// 	return bIsAppTime;

	const int Day = CalcCurrentDay();

	const nsRainbowRaceData::T_RACE_APPLICATION_DATA* pApplicationData	= GetApplicationData( Day );
	if( pApplicationData == NULL )
	{
		return FALSE;
	}

	const FLTm CurrDate	= DATE_TIMER().GetCurrentTime();
	const FLTm CurrTime	= FLExtractTime( CurrDate );

	const BOOL Can		= CurrTime >= pApplicationData->PeriodData.StartTime && CurrTime <= pApplicationData->PeriodData.EndTime ? TRUE : FALSE;

	if( Can == TRUE )
	{
		SetCurrentDay( Day );
	}

	return Can;
}

BOOL CRainbowRaceMng::IsOpenTime()
{
// 	BOOL bIsOpenTime = FALSE;
// 	if( m_Lua.GetLuaFunction( "IsOpenTime" ) )
// 	{
// 		m_Lua.CallLuaFunction( 0, 1 );
// 		bIsOpenTime = static_cast<BOOL>( m_Lua.ToBool( -1 ) );
// 	}
// 
// 	m_Lua.Pop( 0 );
// 
// 	return bIsOpenTime;

	const int Day = GetCurrentDay();

	const nsRainbowRaceData::T_RACE_APPLICATION_DATA* pApplicationData	= GetApplicationData( Day );
	if( pApplicationData == NULL )
	{
		return FALSE;
	}

	const FLTm CurrDate	= DATE_TIMER().GetCurrentTime();

	const BOOL Can		= CurrDate.hour == pApplicationData->PeriodData.OpenTime.hour && CurrDate.minute == pApplicationData->PeriodData.OpenTime.minute ? TRUE : FALSE;

	return Can;
}

CRainbowRace* CRainbowRaceMng::GetRainbowRacerPtr( DWORD dwPlayerId )
{
	MAPRR::iterator it = m_mapRainbowRace.find( dwPlayerId );
	if( it == m_mapRainbowRace.end() )
		return NULL;

	return it->second;
}

BOOL CRainbowRaceMng::IsEntry( DWORD dwPlayerId )
{
	if( GetState() == RR_PROGRESS )
	{
		CRainbowRace* pRainbowRace = GetRainbowRacerPtr( dwPlayerId );
		if( pRainbowRace && !pRainbowRace->IsFinished() )
			return TRUE;
	}
	
	return FALSE;
}

void CRainbowRaceMng::SetApplicationUser( FLWSUser* pUser )
{
	if( !g_eLocal.GetState( EVE_RAINBOWRACE ) )
		return;

	if( GetState() != RR_APPLICATION )
	{
		// GM은 CLOSE 상태에서도 신청 가능함(테스트 용)
		if( pUser->IsAuthHigher( AUTH_GAMEMASTER ) && GetState() == RR_CLOSED )
			pUser->AddText( "GM Application..." );
		else
		{
			pUser->AddDefinedText( TID_GAME_RAINBOWRACE_ISNOTAPPLICATIONTIME );
			return;
		}
	}

// 	int nPayPenya = 0;
// 	if( m_Lua.GetLuaFunction( "GetPayPenya" ) )
// 	{
// 		m_Lua.CallLuaFunction( 0, 1 );
// 		nPayPenya = (int)( m_Lua.ToNumber( -1 ) );
// 	}
// 	m_Lua.Pop( 0 );

	const int Day			= GetCurrentDay();
	const int nPayPenya		= GetApplicationPenya( Day );

	if( pUser->CheckUserGold( nPayPenya, false ) == false )
	{
		pUser->AddDefinedText( TID_GAME_LACKMONEY );
		return;
	}

	if( IsApplicant( pUser->m_idPlayer ) )	// 이미 신청한 놈이면...
	{
		pUser->AddDefinedText( TID_GAME_RAINBOWRACE_ISAPPLICATIONED );
		return;
	}
	else	// 처음 신청하는 놈이면...
	{
		SetApplication( pUser->m_idPlayer );
		pUser->AddGold( -nPayPenya );
		pUser->AddDefinedText( TID_GAME_RAINBOWRACE_APPLICATION_COMPLETE );
		g_DPSrvr.PutPenyaLog( pUser, "a", "RAINBOW_RACE_APP", nPayPenya );

		g_dpDBClient.SendRainbowRaceApplication( pUser->m_idPlayer );
	}
}

void CRainbowRaceMng::SetApplication( DWORD dwPlayerId )
{
	if( !g_eLocal.GetState( EVE_RAINBOWRACE ) )
		return;
	
	CRainbowRace* pRainbowRace	= new CRainbowRace;
	bool bResult = m_mapRainbowRace.insert( MAPRR::value_type( dwPlayerId, pRainbowRace ) ).second;

	if( bResult == false )
	{
		SAFE_DELETE( pRainbowRace );
	}
}

BOOL CRainbowRaceMng::SetDropOut( DWORD dwPlayerId )
{
	// 현재 참가자이고 완주하지 않은 놈은 탈락!!
	if( GetState() != RR_PROGRESS )
		return FALSE;
	
	MAPRR::iterator it = m_mapRainbowRace.find( dwPlayerId );
	if( it != m_mapRainbowRace.end() && !it->second->IsFinished() )
	{
		g_dpDBClient.SendRainbowRaceFailedUser( it->first );
		SAFE_DELETE( it->second );
		m_mapRainbowRace.erase( it );
		return TRUE;
	}

	return FALSE;
}

void CRainbowRaceMng::BeginRainbowRace()
{
	FLWSUser* pUser = NULL;
	for( MAPRR::iterator it=m_mapRainbowRace.begin(); it!=m_mapRainbowRace.end(); )
	{
		pUser = static_cast<FLWSUser*>( prj.GetUserByID( it->first ) );
		if( !IsValidObj(pUser) )
		{
			g_dpDBClient.SendRainbowRaceFailedUser( it->first );
			SAFE_DELETE( it->second );
			m_mapRainbowRace.erase( it++ );
		}
		else
		{
			pUser->AddRainbowRaceState( it->second );
			it++;
		}
	}
}

bool CRainbowRaceMng::SetNPC()
{
// 	m_Lua.GetGloabal( "tNPC" );
// 
// 	m_Lua.PushNil();
// 	while( m_Lua.TableLoop( -2 ) )
// 	{
// 		DWORD dwNPCID = CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strNPCId" ) );
// 		string strCharKey = m_Lua.GetFieldToString( -1, "strCharKey" );
// 		float x = static_cast<float>( m_Lua.GetFieldToNumber( -1, "xPos" ) );
// 		float y = static_cast<float>( m_Lua.GetFieldToNumber( -1, "yPos" ) );
// 		float z = static_cast<float>( m_Lua.GetFieldToNumber( -1, "zPos" ) );
// 		D3DXVECTOR3 vPos( x, y, z );
// 		
// 		CMover* pMover	= (CMover*)CreateObj( D3DDEVICE, OT_MOVER, dwNPCID );
// 		if( !pMover )
// 		{
// 			FLERROR_LOG( PROGRAM_NAME, _T( "pMover is NULL - %s, %s" ),
// 					m_Lua.GetFieldToString( -1, "strNPCId" ), strCharKey.c_str() );
// 			return;
// 		}
// 		FLStrcpy( pMover->m_szCharacterKey, _countof( pMover->m_szCharacterKey ), strCharKey.c_str() );
// 		pMover->InitNPCProperty();
// 		pMover->InitCharacter( pMover->GetCharacter() );
// 		pMover->SetPos( vPos );
// 		pMover->InitMotion( MTI_STAND );
// 		pMover->UpdateLocalMatrix();
// 		pMover->AddItToGlobalId();
// 		CWorld* pWorld	= g_WorldMng.GetWorld( WI_WORLD_MADRIGAL );
// 		if( pWorld )
// 		{
// 			if( pWorld->ADDOBJ( pMover, FALSE, nDefaultLayer ) == TRUE )
// 			{
// 				m_vecNPCId.push_back( pMover->GetId() );
// 			}
// 			else
// 			{
// 				SAFE_DELETE( pMover );
// 			}
// 		}
// 		else
// 		{
// 			SAFE_DELETE( pMover );
// 		}
// 
// 		m_Lua.Pop( 1 );
// 	}
// 	m_Lua.Pop( 0 );

	for( nsRainbowRaceData::NPCDataVec::const_iterator pos = m_NPCData.begin(); pos != m_NPCData.end(); ++pos )
	{
		CMover* pMover	= (CMover*)CreateObj( D3DDEVICE, OT_MOVER, (*pos).NPCID );
		if( pMover == NULL )
		{
			FLASSERT( 0 );
			FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED NPC CREATE. ID(%u), NAME(%s) ]" ), (*pos).NPCID, (*pos).NPCKey );
			return false;
		}
		FLStrcpy( pMover->m_szCharacterKey, _countof( pMover->m_szCharacterKey ), (*pos).NPCKey );
		pMover->InitNPCProperty();
		pMover->InitCharacter( pMover->GetCharacter() );
		pMover->SetPos( (*pos).Pos );
		pMover->InitMotion( MTI_STAND );
		pMover->UpdateLocalMatrix();
		pMover->AddItToGlobalId();
		CWorld* pWorld	= g_WorldMng.GetWorld( (*pos).WorldID );
		if( pWorld )
		{
			if( pWorld->ADDOBJ( pMover, FALSE, nDefaultLayer ) == TRUE )
			{
				m_vecNPCId.push_back( pMover->GetId() );
			}
			else
			{
				SAFE_DELETE( pMover );
				return false;
			}
		}
		else
		{
			SAFE_DELETE( pMover );
			return false;
		}
	}

	return true;
}

void CRainbowRaceMng::RemoveNPC()
{
	for( DWORD i=0; i<m_vecNPCId.size(); i++ )
	{
		CMover* pMover = prj.GetMover( m_vecNPCId[i] );
		if( IsValidObj( pMover ) )
		{
			g_xWSUserManager->AddCreateSfxObj( pMover, XI_INDEX( 229, XI_SKILL_MER_ONE_SPECIALHIT02 ) );
			pMover->Delete();
		}
	}

	m_vecNPCId.clear();
}

void CRainbowRaceMng::OnMiniGamePacket( FLWSUser* pUser, __MINIGAME_PACKET* pMiniGamePacket )
{
	CRainbowRace* pRainbowRace = GetRainbowRacerPtr( pUser->m_idPlayer );
	if( pRainbowRace )
		pRainbowRace->OnMiniGamePacket( pUser, pMiniGamePacket );
}

void CRainbowRaceMng::SetRanking( FLWSUser* pUser )
{
	CRainbowRace* pRainbowRace = GetRainbowRacerPtr( pUser->m_idPlayer );
	if( pRainbowRace )
	{
		if( !pRainbowRace->IsFinished() )
		{
			if( pRainbowRace->IsAllCompleted() )
			{
				pRainbowRace->SetFinish();	// 완주 설정...
				pUser->AddRainbowRaceState( pRainbowRace );
				m_vecdwRankingId.push_back( pUser->m_idPlayer );
				g_dpDBClient.SendRainbowRaceRanking( pUser->m_idPlayer, m_vecdwRankingId.size() );
				char szText[255]  = {0,};
				FLSPrintf( szText, _countof( szText ), GETTEXT( TID_GAME_RAINBOWRACE_FINISH ), m_vecdwRankingId.size() );
				pUser->AddDiagText( szText );
			}
			else
				pUser->AddDefinedText( TID_GAME_RAINBOWRACE_ISNOTALLCOMPLETED );
				
		}
		else
			pUser->AddDefinedText( TID_GAME_RAINBOWRACE_ISFINISHED );
	}
}

void CRainbowRaceMng::SetPrize()
{
// 	int nCount = 0;
// 	if( m_Lua.GetLuaFunction( "GetPrize" ) )
// 	{
// 		m_Lua.CallLuaFunction( 0, 1 );
// 		if( m_Lua.IsNil( -1 ) )
// 		{
// 			m_Lua.Pop( 0 );
// 			return;
// 		}
// 	}
// 
// 	m_Lua.PushNil();
// 	while( m_Lua.TableLoop( -2 ) )
// 	{
// 		int nPenya = 0;
// 		FLItemElem itemElem;
// 		
// 		DWORD dwItemId = static_cast<DWORD>( CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strItemId" ) ) );
// 		if( dwItemId != NULL_ID )
// 		{
// 			itemElem.m_dwItemId = dwItemId;
// 			itemElem.m_nItemNum = static_cast<int>( m_Lua.GetFieldToNumber( -1, "nItemNum" ) );
// 			itemElem.SetFlag( static_cast<BYTE>( m_Lua.GetFieldToNumber( -2, "byFlag" ) ) );
// 			itemElem.m_bCharged = itemElem.GetProp()->bCharged;
// 		}
// 		nPenya = static_cast<int>( m_Lua.GetFieldToNumber( -1, "nPenya" ) * GetApplicantNum() );
// 		if( nPenya < 0 ) // overflow이면 0페냐
// 			nPenya = 0;
// 		
// 		if( (int)( m_vecdwRankingId.size() ) <= nCount )	// 순위안에 든 사람이 상품 받을 수보다 적으면 중지.
// 			break;
// 
// 		char szTitle[100] = {0,}, szText[255] = {0,}, szTemp[100] = {0,};
// 		FLSPrintf( szTitle, _countof( szTitle ), GETTEXT( TID_GAME_RAINBOWRACE_POSTPRIZE_TITLE ) );
// 		FLSPrintf( szText, _countof( szText ), GETTEXT( TID_GAME_RAINBOWRACE_POSTPRIZE_TEXT_RANKING ), nCount+1 );
// 		if( nPenya > 0 )	// 페냐가 지급되면 우편 내용에 페냐 금액 표시
// 		{
// 			FLSPrintf( szTemp, _countof( szTemp ), GETTEXT( TID_GAME_RAINBOWRACE_POSTPRIZE_TEXT_PENYA ), nPenya );
// 			FLStrcat( szText, _countof( szText ), szTemp );
// 		}
// 
// 		if( dwItemId != NULL_ID )	// 아이템이 지급되면 우편 내용에 아이템명과 개수 표시
// 		{
// 			FLSPrintf( szTemp, _countof( szTemp ), GETTEXT( TID_GAME_RAINBOWRACE_POSTPRIZE_TEXT_ITEM ), itemElem.GetName(), itemElem.m_nItemNum );
// 			FLStrcat( szText, _countof( szText ), szTemp );
// 		}
// 
// 		g_dpDBClient.SendQueryPostMail( m_vecdwRankingId[nCount], 0, itemElem, nPenya, szTitle, szText );
// 		nCount++;
// 		m_Lua.Pop( 1 );
// 	}
// 
// 	m_Lua.Pop( 0 );

	if( m_vecdwRankingId.empty() == true )
	{
		return;
	}

	const int Day	= GetCurrentDay();
	nsRainbowRaceData::PrizeDataMap::const_iterator itr = m_PrizeData.find( Day );
	if( itr == m_PrizeData.end() )
	{
		return;
	}

	const int ApplicationPenya	= GetApplicationPenya( Day );
	const DWORD	RankingSize		= m_vecdwRankingId.size();

	for( nsRainbowRaceData::PrizeDataVec::const_iterator pos = itr->second.begin(); pos != itr->second.end(); ++pos )
	{
		if( RankingSize < (*pos).Ranking )	// 순위안에 든 사람이 상품 받을 수보다 적으면 중지.
		{
			break;
		}

		FLItemElem itemElem;

		const PT_ITEM_SPEC pItemSpec	= g_xSpecManager->GetSpecItem( (*pos).ItemData.ItemID );
		if( pItemSpec != NULL )
		{
			itemElem.m_dwItemId		= (*pos).ItemData.ItemID;
			itemElem.m_nItemNum		= (*pos).ItemData.Quantity;
			itemElem.m_bCharged		= pItemSpec->bCharged;
			if( (*pos).ItemData.Bind == true )
			{
				itemElem.SetFlag( FLItemElem::binds );
			}
			itemElem.SetSerialNumber();
		}

		const __int64 PayPenyaOriginal		= static_cast<__int64>( ApplicationPenya * ( (*pos).ReturnRate / 100 ) ) * static_cast<__int64>( GetApplicantNum() );
		const int PayPenya					= ( PayPenyaOriginal > INT_MAX ) ? 0 : static_cast<int>( PayPenyaOriginal );

		char szTitle[MAX_MAILTITLE] = {0,}, szText[MAX_MAILTEXT] = {0,}, szTemp[100] = {0,};
		FLSPrintf( szTitle, _countof( szTitle ), GETTEXT( TID_GAME_RAINBOWRACE_POSTPRIZE_TITLE ) );
		FLSPrintf( szText, _countof( szText ), GETTEXT( TID_GAME_RAINBOWRACE_POSTPRIZE_TEXT_RANKING ), (*pos).Ranking );
		
		if( PayPenya > 0 )	// 페냐가 지급되면 우편 내용에 페냐 금액 표시
		{
			FLSPrintf( szTemp, _countof( szTemp ), GETTEXT( TID_GAME_RAINBOWRACE_POSTPRIZE_TEXT_PENYA ), PayPenya );
			FLStrcat( szText, _countof( szText ), szTemp );
		}

		if( pItemSpec != NULL )	// 아이템이 지급되면 우편 내용에 아이템명과 개수 표시
		{
			FLSPrintf( szTemp, _countof( szTemp ), GETTEXT( TID_GAME_RAINBOWRACE_POSTPRIZE_TEXT_ITEM ), pItemSpec->szName, itemElem.m_nItemNum );
			FLStrcat( szText, _countof( szText ), szTemp );
		}

		g_dpDBClient.SendQueryPostMail( m_vecdwRankingId.at( (*pos).Ranking - 1 ), 0, itemElem, PayPenya, szTitle, szText );
	}
}

void CRainbowRaceMng::SetMiniGamePrize( DWORD dwTick )
{
// 	for( std::vector<MINIGMAME_PRIZE_LIST>::iterator it=m_vecMiniGamePrizeList.begin(); it!=m_vecMiniGamePrizeList.end(); )
// 	{
// 		if( (dwTick != 0) && ((*it).dwCompletedTick + SEC(15) > dwTick) )
// 		{
// 			it++;
// 			continue;
// 		}
// 	
// 		if( m_Lua.GetLuaFunction( "GetMiniGamePrize" ) )
// 		{
// 			m_Lua.PushNumber( (*it).nCompletedNum );
// 			m_Lua.CallLuaFunction( 1, 1 );
// 			if( !m_Lua.IsNil( -1 ) )
// 			{
// 				FLItemElem itemElem;
// 				itemElem.m_dwItemId = static_cast<DWORD>( CScript::GetDefineNum( m_Lua.GetFieldToString( -1, "strItemId" ) ) );
// 				itemElem.m_nItemNum = static_cast<int>( m_Lua.GetFieldToNumber( -1, "nItemNum" ) );
// 				itemElem.SetFlag( static_cast<BYTE>( m_Lua.GetFieldToNumber( -1, "byFlag" ) ) );
// 				itemElem.m_bCharged = itemElem.GetProp()->bCharged;
// 				char szText[128] = {0,};
// 				FLStrcpy( szText, _countof( szText ), GETTEXT( TID_GAME_RAINBOWRACE_MINIGAMEPRIZE ) );
// 				g_dpDBClient.SendQueryPostMail( (*it).dwPlayerId, 0, itemElem, 0, szText, szText );
// 			}
// 		}
// 		it = m_vecMiniGamePrizeList.erase( it );
// 		m_Lua.Pop( 0 );
// 	}


	const int Day = GetCurrentDay();

	for( std::vector<MINIGMAME_PRIZE_LIST>::iterator it=m_vecMiniGamePrizeList.begin(); it!=m_vecMiniGamePrizeList.end(); )
	{
		if( (dwTick != 0) && ((*it).dwCompletedTick + SEC(15) > dwTick) )
		{
			it++;
			continue;
		}

		const nsRainbowRaceData::T_RACE_MINIGAME_PRIZE_DATA* pMiniGamePrizeData = GetMiniGamePrizeData( Day, (*it).nCompletedNum );
		if( pMiniGamePrizeData != NULL )
		{
			const PT_ITEM_SPEC pItemSpec = g_xSpecManager->GetSpecItem( pMiniGamePrizeData->ItemData.ItemID );
			if( pItemSpec != NULL )
			{
				FLItemElem itemElem;
				itemElem.m_dwItemId 	= pMiniGamePrizeData->ItemData.ItemID;
				itemElem.m_nItemNum 	= pMiniGamePrizeData->ItemData.Quantity;
				itemElem.m_bCharged		= pItemSpec->bCharged;
				if( pMiniGamePrizeData->ItemData.Bind == true )
				{
					itemElem.SetFlag( FLItemElem::binds );
				}
				itemElem.SetSerialNumber();

				char szText[MAX_MAILTEXT] = {0,};
				FLStrcpy( szText, _countof( szText ), GETTEXT( TID_GAME_RAINBOWRACE_MINIGAMEPRIZE ) );

				g_dpDBClient.SendQueryPostMail( (*it).dwPlayerId, 0, itemElem, 0, szText, szText );
			}
		}

		it = m_vecMiniGamePrizeList.erase( it );
	}
}

void CRainbowRaceMng::SetPrevRanking( std::vector<DWORD> & vecPrevRanking )
{
	if( !g_eLocal.GetState( EVE_RAINBOWRACE ) )
		return;

	m_vecPrevRanking.clear();
	m_vecPrevRanking.assign( vecPrevRanking.begin(), vecPrevRanking.end() );
}

std::vector<DWORD>	CRainbowRaceMng::GetPrevRanking()
{
	std::vector<DWORD> vecPrevRanking;
	for( DWORD i=0; (i<5 && i<m_vecPrevRanking.size()); i++ )		// 상위 5위까지만...
		vecPrevRanking.push_back( m_vecPrevRanking[i] );

	return vecPrevRanking;
}

bool	CRainbowRaceMng::Load()
{
	const bool Ret = LoadScript_Time( "RainbowRace\\RainbowRace_Time.csv" )
		&& LoadScript_NPC( "RainbowRace\\RainbowRace_NPC.csv" )
		&& LoadScript_Prize( "RainbowRace\\RainbowRace_Prize.csv" )
		&& LoadScript_MiniGamePrize( "RainbowRace\\RainbowRace_MiniGamePrize.csv" )
		&& CheckLoadScript();

	SetActivate( Ret );
	
	return Ret;
}

#pragma warning( disable : 4482 )

bool	CRainbowRaceMng::LoadScript_Time( const TCHAR* _pFileName )
{
	CScript Scanner;

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), _pFileName );

	if( Scanner.Load( szFullPath, FALSE ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return false;
	}

	ReadState LastRead	= ReadState::None;		// 마지막으로 인서트한 데이터가 공용 설정인지 해당 국가 설정인지 체크 하는 변수.

	const int MyLang	= g_xFlyffConfig->GetMainLanguage();

	int	Lang										= Scanner.GetNumber( TRUE );													// 국가 설정

	while( Scanner.tok != FINISHED )
	{
		Scanner.GetToken( TRUE );
		std::string strDay							= Scanner.Token;
		const int	Day								= DayStringToDefine( strDay );													// 요일

		nsRainbowRaceData::T_RACE_APPLICATION_DATA	ApplicationData;

		Scanner.GetToken( TRUE );
		const std::string strApplicationStartDate	= Scanner.Token;
		ApplicationData.PeriodData.StartTime		= FLStringToFLTm( strApplicationStartDate.c_str(), FLTm::E_FORMAT::HHMMSS );	// 신청 시작 시간

		Scanner.GetToken( TRUE );
		const std::string strApplicationEndDate		= Scanner.Token;
		ApplicationData.PeriodData.EndTime			= FLStringToFLTm( strApplicationEndDate.c_str(), FLTm::E_FORMAT::HHMMSS );		// 신청 종료 시간

		Scanner.GetToken( TRUE );
		const std::string strOpenTime				= Scanner.Token;
		ApplicationData.PeriodData.OpenTime			= FLStringToFLTm( strOpenTime.c_str(), FLTm::E_FORMAT::HHMMSS );				// 오픈 시간

		const DWORD	WaitingTime						= static_cast<DWORD>( Scanner.GetNumber( TRUE ) );								// 준비 대기 시간(분단위)
		ApplicationData.ProgressData.WaitingTick	= MIN( WaitingTime );

		const DWORD ReadyTime						= static_cast<DWORD>( Scanner.GetNumber( TRUE ) );								// 준비 시간(분단위)
		ApplicationData.ProgressData.ReadyTick		= MIN( ReadyTime );

		const DWORD	ProgressTime					= static_cast<DWORD>( Scanner.GetNumber( TRUE ) );								// 진행 시간(분단위)
		ApplicationData.ProgressData.ProgressTick	= MIN( ProgressTime );

		ApplicationData.ApplicationPenya			= Scanner.GetNumber( TRUE );													// 신청 시 필요 페냐

		if( Lang == nsRainbowRaceData::LANG_COMMON || Lang == MyLang )
		{
			if( LastRead != ReadState::MyLanguage && MyLang == Lang )
			{
				InitializeApplicationData();
			}

			LastRead			= ( Lang == nsRainbowRaceData::LANG_COMMON && LastRead != ReadState::MyLanguage ) ? ReadState::Common : ReadState::MyLanguage;

			const bool Insert	= ( Lang == MyLang || ( Lang == nsRainbowRaceData::LANG_COMMON && LastRead == ReadState::Common ) ) ? true : false;
			
			if( Insert == true && InsertApplicationData( Day, ApplicationData ) == false )
			{
				FLASSERT( 0 );
				FLERROR_LOG( PROGRAM_NAME, _T( "[ 레인보우 레이스 시간 설정이 잘못되었습니다. (%s) ]" ), _pFileName );
				return false;
			}
		}

		Lang										= Scanner.GetNumber( TRUE );													// 국가 설정
	}

	return true;
}

bool	CRainbowRaceMng::LoadScript_NPC( const TCHAR* _pFileName )
{
	CScript Scanner;

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), _pFileName );

	if( Scanner.Load( szFullPath, FALSE ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return false;
	}

	m_NPCData.reserve( RMG_MAX );

	DWORD NPCID										= static_cast<DWORD>( Scanner.GetNumber( TRUE ) );								// NPC ID

	while( Scanner.tok != FINISHED )
	{
		nsRainbowRaceData::T_RACE_NPC_DATA	NPCData;

		NPCData.NPCID								= NPCID;

		Scanner.GetToken( TRUE );
		std::string NPCKey							= Scanner.Token;																// NPC Key
		FLStrcpy( NPCData.NPCKey, _countof( NPCData.NPCKey ), NPCKey.c_str() );

		NPCData.WorldID								= static_cast<DWORD>( Scanner.GetNumber( TRUE ) );								// World ID

		NPCData.Pos.x								= Scanner.GetFloat( TRUE );														// 좌표
		NPCData.Pos.y								= Scanner.GetFloat( TRUE );
		NPCData.Pos.z								= Scanner.GetFloat( TRUE );

		if( InsertNPCData( NPCData ) == false )
		{
			FLASSERT( 0 );
			FLERROR_LOG( PROGRAM_NAME, _T( "[ 레인보우 레이스 NPC 설정이 잘못되었습니다. (%s) ]" ), _pFileName );
			return false;
		}

		NPCID										= static_cast<DWORD>( Scanner.GetNumber( TRUE ) );								// NPC ID
	}

	return true;
}

bool	CRainbowRaceMng::LoadScript_Prize( const TCHAR* _pFileName )
{
	CScript Scanner;

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), _pFileName );

	if( Scanner.Load( szFullPath, FALSE ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return false;
	}

	ReadState LastRead	= ReadState::None;		// 마지막으로 인서트한 데이터가 공용 설정인지 해당 국가 설정인지 체크 하는 변수.

	const int MyLang	= g_xFlyffConfig->GetMainLanguage();

	int	Lang										= Scanner.GetNumber( TRUE );													// 국가 설정

	while( Scanner.tok != FINISHED )
	{
		Scanner.GetToken( TRUE );
		std::string strDay							= Scanner.Token;
		const int	Day								= DayStringToDefine( strDay );													// 요일

		nsRainbowRaceData::T_RACE_PRIZE_DATA	PrizeData;

		PrizeData.Ranking							= static_cast<DWORD>( Scanner.GetNumber( TRUE ) );								// 순위
		
		PrizeData.ItemData.ItemID					= static_cast<DWORD>( Scanner.GetNumber( TRUE ) );								// 아이템 아이디

		PrizeData.ItemData.Quantity					= Scanner.GetNumber( TRUE );													// 지급 개수

		PrizeData.ItemData.Bind						= static_cast<bool>( Scanner.GetNumber( TRUE ) != 0 );							// 귀속 여부

		PrizeData.ReturnRate						= Scanner.GetFloat( TRUE );														// 신청 페냐 반환 비율

		if( Lang == nsRainbowRaceData::LANG_COMMON || Lang == MyLang )
		{
			if( LastRead != ReadState::MyLanguage && MyLang == Lang )
			{
				InitializePrizeData();
			}

			LastRead			= ( Lang == nsRainbowRaceData::LANG_COMMON && LastRead != ReadState::MyLanguage ) ? ReadState::Common : ReadState::MyLanguage;

			const bool Insert	= ( Lang == MyLang || ( Lang == nsRainbowRaceData::LANG_COMMON && LastRead == ReadState::Common ) ) ? true : false;

			if( Insert == true && InsertPrizeData( Day, PrizeData ) == false )
			{
				FLASSERT( 0 );
				FLERROR_LOG( PROGRAM_NAME, _T( "[ 레인보우 레이스 순위에 따른 상품 설정이 잘못되었습니다. (%s) ]" ), _pFileName );
				return false;
			}
		}

		Lang										= Scanner.GetNumber( TRUE );													// 국가 설정
	}

	return true;
}

bool	CRainbowRaceMng::LoadScript_MiniGamePrize( const TCHAR* _pFileName )
{
	CScript Scanner;

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), _pFileName );

	if( Scanner.Load( szFullPath, FALSE ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return false;
	}

	ReadState LastRead	= ReadState::None;		// 마지막으로 인서트한 데이터가 공용 설정인지 해당 국가 설정인지 체크 하는 변수.

	const int MyLang	= g_xFlyffConfig->GetMainLanguage();

	int	Lang										= Scanner.GetNumber( TRUE );													// 국가 설정

	while( Scanner.tok != FINISHED )
	{
		Scanner.GetToken( TRUE );
		std::string strDay							= Scanner.Token;
		const int	Day								= DayStringToDefine( strDay );													// 요일

		nsRainbowRaceData::T_RACE_MINIGAME_PRIZE_DATA	MiniGameData;

		MiniGameData.CompleteCount					= static_cast<DWORD>( Scanner.GetNumber( TRUE ) );								// 완료 회수

		MiniGameData.ItemData.ItemID				= static_cast<DWORD>( Scanner.GetNumber( TRUE ) );								// 아이템 아이디

		MiniGameData.ItemData.Quantity				= Scanner.GetNumber( TRUE );													// 지급 개수

		MiniGameData.ItemData.Bind					= static_cast<bool>( Scanner.GetNumber( TRUE ) != 0 );							// 귀속 여부

		if( Lang == nsRainbowRaceData::LANG_COMMON || Lang == MyLang )
		{
			if( LastRead != ReadState::MyLanguage && MyLang == Lang )
			{
				InitializeMiniGamePrizeData();
			}

			LastRead			= ( Lang == nsRainbowRaceData::LANG_COMMON && LastRead != ReadState::MyLanguage ) ? ReadState::Common : ReadState::MyLanguage;

			const bool Insert	= ( Lang == MyLang || ( Lang == nsRainbowRaceData::LANG_COMMON && LastRead == ReadState::Common ) ) ? true : false;

			if( Insert == true && InsertMiniGamePrizeData( Day, MiniGameData ) == false )
			{
				FLASSERT( 0 );
				FLERROR_LOG( PROGRAM_NAME, _T( "[ 레인보우 레이스 미니게임 완료 회수에 따른 상품 설정이 잘못되었습니다. (%s) ]" ), _pFileName );
				return false;
			}
		}

		Lang										= Scanner.GetNumber( TRUE );													// 국가 설정
	}

	return true;
}

bool	CRainbowRaceMng::InsertApplicationData( const int _Day, const nsRainbowRaceData::T_RACE_APPLICATION_DATA & _ApplicationData )
{
	if( _ApplicationData.PeriodData.StartTime == INVALID_DATETIME
		|| _ApplicationData.PeriodData.EndTime == INVALID_DATETIME
		|| _ApplicationData.PeriodData.OpenTime == INVALID_DATETIME )
	{
		FLASSERT( 0 );
		return false;
	}

	// 시작 시간 및 종료 시간 체크.
	if( _ApplicationData.PeriodData.StartTime >= _ApplicationData.PeriodData.EndTime
		|| _ApplicationData.PeriodData.EndTime >= _ApplicationData.PeriodData.OpenTime )
	{
		FLASSERT( 0 );
		return false;
	}

	// 진행 시간 간격 체크.
	if( _ApplicationData.ProgressData.WaitingTick <= 0 || _ApplicationData.ProgressData.ReadyTick <= 0 || _ApplicationData.ProgressData.ProgressTick <= 0 )
	{
		FLASSERT( 0 );
		return false;
	}

	const bool Ret = m_ApplicationData.insert( nsRainbowRaceData::ApplicationDataMap::value_type( _Day, _ApplicationData ) ).second;

	return Ret;
}

bool	CRainbowRaceMng::InsertNPCData( const nsRainbowRaceData::T_RACE_NPC_DATA & _NPCData )
{
	m_NPCData.push_back( _NPCData );

	return true;
}

bool	CRainbowRaceMng::InsertPrizeData( const int _Day, const nsRainbowRaceData::T_RACE_PRIZE_DATA & _PrizeData )
{
	/************************************************************************/
	/*
	순위에 따라 지급되는 아이템은 없을수도 있다. 하지만 랭킹에 의해 반환되는 페냐는 있기 때문에 아이템 설정이 없어도 에러는 아님.
	*/
	/************************************************************************/
	const PT_ITEM_SPEC pItemSpec	= g_xSpecManager->GetSpecItem( _PrizeData.ItemData.ItemID );
	if( pItemSpec != NULL )
	{
		if( pItemSpec->dwPackMax < static_cast<DWORD>( _PrizeData.ItemData.Quantity ) )
		{
			FLASSERT( 0 );
			return false;
		}
	}

	if( m_PrizeData.find( _Day ) == m_PrizeData.end() )
	{
		if( m_PrizeData.insert( nsRainbowRaceData::PrizeDataMap::value_type( _Day, nsRainbowRaceData::PrizeDataVec() ) ).second == false )
		{
			FLASSERT( 0 );
			return false;
		}
	}

	nsRainbowRaceData::PrizeDataMap::iterator itr = m_PrizeData.find( _Day );

	itr->second.push_back( _PrizeData );
	
	return true;
}

bool	CRainbowRaceMng::InsertMiniGamePrizeData( const int _Day, const nsRainbowRaceData::T_RACE_MINIGAME_PRIZE_DATA & _MiniGamePrizeData )
{
	const PT_ITEM_SPEC pItemSpec	= g_xSpecManager->GetSpecItem( _MiniGamePrizeData.ItemData.ItemID );
	if( pItemSpec == NULL )
	{
		FLASSERT( 0 );
		return false;
	}

	if( pItemSpec->dwPackMax < static_cast<DWORD>( _MiniGamePrizeData.ItemData.Quantity ) )
	{
		FLASSERT( 0 );
		return false;
	}

	if( m_MiniGamePrizeData.find( _Day ) == m_MiniGamePrizeData.end() )
	{
		if( m_MiniGamePrizeData.insert( nsRainbowRaceData::MiniGamePrizeDataMap::value_type( _Day, nsRainbowRaceData::MiniGamePrizeDataVec() ) ).second == false )
		{
			FLASSERT( 0 );
			return false;
		}
	}

	nsRainbowRaceData::MiniGamePrizeDataMap::iterator itr = m_MiniGamePrizeData.find( _Day );

	itr->second.push_back( _MiniGamePrizeData );

	return true;
}

void	CRainbowRaceMng::InitializeApplicationData()
{
	m_ApplicationData.clear();
}

void	CRainbowRaceMng::InitializePrizeData()
{
	for( nsRainbowRaceData::PrizeDataMap::iterator pos = m_PrizeData.begin(); pos != m_PrizeData.end(); ++pos )
	{
		pos->second.clear();
	}

	m_PrizeData.clear();
}

void	CRainbowRaceMng::InitializeMiniGamePrizeData()
{
	for( nsRainbowRaceData::MiniGamePrizeDataMap::iterator pos = m_MiniGamePrizeData.begin(); pos != m_MiniGamePrizeData.end(); ++pos )
	{
		pos->second.clear();
	}

	m_MiniGamePrizeData.clear();
}

bool	CRainbowRaceMng::CheckLoadScript() const
{
	// 시간 설정이 안되어 있는 경우 체크.
	if( m_ApplicationData.empty() == true )
	{
		FLASSERT( 0 );
		return false;
	}

	// NPC 설정이 안되어 있거나 미니 게임 수 보다 NPC 수가 모자랄 경우 체크.
	if( m_NPCData.empty() == true || m_NPCData.size() < RMG_MAX )
	{
		FLASSERT( 0 );
		return false;
	}
	
	for( nsRainbowRaceData::PrizeDataMap::const_iterator pos = m_PrizeData.begin(); pos != m_PrizeData.end(); ++pos )
	{
		// 랭킹 체크.
		DWORD RankingNth	= 0;
		for( nsRainbowRaceData::PrizeDataVec::const_iterator pos2 = pos->second.begin(); pos2 != pos->second.end(); ++pos2 )
		{
			if( ++RankingNth != (*pos2).Ranking )
			{
				FLASSERT( 0 );
				return false;
			}
		}
	}

	for( nsRainbowRaceData::MiniGamePrizeDataMap::const_iterator pos = m_MiniGamePrizeData.begin(); pos != m_MiniGamePrizeData.end(); ++pos )
	{
		// 미니게임 완료 회수 체크.
		for( nsRainbowRaceData::MiniGamePrizeDataVec::const_iterator pos2 = pos->second.begin(); pos2 != pos->second.end(); ++pos2 )
		{
			if( (*pos2).CompleteCount <= 0 || (*pos2).CompleteCount > RMG_MAX )
			{
				FLASSERT( 0 );
				return false;
			}
		}
	}

	return true;
}

bool	CRainbowRaceMng::IsActivate() const
{
	return m_Activation;
}

void	CRainbowRaceMng::SetActivate( const bool _Activation )
{
	m_Activation	= _Activation;
	
	if( m_Activation == false )
	{
		FLINFO_LOG( PROGRAM_NAME, _T( "Deactivated Content. 레인보우 레이스가 비활성화 되었습니다." ) );
	}
}

int		CRainbowRaceMng::DayStringToDefine( std::string & _strDay ) const
{
	std::transform( _strDay.begin(), _strDay.end(), _strDay.begin(), toupper );

	if( _strDay == "SUN" )
	{
		return Day::SUN;
	}
	else if( _strDay == "MON" )
	{
		return Day::MON;
	}
	else if( _strDay == "TUE")
	{
		return Day::TUE;
	}
	else if( _strDay == "WED")
	{
		return Day::WED;
	}
	else if( _strDay == "THU")
	{
		return Day::THU;
	}
	else if( _strDay == "FRI")
	{
		return Day::FRI;
	}
	else if( _strDay == "SAT")
	{
		return Day::SAT;
	}
	else
	{
		return DAY_NONE;
	}
}

int		CRainbowRaceMng::CalcCurrentDay() const
{
	const CTime CurrTime	= CTime::GetCurrentTime();
	const int CurrDay		= CurrTime.GetDayOfWeek();

	return CurrDay;
}

bool	CRainbowRaceMng::SetCurrentDay( const int _Day )
{
	FLASSERT( _Day > DAY_NONE && _Day < DAY_MAX );
	
	if( _Day > DAY_NONE && _Day < DAY_MAX )
	{
		m_Day	= _Day;

		return true;
	}

	return false;
}

int		CRainbowRaceMng::GetCurrentDay() const
{
	return m_Day;
}

int		CRainbowRaceMng::GetApplicationPenya( const int _Day ) const
{
	const nsRainbowRaceData::T_RACE_APPLICATION_DATA* pApplicationData	= GetApplicationData( _Day );
	if( pApplicationData == NULL )
	{
		FLASSERT( 0 );
		return INT_MAX;
	}

	return pApplicationData->ApplicationPenya;
}

DWORD	CRainbowRaceMng::GetWaitingTick( const int _Day ) const
{
	const nsRainbowRaceData::T_RACE_APPLICATION_DATA* pApplicationData	= GetApplicationData( _Day );
	if( pApplicationData == NULL )
	{
		FLASSERT( 0 );
		return NULL_ID;
	}

	return pApplicationData->ProgressData.WaitingTick;
}

DWORD	CRainbowRaceMng::GetReadyTick( const int _Day ) const
{
	const nsRainbowRaceData::T_RACE_APPLICATION_DATA* pApplicationData	= GetApplicationData( _Day );
	if( pApplicationData == NULL )
	{
		FLASSERT( 0 );
		return NULL_ID;
	}

	return pApplicationData->ProgressData.ReadyTick;
}

DWORD	CRainbowRaceMng::GetProgressTick( const int _Day ) const
{
	const nsRainbowRaceData::T_RACE_APPLICATION_DATA* pApplicationData	= GetApplicationData( _Day );
	if( pApplicationData == NULL )
	{
		FLASSERT( 0 );
		return NULL_ID;
	}

	return pApplicationData->ProgressData.ProgressTick;
}

const nsRainbowRaceData::T_RACE_APPLICATION_DATA*	CRainbowRaceMng::GetApplicationData( const int _Day ) const
{
	nsRainbowRaceData::ApplicationDataMap::const_iterator itr = m_ApplicationData.find( _Day );
	if( itr != m_ApplicationData.end() )
	{
		return &(itr->second);
	}

	return NULL;
}

const nsRainbowRaceData::T_RACE_MINIGAME_PRIZE_DATA*	CRainbowRaceMng::GetMiniGamePrizeData( const int _Day, const DWORD _CompleteCount ) const
{
	nsRainbowRaceData::MiniGamePrizeDataMap::const_iterator itr = m_MiniGamePrizeData.find( _Day );
	if( itr == m_MiniGamePrizeData.end() )
	{
		return NULL;
	}

	for( nsRainbowRaceData::MiniGamePrizeDataVec::const_iterator pos = itr->second.begin(); pos != itr->second.end(); ++pos )
	{
		if( (*pos).CompleteCount == _CompleteCount )
		{
			return &(*pos);
		}
	}

	return NULL;
}

#endif // __WORLDSERVER
