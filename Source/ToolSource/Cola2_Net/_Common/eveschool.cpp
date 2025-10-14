#include "stdafx.h"

#include "eveschool.h"
#ifdef __WORLDSERVER
#ifdef __SCHOOL0701
#include "DPSrvr.h"
#include "DPCoreClient.h"
#include "guild.h"
#include "dpdatabaseclient.h"
#include "user.h"
extern	CDPSrvr		g_DPSrvr;
extern	CDPCoreClient	g_DPCoreClient;
extern	CGuildMng	g_GuildMng;

extern	CDPDatabaseClient	g_dpDBClient;

extern	CUserMng	g_UserMng;

#include "WorldMng.h"
extern CWorldMng g_WorldMng;

#ifdef __GUILDCOMBAT
#include "definetext.h"
#include "defineobj.h"
#include "definesound.h"
#endif //__GUILDCOMBAT

CEveSchool::CEveSchool()
{
	m_pSchoolMng	= &g_GuildMng;
#ifdef __SCHOOL0719
	m_dwsbstart2	= GetTickCount();
#endif	// __SCHOOL0719
}

BOOL CEveSchool::Ready( void )
{
// "/sbready
	if( g_eLocal.GetState( EVE_SCHOOL_BATTLE ) != SBS_END )
		return FALSE;

	g_UserMng.ModifyMode( OBSERVE_MODE );	// Observe mode	// all

	prj.m_AddRemoveLock.Enter( theLineFile );
	m_pSchoolMng->m_AddRemoveLock.Enter( theLineFile );

	for( map<u_long, CGuild*>::iterator i = m_pSchoolMng->m_mapPGuild.begin(); i != m_pSchoolMng->m_mapPGuild.end(); ++i )
	{
		CGuild* pSchool	= i->second;
#ifdef __SCHOOL0719
		pSchool->m_nDead	= 0;
#endif	// __SCHOOL0719
		for( map<u_long, CGuildMember*>::iterator i2 = pSchool->m_mapPMember.begin(); i2 != pSchool->m_mapPMember.end(); ++i2 )
		{
			CGuildMember* pMember	= i2->second;
			CUser* pUser	= g_UserMng.GetUserByPlayerID( pMember->m_idPlayer );
			if( IsValidObj( (CObj*)pUser ) )
			{
				pUser->Replace( g_uIdofMulti, WI_WORLD_EVENT01, GetPos( pSchool->m_idGuild ) );	// Replace
				pUser->SetNotMode( OBSERVE_MODE );	// Not observe mode
				g_UserMng.AddModifyMode( pUser );
			}
		}
	}

	m_pSchoolMng->m_AddRemoveLock.Leave( theLineFile );

	if( g_eLocal.SetState( EVE_SCHOOL_BATTLE, SBS_READY ) )
		g_UserMng.AddSetLocalEvent( EVE_SCHOOL_BATTLE, SBS_READY );

	prj.m_AddRemoveLock.Leave( theLineFile );

	return TRUE;
}

BOOL CEveSchool::Start( void )
{
// "/sbstart"
	if( g_eLocal.GetState( EVE_SCHOOL_BATTLE ) != SBS_READY )
		return FALSE;

	if( g_eLocal.SetState( EVE_SCHOOL_BATTLE, SBS_START ) )
		g_UserMng.AddSetLocalEvent( EVE_SCHOOL_BATTLE, SBS_START );
	return TRUE;
}

BOOL CEveSchool::Start2( void )
{
// "/sbstart2"
	if( g_eLocal.GetState( EVE_SCHOOL_BATTLE ) != SBS_START )
		return FALSE;

	if( g_eLocal.SetState( EVE_SCHOOL_BATTLE, SBS_START2 ) )
		g_UserMng.AddSetLocalEvent( EVE_SCHOOL_BATTLE, SBS_START2 );

#ifdef __SCHOOL0719
	m_dwsbstart2	= GetTickCount();
#endif	// __SCHOOL0719
	return TRUE;
}

BOOL CEveSchool::Report( void )
{
//	"/sbreport"
	if( g_eLocal.GetState( EVE_SCHOOL_BATTLE ) == SBS_END )
		return FALSE;

	SCHOOL_ENTRY	school[MAX_SCHOOL];

	m_pSchoolMng->m_AddRemoveLock.Enter( theLineFile );

	int iIndex	= 0;
	for( map<u_long, CGuild*>::iterator i = m_pSchoolMng->m_mapPGuild.begin();
		i != m_pSchoolMng->m_mapPGuild.end(); ++i )
	{
		CGuild* pSchool	= i->second;
		
		school[iIndex].id	= pSchool->m_idGuild;
		lstrcpy( school[iIndex].lpName, pSchool->m_szGuild );
#ifdef __SCHOOL0719
		school[iIndex].nDead	= pSchool->m_nDead;
#endif	// __SCHOOL0719

		for( map<u_long, CGuildMember*>::iterator i2 = pSchool->m_mapPMember.begin();
		i2 != pSchool->m_mapPMember.end(); ++i2 )
		{
			CGuildMember* pMember	= i2->second;
			CUser* pUser	= g_UserMng.GetUserByPlayerID( pMember->m_idPlayer );
			if( IsValidObj( (CObj*)pUser ) )
			{
				school[iIndex].nSize++;
				if( pUser->IsRegionAttr( RA_FIGHT ) )
				{
					if( !pUser->IsMode( OBSERVE_MODE ) )
					{
						school[iIndex].nSurvivor++;
						school[iIndex].nLevel	+= pUser->GetLevel();
					}
				}
			}
		}
		iIndex++;
	}
	m_pSchoolMng->m_AddRemoveLock.Leave( theLineFile );

	// sort
	for( int j = 0; j < MAX_SCHOOL-1; j++ )
	{
		for( int k = j + 1; k < MAX_SCHOOL; k++ )
		{
			int nAvg1, nAvg2;
			if( school[j].nSurvivor == 0 )
				nAvg1	= 0;
			else
				nAvg1	= school[j].nLevel / school[j].nSurvivor;
			if( school[k].nSurvivor == 0 )
				nAvg2	= 0;
			else
				nAvg2	= school[k].nLevel / school[k].nSurvivor;

			if( ( school[j].nSurvivor < school[k].nSurvivor ) ||
				( school[j].nSurvivor == school[k].nSurvivor && nAvg1 < nAvg2 ) )
			{	// swap
				SCHOOL_ENTRY tmp;
				memcpy( &tmp, &school[j], sizeof(SCHOOL_ENTRY) );
				memcpy( &school[j], &school[k], sizeof(SCHOOL_ENTRY) );
				memcpy( &school[k], &tmp, sizeof( SCHOOL_ENTRY) );
			}
		}
	}

#ifdef __SCHOOL0719
	short nElapse	= (short)( (int)( GetTickCount() - m_dwsbstart2 ) / (int)MIN( 1 ) );
	g_UserMng.AddSchoolReport( school, nElapse );	// neuz
#else	// __SCHOOL0719
	g_UserMng.AddSchoolReport( school );	// neuz
#endif	// __SCHOOL0719

	return TRUE;
}

BOOL CEveSchool::End( void )
{
// "/sbend"
#ifdef __SCHOOL0719
	if( g_eLocal.GetState( EVE_SCHOOL_BATTLE ) != SBS_START2 )	// '/sbstart2'
#else	// __SCHOOL0719
	if( g_eLocal.GetState( EVE_SCHOOL_BATTLE ) < SBS_START )	// SBS_START, SBS_START2
#endif	// __SCHOOL0719
		return FALSE;

	SCHOOL_ENTRY	school[MAX_SCHOOL];

	m_pSchoolMng->m_AddRemoveLock.Enter( theLineFile );

	int iIndex	= 0;
	for( map<u_long, CGuild*>::iterator i = m_pSchoolMng->m_mapPGuild.begin();
		i != m_pSchoolMng->m_mapPGuild.end(); ++i )
	{
		CGuild* pSchool	= i->second;
		
		school[iIndex].id	= pSchool->m_idGuild;
		lstrcpy( school[iIndex].lpName, pSchool->m_szGuild );
#ifdef __SCHOOL0719
		school[iIndex].nDead	= pSchool->m_nDead;
#endif	// __SCHOOL0719

		for( map<u_long, CGuildMember*>::iterator i2 = pSchool->m_mapPMember.begin();
		i2 != pSchool->m_mapPMember.end(); ++i2 )
		{
			CGuildMember* pMember	= i2->second;
			CUser* pUser	= g_UserMng.GetUserByPlayerID( pMember->m_idPlayer );
			if( IsValidObj( (CObj*)pUser ) )
			{
				if( pUser->IsRegionAttr( RA_FIGHT ) )
				{
					if( !pUser->IsMode( OBSERVE_MODE ) )
					{
						school[iIndex].nSurvivor++;
						school[iIndex].nLevel	+= pUser->GetLevel();
					}
				}
				else
				{
					pUser->SetMode( OBSERVE_MODE );		// MAKE observe mode
					g_UserMng.AddModifyMode( pUser );
				}
			}
		}
		iIndex++;
	}

	m_pSchoolMng->m_AddRemoveLock.Leave( theLineFile );

	// sort
	for( int j = 0; j < MAX_SCHOOL-1; j++ )
	{
		for( int k = j + 1; k < MAX_SCHOOL; k++ )
		{
			int nAvg1, nAvg2;
			if( school[j].nSurvivor == 0 )
				nAvg1	= 0;
			else
				nAvg1	= school[j].nLevel / school[j].nSurvivor;
			if( school[k].nSurvivor == 0 )
				nAvg2	= 0;
			else
				nAvg2	= school[k].nLevel / school[k].nSurvivor;

			if( ( school[j].nSurvivor < school[k].nSurvivor ) ||
				( school[j].nSurvivor == school[k].nSurvivor && nAvg1 < nAvg2 ) )
			{	// swap
				SCHOOL_ENTRY tmp;
				memcpy( &tmp, &school[j], sizeof(SCHOOL_ENTRY) );
				memcpy( &school[j], &school[k], sizeof(SCHOOL_ENTRY) );
				memcpy( &school[k], &tmp, sizeof( SCHOOL_ENTRY) );
			}
		}
	}

	if( g_eLocal.SetState( EVE_SCHOOL_BATTLE, SBS_END ) )
		g_UserMng.AddSetLocalEvent( EVE_SCHOOL_BATTLE, SBS_END );

	g_dpDBClient.SendSchoolReport( school );	// log
	g_UserMng.AddSchoolReport( school );	// neuz	

	return TRUE;
}

BOOL CEveSchool::LoadPos( LPCSTR lpszFileName )
{
	CScanner s;

	if( !s.Load( lpszFileName ) )
		return FALSE;
	
	u_long id;
	D3DXVECTOR3 vPos;

	id	= s.GetNumber();
	while( s.tok != FINISHED )
	{
		vPos.x	= s.GetFloat();
		vPos.y	= s.GetFloat();
		vPos.z	= s.GetFloat();

		if( id > MAX_SCHOOL || id < 1 )
			m_vDefault	= vPos;
		else
			m_avPos[id-1]	= vPos;
		id	= s.GetNumber();
	}
	return TRUE;
}

D3DXVECTOR3 CEveSchool::GetPos( u_long id )
{
	if( id > MAX_SCHOOL || id < 1 )
//		return D3DXVECTOR3( 0, 0, 0 );
		return m_vDefault;
	return m_avPos[id-1];
}

CEveSchool* CEveSchool::GetInstance()
{
	static	CEveSchool	sEveSchool;
	return & sEveSchool;
}
#endif	// __SCHOOL0701
#endif // __WORLDSERVER

#ifdef __GUILDCOMBAT
CGuildCombat::CGuildCombat()
{
	Clear();
#ifdef __WORLDSERVER
	memset( __AutoOpen, 0, sizeof(__AUTO_OPEN) * 7 );	
#endif // __WORLDSERVER
}

CGuildCombat::~CGuildCombat()
{
	Clear();
}

void CGuildCombat::Clear( BOOL bAll )
{
	m_nState = CLOSE_STATE;
	m_nGuildCombatState = WAR_CLOSE_STATE;

	if( bAll )
	{
		m_nGuildCombatIndex = 0;
		m_nWinGuildId = 0;
		m_nWinGuildCount = 0;
		m_GuildCombatRanking.clear();
	}	
#ifdef __WORLDSERVER
	for( map<u_long, __GuildCombatMember*>::iterator i = m_GuildCombatMem.begin(); i != m_GuildCombatMem.end(); ++i )
		delete i->second;
	m_GuildCombatMem.clear();
	m_dwTime = 0;
	m_nStopWar = 0;
	if( g_eLocal.GetState( EVE_GUILDCOMBAT ) )
		g_DPCoreClient.SendGuildCombatState( CLOSE_STATE );		
#endif // __WORLDSERVER
}

void CGuildCombat::GameClear()
{
	Clear( FALSE );
}

#ifdef __WORLDSERVER
void CGuildCombat::JoinGuildCombat( u_long idGuild )
{
	__GuildCombatMember* pGuildCombatMember	= new __GuildCombatMember;
	m_GuildCombatMem.insert( map<u_long, __GuildCombatMember*>::value_type( idGuild, pGuildCombatMember ) );
}

void CGuildCombat::OutGuildCombat( u_long idGuild )
{
	m_GuildCombatMem.erase( idGuild );
}

// 단일 유저의 의 길드 전 참가 함수인가?
void CGuildCombat::JoinWar( CUser* pUser, BOOL bWar )
{
	((CMover*)pUser)->Replace( g_uIdofMulti, WI_WORLD_GUILDWAR, D3DXVECTOR3( 1288.0f, 0.0f, 1283.3f ) );
	if( bWar )
	{
//		조금 있다가 정비시간에 들어가겠습니다
		map<u_long, __GuildCombatMember*>::iterator it = m_GuildCombatMem.find( pUser->m_idGuild );
		if( it != m_GuildCombatMem.end() )
		{
			__GuildCombatMember* pGuildCombatMem	= it->second;
			set<u_long>::iterator itJoin = pGuildCombatMem->GuildCombatJoinMember.find( pUser->m_idPlayer );
			if( itJoin == pGuildCombatMem->GuildCombatJoinMember.end() )
			{
				pGuildCombatMem->GuildCombatJoinMember.insert( pUser->m_idPlayer );
				g_UserMng.AddGuildCombatWarListIng( FALSE, pUser->m_idGuild, NULL );
			}
			else
			{
				WriteLog( "Error: JoinWar:: 참가시킬려구 하는데 왜? 있지?" );
			}
		}
		else
		{
			WriteLog( "Error: JoinWar:: m_GuildCombatMem에 없다니.." );
		}		
	}
	else
	{
		pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_WELCOME) );
		pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_MASTER_MAP) );
	}
}

void CGuildCombat::OutWar( CUser* pUser, BOOL bApplication )
{
	map<u_long, __GuildCombatMember*>::iterator it = m_GuildCombatMem.find( pUser->m_idGuild );
	if( it != m_GuildCombatMem.end() )
	{
		__GuildCombatMember* pGuildCombatMem	= it->second;
		pGuildCombatMem->GuildCombatJoinMember.erase( pUser->m_idPlayer );
		if( bApplication )
		{
			((CMover*)pUser)->m_nGuildCombatState = 0;
			g_UserMng.AddGuildCombatUserState( ((CMover*)pUser) );

			pGuildCombatMem->GuildCombatOutMember.insert( pUser->m_idPlayer );
			UserOutGuildCombatResult( pUser );
		}
		g_UserMng.AddGuildCombatWarListIng( FALSE, pUser->m_idGuild, NULL );
	}
}

void CGuildCombat::JoinObserver( CUser* pUser )
{
	((CMover*)pUser)->Replace( g_uIdofMulti, WI_WORLD_GUILDWAR, D3DXVECTOR3( 1361.6f, 0.0f, 1273.3f ) );
	// 길드대전에 오신걸 환영합니다.
	pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_WELCOME) );
	// 관전자 모드로 입장하셨습니다.
	pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_JOIN_OBSERVER) );
	pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_ZOOM_USE) );
}

void CGuildCombat::GuildCombatRequest( CUser* pUser )
{
	CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
	if( pGuild && pGuild->IsMaster( pUser->m_idPlayer ) )
	{
		// 시간 검사 ( 월 ~ 금 )
		map<u_long, __GuildCombatMember*>::iterator it	= m_GuildCombatMem.find( pUser->m_idGuild );
		if( it == m_GuildCombatMem.end() )
		{
			if( m_nState != CGuildCombat::CLOSE_STATE )
			{
				//길드대전중에는 신청할 수 없습니다.
				pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_INV) );	
				return;
			}

			int nCost = 0;
		#if __CURRENT_LANG == LANG_KOR
			nCost = 80000;
		#elif __CURRENT_LANG == LANG_JAP
			nCost = 80000;
		#elif __CURRENT_LANG == LANG_THA
			nCost = 80000;
		#elif __CURRENT_LANG == LANG_CHI
			nCost = 80000;
		#elif __CURRENT_LANG == LANG_TWN
			nCost = 80000;
		#elif __CURRENT_LANG == LANG_ENG
			nCost = 80000;
		#endif	
			
			if( pUser->GetGold() < nCost )
			{
				pUser->AddText( prj.GetText(TID_GAME_LACKMONEY) );	
				return;
			}

			pUser->AddGold( -nCost );

			g_dpDBClient.SendInGuildCombat( pUser->m_idGuild );
		}
		else
		{
			//이미 신청을 하셨습니다.
			pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_EXISTS_APP) );
		}
	}
	else
	{
		//길드가 없거나 길드장이 아닙니다.
		pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_GUILD_LEADER) );
	}
}

void CGuildCombat::GuildCombatCancel( CUser* pUser )
{
	CGuild* pGuild	= g_GuildMng.GetGuild( pUser->m_idGuild );
	if( pGuild && pGuild->IsMaster( pUser->m_idPlayer ) )
	{
		// 시간 검사 ( 월 ~ 금 )
		map<u_long, __GuildCombatMember*>::iterator it	= m_GuildCombatMem.find( pUser->m_idGuild );
		if( it != m_GuildCombatMem.end() )
		{
			if( m_nState != CGuildCombat::CLOSE_STATE )
			{
				pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_USE));	// 지금은 길드대전중에는 길드대전 탈퇴를 할수 없습니다
				return;
			}

			int nCost = 0;
		#if __CURRENT_LANG == LANG_KOR
			nCost = 80000;
		#elif __CURRENT_LANG == LANG_JAP
			nCost = 80000;
		#elif __CURRENT_LANG == LANG_THA
			nCost = 80000;
		#elif __CURRENT_LANG == LANG_CHI
			nCost = 80000;
		#elif __CURRENT_LANG == LANG_TWN
			nCost = 80000;
		#elif __CURRENT_LANG == LANG_ENG
			nCost = 80000;
		#endif	

			// 20% 수수료
			nCost = nCost * 0.8f;
			pUser->AddGold( nCost );
			
			g_dpDBClient.SendOutGuildCombat( pUser->m_idGuild );
		}
		else
		{
			//길드대전 신청을 하지 않았습니다.
			pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_NOTAPP) );
		}
	}
	else
	{
		//길드가 없거나 길드장이 아닙니다.
		pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_GUILD_LEADER) );
	}
}

void CGuildCombat::UserOutGuildCombatResult( CUser* pUser )
{
	u_long idGuildWin;
	int nCount = 0;
	map<u_long, __GuildCombatMember*>::iterator it = m_GuildCombatMem.find( pUser->m_idGuild );
	if( it != m_GuildCombatMem.end() )
	{
		__GuildCombatMember *pGuildCombatMember	= it->second;
		if( pGuildCombatMember->GuildCombatJoinMember.size() == 0 )
		{
			for( map<u_long, __GuildCombatMember*>::iterator it2 = m_GuildCombatMem.begin() ;
				it2 != m_GuildCombatMem.end() ; ++it2 )
			{
				__GuildCombatMember *pGuildCombatMember2 = it2->second;
				if( 0 < pGuildCombatMember2->GuildCombatJoinMember.size() )
				{
					++nCount;
					idGuildWin = it2->first;
					if( nCount == 2 )
					{
						return; // 2개 이상이면 검사할 필요 없음..
					}
				}
			}

			if( nCount == 1 )	// 길드가 하나밖에 없으므로 게임 종료
			{
				m_nStopWar = 1;
			}
		}
	}
}

void CGuildCombat::GuildCombatResult( BOOL nResult, u_long idGuildWin )
{
	set<u_long> SameidGuildWin; 
	SameidGuildWin.clear();

	if( nResult == FALSE )
	{
		// 검사를 해야 함( 모든 길드를 돌면서 길드원이 많이 남은수 가 Win )
		int nCount = 0;
		for( map<u_long, __GuildCombatMember*>::iterator it2 = m_GuildCombatMem.begin() ;
		it2 != m_GuildCombatMem.end() ; ++it2 )
		{
			__GuildCombatMember* pGuildCombatMember2 = it2->second;
			if( 0 < pGuildCombatMember2->GuildCombatJoinMember.size() )
			{
				/*
				BOOL bRefresh = FALSE;
				if( nCount < pGuildCombatMember2->GuildCombatJoinMember.size() )
				{
					idGuildWin = it2->first;
					nCount	= pGuildCombatMember2->GuildCombatJoinMember.size();
					bRefresh = TRUE;
					SameidGuildWin.clear();
				}

				if( bRefresh == FALSE && pGuildCombatMember2->GuildCombatJoinMember.size() == nCount )
				{
					SameidGuildWin.insert( it2->first ); 
				}
				*/
				if( nCount < pGuildCombatMember2->GuildCombatJoinMember.size() )
				{
					idGuildWin = it2->first;
					nCount	= pGuildCombatMember2->GuildCombatJoinMember.size();
					SameidGuildWin.clear();
					SameidGuildWin.insert( it2->first );
				}
				else if( nCount == pGuildCombatMember2->GuildCombatJoinMember.size() )
				{
					SameidGuildWin.insert( it2->first ); 
				}
			}
		}
	}
	if( 0 < SameidGuildWin.size() )
	{
		// 공동 우승
		// idGuildWin의 길드와 공동우승
		// SameidGuildWin
		// 참가한 길드원의 평균레벨이 낮은곳이 우승
//		SameidGuildWin.insert( idGuildWin ); 
		FLOAT fAvgLevel = 9999.9f;
		for( set<u_long>::iterator GuildWinIt = SameidGuildWin.begin() ;
			GuildWinIt != SameidGuildWin.end() ; ++GuildWinIt )
		{
			map<u_long, __GuildCombatMember*>::iterator it3 = m_GuildCombatMem.find( *GuildWinIt );
			__GuildCombatMember* pGuildCombatMember3 = it3->second;
			if( fAvgLevel > pGuildCombatMember3->fAvgLevel )
			{
				fAvgLevel = pGuildCombatMember3->fAvgLevel;
				idGuildWin = it3->first;
			}
		}
	}

	// idGuildWin
	CGuild* pGuild	= g_GuildMng.GetGuild( idGuildWin );
	if( pGuild )
	{
		g_UserMng.AddWorldCreateSfxObj( XI_NAT_ROCKET02, 1291.0f, 85.0f, 1279.0f, FALSE, WI_WORLD_GUILDWAR );
		g_DPCoreClient.SendPlayMusic( WI_WORLD_GUILDWAR, BGM_IN_FITUP );
		
		// pGuild가 이긴... 길드이다.
		if( m_nWinGuildId == idGuildWin )
			++m_nWinGuildCount;
		else
			m_nWinGuildCount = 1;
		m_nWinGuildId = idGuildWin;
		// 길드대전이 종료되었습니다.
		g_DPCoreClient.SendSystem( prj.GetText(TID_GAME_GUILDCOMBAT_END) );
		char str[512] = {0,};
		// 우승한 길드는 %s 입니다. \n축하 드립니다~		
		sprintf( str, prj.GetText(TID_GAME_GUILDCOMBAT_WINNER), pGuild->m_szGuild );
		g_DPCoreClient.SendSystem( str );
		g_DPCoreClient.SendCaption( str, WI_WORLD_GUILDWAR );

		sprintf( str, prj.GetText(TID_GAME_GUILDCOMBAT_CELE) );
//		g_DPCoreClient.SendSystem( str );
		g_DPCoreClient.SendCaption( str, WI_WORLD_GUILDWAR );
		
	}
	++m_nGuildCombatIndex;
	g_UserMng.AddGuildCombatWinGuild();

//	GuildCombatCloseTeleport();
}

void CGuildCombat::GuildCombatCloseTeleport()
{
	if( m_nStopWar != 200 )
		g_dpDBClient.SendGuildCombatResult();
	
	PRegionElem pRgnElem = g_WorldMng.GetRevivalPos( WI_WORLD_MADRIGAL, "flaris" );
	if( NULL == pRgnElem )
	{
		return;
	}

	g_UserMng.ReplaceWorld( WI_WORLD_GUILDWAR, WI_WORLD_MADRIGAL, 6968.0f, 3328.8f );

	m_nState = CLOSE_STATE;
	m_nGuildCombatState = WAR_CLOSE_STATE;
	m_dwTime = 0;
	m_nStopWar = 0;
	if( g_eLocal.GetState( EVE_GUILDCOMBAT ) )
		g_DPCoreClient.SendGuildCombatState( CLOSE_STATE );	
}

void CGuildCombat::GuildCombatOpen( void )
{
	// 길드전쟁 서버에 있는 모든 캐릭터들을 모두 나가게함..
	if( m_nState != CLOSE_STATE )
		return;
	
	if( g_eLocal.GetState( EVE_GUILDCOMBAT ) )
		g_DPCoreClient.SendGuildCombatState( OPEN_STATE );	
	m_nGuildCombatState = NOTENTER_STATE;
	m_nProcessGo	= 0;
}

void CGuildCombat::GuildCombatEnter( CUser* pUser )
{
	BOOL bJoin = TRUE;
	if( m_nState == OPEN_STATE )
	{
		if( m_nGuildCombatState == ENTER_STATE )
		{
			map<u_long, __GuildCombatMember*>::iterator it	= m_GuildCombatMem.find( pUser->m_idGuild );
			if( it != m_GuildCombatMem.end() )
			{	// 전쟁 참여가능한캐릭터			
				JoinWar( pUser, TRUE );
			}
			else
			{	// 관전자			
				JoinObserver( pUser );
			}	
		}
		else
		{
			pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_JOIN) );
		}
	}
	else if( m_nState == CLOSE_STATE )
	{
//		if( m_nWinGuildId == pUser->m_idGuild )
//		{
//			JoinWar( pUser, FALSE );
//		}
//		else
		{
			JoinObserver( pUser );
		}
		// 참여할수 있는 길드는 전에 승리한 길드만 들어갈수 있음
		// 나머지는 관전자로 들어가게 함		
	}
	else if( m_nState == WAR_STATE )
	{
		// 전쟁중이므로 모두 관전자로 들어가게 함
		JoinObserver( pUser );
	}
	else if( m_nState == COMPLET_CLOSE_STATE )
	{
		// 아무도 못들어옴.. 지금은 닫혀있는 상태
		pUser->AddText( prj.GetText(TID_GAME_GUILDCOMBAT_NOT_OPEN) );
		bJoin = FALSE;
	}
	else
	{
		bJoin = FALSE;
	}

	if( bJoin )
	{
		if( 0 < m_dwTime )
		{		
			pUser->AddGuildCombatNextTime( m_dwTime - GetTickCount(), GuildCombatProcess[m_nProcessGo - 1].dwParam );
			if( m_nState != CLOSE_STATE )
				g_UserMng.AddGuildCombatWarListIng( TRUE, 0, pUser );
		}
	}
}

void CGuildCombat::SetMaintenance()
{
	int nCount = 0;
	for( map<u_long, __GuildCombatMember*>::iterator ita = m_GuildCombatMem.begin() ; ita != m_GuildCombatMem.end() ; ++ita )
	{
		__GuildCombatMember* pGuildCombatMem = ita->second;
		if( 0 < pGuildCombatMem->GuildCombatJoinMember.size() )
		{
			pGuildCombatMem->fAvgLevel = 0.0f;
			for( set<u_long>::iterator itb = pGuildCombatMem->GuildCombatJoinMember.begin() ; itb != pGuildCombatMem->GuildCombatJoinMember.end() ; ++itb )
			{
				CMover* pMover = prj.GetUserByID( *itb );
				if( IsValidObj( pMover ) )
				{
					pGuildCombatMem->fAvgLevel += pMover->GetLevel();
				}
			}
			pGuildCombatMem->fAvgLevel = (float)(pGuildCombatMem->fAvgLevel / (float)pGuildCombatMem->GuildCombatJoinMember.size());
			++nCount;
		}
	}

	if( nCount < 2 )
		SetGuildCombatCloseWait( TRUE );
}

void CGuildCombat::SetGuildCombat()
{
	for( map<u_long, __GuildCombatMember*>::iterator ita = m_GuildCombatMem.begin() ; ita != m_GuildCombatMem.end() ; ++ita )
	{
		__GuildCombatMember* pGuildCombatMem = ita->second;
		for( set<u_long>::iterator itb = pGuildCombatMem->GuildCombatJoinMember.begin() ; itb != pGuildCombatMem->GuildCombatJoinMember.end() ; ++itb )
		{
			CMover* pMover = prj.GetUserByID( *itb );
			if( IsValidObj( pMover ) )
			{
				pMover->m_nGuildCombatState = 1;
				g_UserMng.AddGuildCombatUserState( pMover );
			}
		}
	}
}

void CGuildCombat::SetGuildCombatClose( BOOL bGM )
{
	if( m_nState == CLOSE_STATE )
		return;

	if( bGM )
	{
		m_nStopWar = 1;	
		return;
	}
	for( map<u_long, __GuildCombatMember*>::iterator it = m_GuildCombatMem.begin(); it != m_GuildCombatMem.end(); ++it )
	{
		CGuild* pGuild	= g_GuildMng.GetGuild( it->first );
		if( pGuild )
		{
			map<u_long, CGuildMember*>::iterator iter = pGuild->m_mapPMember.begin();
			for( ; iter != pGuild->m_mapPMember.end() ; ++iter )
			{
				CGuildMember * pGuildMember = (CGuildMember*)iter->second;
				CMover* pMover = prj.GetUserByID( pGuildMember->m_idPlayer );
				if( IsValidObj( pMover ) )
				{
					pMover->m_nGuildCombatState = 0;
					g_UserMng.AddGuildCombatUserState( pMover );
				}
			}
		}
	}
//	GuildCombatResult();
}

void CGuildCombat::SetGuildCombatCloseWait( BOOL bGM )
{
	if( m_nState == CLOSE_STATE )
		return;

	if( bGM )
	{
		m_nStopWar = 2;
		return;
	}
	else
	{
		GuildCombatResult();
	}	
}

void CGuildCombat::Process()
{

#ifdef _DEBUG	
	TRACE( "GuildWarFlyff::Process <%d>, <%d>\n", m_nState, m_nGuildCombatState);
#endif //_DEBUG
	if( m_nState != CLOSE_STATE )
	{
		if( m_nProcessGo < m_nProcessCount[0] )
		{ 
			ProcessCommand();
		}
	}
#ifdef __WORLDSERVER
	else
	{
		CTime ctime = CTime::GetCurrentTime();

		if( __AutoOpen[ctime.GetDayOfWeek()-1].bUseing )
		{
			if( __AutoOpen[ctime.GetDayOfWeek()-1].nHour == ctime.GetHour() &&
				__AutoOpen[ctime.GetDayOfWeek()-1].nMinute == ctime.GetMinute() && ctime.GetSecond() == 0 )
			{
				if( m_GuildCombatMem.size() > 1 )
				{
					GuildCombatOpen();
				}
				else
				{
					g_DPCoreClient.SendSystem( prj.GetText( TID_GAME_GUILDCOMBAT_NEXT_COMBAT ) );	
					g_DPCoreClient.SendSystem( prj.GetText( TID_GAME_GUILDCOMBAT_ENJOY ) );	
				}

			}
		}
	}
#endif //__WORLDSERVER
}

void CGuildCombat::ProcessCommand()
{
	if( m_nStopWar == 1 )
	{
		m_nProcessGo = m_nProcessCount[COMPLET_CLOSE_STATE];
		m_dwTime = 0;
		m_nStopWar = 0;
	}
	else if( m_nStopWar == 2 )
	{
		m_nProcessGo = m_nProcessCount[GM_COLSE_STATE];
		m_dwTime = 0;
		m_nStopWar = 200;
	}
	if( m_dwTime <= GetTickCount() )
	{
		if( GuildCombatProcess[m_nProcessGo].nState == WARSTATE )
		{
			if( GuildCombatProcess[m_nProcessGo].dwCommand == NOTENTER_COUNT_STATE )
				g_UserMng.ReplaceWorld( WI_WORLD_GUILDWAR, WI_WORLD_MADRIGAL, 6968.0f, 3328.8f );
			else if( GuildCombatProcess[m_nProcessGo].dwCommand == MAINTENANCE_STATE )
			{
				// 전쟁 준비 시작
				prj.m_AddRemoveLock.Enter( theLineFile );
				g_GuildMng.m_AddRemoveLock.Enter( theLineFile );
				SetMaintenance();
				g_GuildMng.m_AddRemoveLock.Leave( theLineFile );
				prj.m_AddRemoveLock.Leave( theLineFile );
				
			}
			else if( GuildCombatProcess[m_nProcessGo].dwCommand == WAR_WAR_STATE )
			{
				// 전쟁 시작 
				prj.m_AddRemoveLock.Enter( theLineFile );
				g_GuildMng.m_AddRemoveLock.Enter( theLineFile );
				SetGuildCombat();
				g_GuildMng.m_AddRemoveLock.Leave( theLineFile );
				prj.m_AddRemoveLock.Leave( theLineFile );
			}
			else if( GuildCombatProcess[m_nProcessGo].dwCommand == WAR_CLOSE_STATE )
			{
				// 전쟁 종료
				prj.m_AddRemoveLock.Enter( theLineFile );
				g_GuildMng.m_AddRemoveLock.Enter( theLineFile );
				SetGuildCombatClose();
				g_GuildMng.m_AddRemoveLock.Leave( theLineFile );
				prj.m_AddRemoveLock.Leave( theLineFile );
			}
			else if( GuildCombatProcess[m_nProcessGo].dwCommand == WAR_CLOSE_WAIT_STATE )
			{
				// 전쟁 종료후 결과값 나오기 대기
				prj.m_AddRemoveLock.Enter( theLineFile );
				g_GuildMng.m_AddRemoveLock.Enter( theLineFile );
				SetGuildCombatCloseWait();
				g_GuildMng.m_AddRemoveLock.Leave( theLineFile );
				prj.m_AddRemoveLock.Leave( theLineFile );
			}
			else if( GuildCombatProcess[m_nProcessGo].dwCommand == WAR_TELEPORT_STATE )
			{
				// 전쟁 종료후 텔레포트
				prj.m_AddRemoveLock.Enter( theLineFile );
				g_GuildMng.m_AddRemoveLock.Enter( theLineFile );
				GuildCombatCloseTeleport();
				g_GuildMng.m_AddRemoveLock.Leave( theLineFile );
				prj.m_AddRemoveLock.Leave( theLineFile );
			}
		}

		switch( GuildCombatProcess[m_nProcessGo].nState )
		{
		case ALLMSG:
			{
				CString str;
				str.Format( prj.GetText( GuildCombatProcess[m_nProcessGo].dwCommand ), GuildCombatProcess[m_nProcessGo].dwTime / 1000 );
				g_DPCoreClient.SendSystem( str );

				if( GuildCombatProcess[m_nProcessGo].dwCommand == TID_GAME_GUILDCOMBAT_OPEN_MSG )
					SendGuildCombatEnterTime( );
			}
			break;
		case GUILDMSG:
			{
			}
			break;
		case JOINMSG:
			{
				CString str;
				str.Format( prj.GetText( GuildCombatProcess[m_nProcessGo].dwCommand ), GuildCombatProcess[m_nProcessGo].dwTime / 1000 );
				SendJoinMsg( str );
			}
			break;
		case WORLDMSG:
			{
				CString str;
				str.Format( prj.GetText( GuildCombatProcess[m_nProcessGo].dwCommand ), GuildCombatProcess[m_nProcessGo].dwTime / 1000 );
				g_UserMng.AddWorldMsg( WI_WORLD_GUILDWAR, str );
			}
			break;
		case STATE:
			{
				g_DPCoreClient.SendGuildCombatState( GuildCombatProcess[m_nProcessGo].dwCommand );
//				m_nState = GuildCombatProcess[m_nProcessGo].dwCommand;
			}
			break;
		case WARSTATE:
			{
				m_nGuildCombatState = GuildCombatProcess[m_nProcessGo].dwCommand;
				g_UserMng.AddGuildCombatState();
			}
			break;
		}
		m_dwTime = GetTickCount() + GuildCombatProcess[m_nProcessGo].dwTime;
		if( GuildCombatProcess[m_nProcessGo].dwParam != 0 )
			g_UserMng.AddGuildCombatNextTimeWorld( GuildCombatProcess[m_nProcessGo].dwTime, GuildCombatProcess[m_nProcessGo].dwParam );
		++m_nProcessGo;
	}
}


BOOL CGuildCombat::LoadScript( LPCSTR lpszFileName )
{
	CScanner s;
	
	if( !s.Load( lpszFileName ) )
		return FALSE;

	ZeroMemory( m_nProcessCount, sizeof( int ) * 25 );
	ZeroMemory( GuildCombatProcess, sizeof( __GuildCombatProcess ) * 250 );
	DWORD	dwTime;
	int		nCount;
	int		nAutoOpenCnt = 0;

	while( s.tok != FINISHED )
	{
		nCount = -1;

#if defined(__MAINSERVER)
		if( s.Token == _T( "AUTO_OPEN_IDC" ) )
		{
			s.GetToken();	// {
			s.GetToken();	// {
			
			int nDay, nTime1, nTime2;
			while( *s.token != '}' )
			{
				if( nAutoOpenCnt >= 7 )
				{
					Error( "CGuildCombat::LoadScript() 자동 시작 설정 갯수가 넘음!!" );
					return FALSE;
				}

				nDay = atoi( s.Token );
				if( nDay <= 0 || nDay > 7 )
				{
					Error( "CGuildCombat::LoadScript() Day Error = %d", nDay );
					return FALSE;
				}
				__AutoOpen[nDay-1].bUseing = TRUE;

				nTime1 = s.GetNumber();
				if( nTime1 < 0 || nTime1 > 24 )
				{
					Error( "CGuildCombat::LoadScript() Time Error = %d", nTime1 );
					return FALSE;
				}
				__AutoOpen[nDay-1].nHour = nTime1;
				
				nTime2 = s.GetNumber();
				if( nTime2 < 0 || nTime2 > 59 )
				{
					Error( "CGuildCombat::LoadScript() Time Error = %d", nTime2 );
					return FALSE;
				}
				__AutoOpen[nDay-1].nMinute = nTime2;

				nAutoOpenCnt++;

				s.GetToken();
			}		
		}
#else
		if( s.Token == _T( "AUTO_OPEN_TEST" ) )
		{
			s.GetToken();	// {
			s.GetToken();	// {
			
			int nDay, nTime1, nTime2;
			while( *s.token != '}' )
			{
				if( nAutoOpenCnt >= 7 )
				{
					Error( "CGuildCombat::LoadScript() 자동 시작 설정 갯수가 넘음!!" );
					return FALSE;
				}
				
				nDay = atoi( s.Token );
				if( nDay <= 0 || nDay > 7 )
				{
					Error( "CGuildCombat::LoadScript() Day Error = %d", nDay );
					return FALSE;
				}
				__AutoOpen[nDay-1].bUseing = TRUE;
				
				nTime1 = s.GetNumber();
				if( nTime1 < 0 || nTime1 > 24 )
				{
					Error( "CGuildCombat::LoadScript() Time Error = %d", nTime1 );
					return FALSE;
				}
				__AutoOpen[nDay-1].nHour = nTime1;
				
				nTime2 = s.GetNumber();
				if( nTime2 < 0 || nTime2 > 59 )
				{
					Error( "CGuildCombat::LoadScript() Time Error = %d", nTime2 );
					return FALSE;
				}
				__AutoOpen[nDay-1].nMinute = nTime2;
				
				nAutoOpenCnt++;
				
				s.GetToken();
			}		
		}
#endif
		else if( s.Token == _T( "OPEN" ) )
		{
			nCount = 0;
		}
		else if( s.Token == _T( "MAINTENANCE" ) )
		{
			nCount = 1;
		}
		else if( s.Token == _T( "WAR" ) )
		{
			nCount = 2;
		}

		if( nCount != -1 )
		{
			m_nProcessCount[nCount] = s.GetNumber();
			s.GetToken();	// {
			s.GetToken();	// {
			int nSoCount = 0;
			while( *s.token != '}' )
			{
				if( s.Token == _T( "ALLMSG" ) )
				{
					GuildCombatProcess[25*nCount+nSoCount].nState	= ALLMSG;
				}
				else if( s.Token == _T( "GUILDMSG" ) )
				{
					GuildCombatProcess[25*nCount+nSoCount].nState	= GUILDMSG;
				}
				else if( s.Token == _T( "JOINMSG" ) )
				{
					GuildCombatProcess[25*nCount+nSoCount].nState	= JOINMSG;
				}
				else if( s.Token == _T( "WORLDMSG" ) )
				{
					GuildCombatProcess[25*nCount+nSoCount].nState	= WORLDMSG;
				}
				else if( s.Token == _T( "STATE" ) )
				{
					GuildCombatProcess[25*nCount+nSoCount].nState	= STATE;
				}
				else if( s.Token == _T( "WARSTATE" ) )
				{
					GuildCombatProcess[25*nCount+nSoCount].nState	= WARSTATE;
				}
				else if( s.Token == _T( "WAIT" ) )
				{
					GuildCombatProcess[25*nCount+nSoCount].nState	= WAIT;
				}
				GuildCombatProcess[25*nCount+nSoCount].dwTime	= s.GetNumber();
				GuildCombatProcess[25*nCount+nSoCount].dwCommand	= s.GetNumber();
				GuildCombatProcess[25*nCount+nSoCount].dwParam		= s.GetNumber();
				if( GuildCombatProcess[25*nCount+nSoCount].nState == STATE )
					m_nProcessCount[GuildCombatProcess[25*nCount+nSoCount].dwCommand] = 25*nCount+nSoCount;
				
				++nSoCount;
				s.GetToken();
			}
		}

		s.GetToken();
	}	
	return TRUE;
}

void CGuildCombat::SendJoinMsg( LPCTSTR lpszString  )
{
	prj.m_AddRemoveLock.Enter( theLineFile );
	g_GuildMng.m_AddRemoveLock.Enter( theLineFile );
	for( map<u_long, __GuildCombatMember*>::iterator ita = m_GuildCombatMem.begin() ; ita != m_GuildCombatMem.end() ; ++ita )
	{
		__GuildCombatMember* pGuildCombatMem = ita->second;
		for( set<u_long>::iterator itb = pGuildCombatMem->GuildCombatJoinMember.begin() ; itb != pGuildCombatMem->GuildCombatJoinMember.end() ; ++itb )
		{
			CMover* pMover = prj.GetUserByID( *itb );
			if( IsValidObj( pMover ) )
			{
				((CUser*)pMover)->AddText( lpszString );
			}
		}
	}
	g_GuildMng.m_AddRemoveLock.Leave( theLineFile );
	prj.m_AddRemoveLock.Leave( theLineFile );
}

void CGuildCombat::SendGuildCombatEnterTime( void )
{
	prj.m_AddRemoveLock.Enter( theLineFile );
	g_GuildMng.m_AddRemoveLock.Enter( theLineFile );
	
	for( map<u_long, __GuildCombatMember*>::iterator ita = m_GuildCombatMem.begin() ; ita != m_GuildCombatMem.end() ; ++ita )
	{
		CGuild* pGuild = g_GuildMng.GetGuild( ita->first );
		if( pGuild )
		{
			CGuildMember*	pMember;
			CUser*			pUsertmp;
			for( map<u_long, CGuildMember*>::iterator i = pGuild->m_mapPMember.begin();
			i != pGuild->m_mapPMember.end(); ++i )
			{
				pMember		= i->second;
				pUsertmp	= (CUser*)prj.GetUserByID( pMember->m_idPlayer );
				if( IsValidObj( pUsertmp ) )
				{
					pUsertmp->AddGuildCombatEnterTime( GuildCombatProcess[m_nProcessGo].dwTime );
				}
			}
		}
	}
	g_GuildMng.m_AddRemoveLock.Leave( theLineFile );
	prj.m_AddRemoveLock.Leave( theLineFile );
}

#endif	// __WORLDSERVER

CGuildCombat	g_GuildCombatMng;

#endif // __GUILDCOMBAT
