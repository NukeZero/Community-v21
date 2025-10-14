
#include "stdafx.h"

#include "FLEventArenaSpecLoader.h"
#include "FLEventArenaSpecStruct.h"


FLEventArenaSpecLoader::FLEventArenaSpecLoader()
{
}

FLEventArenaSpecLoader::~FLEventArenaSpecLoader()
{
}

bool	FLEventArenaSpecLoader::Load( const TCHAR* pszConfigFile, FLEventArenaSpec* pSpec, FLEventArenaTeamManager* pTeamManager )
{
	if( pszConfigFile == NULL || pSpec == NULL || pTeamManager == NULL )
	{
		return false;
	}

	TCHAR szFullConfigPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullConfigPath, _countof( szFullConfigPath ), pszConfigFile );

	CScript kScanner;
	if( kScanner.Load( szFullConfigPath, FALSE ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullConfigPath );
		return false;
	}

	pSpec->Clear();

	bool bLoadResult = true;
	kScanner.GetToken();

	while( kScanner.tok != FINISHED )
	{
		if( kScanner.Token == "TEAM_LIST" )
		{
			kScanner.GetToken();

			TCHAR szFullPath[ MAX_PATH ]	= { 0, };
			g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), kScanner.Token );

			if( LoadTeamList( szFullPath, pTeamManager ) == false )
			{
				bLoadResult = false;
				break;
			}
		}
		else if( kScanner.Token == "TONEMENT_LIST" )
		{
			kScanner.GetToken();

			TCHAR szFullPath[ MAX_PATH ]	= { 0, };
			g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), kScanner.Token );

			if( LoadTonementList( szFullPath, pSpec ) == false )
			{
				bLoadResult = false;
				break;
			}
		}
		else if( kScanner.Token == "GIVE_ITEM_LIST" )
		{
			kScanner.GetToken();

			TCHAR szFullPath[ MAX_PATH ]	= { 0, };
			g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), kScanner.Token );

			if( LoadGiveItemList( szFullPath, pSpec ) == false )
			{
				bLoadResult = false;
				break;
			}
		}
		else if( kScanner.Token == "TIME_CONFIG" )
		{
			kScanner.GetToken();

			TCHAR szFullPath[ MAX_PATH ]	= { 0, };
			g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), kScanner.Token );

			if( LoadTimeConfig( szFullPath, pSpec ) == false )
			{
				bLoadResult = false;
				break;
			}
		}
		else if( kScanner.Token == "START_POSITION_LIST" )
		{
			kScanner.GetToken();

			TCHAR szFullPath[ MAX_PATH ]	= { 0, };
			g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), kScanner.Token );

			if( LoadStartPositionList( szFullPath, pSpec ) == false )
			{
				bLoadResult = false;
				break;
			}
		}
		else if( kScanner.Token == "SUMMON_LIST" )
		{
			kScanner.GetToken();

			TCHAR szFullPath[ MAX_PATH ]	= { 0, };
			g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), kScanner.Token );

			if( LoadSummonList( szFullPath, pSpec ) == false )
			{
				bLoadResult = false;
				break;
			}
		}

		kScanner.GetToken();
	}

	if( bLoadResult == false )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "EventArena %s File Load Failed." ), (const TCHAR*) kScanner.Token );
	}

	return bLoadResult;
}

bool	FLEventArenaSpecLoader::LoadTeamList( const TCHAR* pszTeamList, FLEventArenaTeamManager* pTeamManager )
{
	CScript kScanner;
	if( kScanner.Load( pszTeamList, FALSE ) == FALSE )
	{
		return false;
	}

	kScanner.GetToken();
	while( kScanner.tok != FINISHED )
	{
		TCHAR szTeamName[128] = "";

		T_EVENT_ARENA_TEAM_MEMBER_SPEC tMember = { 0 };
		//::mem_set( &tMember, 0, sizeof( tMember ) );

		kScanner.Token.TrimRight();
		FLStrcpy( szTeamName, _countof( szTeamName ), kScanner.Token );

		tMember.byPartyNumber = static_cast< BYTE >( kScanner.GetNumber() );

		kScanner.GetToken();
		kScanner.Token.TrimRight();
		FLStrcpy( tMember.szCharName, _countof( tMember.szCharName ), kScanner.Token );

		if( pTeamManager->AddTeamMember( szTeamName, &tMember ) == false )
		{
			return false;
		}

		kScanner.GetToken();
	}

	return pTeamManager->Check();
}

bool	FLEventArenaSpecLoader::LoadTonementList( const TCHAR* pszTonementList, FLEventArenaSpec* pSpec )
{
	CScript kScanner;
	if( kScanner.Load( pszTonementList, FALSE ) == FALSE )
	{
		return false;
	}

	kScanner.GetToken();
	while( kScanner.tok != FINISHED )
	{
		T_EVENT_ARENA_TONEMENT_SPEC tTonement = { 0 };
		//::mem_set( &tTonement, 0, sizeof( tTonement ) );

		kScanner.Token.TrimRight();
		FLStrcpy( tTonement.szName, _countof( tTonement.szName ), kScanner.Token );

		tTonement.nNeedWinCount = kScanner.GetNumber();

		kScanner.GetToken();
		kScanner.Token.TrimRight();
		FLStrcpy( tTonement.szDesc, _countof( tTonement.szDesc ), kScanner.Token );

		if( pSpec->AddTonement( &tTonement ) == false )
		{
			return false;
		}

		kScanner.GetToken();
	}

	return true;
}

bool	FLEventArenaSpecLoader::LoadGiveItemList( const TCHAR* pszGiveItemList, FLEventArenaSpec* pSpec )
{
	CScript kScanner;
	if( kScanner.Load( pszGiveItemList, FALSE ) == FALSE )
	{
		return false;
	}

	DWORD dwItemIndex = kScanner.GetNumber();
	while( kScanner.tok != FINISHED )
	{
		T_EVENT_ARENA_GIVE_ITEM_SPEC tGiveItem = { 0 };
		//::mem_set( &tGiveItem, 0, sizeof( tGiveItem ) );

		tGiveItem.dwItemIndex = dwItemIndex;
		tGiveItem.byCount = static_cast< BYTE >( kScanner.GetNumber() );

		if( pSpec->AddGiveItem( &tGiveItem ) == false )
		{
			return false;
		}

		dwItemIndex = kScanner.GetNumber();
	}

	return true;
}

bool	FLEventArenaSpecLoader::LoadTimeConfig( const TCHAR* pszTimeConfig, FLEventArenaSpec* pSpec )
{
	CScript kScanner;
	if( kScanner.Load( pszTimeConfig, FALSE ) == FALSE )
	{
		return false;
	}

	T_EVENT_ARENA_TIME_CONFIG_SPEC tTimeConfig = { 0 };
	//::mem_set( &tTimeConfig, 0, sizeof( tTimeConfig ) );

	// 이 데이터는 한 라인
	tTimeConfig.dwBattleTime = kScanner.GetNumber();
	tTimeConfig.dwCallWaitTime = kScanner.GetNumber();
	tTimeConfig.dwBattleStandbyTime = kScanner.GetNumber();
	tTimeConfig.dwWinnerDisplayTime = kScanner.GetNumber();

	if( pSpec->SetTimeConfig( &tTimeConfig ) == false )
	{
		return false;
	}

	return true;
}

bool	FLEventArenaSpecLoader::LoadStartPositionList( const TCHAR* pszStartPositionList, FLEventArenaSpec* pSpec )
{
	CScript kScanner;
	if( kScanner.Load( pszStartPositionList, FALSE ) == FALSE )
	{
		return false;
	}

	DWORD dwWorldID = kScanner.GetNumber();
	while( kScanner.tok != FINISHED )
	{
		T_EVENT_ARENA_START_POSITION_SPEC tStartPos = { 0 };
		//::mem_set( &tStartPos, 0, sizeof( tStartPos ) );

		tStartPos.nStartKind = kScanner.GetNumber();
		tStartPos.nTeam = kScanner.GetNumber();

		tStartPos.tPosition.dwWorldID = dwWorldID;
		tStartPos.tPosition.tPos.x = kScanner.GetFloat();
		tStartPos.tPosition.tPos.y = kScanner.GetFloat();
		tStartPos.tPosition.tPos.z = kScanner.GetFloat();
		tStartPos.dwRadius = kScanner.GetNumber();

		if( pSpec->AddStartPos( &tStartPos ) == false )
		{
			return false;
		}

		dwWorldID = kScanner.GetNumber();
	}

	return true;
}

bool	FLEventArenaSpecLoader::LoadSummonList( const TCHAR* pszSummonList, FLEventArenaSpec* pSpec )
{
	CScript kScanner;
	if( kScanner.Load( pszSummonList, FALSE ) == FALSE )
	{
		return false;
	}

	// 이 데이터는 한 라인
	pSpec->SetDeathNPCID( kScanner.GetNumber() );
	pSpec->SetWallCtrlID( kScanner.GetNumber() );
	pSpec->SetTransformMoverID( kScanner.GetNumber() );

	return true;
}

