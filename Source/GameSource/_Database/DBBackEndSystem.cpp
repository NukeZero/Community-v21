#include "StdAfx.h"
#include "dbmanager.h"
#include "../DatabaseServer/dploginsrvr.h"
#include "../DatabaseServer/dpcoresrvr.h"
#include "../DatabaseServer/dptrans.h"
#include "..\_Network\Objects\Obj.h"
#include "misc.h"
#include <logging/FLMyTrace.h>

extern  CDPCoreSrvr			g_dpCoreSrvr;
extern	CDPLoginSrvr		g_dpLoginSrvr;
extern	CProject			prj;
extern	APP_INFO			g_appInfo;
extern	FLMyTrace			g_MyTrace;

const int MAX_BACKENDSYSTEM = 2;

#ifdef __S1108_BACK_END_SYSTEM
void CDbManager::BackSystem( void )
{
	if( prj.m_bBackEndSystem == FALSE )
		return;

	CQuery* pQuery	= new CQuery;
#ifdef __DbStuff
	if (!pQuery->Connect(CQuery::SqlDatabase::BACKEND))
#else
	if( FALSE == pQuery->Connect( 3, DSN_NAME_BACKSYSTEM, DB_ADMIN_ID_BACKSYSTEM, DB_ADMIN_PASS_BACKSYSTEM ) )
#endif
	{
		char s[128];
		FLSPrintf( s, _countof( s ), "%s, %d BackSystem Connect Failed", __FILE__, __LINE__ );
		AfxMessageBox( s );
		SAFE_DELETE( pQuery );
		return;
	}

	prj.m_nAddMonsterRespawnSize = 0;
	prj.m_nAddMonsterPropSize = 0;
	prj.m_nRemoveMonsterPropSize = 0;
	int nRemoveMonsterRespawnSize = 0;
//	int nRemoveMonsterPropSize = 0;

	GetGemeSettingtime( pQuery, 2 );
	GetBaseGameSetting( pQuery );
	GetMonsterRespawnSetting( pQuery, nRemoveMonsterRespawnSize );
	GetMonsterPropSetting( pQuery );

	BOOL bComplete = TRUE;
	HANDLE hHandle	= m_hCloseWorker;

	
	
	int nRemoveRespawn[ 1024 ];
	
	char chBackEndSystemTime[15];
	char chBackEndSystemChatTime[15];
	int	nChatCount;
	int nCount = 0;
	while( WaitForSingleObject( hHandle, 5000 ) == WAIT_TIMEOUT && bComplete == TRUE )	// 10초	= 1000 * 10 //1000 * 60 * 5
	{
		CTime timeCurr	= CTime::GetCurrentTime();
		bComplete = FALSE;

		FLStrcpy( chBackEndSystemChatTime, _countof( chBackEndSystemChatTime ), prj.m_chBackEndSystemChatTime );
		GetGemeSettingtime( pQuery, 0 );
		nChatCount = 0;
		if( GetGMChat( pQuery, chBackEndSystemChatTime, nChatCount ) )
		{
			CDPTrans::GetInstance()->SendGMChat( nChatCount );
		}
		++nCount;

		if( nCount == MAX_BACKENDSYSTEM )
		{
			nCount = 0;
			FLStrcpy( chBackEndSystemTime, _countof( chBackEndSystemTime ), prj.m_chBackEndSystemTime );
			
			GetGemeSettingtime( pQuery, 1 );
			if( GetBaseGameSetting( pQuery, chBackEndSystemTime ) )
			{
				CDPTrans::GetInstance()->SendBaseGameSetting( FALSE, DPID_ALLPLAYERS );
			}
			
			prj.m_nAddMonsterRespawnSize = 0;
			prj.m_nAddMonsterPropSize = 0;
			prj.m_nRemoveMonsterPropSize = 0;
			nRemoveMonsterRespawnSize = 0;
			
			
			GetMonsterRespawnSetting( pQuery, nRemoveMonsterRespawnSize, nRemoveRespawn, chBackEndSystemTime );
			if( 0 < prj.m_nAddMonsterRespawnSize || 0 < nRemoveMonsterRespawnSize )
			{
				CDPTrans::GetInstance()->SendMonsterRespawnSetting( nRemoveMonsterRespawnSize, nRemoveRespawn, DPID_ALLPLAYERS );
			}
			
			GetMonsterPropSetting( pQuery, chBackEndSystemTime );
			if( 0 < prj.m_nAddMonsterPropSize || 0 < prj.m_nRemoveMonsterPropSize )
			{
				CDPTrans::GetInstance()->SendMonsterPropSetting( FALSE, DPID_ALLPLAYERS );
			}
		}

		bComplete = TRUE;

		FLTRACE_LOG( PROGRAM_NAME, _T( "Worker BackSystem" ) );
	}	
	SAFE_DELETE( pQuery );
}

void CDbManager::GetGemeSettingtime( CQuery* pQuery, int nChat )
{
	char szQuery[1024] = { 0, };
//	BOOL bResult = FALSE;

	FLSPrintf( szQuery, _countof( szQuery ), "GAME_BACKSYSTEM_STR 'S0'" );
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return;
	}	
	
	if( pQuery->Fetch() )
	{
		if( nChat == 0 )
		{
			pQuery->GetStr( "curr_time", prj.m_chBackEndSystemChatTime, _countof( prj.m_chBackEndSystemChatTime ) );
		}
		else
		if( nChat == 1 )
		{
			pQuery->GetStr( "curr_time", prj.m_chBackEndSystemTime, _countof( prj.m_chBackEndSystemTime ) );
		}
		else
		{
			pQuery->GetStr( "curr_time", prj.m_chBackEndSystemTime, _countof( prj.m_chBackEndSystemTime ) );
			FLStrcpy( prj.m_chBackEndSystemChatTime, _countof( prj.m_chBackEndSystemChatTime ), prj.m_chBackEndSystemTime );			
		}
		
	}		
}

BOOL CDbManager::GetGMChat( CQuery* pQuery, char * lpstrTime, int& nChatCount )
{
	char szQuery[1024];
	BOOL bResult = FALSE;
	
	FLSPrintf( szQuery, _countof( szQuery ), "GAME_BACKSYSTEM_STR 'S4','%02d','%s'", g_appInfo.dwSys, lpstrTime );
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return bResult;
	}	
	
	while( pQuery->Fetch() )
	{
		if( nChatCount < 10 )
		{
			pQuery->GetStr( "msg", prj.m_chGMChat[nChatCount], _countof( prj.m_chGMChat[nChatCount] ) );
			++nChatCount;
		}
		else
		{
			break;
		}
		bResult = TRUE;
	}		
	
	return bResult;
}

BOOL CDbManager::GetBaseGameSetting( CQuery* pQuery, char * lpstrTime )
{
	char szQuery[1024];
	
	FLSPrintf( szQuery, _countof( szQuery ), "GAME_BACKSYSTEM_STR 'S1','%02d','%s'", g_appInfo.dwSys, lpstrTime );
	
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return FALSE;
	}	
	BOOL bResult = FALSE;	
	int nBufValue = 0;
	char chBufName[128];
	int nResult;
	while( pQuery->Fetch() )
	{
		nResult = pQuery->GetInt( "bResult" );
		ZeroMemory( chBufName, sizeof( chBufName ) );
		pQuery->GetStr( "szName", chBufName, _countof( chBufName ) );
		if( nResult == 1)	// 추가
		{
			nBufValue = pQuery->GetInt( "rate" );
			if( strcmp( chBufName, "Exp" ) == 0 )
			{
				prj.m_fMonsterExpRate = (float)nBufValue / 100.0f;
				FLTRACE_LOG( PROGRAM_NAME, _T( "Exp GAME_SETTING %f" ), prj.m_fMonsterExpRate );
			}
			else
			if( strcmp( chBufName, "Penya" ) == 0 )
			{
				prj.m_fGoldDropRate = (float)nBufValue / 100.0f;
				FLTRACE_LOG( PROGRAM_NAME, _T( "Penya GAME_SETTING %f" ), prj.m_fGoldDropRate );
			}
			else
			if( strcmp( chBufName, "ItemDrop" ) == 0 )
			{
				prj.m_fItemDropRate = (float)nBufValue / 100.0f;
				FLTRACE_LOG( PROGRAM_NAME, _T( "ItemDrop GAME_SETTING %f" ), prj.m_fItemDropRate );
			}
			else
			if( strcmp( chBufName, "Rebirth" ) == 0 )
			{
				prj.m_fMonsterRebirthRate = (float)nBufValue / 100.0f;
				FLTRACE_LOG( PROGRAM_NAME, _T( "Rebirth GAME_SETTING %f" ), prj.m_fMonsterRebirthRate );
			}
			else
			if( strcmp( chBufName, "HitPoint" ) == 0 )
			{
				prj.m_fMonsterHitpointRate = (float)nBufValue / 100.0f;
				FLTRACE_LOG( PROGRAM_NAME, _T( "HitPointExp GAME_SETTING %f" ), prj.m_fMonsterHitpointRate );
			}
			else
			if( strcmp( chBufName, "AttackPower" ) == 0 )
			{
				prj.m_fMonsterHitRate = (float)nBufValue / 100.0f;
				FLTRACE_LOG( PROGRAM_NAME, _T( "AttackPower GAME_SETTING %f" ), prj.m_fMonsterHitRate );
			}
			else
			if( strcmp( chBufName, "Respawn" ) == 0 )
			{
				prj.m_fMonsterRespawnRate = (float)nBufValue / 100.0f;
				FLTRACE_LOG( PROGRAM_NAME, _T( "Respawn GAME_SETTING %f" ), prj.m_fMonsterRespawnRate );
			}
			else
			if( strcmp( chBufName, "Aggressive" ) == 0 )
			{
				prj.m_fMonsterAggressiveRate = (float)nBufValue / 100.0f;
				FLTRACE_LOG( PROGRAM_NAME, _T( "Aggressive GAME_SETTING %f" ), prj.m_fMonsterAggressiveRate );
			}
			else
			if( strcmp( chBufName, "Price" ) == 0 )
			{
				prj.m_fShopCost = (float)nBufValue / 100.0f;
				FLTRACE_LOG( PROGRAM_NAME, _T( "Price GAME_SETTING %f" ), prj.m_fShopCost );
			}
		}
		else if( lpstrTime != '\0' )
		{
			if( strcmp( chBufName, "Exp" ) == 0 )
			{
				prj.m_fMonsterExpRate = 1.0f;
				FLTRACE_LOG( PROGRAM_NAME, _T( "Exp GAME_SETTING %f" ), prj.m_fMonsterExpRate );
			}
			else
			if( strcmp( chBufName, "Penya" ) == 0 )
			{
				prj.m_fGoldDropRate = 1.0f;
				FLTRACE_LOG( PROGRAM_NAME, _T( "Penya GAME_SETTING %f" ), prj.m_fGoldDropRate );
			}
			else
			if( strcmp( chBufName, "ItemDrop" ) == 0 )
			{
				prj.m_fItemDropRate = 1.0f;
				FLTRACE_LOG( PROGRAM_NAME, _T( "ItemDrop GAME_SETTING %f" ), prj.m_fItemDropRate );
			}
			else
			if( strcmp( chBufName, "Rebirth" ) == 0 )
			{
				prj.m_fMonsterRebirthRate = 1.0f;
				FLTRACE_LOG( PROGRAM_NAME, _T( "Rebirth GAME_SETTING %f" ), prj.m_fMonsterRebirthRate );
			}
			else
			if( strcmp( chBufName, "HitPoint" ) == 0 )
			{
				prj.m_fMonsterHitpointRate = 1.0f;
				FLTRACE_LOG( PROGRAM_NAME, _T( "HitPointExp GAME_SETTING %f" ), prj.m_fMonsterHitpointRate );
			}
			else
			if( strcmp( chBufName, "AttackPower" ) == 0 )
			{
				prj.m_fMonsterHitRate = 1.0f;
				FLTRACE_LOG( PROGRAM_NAME, _T( "AttackPower GAME_SETTING %f" ), prj.m_fMonsterHitRate );
			}
			else
			if( strcmp( chBufName, "Respawn" ) == 0 )
			{
				prj.m_fMonsterRespawnRate = 1.0f;
				FLTRACE_LOG( PROGRAM_NAME, _T( "Respawn GAME_SETTING %f" ), prj.m_fMonsterRespawnRate );
			}
			else
			if( strcmp( chBufName, "Aggressive" ) == 0 )
			{
				prj.m_fMonsterAggressiveRate = 0.2f;
				FLTRACE_LOG( PROGRAM_NAME, _T( "Aggressive GAME_SETTING %f" ), prj.m_fMonsterAggressiveRate );
			}
			else
			if( strcmp( chBufName, "Price" ) == 0 )
			{
				prj.m_fShopCost = 1.0f;
				FLTRACE_LOG( PROGRAM_NAME, _T( "Price GAME_SETTING %f" ), prj.m_fShopCost );
			}
		}
		prj.m_bBaseGameSetting = TRUE;
		bResult = TRUE;
	}
	return bResult;
}

void CDbManager::GetMonsterRespawnSetting( CQuery* pQuery, int& nRemoveMaxRespawn, int aRemoveRespawn[], char* lpstrTime )
{
	char szQuery[1024];

	FLSPrintf( szQuery, _countof( szQuery ), "GAME_BACKSYSTEM_STR 'S2','%02d','%s'", g_appInfo.dwSys, lpstrTime );

	if( FALSE == pQuery->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return;
	}

	int nResult;
//	int nAddUpdateSize = 0;
	MONSTER_RESPAWN MonsterRespawn;
	while( pQuery->Fetch() )
	{
		nResult = pQuery->GetInt( "bResult" );
		MonsterRespawn.nRespawnIndex = pQuery->GetInt( "Num" );
		if( nResult == 1)	// 추가
		{
			pQuery->GetStr( "Name", MonsterRespawn.szMonsterName, _countof( MonsterRespawn.szMonsterName ) );
			MonsterRespawn.vPos.x = (float)( pQuery->GetInt( "Pos_x" ) );
			MonsterRespawn.vPos.y = (float)( pQuery->GetInt( "Pos_y" ) );
			MonsterRespawn.vPos.z = (float)( pQuery->GetInt( "Pos_z" ) );
			MonsterRespawn.nQuantity = pQuery->GetInt( "Quantity" );
			MonsterRespawn.nAggressive = pQuery->GetInt( "Aggressive" );
			MonsterRespawn.nRange = pQuery->GetInt( "Range" );
			MonsterRespawn.nTime = pQuery->GetInt( "dwTime" );
				
			AddMonsterRespawn( MonsterRespawn );
		}
		else		// 삭제
		{
			if( aRemoveRespawn != NULL )
			{
				RemoveMonsterRespawn( MonsterRespawn.nRespawnIndex, nRemoveMaxRespawn, aRemoveRespawn );
			}
		}
	}
	
	FLTRACE_LOG( PROGRAM_NAME, _T( "%d MONSTER_RESPAWN" ), prj.m_nMonsterRespawnSize );
}

void CDbManager::GetMonsterPropSetting( CQuery* pQuery, char* lpstrTime )
{
	char szQuery[1024];
	
	FLSPrintf( szQuery, _countof( szQuery ), "GAME_BACKSYSTEM_STR 'S3','%02d','%s'", g_appInfo.dwSys, lpstrTime );
	
	if( FALSE == pQuery->Exec( szQuery ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return;
	}

	int nResult;
//	int nAddUpdateSize = 0;
	MONSTER_PROP MonsterProp;
	while( pQuery->Fetch() )
	{
		nResult = pQuery->GetInt( "bResult" );
		pQuery->GetStr( "MonName", MonsterProp.szMonsterName, _countof( MonsterProp.szMonsterName ) );
		if( nResult == 1)	// 추가
		{
			MonsterProp.nHitPoint = pQuery->GetInt( "HitPoint" );
			MonsterProp.nAttackPower = pQuery->GetInt( "AttackPower" );
			MonsterProp.nDefence = pQuery->GetInt( "Defence" );
			MonsterProp.nExp = pQuery->GetInt( "Exp" );
			MonsterProp.nItemDrop = pQuery->GetInt( "ItemDrop" );
			MonsterProp.nPenya = pQuery->GetInt( "Penya" );

			AddMonsterProp( MonsterProp );
		}
		else		// 삭제
		{
			if( lpstrTime != '\0' )
			{
				RemoveMonsterProp( MonsterProp );
			}
		}
	}
	
	FLTRACE_LOG( PROGRAM_NAME, _T( "%d MONSTER_PROP" ), prj.m_nMonsterPropSize );

	return;
}

BOOL CDbManager::AddMonsterRespawn( MONSTER_RESPAWN BufMonsterRespawn )
{
	if( prj.m_nMonsterPropSize < MAX_RESPAWN )
	{
		FLStrcpy( prj.m_aMonsterRespawn[ prj.m_nMonsterRespawnSize ].szMonsterName, _countof( prj.m_aMonsterRespawn[ prj.m_nMonsterRespawnSize ].szMonsterName ), BufMonsterRespawn.szMonsterName );
		prj.m_aMonsterRespawn[ prj.m_nMonsterRespawnSize ].vPos = BufMonsterRespawn.vPos;
		prj.m_aMonsterRespawn[ prj.m_nMonsterRespawnSize ].nQuantity = BufMonsterRespawn.nQuantity;
		prj.m_aMonsterRespawn[ prj.m_nMonsterRespawnSize ].nAggressive = BufMonsterRespawn.nAggressive;
		prj.m_aMonsterRespawn[ prj.m_nMonsterRespawnSize ].nRange = BufMonsterRespawn.nRange;
		prj.m_aMonsterRespawn[ prj.m_nMonsterRespawnSize ].nTime = BufMonsterRespawn.nTime;
		prj.m_aMonsterRespawn[ prj.m_nMonsterRespawnSize ].nRespawnIndex = BufMonsterRespawn.nRespawnIndex;
		prj.n_aAddRespawn[ prj.m_nAddMonsterRespawnSize ] = prj.m_aMonsterRespawn[ prj.m_nMonsterRespawnSize ];

		++prj.m_nMonsterRespawnSize;
		++prj.m_nAddMonsterRespawnSize;
		FLTRACE_LOG( PROGRAM_NAME, _T( "Add MONSTER_RESPAWN : MAX(%d), MonName(%s)" ), prj.m_nMonsterRespawnSize,  prj.m_aMonsterRespawn[ prj.m_nMonsterRespawnSize - 1].szMonsterName);
		return TRUE;
	}
	else
		FLERROR_LOG( PROGRAM_NAME, _T( "AddMonsterRespawn Overflow Error" ) );

	return FALSE;
}

BOOL CDbManager::RemoveMonsterRespawn( int nIndex, int &nMaxRemove, int aRemoveRespawn[] )
{
	BOOL bFind = FALSE;
	int nRemoveBuf = 0;
	for( DWORD i = 0 ; i < prj.m_nMonsterRespawnSize ; ++i )
	{
		if( prj.m_aMonsterRespawn[i].nRespawnIndex == nIndex )
		{
			nRemoveBuf = i;
			bFind = TRUE;
			break;
		}
	}
	
	if( bFind )
	{
		for( DWORD i = nRemoveBuf ; i < prj.m_nMonsterRespawnSize - 1 ; ++i )
		{
			prj.m_aMonsterRespawn[i] = prj.m_aMonsterRespawn[i + 1];
		}
		aRemoveRespawn[nMaxRemove] = nIndex;
		++nMaxRemove;
		--prj.m_nMonsterRespawnSize;
	}
	return FALSE;
}

BOOL CDbManager::AddMonsterProp( MONSTER_PROP BufMonsterProp )
{
	if( prj.m_nMonsterPropSize < MAX_MONSTER_PROP )
	{
		FLStrcpy( prj.m_aMonsterProp[ prj.m_nMonsterPropSize ].szMonsterName, _countof( prj.m_aMonsterProp[ prj.m_nMonsterPropSize ].szMonsterName ), BufMonsterProp.szMonsterName );
		prj.m_aMonsterProp[ prj.m_nMonsterPropSize ].nHitPoint = BufMonsterProp.nHitPoint;
		prj.m_aMonsterProp[ prj.m_nMonsterPropSize ].nAttackPower = BufMonsterProp.nAttackPower;
		prj.m_aMonsterProp[ prj.m_nMonsterPropSize ].nDefence = BufMonsterProp.nDefence;
		prj.m_aMonsterProp[ prj.m_nMonsterPropSize ].nExp = BufMonsterProp.nExp;
		prj.m_aMonsterProp[ prj.m_nMonsterPropSize ].nItemDrop = BufMonsterProp.nItemDrop;
		prj.m_aMonsterProp[ prj.m_nMonsterPropSize ].nPenya = BufMonsterProp.nPenya;
		prj.m_aAddProp[ prj.m_nAddMonsterPropSize ] = prj.m_aMonsterProp[ prj.m_nMonsterPropSize ];
		
		++prj.m_nMonsterPropSize;
		++prj.m_nAddMonsterPropSize;
		FLTRACE_LOG( PROGRAM_NAME, _T( "Add MONSTER_PROP : MAX(%d), MonName(%s)" ), prj.m_nMonsterPropSize,  prj.m_aMonsterProp[ prj.m_nMonsterPropSize - 1].szMonsterName);
		return TRUE;
	}
	else
		FLERROR_LOG( PROGRAM_NAME, _T( "AddMonsterProp Overflow Error" ) );
	
	return FALSE;
}

BOOL CDbManager::RemoveMonsterProp( MONSTER_PROP BufMonsterProp )
{
	BOOL bFind = FALSE;
	int nRemoveBuf = 0;
	for( DWORD i = 0 ; i < prj.m_nMonsterPropSize ; ++i )
	{
		if( strcmp( prj.m_aMonsterProp[i].szMonsterName, BufMonsterProp.szMonsterName ) == 0 )
		{
			nRemoveBuf = i;
			bFind = TRUE;
			break;
		}
	}
	
	if( bFind )
	{
		for( DWORD i = nRemoveBuf ; i < prj.m_nMonsterPropSize - 1 ; ++i )
		{
			prj.m_aMonsterProp[i] = prj.m_aMonsterProp[i + 1];
		}
		FLStrcpy( prj.m_aRemoveProp[prj.m_nRemoveMonsterPropSize], _countof( prj.m_aRemoveProp[prj.m_nRemoveMonsterPropSize] ), BufMonsterProp.szMonsterName );
		++prj.m_nRemoveMonsterPropSize;
		--prj.m_nMonsterPropSize;
	}
	return FALSE;
}


#endif // __S1108_BACK_END_SYSTEM
