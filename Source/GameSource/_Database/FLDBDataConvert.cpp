#include "StdAfx.h"
#include "FLDBDataConvert.h"
extern AppInfo g_appInfo;

FLDBDataConvert::FLDBDataConvert()
{
	Init();
}

FLDBDataConvert::~FLDBDataConvert()
{
	Clear();
}

FLDBDataConvert* FLDBDataConvert::GetInstance()
{
	static FLDBDataConvert kFLDBDataConvert;

	return & kFLDBDataConvert;
}

bool	FLDBDataConvert::ConvertGuildBank()
{
	if( m_pOldCharacterQuery == NULL || m_pNewCharacterQuery == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "m_pCharacterQuery is NULL" ) );
		
		return false;
	}

	TCHAR szQuery[64] = { 0, };

	FLSPrintf( szQuery, _countof( szQuery ), "GUILD_BANK_STR 'S1','0','%02d'", g_appInfo.dwSys );
	if( m_pOldCharacterQuery->Exec( szQuery ) == FALSE )
	{
		return false;
	}


	DWORD dwGuildId;

	while( m_pOldCharacterQuery->Fetch() )
	{
		CItemContainer<FLItemElem>	kGuildBank;	// 길드 창고
		kGuildBank.SetItemContainer( ITYPE_ITEM, CONTAINER_TYPE_GUILD_BANK, MAX_GUILDBANK );

		dwGuildId		= static_cast<DWORD>( m_pOldCharacterQuery->GetInt( "m_idGuild" ) );

		int		CountStr		= 0;
		int		IndexItem		= 0;
		int		itemCount		= 0;


		TCHAR	szGuildBank[7500]		= { 0, };
		m_pOldCharacterQuery->GetStr( "m_GuildBank", szGuildBank, _countof( szGuildBank ) );

		if( IsValidString( szGuildBank ) == false )
		{
			return false;
		}

		while( '$' != szGuildBank[CountStr] )
		{
			FLItemElem BufItemElem;
			IndexItem = CDbManager::GetInstance().GetOneItem( &BufItemElem, szGuildBank, &CountStr );
			if( IndexItem == -1 )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "프로퍼티 없음. %d, %d" ), dwGuildId, BufItemElem.m_dwItemId );
			}
			else
			{
				if( IndexItem >= MAX_GUILDBANK )
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "IndexItem %d, %d" ), dwGuildId, IndexItem );
					FLERROR_LOG( PROGRAM_NAME, _T( "GuildBank = %s" ), szGuildBank );
					return false;
				}
				kGuildBank.m_apItem[IndexItem] = BufItemElem;
			}
		}

		kGuildBank.SetAllocedSerialNumber( CDbManager::GetInstance().GetSerialNumberStatus( szGuildBank, &CountStr ) );


		CountStr		= 0;
		TCHAR	szIndex[512]		= { 0, };
		m_pOldCharacterQuery->GetStr( "m_apIndex", szIndex, _countof( szIndex ) );

		if( IsValidString( szIndex ) == false )
		{
			return false;
		}

		while( '$' != szIndex[CountStr] )
		{
			kGuildBank.m_apIndex[itemCount]	= (DWORD)GetIntFromStr( szIndex, &CountStr );
			itemCount++;
		}

		CountStr	= 0;
		int IndexObjIndex	= 0;
		TCHAR	szObjIndex[512]	= {0,};
		m_pOldCharacterQuery->GetStr( "m_dwObjIndex", szObjIndex, _countof( szObjIndex ) );

		if( IsValidString( szObjIndex ) == false )
		{
			return false;
		}

		while( '$' != szObjIndex[CountStr] )
		{
			kGuildBank.m_apItem[IndexObjIndex].m_dwObjIndex	= (DWORD)GetIntFromStr( szObjIndex, &CountStr );
			IndexObjIndex++;
		}


		CountStr	= 0;
		int nExtBank = 0;
		TCHAR	szExtGuildBank[2000] = {0,};
		m_pOldCharacterQuery->GetStr( "m_extGuildBank", szExtGuildBank, _countof( szExtGuildBank ) );

		if( IsValidString( szExtGuildBank ) == false )
		{
			return false;
		}

		while( '$' != szExtGuildBank[CountStr] )
		{
			kGuildBank.m_apItem[nExtBank].m_bCharged				= (BOOL)GetIntPaFromStr( szExtGuildBank, &CountStr );
			if( kGuildBank.m_apItem[nExtBank].m_bCharged != 1 )
			{
				kGuildBank.m_apItem[nExtBank].m_bCharged	= 0;
			}
			kGuildBank.m_apItem[nExtBank].m_dwKeepTime				= (DWORD)GetIntPaFromStr( szExtGuildBank, &CountStr );

			//				GuildBank.m_apItem[nExtBank].SetRandomOptItemId( GetInt64PaFromStr( ExtBank, &CountStr ) );

			__int64 n64RandomOpionId = GetInt64PaFromStr( szExtGuildBank, &CountStr );
			kGuildBank.m_apItem[nExtBank].RandomOptionDataConverting( n64RandomOpionId );

			kGuildBank.m_apItem[nExtBank].m_bTranformVisPet = static_cast<BOOL>( GetIntPaFromStr( szExtGuildBank, &CountStr ) );

			++CountStr;
			++nExtBank;
		}

		CountStr	= 0;
		int nPirecingBank = 0;
		TCHAR	szGuildBankPiercing[4000] = {0,};
		m_pOldCharacterQuery->GetStr( "m_GuildBankPiercing", szGuildBankPiercing, _countof( szGuildBankPiercing ) );

		if( IsValidString( szGuildBankPiercing ) == false )
		{
			return false;
		}

		while( '$' != szGuildBankPiercing[CountStr] )
		{
			CDbManager::GetInstance().LoadPiercingInfo( kGuildBank.m_apItem[nPirecingBank], szGuildBankPiercing, &CountStr );
			++nPirecingBank;
		}

		CountStr	= 0;
		int nGuildBankPet = 0;
		TCHAR	szGuildBankPet[4200] = {0,};
		m_pOldCharacterQuery->GetStr( "szGuildBankPet", szGuildBankPet, _countof( szGuildBankPet ) );

		if( IsValidString( szGuildBankPet ) == false )
		{
			return false;
		}

		while( '$' != szGuildBankPet[CountStr] )
		{
			//			SAFE_DELETE( GuildBank.m_apItem[nGuildBankPet].m_pPet );
			BOOL bPet	= (BOOL)GetIntFromStr( szGuildBankPet, &CountStr );
			if( bPet )
			{
				CPet* pPet	= kGuildBank.m_apItem[nGuildBankPet].m_pPet	= new CPet;
				BYTE nKind	= (BYTE)GetIntFromStr( szGuildBankPet, &CountStr );
				pPet->SetKind( nKind );
				BYTE nLevel	= (BYTE)GetIntFromStr( szGuildBankPet, &CountStr );
				pPet->SetLevel( nLevel );
				DWORD dwExp	= (DWORD)GetIntFromStr( szGuildBankPet, &CountStr );
				pPet->SetExp( dwExp );
				WORD wEnergy	= (WORD)GetIntFromStr( szGuildBankPet, &CountStr );
				pPet->SetEnergy( wEnergy );
				WORD wLife	= (WORD)GetIntPaFromStr( szGuildBankPet, &CountStr );
				pPet->SetLife( wLife );
				for( BYTE i = PL_D; i <= pPet->GetLevel(); i++ )
				{
					BYTE nAvailLevel	= (BYTE)GetIntPaFromStr( szGuildBankPet, &CountStr );
					pPet->SetAvailLevel( i, nAvailLevel );
				}
				char szFmt[MAX_PET_NAME_FMT]	= { 0,};
				GetStrFromStr( szGuildBankPet, szFmt, &CountStr );
				char szName[MAX_PET_NAME]	= { 0,};
				CDbManager::GetInstance().GetDBFormatStr( szName, MAX_PET_NAME, szFmt );
				pPet->SetName( szName );
			}
			//			++CountStr;
			++nGuildBankPet;
		}

		kGuildBank.RefreshIndex();


		//////////////////////////////////////////////////////////////////////////
		// BEGIN Convert Data
		if( kGuildBank.IsAllEmpty() == FALSE )
		{
			if( CDbManager::GetInstance().SaveGuildBank( dwGuildId, &kGuildBank, m_pNewCharacterQuery ) == true )
			{
				OutputConvertLog( CD_GUILD_BANK, dwGuildId, "m_apIndex", szIndex );
				OutputConvertLog( CD_GUILD_BANK, dwGuildId, "m_dwObjIndex", szObjIndex );
				OutputConvertLog( CD_GUILD_BANK, dwGuildId, "m_GuildBank", szGuildBank );
				OutputConvertLog( CD_GUILD_BANK, dwGuildId, "m_extGuildBank", szExtGuildBank );
				OutputConvertLog( CD_GUILD_BANK, dwGuildId, "m_GuildBankPiercing", szGuildBankPiercing );
				OutputConvertLog( CD_GUILD_BANK, dwGuildId, "szGuildBankPet", szGuildBankPet );
			}
			else
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "SaveGuildBank Failed. GuildId:[%06d]" ), dwGuildId );
				return false;
			}
		}
		// END Convert Data
		//////////////////////////////////////////////////////////////////////////
	}

	//////////////////////////////////////////////////////////////////////////
	// BEGIN Old Data Delete
	memset( szQuery, 0, sizeof( szQuery ) );

	FLSPrintf( szQuery, _countof( szQuery ), "uspRowGuildBankInitialize '%02d'", g_appInfo.dwSys );
	if( m_pNewCharacterQuery->Exec( szQuery ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "%s" ), szQuery );
		return false;
	}
	else
	{
		if( m_pNewCharacterQuery->Fetch() == TRUE )
		{
			int nError = 0;
			nError = m_pNewCharacterQuery->GetInt( "fError" );

			if( nError != 0 )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "Initialize Guild Bank Old Data Failed." ) );
				return false;
			}
		}
		else
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "uspRowGuildBankInitialize Fetch Failed." ) );
			return false;
		}
	}
	// END Old Data Delete
	//////////////////////////////////////////////////////////////////////////

	FLINFO_LOG( PROGRAM_NAME, _T( "ConvertGuildBank Complete" ) );

	return true;
}

void	FLDBDataConvert::Init()
{
	m_pOldCharacterQuery = NULL;
	m_pNewCharacterQuery = NULL;

	const char* pass	= CDbManager::GetInstance().DB_ADMIN_PASS_CHARACTER01;

	m_pOldCharacterQuery = new CQuery;
	
#ifdef __DbStuff
	if (!m_pOldCharacterQuery->Connect(CQuery::SqlDatabase::CHARACTER))
#else
	if( m_pOldCharacterQuery->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, pass ) == FALSE )
#endif
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Can't connect db: CreateCharacterQuery" ) );
		
		SAFE_DELETE( m_pOldCharacterQuery );

		return;
	}

	m_pNewCharacterQuery = new CQuery;

#ifdef __DbStuff
	if (!m_pNewCharacterQuery->Connect(CQuery::SqlDatabase::CHARACTER))
#else
	if( m_pNewCharacterQuery->Connect( 3, DSN_NAME_CHARACTER01, DB_ADMIN_ID_CHARACTER01, pass ) == FALSE )
#endif
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "Can't connect db: CreateCharacterQuery" ) );

		SAFE_DELETE( m_pOldCharacterQuery );
		SAFE_DELETE( m_pNewCharacterQuery );
	}

	return;
}

void	FLDBDataConvert::Clear()
{
	SAFE_DELETE( m_pOldCharacterQuery );
	SAFE_DELETE( m_pNewCharacterQuery );
}

bool	FLDBDataConvert::IsValidString( const TCHAR* pszData )
{
	int nLen = _tcslen( pszData );
	if( nLen > 0 && pszData[ nLen - 1 ] == '$' )
	{
		return true;
	}

	return false;
}

void	FLDBDataConvert::OutputConvertLog( int nConvertDataKind, DWORD dwId, const TCHAR* pszColumn, const TCHAR* pszData )
{
	switch( nConvertDataKind )
	{
	case CD_GUILD_BANK:
		FLINFO_LOG( "CONVERT_GUILD_BANK", _T( "GuildId:[%06d], Column:[%s], Data:[%s]" ), dwId, pszColumn, pszData );
		break;

	default:
		break;
	}
}

