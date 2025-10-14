// CampusHelper.cpp: implementation of the CCampusHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CampusHelper.h"

#include "User.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCampusHelper::CCampusHelper()
{
	m_pCampusMng = new CCampusMng;
	m_dwRecoveryTime	= 0;
	m_nRecoveryPoint	= 0;
}

CCampusHelper::~CCampusHelper()
{
	Clear();
}

CCampusHelper* CCampusHelper::GetInstance()
{
	static CCampusHelper sCH;
	return & sCH;
}

void CCampusHelper::Clear()
{
	m_vecCQuest.clear();
	m_mapCBuff.clear();
	m_mapCReward.clear();
	SAFE_DELETE( m_pCampusMng );
}

void CCampusHelper::OnAddCampusMember( CAr & ar )
{
	u_long idCampus, idMaster, idPupil;
	ar >> idCampus >> idMaster >> idPupil;
	
	FLWSUser* pMaster = g_xWSUserManager->GetUserByPlayerID( idMaster );
	FLWSUser* pPupil = g_xWSUserManager->GetUserByPlayerID( idPupil );

	CCampus* pCampus = GetCampus( idCampus );
	if( pCampus )
	{
		if( pCampus->IsMaster( idMaster ) )
		{
			CCampusMember* pCMPupil = new CCampusMember;
			pCMPupil->SetLevel( CAMPUS_PUPIL );
			pCMPupil->SetPlayerId( idPupil );
			if( pCampus->AddMember( pCMPupil ) && AddPlayerId2CampusId( idPupil, pCampus->GetCampusId() ) )
			{
				if( IsValidObj( pPupil ) )
				{
					pPupil->SetCampusId( pCampus->GetCampusId() );
					pPupil->AddDefinedText( TID_GAME_TS_ACCSTUDENT, "\"%s\"", pPupil->GetName() );
				}
				if( IsValidObj( pMaster ) )
					pMaster->AddDefinedText( TID_GAME_TS_ACCTEATHER, "\"%s\"", pMaster->GetName() );
			}
			else
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Add Pupil failed!" ) );
				SAFE_DELETE( pCMPupil );
				return;
			}
		}
	}
	else
	{
		pCampus = new CCampus;
		pCampus->SetCampusId( idCampus );
		pCampus->SetMaster( idMaster );
		if( AddCampus( pCampus ) )
		{
			CCampusMember* pCMMaster = new CCampusMember;
			CCampusMember* pCMPupil = new CCampusMember;
			pCMMaster->SetPlayerId( idMaster );
			pCMMaster->SetLevel( CAMPUS_MASTER );
			pCMPupil->SetPlayerId( idPupil );
			pCMPupil->SetLevel( CAMPUS_PUPIL );
			if( pCampus->AddMember( pCMMaster ) && pCampus->AddMember( pCMPupil )
				&& AddPlayerId2CampusId( idMaster, idCampus ) && AddPlayerId2CampusId( idPupil, idCampus ) )
			{
				if( IsValidObj( pMaster ) )
				{
					pMaster->SetCampusId( pCampus->GetCampusId() );
					pMaster->AddDefinedText( TID_GAME_TS_ACCTEATHER, "\"%s\"", pMaster->GetName() );
				}
				if( IsValidObj( pPupil ) )
				{
					pPupil->SetCampusId( pCampus->GetCampusId() );
					pPupil->AddDefinedText( TID_GAME_TS_ACCSTUDENT, "\"%s\"", pPupil->GetName() );
				}
			}
			else
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Add Master & Pupil failed!" ) );
				SAFE_DELETE( pCMMaster );
				SAFE_DELETE( pCMPupil );
				RemoveCampus( idCampus );
				return;
			}
		}
		else
		{
			SAFE_DELETE( pCampus );
			return;
		}
	}
	AddAllMemberUpdateCampus( pCampus );
}

void CCampusHelper::OnRemoveCampusMember( CAr & ar )
{
	u_long idCampus, idPlayer;
	ar >> idCampus >> idPlayer;

	CCampus* pCampus = GetCampus( idCampus );
	if( pCampus && pCampus->IsMember( idPlayer ) )
	{
		if( pCampus->IsMaster( idPlayer ) )
		{
			AddAllMemberRemoveCampus( pCampus );
			RemoveCampus( idCampus );
		}
		else
		{
			pCampus->RemoveMember( idPlayer );
			RemovePlayerId2CampusId( idPlayer );
			FLWSUser* pPupil = g_xWSUserManager->GetUserByPlayerID( idPlayer );
			if( IsValidObj( pPupil ) )
			{
				pPupil->AddRemoveCampus( idCampus );
				pPupil->SetCampusId( 0 );
				PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData( pCampus->GetMaster() );
				if( pPlayerData )
					pPupil->AddDefinedText( TID_GAME_TS_BREAK, "\"%s\"", pPlayerData->szPlayer );
			}

			FLWSUser* pMaster = g_xWSUserManager->GetUserByPlayerID( pCampus->GetMaster() );
			if( IsValidObj( pMaster ) )
			{
				PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData( idPlayer );
				if( pPlayerData )
					pMaster->AddDefinedText( TID_GAME_TS_BREAK, "\"%s\"", pPlayerData->szPlayer );
			}

			if( pCampus->GetMemberSize() < 2 )
			{
				AddAllMemberRemoveCampus( pCampus );
				RemoveCampus( idCampus );
			}
			else
				AddAllMemberUpdateCampus( pCampus );
		}
	}
}

void CCampusHelper::OnUpdateCampusPoint( CAr & ar )
{
	u_long idPlayer;
	int nCampusPoint;
	ar >> idPlayer >> nCampusPoint;
	FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( idPlayer );
	if( IsValidObj( pUser ) )
	{
		pUser->SetCampusPoint( nCampusPoint );
		pUser->AddUpdateCampusPoint( nCampusPoint );
		if( pUser->GetCampusPoint() > 0 )
			pUser->SetCampusTick( NULL_ID );
	}
}

void CCampusHelper::OnUpdatePlayerData( u_long idPlayer, PlayerData* pPlayerData )
{
	CCampus* pCampus = GetCampus( GetCampusIdByPlayerId( idPlayer ) );
	if( pCampus )
	{
		std::vector<u_long> vecMember = pCampus->GetAllMemberPlayerId();
		for( std::vector<u_long>::iterator it = vecMember.begin(); it != vecMember.end(); ++it )
		{
			FLWSUser* pMember = g_xWSUserManager->GetUserByPlayerID( *it );
			if( IsValidObj( pMember ) )
				pMember->AddQueryPlayerData( idPlayer, pPlayerData );
		}
	}
}

void CCampusHelper::LoadScript()
{
	CLuaBase Lua;

	const TCHAR szFileName[]	= _T( "Campus.lua" );

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), szFileName );

	if( Lua.RunScript( szFullPath ) != 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		exit(0);
	}
	m_dwRecoveryTime		=	static_cast<DWORD>( Lua.GetGlobalNumber( "RecoveryTime" ) );
	m_nRecoveryPoint		=	static_cast<int>( Lua.GetGlobalNumber( "RecoveryPoint" ) );

	Lua.GetGloabal( "tCampusQuest" );
	Lua.PushNil();
	while( Lua.TableLoop( -2 ) )
	{
		int nQuestId		=	static_cast<int>( CScript::GetDefineNum( Lua.GetFieldToString( -1, "strQuestId" ) ) );
		m_vecCQuest.push_back( nQuestId );
		Lua.Pop( 1 );
	}
	Lua.Pop( 0 );

	Lua.GetGloabal( "tCampusBuff" );
	Lua.PushNil();
	while( Lua.TableLoop( -2 ) )
	{
		int nLevel			=	static_cast<int>( Lua.GetFieldToNumber( -1, "nLevel" ) );
		DWORD dwBuffId		=	static_cast<DWORD>( CScript::GetDefineNum( Lua.GetFieldToString( -1, "strBuffId" ) ) );
		m_mapCBuff.insert( MAP_CBUFF::value_type( nLevel, dwBuffId ) );
		Lua.Pop( 1 );
	}
	Lua.Pop( 0 );

	Lua.GetGloabal( "tCampusReward" );
	Lua.PushNil();
	while( Lua.TableLoop( -2 ) )
	{
		REWARD reward;
		int nLevel				=	static_cast<int>( Lua.GetFieldToNumber( -1, "nLevel" ) );
		reward.nMasterReward	=	static_cast<int>( Lua.GetFieldToNumber( -1, "nMasterReward" ) );
		reward.nPupilReward		=	static_cast<int>( Lua.GetFieldToNumber( -1, "nPupilReward" ) );
		
		m_mapCReward.insert( MAP_CREWORD::value_type( nLevel, reward ) );
		Lua.Pop( 1 );
	}
	Lua.Pop( 0 );
}

BOOL CCampusHelper::IsInviteAble( FLWSUser* pRequest, FLWSUser* pTarget )
{
	if( !IsValidObj( pRequest ) || !IsValidObj( pTarget ) )
		return FALSE;

	// 나의 사제포인트가 0 미만이면 안됨
	if( pRequest->GetCampusPoint() < 0 )
	{
		pRequest->AddDefinedText( TID_GAME_TS_WANTMYTSP );
		return FALSE;
	}
	// 상대방의 사제포인트가 0 미만이면 안됨
	if( pTarget->GetCampusPoint() < 0 )
	{
		pRequest->AddDefinedText( TID_GAME_TS_WANTYOURTSP );
		return FALSE;
	}
	// 사제맺기를 신청한 유저가 스승일 때
	if( IsMasterLevel( pRequest ) && IsPupilLevel( pTarget ) )
	{
		// 사제 퀘스트를 안했다면 안됨
		if( !IsCompleteCampusQuest( pRequest ) )
		{
			pRequest->AddDefinedText( TID_GAME_TS_NOTQUEST, "\"%s\"", pRequest->GetName() );
			return FALSE;
		}
		CCampus* pCampus = GetCampus( pRequest->GetCampusId() );
		if( pCampus )
		{
			// 동시에 받을 수 있는 제자의 수를 넘으면 안됨
			if( pCampus->GetPupilNum() >= GetMaxPupilNum( pRequest ) )
			{
				pRequest->AddDefinedText( TID_GAME_TS_FULLSTUDENT, "\"%s\"", pRequest->GetName() );
				return FALSE;
			}
		}
		// 상대방이 사제관계를 맺고 있으면 안됨
		if( GetCampus( pTarget->GetCampusId() ) )
		{
			pRequest->AddDefinedText( TID_GAME_TS_ALREADY, "\"%s\"", pTarget->GetName() );
			return FALSE;
		}
		return TRUE;
	}
	// 사제맺기를 신청한 유저가 제자일 때
	else if( IsPupilLevel( pRequest ) && IsMasterLevel( pTarget ) )
	{
		// 사제관계를 맺고 있으면 안됨
		if( GetCampus( pRequest->GetCampusId() ) )
		{
			pRequest->AddDefinedText( TID_GAME_TS_ALREADY, "\"%s\"", pRequest->GetName() );
			return FALSE;
		}
		// 상대방이 사제 퀘스트를 안했다면 안됨
		if( !IsCompleteCampusQuest( pTarget ) )
		{
			pRequest->AddDefinedText( TID_GAME_TS_NOTQUEST, "\"%s\"", pTarget->GetName() );
			return FALSE;
		}
		CCampus* pCampus = GetCampus( pTarget->GetCampusId() );
		if( pCampus )
		{
			// 상대방이 동시에 받을 수 있는 제자의 수를 넘으면 안됨
			if( pCampus->GetPupilNum() >= GetMaxPupilNum( pTarget ) )
			{
				pRequest->AddDefinedText( TID_GAME_TS_FULLSTUDENT, "\"%s\"", pTarget->GetName() );
				return FALSE;
			}
		}
		return TRUE;
	}
	// 이도 저도 아닐때...
	else
		pRequest->AddDefinedText( TID_GAME_TS_NOTLEVEL );
	return FALSE;
}

BOOL CCampusHelper::IsMasterLevel( FLWSUser* pUser )
{
	if( IsValidObj( pUser ) )
	{
		if( pUser->GetLevel() >= MIN_MASTER_LEVEL )
			return TRUE;
	}
	return FALSE;
}

BOOL CCampusHelper::IsPupilLevel( FLWSUser* pUser )
{
	if( IsValidObj( pUser ) )
	{
		if( pUser->GetJob() < MAX_EXPERT )
			return TRUE;
	}
	return FALSE;
}

int CCampusHelper::GetMaxPupilNum( FLWSUser* pUser )
{
	if( IsValidObj( pUser ) )
	{
		if( pUser->GetCampusPoint() >= 0 && pUser->GetCampusPoint() < MIN_LV2_POINT )
			return 1;
		else if( pUser->GetCampusPoint() >= MIN_LV2_POINT && pUser->GetCampusPoint() < MIN_LV3_POINT )
			return 2;
		else if( pUser->GetCampusPoint() >= MIN_LV3_POINT )
			return 3;
	}
	return 0;
}

BOOL CCampusHelper::IsCompleteCampusQuest( FLWSUser* pUser )
{
	if( !IsValidObj( pUser ) )
		return FALSE;

	for( std::vector<int>::iterator it = m_vecCQuest.begin(); it != m_vecCQuest.end(); ++it )
	{
		if( !pUser->IsCompleteQuest( *it ) )
			return FALSE;
	}
	return TRUE;
}

void CCampusHelper::AddAllMemberUpdateCampus( CCampus* pCampus )
{
	if( !pCampus )
		return;

	std::vector<u_long> vecMember = pCampus->GetAllMemberPlayerId();
	for( std::vector<u_long>::iterator it = vecMember.begin(); it != vecMember.end(); ++it )
	{
		FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( *it );
		if( IsValidObj( pUser ) )
			pUser->AddUpdateCampus( pCampus );
	}
}

void CCampusHelper::AddAllMemberRemoveCampus( CCampus* pCampus )
{
	if( !pCampus )
		return;

	std::vector<u_long> vecMember = pCampus->GetAllMemberPlayerId();
	for( std::vector<u_long>::iterator it = vecMember.begin(); it != vecMember.end(); ++it )
	{
		RemovePlayerId2CampusId( *it );
		FLWSUser* pUser = g_xWSUserManager->GetUserByPlayerID( *it );
		if( IsValidObj( pUser ) )
		{
			pUser->AddRemoveCampus( pCampus->GetCampusId() );
			pUser->SetCampusId( 0 );
		}
	}
}

DWORD CCampusHelper::GetCampusBuff( int nLevel )
{
	MAP_CBUFF::iterator it = m_mapCBuff.find( nLevel );
	if( it != m_mapCBuff.end() )
		return it->second;

	return 0;
}

void CCampusHelper::RecoveryCampusPoint( FLWSUser* pUser, DWORD dwTick )
{
	if( pUser->GetCampusPoint() < 0 )
	{
		if( pUser->GetCampusTick() == NULL_ID )
			pUser->SetCampusTick( dwTick );
		else if( dwTick > pUser->GetCampusTick() + m_dwRecoveryTime )
		{
			pUser->SetCampusTick( dwTick );
			g_dpDBClient.SendUpdateCampusPoint( pUser->m_idPlayer, m_nRecoveryPoint, TRUE, 'R' );
		}
	}
}

void CCampusHelper::SetLevelUpReward( FLWSUser* pUser )
{
	if( !IsValidObj( pUser ) )
		return;

	CCampus* pCampus = GetCampus( pUser->GetCampusId() );
	if( pCampus && pCampus->IsMember( pUser->m_idPlayer ) )
	{
		if( pCampus->IsMaster( pUser->m_idPlayer ) )
			return;

		REWARD* pReward = GetReward( pUser->GetLevel() );
		if( pReward )
		{
			g_dpDBClient.SendUpdateCampusPoint( pCampus->GetMaster(), pReward->nMasterReward, TRUE, 'L' );
			g_dpDBClient.SendUpdateCampusPoint( pUser->m_idPlayer, pReward->nPupilReward, TRUE, 'L' );
		}
		// 제자의 레벨이 졸업가능한 레벨이 되는 경우 사제관계 해지
		if( pUser->GetLevel() >= COMPLETE_PUPIL_LEVEL )
			g_dpDBClient.SendRemoveCampusMember( pCampus->GetCampusId(), pUser->m_idPlayer );
	}
}

CCampusHelper::REWARD* CCampusHelper::GetReward( int nLevel )
{
	MAP_CREWORD::iterator it = m_mapCReward.find( nLevel );
	if( it != m_mapCReward.end() )
		return &it->second;

	return NULL;
}

void CCampusHelper::OnInviteCampusMember( FLWSUser* pRequest, FLWSUser* pTarget )
{
	if( !IsValidObj( pRequest ) || !IsValidObj( pTarget ) )
		return;
	
	if( IsInviteAble( pRequest, pTarget ) )
		pTarget->AddInviteCampusMember( pRequest );
}

void CCampusHelper::OnAcceptCampusMember( FLWSUser* pRequest, FLWSUser* pTarget )
{
	if( !IsValidObj( pRequest ) || !IsValidObj( pTarget ) )
		return;

	if( IsMasterLevel( pRequest ) && IsPupilLevel( pTarget ) && IsInviteAble( pRequest, pTarget ) )
		g_dpDBClient.SendAddCampusMember( pRequest->m_idPlayer, pRequest->GetCampusPoint(), pTarget->m_idPlayer, pTarget->GetCampusPoint() );
	
	else if( IsMasterLevel( pTarget ) && IsPupilLevel( pRequest ) && IsInviteAble( pRequest, pTarget ) )
		g_dpDBClient.SendAddCampusMember( pTarget->m_idPlayer, pTarget->GetCampusPoint(), pRequest->m_idPlayer, pRequest->GetCampusPoint() );
}

void CCampusHelper::OnRemoveCampusMember( FLWSUser* pRequest, u_long idTarget )
{
	if( !IsValidObj( pRequest ) || idTarget <= 0 )
		return;
	
	CCampus* pCampus = GetCampus( pRequest->GetCampusId() );
	if( pCampus && pCampus->IsMember( idTarget ) )
	{
		if( pCampus->IsMaster( pRequest->m_idPlayer ) && pCampus->IsPupil( idTarget ) )
			g_dpDBClient.SendRemoveCampusMember( pCampus->GetCampusId(), idTarget );
		else if( pCampus->IsPupil( pRequest->m_idPlayer ) && pCampus->IsMaster( idTarget ) )
			g_dpDBClient.SendRemoveCampusMember( pCampus->GetCampusId(), pRequest->m_idPlayer );
		else
			return;
		
		g_dpDBClient.SendUpdateCampusPoint( pRequest->m_idPlayer, REMOVE_CAMPUS_POINT, FALSE, 'D' );
	}
}
